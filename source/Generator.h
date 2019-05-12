#pragma once
#include <cinttypes>

namespace Generator
{
    const char * const * GetNames();
    size_t GetCount();
    void Changed(int32_t index);
}
