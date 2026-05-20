#pragma once
#include <cstdint>

void platform_init();
void platform_init_display();
void platform_init_storage();
void platform_init_input();
void platform_delay(uint32_t ms);
// Process platform-specific events (returns true to request exit)
bool platform_poll_events();
// Feed mouse event to LVGL immediately for event-driven responsiveness
void platform_feed_mouse_event();