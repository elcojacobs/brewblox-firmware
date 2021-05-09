#pragma once

#include <cstdint>

namespace cbox {

inline bool
isdigit(char c)
{
    return c >= '0' && c <= '9';
}

inline bool
isxdigit(char c)
{
    return isdigit(c) || (c >= 'A' && c <= 'F') || (c >= 'a' && c <= 'f');
}

/**
 * Converts a hex digit to the corresponding binary value.
 */
inline uint8_t
h2d(unsigned char hex)
{
    if (hex > '9') {
        hex -= 7; // 'A' is 0x41, 'a' is 0x61. -7 =  0x3A, 0x5A
    }
    return uint8_t(hex & 0xf);
}

inline uint8_t
d2h(uint8_t bin)
{
    return uint8_t(bin + (bin > 9 ? 'A' - 10 : '0'));
}

}