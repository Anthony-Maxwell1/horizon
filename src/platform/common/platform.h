#pragma once
#include <cstdint>

void platform_init();
void platform_init_display();
void platform_init_storage();
void platform_init_input();
void platform_delay(uint32_t ms);
// Process platform-specific events (returns true to request exit)
bool platform_poll_events();

// touch
void register_swipe_callback(void (*cb)(const char *dir));