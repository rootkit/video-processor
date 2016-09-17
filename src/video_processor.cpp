#include "video_processor.hpp"

#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>

#include "helpers.hpp"

namespace {
    const std::string kNoSoundOutputFileName   = "output-no-sound.mp4";
    const std::string kFinalOutputFileName     = "output.mp4";
    const auto kFourcc                         = cv::VideoWriter::fourcc('X','2','6','4');
}

VideoProcessor::VideoProcessor()
{
    _detector = new FaceDetectorAndTracker("./data/haarcascade_frontalface_default.xml", 1);
    _imageDetector = new FaceDetectorAndTracker("./data/haarcascade_frontalface_default.xml", 1);
    _swapper = new FaceSwapper("./data/shape_predictor_68_face_landmarks.dat");
}

VideoProcessor::~VideoProcessor()
{
    delete _detector;
    delete _imageDetector;
    delete _swapper;
}

void VideoProcessor::processVideo(std::string videoPath, std::string imagePath, std::string outputPath, std::string thumbnailPath)
{
    cv::VideoCapture* videoInput;
    if (videoPath == "camera") {
        videoInput = new cv::VideoCapture(0);
    } else {
        videoInput = new cv::VideoCapture(videoPath);
    }
    _detector->setInput(videoInput);

    auto inputImage = cv::imread(imagePath);
    cv::Mat scaledInputImage;
    cv::resize(inputImage, scaledInputImage, _detector->originalFrameSize());

    _imageDetector->setDimension(scaledInputImage.size());
    _imageDetector->processFrame(scaledInputImage);
    std::vector<cv::Rect> imageFaces = _imageDetector->faces();

    cv::VideoWriter outputVideo;
    auto size = cv::Size((int)videoInput->get(cv::CAP_PROP_FRAME_WIDTH),
                         (int)videoInput->get(cv::CAP_PROP_FRAME_HEIGHT));

    outputVideo.open(outputPath, kFourcc, (int)videoInput->get(cv::CAP_PROP_FPS), size);

    int currentIndex = 0;

    while (true) {
        // Grab a frame
        cv::Mat frame;
        *_detector >> frame;


        if (frame.empty()) {
            break;
        }
        _detector->processFrame(frame);

        if (currentIndex == 1 || currentIndex == 100) {
            imwrite(thumbnailPath, frame);
        }

        std::vector<cv::Rect> faces = _detector->faces();
        if (faces.size() > 0) {
            _swapper->swapFaces(frame, faces[0], scaledInputImage, imageFaces[0]);
        }

        outputVideo << frame;

        currentIndex++;
        // // Display it all on the screen
        // cv::imshow("Face Swap", frame);
        // if (cv::waitKey(1) == 27) return;
    }
}
