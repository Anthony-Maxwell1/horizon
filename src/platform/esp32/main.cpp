#include <platform/common/platform.h>
#include <Arduino.h>
#include <lvgl.h>

void setup()
{
  platform_init();
  platform_init_display();
  platform_init_storage();
  platform_init_input();
}

// ==========================
// LOOP
// ==========================

void loop()
{
  lv_timer_handler();
  platform_delay(5);
}