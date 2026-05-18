#pragma once
#include <cstdint>

void platform_init();
void platform_init_display();
void platform_init_storage();
void platform_init_input();
void platform_delay(uint32_t ms);