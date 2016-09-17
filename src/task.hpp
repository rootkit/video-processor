#include <string>

class Task
{
public:
    Task(int videoID, std::string s3VideoKey, std::string s3ImageKey);
    int videoID;
    std::string s3VideoKey;
    std::string s3ImageKey;

    static Task fromJSON(std::string rawJSON);
};
