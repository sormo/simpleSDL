#pragma once
#include <cinttypes>
#include <stddef.h>

namespace Generator
{
    const char * const * GetNames();
    size_t GetCount();
    void Changed(int32_t index);
}
