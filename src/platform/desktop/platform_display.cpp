#include <platform/common/platform.h>

#include <SDL2/SDL.h>
#include <lvgl.h>

static SDL_Window *window = nullptr;
static SDL_Renderer *renderer = nullptr;
static SDL_Texture *texture = nullptr;

static int screen_w = 1872;
static int screen_h = 1404;

static void flush_cb(lv_display_t *disp,
                     const lv_area_t *area,
                     uint8_t *px_map)
{
    int w = area->x2 - area->x1 + 1;
    int h = area->y2 - area->y1 + 1;

    SDL_UpdateTexture(texture, nullptr, px_map, screen_w);

    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, nullptr, nullptr);
    SDL_RenderPresent(renderer);

    lv_display_flush_ready(disp);
}

void platform_init_display()
{
    // ── SDL init ─────────────────────────────────────
    SDL_Init(SDL_INIT_VIDEO);

    window = SDL_CreateWindow(
        "Horizon (LVGL Desktop)",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        screen_w,
        screen_h,
        SDL_WINDOW_SHOWN);

    renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    texture = SDL_CreateTexture(
        renderer,
        SDL_PIXELFORMAT_ARGB8888,
        SDL_TEXTUREACCESS_STREAMING,
        screen_w,
        screen_h);

    // ── LVGL display setup ───────────────────────────
    lv_display_t *disp = lv_display_create(screen_w, screen_h);

    static lv_color_t buf1[screen_w * 40]; // partial buffer

    lv_display_set_buffers(
        disp,
        buf1,
        nullptr,
        sizeof(buf1),
        LV_DISPLAY_RENDER_MODE_PARTIAL);

    lv_display_set_flush_cb(disp, flush_cb);

    lv_display_set_color_format(disp, LV_COLOR_FORMAT_ARGB8888);
}