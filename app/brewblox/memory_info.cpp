#include "memory_info.h"
#include <cstdint>
#include <stdio.h>

HeapInfo::HeapInfo()
    : info({0})
{
    info.size = sizeof(info);
}

void
HeapInfo::update()
{
    HAL_Core_Runtime_Info(&info, NULL);
}

void
HeapInfo::print(char* dest, uint8_t maxLen)
{
    update();
    uint8_t freePct = info.total_heap ? (100 * info.freeheap) / info.total_heap : 0;
    uint8_t usedPct = 100 - freePct;
    uint8_t maxPct = info.total_heap ? (100 * info.max_used_heap) / info.total_heap : 0;
    snprintf(dest, maxLen, "%2d%% %2d%%", usedPct, maxPct);
}