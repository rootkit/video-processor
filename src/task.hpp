#pragma once

#include <string>

class Task
{
public:
    Task(int videoID, std::string s3VideoKey, std::string s3ImageKey, bool noAd);
    int videoID;
    std::string s3VideoKey;
    std::string s3ImageKey;

    std::string directory;
    std::string videoPath;
    std::string noAudioOutputPath;
    std::string imagePath;
    std::string outputPath;
    std::string thumbnailPath;

    std::string outputKey;
    std::string thumbnailKey;
    bool noAd = false;

    void setDirectory(std::string directory);

    static Task fromJSON(std::string rawJSON);
};
