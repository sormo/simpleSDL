#pragma once
#include <cstdint>

bool Text2DInitFont(const char * path);
void Text2DPrint(const char * text, int32_t x, int32_t y, int32_t size);
void Text2DCleanup();
