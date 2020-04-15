#pragma once

#include "core_hal.h"
#include <cstdint>

class HeapInfo {
private:
    runtime_info_t info;

public:
    HeapInfo();
    ~HeapInfo() = default;

    void update();
    void print(char* dest, uint8_t maxLen); // also updates
};