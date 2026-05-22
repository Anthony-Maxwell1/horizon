#include <platform/common/platform.h>
#include <chrono>
#include <sys/time.h>
#include <Arduino.h>

std::chrono::system_clock::time_point get_time()
{
    return std::chrono::system_clock::now();
}

void set_time(std::chrono::system_clock::time_point tp)
{
    auto duration = tp.time_since_epoch();

    auto secs = std::chrono::duration_cast<
        std::chrono::seconds>(duration);

    auto usecs = std::chrono::duration_cast<
        std::chrono::microseconds>(duration - secs);

    timeval tv;
    tv.tv_sec = secs.count();
    tv.tv_usec = usecs.count();

    settimeofday(&tv, nullptr);
}

// bool sync_time()
// {
//     configTime(
//         0,
//         0,
//         "pool.ntp.org",
//         "time.nist.gov");

//     struct tm timeinfo;

//     for (int i = 0; i < 20; i++)
//     {
//         if (getLocalTime(&timeinfo))
//         {
//             return true;
//         }

//         delay(500);
//     }

//     return false;
// }