import cv2
import pytesseract
import os
from PIL import Image
import numpy as np
from pytesseract import Output
import concurrent.futures
import re
from collections import defaultdict
import pdf2image
import torch
import torchvision.transforms as transforms
from transformers import BertTokenizer, BertForMaskedLM

# Path to the Tesseract OCR executable
pytesseract.pytesseract.tesseract_cmd = r'C:\Program Files\Tesseract-OCR\tesseract.exe'

# Path to the LSTM model data
lstm_model_path = r'C:\Program Files\Tesseract-OCR\tessdata'

# Path to the directory containing handwritten notes
notes_dir = 'handwritten_notes'

# Confidence threshold for filtering OCR results
confidence_threshold = 80

# Load the pre-trained denoising model
denoising_model = torch.hub.load('pytorch/vision:v0.10.0', 'demucs', pretrained=True).eval()

# Load the pre-trained word embedding-based language model
tokenizer = BertTokenizer.from_pretrained('bert-base-uncased')
language_model = BertForMaskedLM.from_pretrained('bert-base-uncased').eval()

# Function to preprocess the image
def preprocess_image(image):
    # Convert to tensor and normalize
    transform = transforms.Compose([
        transforms.ToTensor(),
        transforms.Normalize(mean=[0.5, 0.5, 0.5], std=[0.5, 0.5, 0.5])
    ])
    tensor_image = transform(image).unsqueeze(0)

    # Denoise with the pre-trained model
    with torch.no_grad():
        denoised_tensor = denoising_model(tensor_image)[0]

    # Convert back to numpy array
    denoised_image = denoised_tensor.squeeze().permute(1, 2, 0).cpu().numpy()
    denoised_image = (denoised_image * 255).astype(np.uint8)

    # Binarization
    gray_image = cv2.cvtColor(denoised_image, cv2.COLOR_RGB2GRAY)
    _, binary_image = cv2.threshold(gray_image, 0, 255, cv2.THRESH_BINARY_INV + cv2.THRESH_OTSU)

    return binary_image

# Function to extract text from an image
def extract_text(image_path):
    # Read the image
    image = cv2.imread(image_path)

    # Preprocess the image
    preprocessed_image = preprocess_image(image)

    # Detect script and orientation
    data = pytesseract.image_to_data(preprocessed_image, config='--psm 6 --oem 3 --lstm-model-data "{}"'.format(lstm_model_path), output_type=Output.DICT)
    script = data['script_name'][0]
    angle = data['orientation'][0]

    # Rotate image if needed
    if angle > 0:
        rows, cols = preprocessed_image.shape
        M = cv2.getRotationMatrix2D((cols / 2, rows / 2), angle, 1)
        rotated_image = cv2.warpAffine(preprocessed_image, M, (cols, rows), flags=cv2.INTER_CUBIC, borderMode=cv2.BORDER_REPLICATE)
    else:
        rotated_image = preprocessed_image

    # Perform OCR with beam search decoding and adaptive word detection
    config = r'--oem 3 --psm 6 --lstm-model-data "{}" --beam-width 10 --word-mode 1'.format(lstm_model_path)
    if script == 'Latin':
        config += ' -l eng+fra+deu+spa+ita'
    elif script == 'Arabic':
        config += ' -l ara'
    elif script == 'Han':
        config += ' -l chi_sim+chi_tra'

    data = pytesseract.image_to_data(rotated_image, config=config, output_type=Output.DICT)

    # Segment lines and words
    lines = []
    for i, word_text in enumerate(data['text']):
        if word_text.strip():
            x, y, w, h = data['left'][i], data['top'][i], data['width'][i], data['height'][i]
            line_num = data['line_num'][i]
            conf = data['conf'][i]

            if conf > confidence_threshold:
                # Combine confidence scores from multiple hypotheses using voting
                word_conf = sum(h['conf'] for h in data['conf_merged'][i]) / len(data['conf_merged'][i])

                # Use the word embedding-based language model for context-aware recognition
                tokens = tokenizer.encode(word_text, add_special_tokens=False, return_tensors='pt')
                with torch.no_grad():
                    outputs = language_model(tokens)[0]
                    word_conf *= torch.max(outputs, dim=2)[0].mean().item()

                if line_num >= len(lines):
                    lines.append([])
                lines[line_num].append((word_text, word_conf, x, y, w, h))

    # Sort words within each line by their positions
    sorted_lines = []
    for line in lines:
        sorted_line = sorted(line, key=lambda x: x[2])
        sorted_lines.append(sorted_line)

    return sorted_lines

# Parallel processing using multiple threads with GPU acceleration
def process_notes_dir():
    device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
    denoising_model.to(device)
    language_model.to(device)

    with concurrent.futures.ThreadPoolExecutor() as executor:
        futures = []
        for filename in os.listdir(notes_dir):
            if filename.endswith('.jpg') or filename.endswith('.png') or filename.endswith('.pdf'):
                if filename.endswith('.pdf'):
                    pdf_path = os.path.join(notes_dir, filename)
                    images = pdf2image.convert_from_path(pdf_path)
                    for i, page_image in enumerate(images):
                        page_image_path = os.path.splitext(filename)[0] + f'_page{i+1}.jpg'
                        page_image_path = os.path.join(notes_dir, page_image_path)
                        page_image.save(page_image_path, 'JPEG')
                        future = executor.submit(extract_text, page_image_path)
                        futures.append(future)
                else:
                    image_path = os.path.join(notes_dir, filename)
                    future = executor.submit(extract_text, image_path)
                    futures.append(future)

        recognized_lines = []
        for future in concurrent.futures.as_completed(futures):
            lines = future.result()
            recognized_lines.extend(lines)

        # Create a searchable PDF document with line and word segmentation
        pdf_filename = 'recognized_text.pdf'
        pdf_path = os.path.join(notes_dir, pdf_filename)
        pdf_lines = []
        for line in recognized_lines:
            line_text = ' '.join(word[0] for word in line)
            pdf_lines.append(line_text)
            for word in line:
                word_text, word_conf, x, y, w, h = word
                pdf_lines.append(f'{{{{:confidence:{word_conf:.2f}}}}} {word_text}')

        pdf = pdf2image.pdf_creator(pdf_lines)
        with open(pdf_path, 'wb') as f:
            f.write(pdf)

        print(f'Recognized text with line and word segmentation saved to {pdf_filename}')

# Run the parallel processing
process_notes_dir()
