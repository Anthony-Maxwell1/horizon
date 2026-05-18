#include <platform/common/platform.h>
#include <lvgl.h>

int main()
{
    platform_init();
    platform_init_display();
    platform_init_storage();
    platform_init_input();

    while (true)
    {
        lv_timer_handler();
        platform_delay(5);
    }

    return 0;
}