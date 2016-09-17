#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>

#include "face_detector.hpp"
#include "face_swapper.hpp"

using namespace std;

int main()
{
    try {
        const size_t num_faces = 1;
        FaceDetectorAndTracker detector("./data/haarcascade_frontalface_default.xml", num_faces);
        FaceSwapper face_swapper("./data/shape_predictor_68_face_landmarks.dat");

        // cv::VideoCapture videoInput(0);
        cv::VideoCapture videoInput("../samples/honda_part.mp4");
        detector.setInput(&videoInput);

        auto inputImage = cv::imread("../samples/sone2_large.jpg");
        cv::Mat scaledInputImage;
        cv::resize(inputImage, scaledInputImage, detector.originalFrameSize());
        FaceDetectorAndTracker imageDetector("./data/haarcascade_frontalface_default.xml", 1);
        imageDetector.setDimension(scaledInputImage.size());
        imageDetector.processFrame(scaledInputImage);
        std::vector<cv::Rect> imageFaces = imageDetector.faces();
        cout << imageFaces.size() << " faces detected in image" << endl;


        double fps = 0;
        while (videoInput.isOpened()) {
            auto time_start = cv::getTickCount();

            // Grab a frame
            cv::Mat frame;
            detector >> frame;
            if (frame.empty()) {
                break;
            }
            detector.processFrame(frame);

            std::vector<cv::Rect> cv_faces = detector.faces();
            if (cv_faces.size() == num_faces) {
                // face_swapper.swapFaces(frame, cv_faces[0], frame, cv_faces[1]);
                face_swapper.swapFaces(frame, cv_faces[0], scaledInputImage, imageFaces[0]);
            }

            auto time_end = cv::getTickCount();
            auto time_per_frame = (time_end - time_start) / cv::getTickFrequency();

            fps = (15 * fps + (1 / time_per_frame)) / 16;

            printf("Total time: %3.5f | FPS: %3.2f\n", time_per_frame, fps);

            // Display it all on the screen
            cv::imshow("Face Swap", frame);

            if (cv::waitKey(1) == 27) return 0;
        }
    } catch (exception& e) {
        cout << e.what() << endl;
    }
}
