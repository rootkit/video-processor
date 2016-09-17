#include "worker.hpp"
#include "easylogging++.h"

#include <json.hpp>
#include <restclient-cpp/restclient.h>
#include <unistd.h>
#include <boost/filesystem.hpp>

#include "helpers.hpp"
#include "video_processor.hpp"

namespace {
    std::string kRedisHost   = "localhost";
    std::string kQueueName   = "kappa";
    std::string kS3Bucket    = "tatakau-kappa";
    std::string kAPIBasePath = "https://tvar.claudetech.com/videos/";
}

using json = nlohmann::json;

Worker::Worker()
{
    _rdx = new redox::Redox;
    if (!_rdx->connect(kRedisHost, 6379)) {
        throw std::runtime_error("could not connect to redis");
    }

    S3::initialize();

    S3::Config config;
    config.bucket = kS3Bucket;
    _s3Client = new S3::Client(config);
}

void Worker::work()
{
    LOG(INFO) << "starting to listen on queue " << kQueueName;
    while (true) {
        try {
            // block until we get something to do
            auto &c = _rdx->commandSync<std::vector<std::string>>({"BRPOP", kQueueName, "0"});
            if (c.ok()) {
                // TODO: use another thread or process instead of blocking here
                auto rawData = c.reply().back();
                LOG(INFO) << "starting to process task: " << rawData;
                auto task = Task::fromJSON(rawData);
                this->_processTask(task);
            } else {
                // TODO: proper error handling, but anyway we should not enter here
                LOG(ERROR) << "redis command failed";
                usleep(1000);
            }
        } catch (std::exception &e) {
            // TODO: notify the API
            LOG(ERROR) << "could not process: " << e.what();
        }
    }
}

void Worker::_processTask(const Task& task)
{
    auto timeStart = cv::getTickCount();

    auto outputDir = mkTempDir("tvar-data");
    auto videoPath = outputDir + "/video.mp4";
    auto imagePath = outputDir + "/image.jpg";
    auto outputPath = outputDir + "/output.mp4";
    auto outputKey = "/processed/" + std::to_string(task.videoID) + "/video.mp4";
    auto thumbnailKey = "/processed/" + std::to_string(task.videoID) + "/thumbnail.jpg";

    _s3Client->downloadFile(task.s3VideoKey, videoPath);
    _s3Client->downloadFile(task.s3ImageKey, imagePath);

    LOG(INFO) << "downloaded: " << task.s3VideoKey << " and " << task.s3ImageKey;

    VideoProcessor processor;
    processor.processVideo(videoPath, imagePath, outputPath);
    LOG(INFO) << "process done, uploading to " << outputKey;
    _s3Client->uploadFile(outputPath, outputKey);

    auto url = kAPIBasePath + std::to_string(task.videoID);
    json j = {{"swapped_uid", outputKey}, {"thumbnail_uid", thumbnailKey}};
    LOG(INFO) << "posting result " << j.dump();
    RestClient::post(url, "application/json", j.dump());

    LOG(DEBUG) << "cleaning up";
    boost::filesystem::remove_all(outputDir);

    auto spentTime = (cv::getTickCount() - timeStart) / cv::getTickFrequency();
    LOG(INFO) << "processed in " << spentTime << "s";
}
