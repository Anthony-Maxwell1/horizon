#include <platform/common/platform.h>
#include <Arduino.h>
#include <lvgl.h>
#include <esp_timer.h>

static uint32_t lv_tick(void)
{
    return esp_timer_get_time() / 1000;
}

void platform_init()
{
    lv_init();
    lv_tick_set_cb(lv_tick);
    platform_init_display();
}