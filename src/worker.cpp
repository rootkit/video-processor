#include "worker.hpp"
#include "easylogging++.h"

#include <json.hpp>
#include <restclient-cpp/restclient.h>
#include <unistd.h>
#include <boost/filesystem.hpp>

#include "config.hpp"

#include "helpers.hpp"
#include "video_processor.hpp"

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
        // block until we get something to do
        auto &c = _rdx->commandSync<std::vector<std::string>>({"BRPOP", kQueueName, "0"});
        if (c.ok()) {
            // TODO: use another thread or process instead of blocking here
            auto rawData = c.reply().back();
            LOG(INFO) << "starting to process task: " << rawData;
            auto task = Task::fromJSON(rawData);
            try {
                this->_processTask(task);
            } catch (std::exception &e) {
                LOG(ERROR) << "could not process: " << e.what();
                auto url = kAPIBasePath + std::to_string(task.videoID);
                RestClient::del(url);
            }
        } else {
            // TODO: proper error handling, but anyway we should not enter here
            LOG(ERROR) << "redis command failed";
            usleep(1000);
        }
    }
}

void Worker::_processTask(Task& task)
{
    auto timeStart = cv::getTickCount();

    auto outputDir = mkTempDir("tvar-data");
    task.setDirectory(outputDir);

    _s3Client->downloadFile(task.s3VideoKey, task.videoPath);
    _s3Client->downloadFile(task.s3ImageKey, task.imagePath);

    LOG(INFO) << "downloaded: " << task.s3VideoKey << " and " << task.s3ImageKey;

    VideoProcessor processor;
    processor.processVideo(task);

    std::ostringstream command;
    command << "ffmpeg -i "
            << task.noAudioOutputPath
            << " -i "
            << task.videoPath
            << " -c:v copy -c:a aac -strict experimental -shortest "
            << task.outputPath;
    system((char*)command.str().c_str());

    LOG(INFO) << "process done, uploading to " << task.outputKey << "and " << task.thumbnailKey;
    _s3Client->uploadFile(task.outputPath, task.outputKey, "video/mp4");
    _s3Client->uploadFile(task.thumbnailPath, task.thumbnailKey, "image/jpeg");

    auto url = kAPIBasePath + std::to_string(task.videoID);
    json j = {{"swapped_uid", task.outputKey}, {"thumbnail_uid", task.thumbnailKey}};
    LOG(INFO) << "posting result " << j.dump();
    RestClient::put(url, "application/json", j.dump());

    LOG(DEBUG) << "cleaning up";
    boost::filesystem::remove_all(outputDir);

    auto spentTime = (cv::getTickCount() - timeStart) / cv::getTickFrequency();
    LOG(INFO) << "processed in " << spentTime << "s";
}
