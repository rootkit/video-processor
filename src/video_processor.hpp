#pragma once

#include <string>

#include "face_detector.hpp"
#include "face_swapper.hpp"
#include "task.hpp"

class VideoProcessor
{
public:
    VideoProcessor();
    ~VideoProcessor();
    void processVideo(const Task& task);

private:
    FaceDetectorAndTracker* _detector;
    FaceDetectorAndTracker* _imageDetector;
    FaceSwapper* _swapper;
};
