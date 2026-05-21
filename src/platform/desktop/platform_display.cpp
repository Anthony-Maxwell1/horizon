#include <platform/common/platform.h>

#include <SDL2/SDL.h>
#include <lvgl.h>

static SDL_Window   *window   = nullptr;
static SDL_Renderer *renderer = nullptr;
static SDL_Texture  *texture  = nullptr;

#define SCREEN_W 1404
#define SCREEN_H 1872
#define DISPLAY_SCALE 3

static void flush_cb(lv_display_t *disp,
                     const lv_area_t *area,
                     uint8_t *px_map)
{
    int x = area->x1;
    int y = area->y1;
    int w = area->x2 - area->x1 + 1;
    int h = area->y2 - area->y1 + 1;

    if (w <= 0 || h <= 0) {
        lv_display_flush_ready(disp);
        return;
    }

    SDL_Rect rect{ x, y, w, h };
    int pitch = SCREEN_W * 4;

    SDL_UpdateTexture(texture, &rect, px_map, pitch);

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);

    lv_display_flush_ready(disp);
}

void platform_init_display()
{
    SDL_SetHint(SDL_HINT_WINDOWS_DPI_AWARENESS, "permonitor");
    SDL_SetHint(SDL_HINT_WINDOWS_DPI_SCALING, "0");
    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(
        "Horizon (LVGL Desktop)",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        SCREEN_W / DISPLAY_SCALE,
        SCREEN_H / DISPLAY_SCALE,
        SDL_WINDOW_SHOWN);

    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    // Full resolution texture, SDL scales it down to the window size
    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        SCREEN_W,
        SCREEN_H);

    // Tell SDL to scale the full-res texture down to fit the window
    SDL_RenderSetLogicalSize(renderer, SCREEN_W, SCREEN_H);

    lv_display_t *disp = lv_display_create(SCREEN_W, SCREEN_H);

    static constexpr size_t buf_size = SCREEN_W * 40;
    static lv_color_t buf1[buf_size];

    lv_display_set_buffers(
        disp,
        buf1,
        nullptr,
        sizeof(buf1),
        LV_DISPLAY_RENDER_MODE_PARTIAL);

    lv_display_set_flush_cb(disp, flush_cb);
    lv_display_set_color_format(disp, LV_COLOR_FORMAT_ARGB8888);
}