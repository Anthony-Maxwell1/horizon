#include <platform/common/platform.h>
#include <chrono>

std::chrono::system_clock::time_point get_time()
{
    return std::chrono::system_clock::now();
}

void set_time(std::chrono::system_clock::time_point t)
{
    // no-op on desktop
}

bool sync_time()
{
    // no-op on desktop
    return true;
}