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

void VideoProcessor::processVideo(const Task& task)
{
    cv::VideoCapture* videoInput;
    if (task.videoPath == "camera") {
        videoInput = new cv::VideoCapture(0);
    } else {
        videoInput = new cv::VideoCapture(task.videoPath);
    }
    _detector->setInput(videoInput);

    auto inputImage = cv::imread(task.imagePath);
    cv::Mat scaledInputImage;
    cv::resize(inputImage, scaledInputImage, _detector->originalFrameSize());

    _imageDetector->setDimension(scaledInputImage.size());
    _imageDetector->processFrame(scaledInputImage);
    std::vector<cv::Rect> imageFaces = _imageDetector->faces();
    if (imageFaces.size() == 0) {
        throw std::runtime_error("could not detect profile image");
    }

    cv::VideoWriter outputVideo;
    auto size = cv::Size((int)videoInput->get(cv::CAP_PROP_FRAME_WIDTH),
                         (int)videoInput->get(cv::CAP_PROP_FRAME_HEIGHT));

    outputVideo.open(task.noAudioOutputPath, kFourcc, (int)videoInput->get(cv::CAP_PROP_FPS), size);

    int currentIndex = 0;
    bool thumbnailCaptured = false;

    while (true) {
        // Grab a frame
        cv::Mat frame;
        *_detector >> frame;

        if (frame.empty()) {
            break;
        }
        _detector->processFrame(frame);

        std::vector<cv::Rect> faces = _detector->faces();

        if (faces.size() > 0) {
            _swapper->swapFaces(frame, faces[0], scaledInputImage, imageFaces[0]);
        }

        if (currentIndex == 1) {
            imwrite(task.thumbnailPath, frame);
        }
        if (currentIndex >= 100 && !thumbnailCaptured && faces.size() > 0) {
            imwrite(task.thumbnailPath, frame);
            thumbnailCaptured = true;
        }

        if (!task.noAd) {
            drawText(frame, cv::Point(frame.size().width - 310, frame.size().height - 30), "Created with TvAR");
        }

        outputVideo << frame;

        currentIndex++;
        // // Display it all on the screen
        // cv::imshow("Face Swap", frame);
        // if (cv::waitKey(1) == 27) return;
    }
}
