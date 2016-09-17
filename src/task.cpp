#include "task.hpp"

#include <json.hpp>

using json = nlohmann::json;

Task::Task(int videoID, std::string s3VideoKey, std::string s3ImageKey)
{
    this->videoID = videoID;
    this->s3VideoKey = s3VideoKey;
    this->s3ImageKey = s3ImageKey;
}

Task Task::fromJSON(std::string rawJSON)
{
    auto data = json::parse(rawJSON);
    auto videoID = data["video_id"].get<int>();
    auto videoS3Key = data["video_uid"].get<std::string>();
    auto imageS3Key = data["image_uid"].get<std::string>();
    return Task(videoID, videoS3Key, imageS3Key);
}
