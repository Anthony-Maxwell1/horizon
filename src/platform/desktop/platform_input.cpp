#include <platform/common/platform.h>

#include <lvgl.h>
#include <SDL2/SDL.h>
#include <vector>

#define DISPLAY_SCALE 3
#define SCREEN_W 1404
#define SCREEN_H 1872

static lv_indev_t *g_mouse_indev = NULL;
static int g_last_x = 0;
static int g_last_y = 0;
static bool g_pressed = false;
static void (*g_swipe_cb)(const char *dir) = nullptr;
static int g_press_start_x = 0;
static int g_press_start_y = 0;

static std::vector<void (*)(const char *dir)> g_swipe_callbacks;

void register_swipe_callback(void (*cb)(const char *dir))
{
    g_swipe_callbacks.push_back(cb);
}

static void sdl_lv_read_cb(lv_indev_t *indev, lv_indev_data_t *data)
{
    int x = g_last_x;
    int y = g_last_y;
    if (x < 0)
        x = 0;
    if (y < 0)
        y = 0;
    if (x >= SCREEN_W)
        x = SCREEN_W - 1;
    if (y >= SCREEN_H)
        y = SCREEN_H - 1;
    data->point.x = (lv_coord_t)x;
    data->point.y = (lv_coord_t)y;
    data->state = g_pressed ? LV_INDEV_STATE_PRESSED : LV_INDEV_STATE_RELEASED;
}

void platform_init_input()
{
    g_mouse_indev = lv_indev_create();
    lv_indev_set_type(g_mouse_indev, LV_INDEV_TYPE_POINTER);
    lv_indev_set_read_cb(g_mouse_indev, sdl_lv_read_cb);
    lv_indev_set_display(g_mouse_indev, lv_display_get_default());
    lv_indev_set_mode(g_mouse_indev, LV_INDEV_MODE_TIMER);
    lv_timer_set_period(lv_indev_get_read_timer(g_mouse_indev), 1);
}

bool platform_poll_events()
{
    SDL_Event e;
    while (SDL_PollEvent(&e))
    {
        if (e.type == SDL_QUIT)
            return true;

        switch (e.type)
        {
        case SDL_MOUSEMOTION:
            g_last_x = e.motion.x;
            g_last_y = e.motion.y;
            break;
        case SDL_MOUSEBUTTONDOWN:
            if (e.button.button == SDL_BUTTON_LEFT)
            {
                g_last_x = e.button.x;
                g_last_y = e.button.y;
                g_press_start_x = g_last_x;
                g_press_start_y = g_last_y;
                g_pressed = true;
            }
            break;
        case SDL_MOUSEBUTTONUP:
            if (e.button.button == SDL_BUTTON_LEFT)
            {
                g_last_x = e.button.x;
                g_last_y = e.button.y;
                g_pressed = false;

                int dx = g_last_x - g_press_start_x;
                int dy = g_last_y - g_press_start_y;
                int adx = dx < 0 ? -dx : dx;
                int ady = dy < 0 ? -dy : dy;
                if (adx > 30 || ady > 30)
                {
                    const char *dir;
                    if (adx >= ady)
                        dir = dx < 0 ? "left" : "right";
                    else
                        dir = dy < 0 ? "up" : "down";
                    for (auto cb : g_swipe_callbacks)
                        cb(dir);
                }
            }
            break;
        default:
            break;
        }
    }

    return false;
}