#include <thread>
#include <chrono>

void platform_delay(uint32_t ms)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}