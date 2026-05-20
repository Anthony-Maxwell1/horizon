#include <platform/common/platform.h>
#include <lvgl.h>

#include <chrono>

// LVGL tick using std::chrono
static uint32_t lv_tick(void)
{
    using namespace std::chrono;

    static auto start = steady_clock::now();
    auto now = steady_clock::now();

    return duration_cast<milliseconds>(now - start).count();
}

void platform_init()
{
    lv_init();
    lv_tick_set_cb(lv_tick);
}