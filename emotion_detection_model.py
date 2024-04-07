import cv2
import numpy as np
from keras.models import load_model
from keras.preprocessing.image import img_to_array, load_img
from keras.applications.vgg16 import preprocess_input

# Load pre-trained VGG-Face model for emotion detection
MODEL_PATH = 'path_to_model.h5'
emotion_detection_model = load_model(MODEL_PATH)

# Define the emotions
EMOTIONS = {
    0: 'Angry',
    1: 'Disgust',
    2: 'Fear',
    3: 'Happy',
    4: 'Neutral',
    5: 'Sad',
    6: 'Surprise'
}

def preprocess_image(image_path, target_size=(224, 224)):
    """
    Preprocess an image for emotion detection.
    
    Args:
        image_path (str): The path to the image file.
        target_size (tuple): The target size for the input image (default is (224, 224)).
    
    Returns:
        numpy.ndarray: The preprocessed image.
    """
    img = load_img(image_path, target_size=target_size)
    x = img_to_array(img)
    x = np.expand_dims(x, axis=0)
    x = preprocess_input(x)
    return x

def detect_emotion(image_path):
    """
    Detect the emotion from an image.
    
    Args:
        image_path (str): The path to the image file.
    
    Returns:
        str: The detected emotion label.
    """
    preprocessed_image = preprocess_image(image_path)
    prediction = emotion_detection_model.predict(preprocessed_image)
    emotion_index = np.argmax(prediction)
    emotion_label = EMOTIONS[emotion_index]
    return emotion_label

def main():
    image_path = 'path_to_image.jpg'
    detected_emotion = detect_emotion(image_path)
    print("Detected Emotion:", detected_emotion)

if __name__ == "__main__":
    main()
