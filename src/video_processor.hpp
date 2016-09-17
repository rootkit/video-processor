#include <string>

#include "face_detector.hpp"
#include "face_swapper.hpp"

class VideoProcessor
{
public:
    VideoProcessor();
    ~VideoProcessor();
    void processVideo(std::string videoPath, std::string imagePath, std::string outputPath);

private:
    FaceDetectorAndTracker* _detector;
    FaceDetectorAndTracker* _imageDetector;
    FaceSwapper* _swapper;
};
