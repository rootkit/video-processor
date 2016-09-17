#include <redox.hpp>
#include <string>

#include "task.hpp"
#include "s3.hpp"

class Worker
{
public:
    Worker();
    void work();
private:
    void _processTask(const Task& task);
    redox::Redox *_rdx;
    S3::Client* _s3Client;
};
