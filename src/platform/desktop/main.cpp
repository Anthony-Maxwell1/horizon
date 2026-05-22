#include <platform/common/platform.h>
#include <lvgl.h>
#include <SDL2/SDL.h>

#ifdef _WIN32
#include <windows.h>
#endif

#include <app/app_entry.h>

static int desktop_main_loop()
{
    platform_init();
    platform_init_display();
    platform_init_storage();
    platform_init_input();

    // Create application UI
    app_entrypoint();

    while (true)
    {
        // Process platform events (SDL -> LVGL input, quit handling)
        if (platform_poll_events())
            return 0;

        lv_timer_handler();
        app_loop();
        platform_delay(1);
    }

    return 0;
}

int main()
{
    return desktop_main_loop();
}

#ifdef _WIN32
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
    return desktop_main_loop();
}
#endif