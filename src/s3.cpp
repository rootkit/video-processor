#include "s3.hpp"

#include <aws/core/Aws.h>
#include <aws/s3/S3Client.h>
#include <aws/s3/model/GetObjectRequest.h>
#include <aws/s3/model/PutObjectRequest.h>
#include <fstream>

#include "helpers.hpp"

namespace S3
{

    void initialize()
    {
        Aws::SDKOptions options;
        options.loggingOptions.logLevel = Aws::Utils::Logging::LogLevel::Info;
        Aws::InitAPI(options);
    }

    Client::Client(Config config)
    {
        Aws::Client::ClientConfiguration conf;
        conf.region = config.region;

        _client = new Aws::S3::S3Client(conf);
        _config = config;
    }

    void Client::downloadFile(const std::string &key, const std::string& outputPath)
    {
        Aws::S3::Model::GetObjectRequest getObjectRequest;

        getObjectRequest
            .WithBucket(_config.bucket.c_str())
            .WithKey(key.c_str());

        auto getObjectOutcome = _client->GetObject(getObjectRequest);

        if (!getObjectOutcome.IsSuccess()) {
            throw std::runtime_error(getObjectOutcome.GetError().GetMessage().c_str());
        }
        std::ofstream outputFile;
        outputFile.open(outputPath);
        outputFile << getObjectOutcome.GetResult().GetBody().rdbuf();
        outputFile.close();
    }

    void Client::uploadFile(const std::string &filePath, const std::string &uploadKey)
    {
        Aws::S3::Model::PutObjectRequest putObjectRequest;
        putObjectRequest
            .WithBucket(_config.bucket.c_str())
            .WithKey(uploadKey.c_str());

        std::ifstream inputFile;
        inputFile.open(filePath.c_str());
        auto requestStream = Aws::MakeShared<Aws::StringStream>("file-data");
        *requestStream << inputFile.rdbuf();
        inputFile.close();
        putObjectRequest.SetBody(requestStream);
        auto putObjectOutcome = _client->PutObject(putObjectRequest);
        if (!putObjectOutcome.IsSuccess()) {
            throw std::runtime_error(putObjectOutcome.GetError().GetMessage().c_str());
        }
    }
}
