#include "memory_info.h"
#include <cstdint>
#include <cstdio>
#include <cstring>

void
printHeapUse(char* dest, uint8_t maxLen)
{
    runtime_info_t info;
    memset(&info, 0, sizeof(info));
    info.size = sizeof(info);
    HAL_Core_Runtime_Info(&info, NULL);

    uint8_t freePct = info.total_heap ? (100 * info.freeheap) / info.total_heap : 0;
    uint8_t usedPct = 100 - freePct;
    uint8_t maxPct = info.total_heap ? (100 * info.max_used_heap) / info.total_heap : 0;
    snprintf(dest, maxLen, "%2d%% %2d%%", usedPct, maxPct);
}