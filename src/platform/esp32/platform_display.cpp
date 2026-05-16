#include <platform/common/platform.h>
#include <SPI.h>
#include <GxEPD2_BW.h>
#include <lvgl.h>

// ── Pin definitions ──────────────────────────────────────────────
#define EPD_CS 5
#define EPD_DC -1
#define EPD_RST 17
#define EPD_HRDY 4

// ── Display object ───────────────────────────────────────────────
GxEPD2_BW<GxEPD2_it103_1872x1404, GxEPD2_it103_1872x1404::HEIGHT>
    display(GxEPD2_it103_1872x1404(EPD_CS, EPD_DC, EPD_RST, EPD_HRDY));

// ── LVGL flush callback ──────────────────────────────────────────
void flush_cb(lv_display_t *disp,
              const lv_area_t *area,
              uint8_t *px_map)
{
    int32_t w = area->x2 - area->x1 + 1;
    int32_t h = area->y2 - area->y1 + 1;

    static uint8_t native_buf[1872 * 20];

    uint8_t *src = px_map;
    uint8_t *dst = native_buf;

    for (int32_t i = 0; i < w * h; i++)
    {
        uint8_t gray = src[i] >> 4;
        dst[i] = (gray << 4) | gray;
    }

    display.writeNative(native_buf, nullptr,
                        area->x1, area->y1,
                        w, h,
                        false, false, false);

    display.refresh();

    lv_display_flush_ready(disp);
}

// ── Platform display initialization ─────────────────────────────
void platform_init_display()
{
    lv_init();

    lv_display_t *disp = lv_display_create(1872, 1404);

    static uint8_t buf[1872 * 20];

    lv_display_set_buffers(
        disp,
        buf,
        nullptr,
        sizeof(buf),
        LV_DISPLAY_RENDER_MODE_PARTIAL);

    lv_display_set_flush_cb(disp, flush_cb);

    lv_display_set_color_format(disp, LV_COLOR_FORMAT_I1);
}