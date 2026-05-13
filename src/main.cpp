#include <lvgl.h>

void setup() {
  lv_init();
  // Initialize your display and input drivers here...
}

void loop() {
  lv_timer_handler(); // Update LVGL timers
  // delay(5);
}