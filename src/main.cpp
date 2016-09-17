#include "worker.hpp"
#include "easylogging++.h"

INITIALIZE_EASYLOGGINGPP

int main()
{
    Worker worker;
    worker.work();

    return 0;
}
