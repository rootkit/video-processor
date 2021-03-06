#include "task.hpp"

#include <json.hpp>

using json = nlohmann::json;

Task::Task(int videoID, std::string s3VideoKey, std::string s3ImageKey, bool noAd)
{
    this->videoID = videoID;
    this->s3VideoKey = s3VideoKey;
    this->s3ImageKey = s3ImageKey;
    this->noAd = noAd;

    if (noAd) {
        outputKey = "/processed/" + std::to_string(videoID) + "/video-adfree.mp4";
    } else {
        outputKey = "/processed/" + std::to_string(videoID) + "/video.mp4";
    }
    thumbnailKey = "/processed/" + std::to_string(videoID) + "/thumbnail.jpg";
}

Task Task::fromJSON(std::string rawJSON)
{
    auto data = json::parse(rawJSON);
    auto videoID = data["video_id"].get<int>();
    auto videoS3Key = data["video_uid"].get<std::string>();
    auto imageS3Key = data["image_uid"].get<std::string>();
    auto noAd = false;
    if (data.count("no_ad") > 0) {
        noAd = data["no_ad"].get<bool>();
    }
    return Task(videoID, videoS3Key, imageS3Key, noAd);
}


void Task::setDirectory(std::string dir)
{
    directory = dir;
    videoPath = directory + "/video.mp4";
    imagePath = directory + "/image.jpg";
    noAudioOutputPath = directory + "/output-no-audio.mp4";
    outputPath = directory + "/output.mp4";
    thumbnailPath = directory + "/thumbnail.jpg";
}
