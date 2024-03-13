import os
import cv2
import numpy as np
from tensorflow.keras.preprocessing.image import ImageDataGenerator
from tensorflow.keras.applications import VGG16
from tensorflow.keras.layers import Dense, GlobalAveragePooling2D
from tensorflow.keras.models import Sequential
from tensorflow.keras.optimizers import Adam

# Define the paths to the two image libraries
library1_path = 'path/to/library1'
library2_path = 'path/to/library2'

# Data preprocessing
data_generator = ImageDataGenerator(rescale=1./255, validation_split=0.2)

train_generator = data_generator.flow_from_directory(
    directory='path/to/combined_library',
    target_size=(224, 224),
    batch_size=32,
    class_mode='binary',
    subset='training'
)

validation_generator = data_generator.flow_from_directory(
    directory='path/to/combined_library',
    target_size=(224, 224),
    batch_size=32,
    class_mode='binary',
    subset='validation'
)

# Build the model
base_model = VGG16(weights='imagenet', include_top=False, input_shape=(224, 224, 3))

model = Sequential()
model.add(base_model)
model.add(GlobalAveragePooling2D())
model.add(Dense(1, activation='sigmoid'))

model.compile(optimizer=Adam(), loss='binary_crossentropy', metrics=['accuracy'])

# Train the model
epochs = 20
steps_per_epoch = train_generator.samples // 32
validation_steps = validation_generator.samples // 32

model.fit(
    train_generator,
    steps_per_epoch=steps_per_epoch,
    epochs=epochs,
    validation_data=validation_generator,
    validation_steps=validation_steps
)

# Function to predict which library an input image belongs to
def predict_library(input_image_path):
    img = cv2.imread(input_image_path)
    img = cv2.resize(img, (224, 224))
    img = np.expand_dims(img, axis=0)
    img = img / 255.0
    prediction = model.predict(img)[0][0]
    if prediction > 0.5:
        return 'Library 2', prediction
    else:
        return 'Library 1', 1 - prediction

# Example usage
input_image_path = 'path/to/input_image.jpg'
library, probability = predict_library(input_image_path)
print(f"The input image is closer to {library} with probability {probability:.2f}")
