#pragma once

#include <cstdint>

enum RESET_USER_REASON {
    NOT_SPECIFIED = 0,
    WATCHDOG = 1,
    CBOX_RESET = 2,
    CBOX_FACTORY_RESET = 3,
    FIRMWARE_UPDATE_FAILED = 4,
    LISTENING_MODE_EXIT = 5,
    FIRMWARE_UPDATE_SUCCESS = 6,
    OUT_OF_MEMORY = 7,
};

void
handleReset(bool exit, uint8_t reason = RESET_USER_REASON::NOT_SPECIFIED);