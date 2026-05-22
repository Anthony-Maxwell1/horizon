#pragma once
#include <cstdint>
#include <chrono>

void platform_init();
void platform_init_display();
void platform_init_storage();
void platform_init_input();
void platform_delay(uint32_t ms);
// Process platform-specific events (returns true to request exit)
bool platform_poll_events();

// touch
void register_swipe_callback(void (*cb)(const char *dir));

std::chrono::system_clock::time_point get_time();
void set_time(std::chrono::system_clock::time_point t);
bool sync_time();