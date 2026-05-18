#include <platform/common/platform.h>
#include <SPI.h>
#include <GxEPD2_BW.h>
#include <lvgl.h>

// ── Pin definitions ──────────────────────────────────────────────
#define EPD_CS 5
#define EPD_DC -1
#define EPD_RST 17
#define EPD_HRDY 4

namespace
{
constexpr int kDisplayWidth = 1872;
constexpr int kDisplayHeight = 1404;
constexpr int kPageLines = 20;
constexpr size_t kNativeBufferSize = static_cast<size_t>(kDisplayWidth) * kPageLines;

void *allocate_display_buffer(size_t size)
{
    void *buffer = heap_caps_malloc(size, MALLOC_CAP_SPIRAM | MALLOC_CAP_8BIT);
    if (buffer == nullptr) {
        buffer = heap_caps_malloc(size, MALLOC_CAP_INTERNAL | MALLOC_CAP_8BIT);
    }

    assert(buffer != nullptr);
    return buffer;
}
}

// ── Display object ───────────────────────────────────────────────
GxEPD2_BW<GxEPD2_it103_1872x1404, kPageLines>
    display(GxEPD2_it103_1872x1404(EPD_CS, EPD_DC, EPD_RST, EPD_HRDY));

// ── LVGL flush callback ──────────────────────────────────────────
void flush_cb(lv_display_t *disp,
              const lv_area_t *area,
              uint8_t *px_map)
{
    static uint8_t *native_buf = nullptr;
    if (native_buf == nullptr) {
        native_buf = (uint8_t *)allocate_display_buffer(kNativeBufferSize);
    }

    int32_t w = area->x2 - area->x1 + 1;
    int32_t h = area->y2 - area->y1 + 1;

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

    lv_display_t *disp = lv_display_create(kDisplayWidth, kDisplayHeight);

    static lv_color_t *buf = nullptr;

    buf = (lv_color_t *)allocate_display_buffer(kNativeBufferSize * sizeof(lv_color_t));

    lv_display_set_buffers(
        disp,
        buf,
        nullptr,
        kNativeBufferSize * sizeof(lv_color_t),
        LV_DISPLAY_RENDER_MODE_PARTIAL);

    lv_display_set_flush_cb(disp, flush_cb);

    lv_display_set_color_format(disp, LV_COLOR_FORMAT_I1);
}