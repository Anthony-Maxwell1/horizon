#include <platform/common/platform.h>

#include <lvgl.h>
#include <SDL2/SDL.h>

// Match display scaling in platform_display.cpp
#define DISPLAY_SCALE 2

// Global input device for event-driven processing
static lv_indev_t *g_mouse_indev = NULL;

static void sdl_lv_read_cb(lv_indev_t *indev, lv_indev_data_t *data)
{
    int mouse_x = 0;
    int mouse_y = 0;
    Uint32 buttons = SDL_GetMouseState(&mouse_x, &mouse_y);

    data->point.x = mouse_x * DISPLAY_SCALE;
    data->point.y = mouse_y * DISPLAY_SCALE;
    data->state = (buttons & SDL_BUTTON_LMASK) ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
}

void platform_init_input()
{
    g_mouse_indev = lv_indev_create();
    lv_indev_set_type(g_mouse_indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(g_mouse_indev, sdl_lv_read_cb);
    lv_indev_set_display(g_mouse_indev, lv_display_get_default());
    // Use EVENT mode for immediate click responsiveness
    lv_indev_set_mode(g_mouse_indev, LV_INDEV_MODE_EVENT);
}

// Feed mouse event to LVGL immediately when SDL event occurs
void platform_feed_mouse_event()
{
    if (g_mouse_indev != NULL) {
        lv_indev_read(g_mouse_indev);
    }
}

bool platform_poll_events()
{
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_QUIT)
            return true;
        
        // Feed mouse button events immediately to LVGL for responsive clicks
        if (e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP) {
            platform_feed_mouse_event();
        }
    }

    return false;
}
