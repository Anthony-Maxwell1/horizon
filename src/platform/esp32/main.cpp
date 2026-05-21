#include <platform/common/platform.h>
#include <app/app_entry.h>
#include <Arduino.h>
#include <lvgl.h>

void setup()
{
  platform_init();
  platform_init_display();
  platform_init_storage();
  platform_init_input();
  app_entrypoint();
}

// ==========================
// LOOP
// ==========================

void loop()
{
  lv_timer_handler();
  app_loop();
  platform_delay(5);
}