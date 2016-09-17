#include "helpers.hpp"

#include <sys/stat.h>

static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";

namespace {
    const std::string kTmpDir = "/tmp";
}

void drawText(cv::InputOutputArray &img, const cv::Point point, const std::string &text)
{
    auto bPoint = cv::Point(point.x + 1, point.y + 1);
    cv::putText(img, text, bPoint, cv::FONT_HERSHEY_PLAIN, 2.0, cv::Scalar(0, 0, 0), 2, cv::LINE_AA);
    cv::putText(img, text, point, cv::FONT_HERSHEY_PLAIN, 2.0, cv::Scalar(255, 255, 255), 2, cv::LINE_AA);
}

bool dirExists(const std::string &path)
{
    struct stat st;
    return stat((char*)path.c_str(), &st) == 0 && (st.st_mode & S_IFDIR) != 0;
}

std::string randomString(size_t len)
{
    char* s = new char[len + 1];
    for (size_t i = 0; i < len; ++i) {
        s[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    s[len] = 0;
    return std::string(s);
}

std::string mkTempDir(const std::string &prefix)
{
    while (true) {
        std::ostringstream dirStream;
        dirStream << kTmpDir << "/" << prefix << "-" << randomString(6);
        if (!dirExists(dirStream.str())) {
            mkdir((char*)dirStream.str().c_str(), 0755);
            return dirStream.str();
        }
    }
}

bool mkdirp(const std::string &path)
{
    return dirExists(path) || mkdir((char*)path.c_str(), 0755) == 0;
}
