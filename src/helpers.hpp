#pragma once

#include <string>
#include <vector>
#include <opencv2/opencv.hpp>

void drawText(cv::InputOutputArray &img, const cv::Point point, const std::string &text);
bool dirExists(const std::string &path);
std::string randomString(size_t len);
std::string mkTempDir(const std::string &prefix = "dir");
bool mkdirp(const std::string &path);

template<typename T>
float computeMean(std::vector<T> &array)
{
    float sum = 0.0;
    for (auto v: array) {
        sum += v;
    }
    return sum / array.size();
}

template<typename T>
float computeSquaredMean(std::vector<T> &array)
{
    float sum = 0.0;
    for (auto v: array) {
        sum += v * v;
    }
    return sum / array.size();
}

template<typename T>
float computeStandardDerivation(std::vector<T> &array)
{
    float mean = computeMean(array);
    float squaredMean = computeSquaredMean(array);
    return squaredMean - mean * mean;
}
