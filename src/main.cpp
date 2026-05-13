#include <Arduino.h>
#include <SPI.h>
#include <lvgl.h>
#include <esp_timer.h>

// ==========================
// PIN CONFIG (EDIT THESE)
// ==========================

#define PIN_CS 5
#define PIN_RST 17
#define PIN_HRDY 16 // HOSTHRDY
#define PIN_MOSI 23
#define PIN_MISO 19
#define PIN_SCK 18

// ==========================
// LVGL TICK
// ==========================

static uint32_t lv_tick(void)
{
  return esp_timer_get_time() / 1000;
}

// ==========================
// IT8951 LOW-LEVEL WRAPPER
// ==========================

class IT8951
{
public:
  void begin()
  {
    pinMode(PIN_CS, OUTPUT);
    pinMode(PIN_RST, OUTPUT);
    pinMode(PIN_HRDY, INPUT);

    digitalWrite(PIN_CS, HIGH);

    SPI.begin(PIN_SCK, PIN_MISO, PIN_MOSI, PIN_CS);

    reset();
    wait_ready();
  }

  void reset()
  {
    digitalWrite(PIN_RST, LOW);
    delay(50);
    digitalWrite(PIN_RST, HIGH);
    delay(200);
  }

  void wait_ready()
  {
    while (digitalRead(PIN_HRDY) == LOW)
    {
      delay(1);
    }
  }

  void write_command(uint16_t cmd)
  {
    wait_ready();

    digitalWrite(PIN_CS, LOW);

    SPI.transfer(cmd & 0xFF);
    SPI.transfer(cmd >> 8);

    digitalWrite(PIN_CS, HIGH);
  }

  void write_data(const uint8_t *data, size_t len)
  {
    wait_ready();

    digitalWrite(PIN_CS, LOW);

    for (size_t i = 0; i < len; i++)
    {
      SPI.transfer(data[i]);
    }

    digitalWrite(PIN_CS, HIGH);
  }

  // simplified framebuffer write (region)
  void write_image(int x, int y, int w, int h, const uint8_t *px)
  {
    // NOTE:
    // Real IT8951 uses "load image area" commands.
    // This is simplified structure only.

    write_command(0x0002); // (placeholder: LOAD_IMAGE_AREA)

    uint32_t header[5] = {
        (uint32_t)x,
        (uint32_t)y,
        (uint32_t)w,
        (uint32_t)h,
        0 // mode
    };

    write_data((uint8_t *)header, sizeof(header));

    write_data(px, w * h);
  }

  void refresh(int x, int y, int w, int h)
  {
    write_command(0x0004); // (placeholder: DISPLAY_AREA)

    uint32_t args[4] = {
        (uint32_t)x,
        (uint32_t)y,
        (uint32_t)w,
        (uint32_t)h};

    write_data((uint8_t *)args, sizeof(args));
  }
};

static IT8951 epd;

// ==========================
// LVGL FLUSH CALLBACK
// ==========================

static void flush_cb(lv_display_t *disp,
                     const lv_area_t *area,
                     uint8_t *px_map)
{
  int w = area->x2 - area->x1 + 1;
  int h = area->y2 - area->y1 + 1;

  epd.write_image(area->x1, area->y1, w, h, px_map);
  epd.refresh(area->x1, area->y1, w, h);

  lv_display_flush_ready(disp);
}

// ==========================
// SETUP
// ==========================

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