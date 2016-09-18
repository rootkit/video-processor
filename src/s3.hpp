#pragma once

#include <string>

#include <aws/s3/S3Client.h>

namespace S3
{
    void initialize();

    class Config
    {
    public:
        std::string bucket;
        const char* region = Aws::Region::AP_NORTHEAST_1;
    };

    class Client
    {
    public:
        Client(Config config);

        void uploadFile(const std::string& filePath, const std::string& uploadKey, const char* contentType);
        void downloadFile(const std::string& key, const std::string& outputPath);

    private:
        Config _config;
        Aws::S3::S3Client* _client;
    };
}
