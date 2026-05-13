#include <Arduino.h>
#include <SPI.h>
#include <lvgl.h>
#include <esp_timer.h>

static uint32_t lv_tick(void)
{
  return esp_timer_get_time() / 1000;
}

#include <SPI.h>
#include <GxEPD2_BW.h>
#include <lvgl.h>

// ── Pin definitions ──────────────────────────────────────────────
#define EPD_CS 5
#define EPD_DC -1 // IT8951 does NOT use a DC pin
#define EPD_RST 17
#define EPD_HRDY 4 // HRDY = BUSY equivalent

// ── VCOM voltage ─────────────────────────────────────────────────
// Check the sticker on your panel, e.g. -2.33V → 2330
#define VCOM_MV 2330

// ── Display object ───────────────────────────────────────────────
// GxEPD2_BW<driver, max_height>(cs, dc, rst, busy)
GxEPD2_BW<GxEPD2_it103_1872x1404, GxEPD2_it103_1872x1404::HEIGHT>
    display(EPD_CS, EPD_DC, EPD_RST, EPD_HRDY);

// ── LVGL draw buffer ─────────────────────────────────────────────
// 1/10th screen = 1872*1404/10 pixels. For 16-bit color that's ~525 KB.
// If RAM is tight, reduce to 1/20th or use LV_COLOR_DEPTH 4 directly.
static const int BUF_LINES = 14; // 14 lines at a time
static lv_color_t lvgl_buf[1872 * BUF_LINES];
static lv_disp_draw_buf_t draw_buf;
static lv_disp_drv_t disp_drv;

// ── LVGL flush callback ──────────────────────────────────────────
void epaper_flush(lv_disp_drv_t *drv, const lv_area_t *area, lv_color_t *color_p)
{
  int32_t w = area->x2 - area->x1 + 1;
  int32_t h = area->y2 - area->y1 + 1;

  // Build a 4bpp grayscale buffer for GxEPD2 writeNative.
  // LVGL 16-bit color: top 5 bits = red. Map top 4 bits → 0..15 gray.
  // GxEPD2 IT8951 native = 1 byte per pixel, 0x00=black 0xFF=white.
  static uint8_t native_buf[1872 * BUF_LINES];
  lv_color_t *src = color_p;
  uint8_t *dst = native_buf;

  for (int32_t y = 0; y < h; y++)
  {
    for (int32_t x = 0; x < w; x++)
    {
      // Extract 4-bit gray from 16-bit color (top 4 bits of red channel)
      uint8_t gray4 = (src->ch.red >> 1) & 0x0F; // 0..15
      *dst++ = gray4 << 4;                       // scale to 0x00..0xF0 (IT8951 4bpp range)
      src++;
    }
  }

  display.writeNative(native_buf, nullptr,
                      area->x1, area->y1, w, h,
                      false, false, false);

  // Trigger a display refresh only on the last chunk
  if (lv_disp_flush_is_last(drv))
  {
    display.refresh(area->x1, area->y1,
                    area->x2 - area->x1 + 1,
                    area->y2 - area->y1 + 1,
                    true); // true = partial refresh
  }

  lv_disp_flush_ready(drv);
}

void setup()
{
  Serial.begin(115200);

  lv_init();
  lv_tick_set_cb(lv_tick);

  epd.begin();

  // 10.3" typical resolution
  lv_display_t *disp = lv_display_create(1872, 1404);

  static uint8_t buf[1872 * 20]; // small line buffer

  lv_display_set_buffers(
      disp,
      buf,
      nullptr,
      sizeof(buf),
      LV_DISPLAY_RENDER_MODE_PARTIAL);

  lv_display_set_flush_cb(disp, flush_cb);
  lv_display_set_color_format(disp, LV_COLOR_FORMAT_I1);

  lv_obj_t *label = lv_label_create(lv_screen_active());
  lv_label_set_text(label, "IT8951 raw SPI OK");
  lv_obj_center(label);
}

// ==========================
// LOOP
// ==========================

void loop()
{
  lv_timer_handler();
  delay(5);
}