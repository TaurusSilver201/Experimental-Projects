#include <iostream>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;

int main(int argc, char** argv) {
    // Check if an image file is provided as an argument
    if (argc != 2) {
        cout << "Usage: " << argv[0] << " <image_file>" << endl;
        return -1;
    }

    // Load the image
    Mat image = imread(argv[1]);

    if (image.empty()) {
        cout << "Could not open or find the image: " << argv[1] << endl;
        return -1;
    }

    Mat originalImage = image.clone();

    // Display the original image
    imshow("Original Image", image);

    // Create a menu-driven interface
    int option;
    bool exitMenu = false;

    while (!exitMenu) {
        cout << "Select an option:" << endl;
        cout << "1. Resize" << endl;
        cout << "2. Crop" << endl;
        cout << "3. Apply Gaussian Blur" << endl;
        cout << "4. Apply Sobel Edge Detection" << endl;
        cout << "5. Apply Sepia Filter" << endl;
        cout << "6. Reset to Original Image" << endl;
        cout << "7. Save Image" << endl;
        cout << "8. Exit" << endl;

        cin >> option;

        switch (option) {
            case 1: {
                // Resize the image
                int newWidth, newHeight;
                cout << "Enter new width: ";
                cin >> newWidth;
                cout << "Enter new height: ";
                cin >> newHeight;

                resize(image, image, Size(newWidth, newHeight));
                break;
            }
            case 2: {
                // Crop the image
                int x, y, width, height;
                cout << "Enter top-left corner coordinates (x, y): ";
                cin >> x >> y;
                cout << "Enter width: ";
                cin >> width;
                cout << "Enter height: ";
                cin >> height;

                Rect roi(x, y, width, height);
                image = originalImage(roi).clone();
                break;
            }
            case 3: {
                // Apply Gaussian blur
                GaussianBlur(image, image, Size(5, 5), 0, 0);
                break;
            }
            case 4: {
                // Apply Sobel edge detection
                Mat grayscaleImage;
                cvtColor(image, grayscaleImage, COLOR_BGR2GRAY);
                Sobel(grayscaleImage, image, CV_8U, 1, 1);
                break;
            }
            case 5: {
                // Apply Sepia filter
                Mat kernel = (Mat_<float>(3, 3) <<
                    0.272, 0.534, 0.131,
                    0.349, 0.686, 0.168,
                    0.393, 0.769, 0.189);
                transform(image, image, kernel);
                break;
            }
            case 6: {
                // Reset to original image
                image = originalImage.clone();
                break;
            }
            case 7: {
                // Save the modified image
                imwrite("modified_image.jpg", image);
                cout << "Modified image saved as modified_image.jpg" << endl;
                break;
            }
            case 8: {
                // Exit
                exitMenu = true;
                break;
            }
            default:
                cout << "Invalid option! Please try again." << endl;
                break;
        }

        // Display the modified image
        imshow("Modified Image", image);
        waitKey(0);
    }

    return 0;
}
