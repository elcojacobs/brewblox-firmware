#include "TestHelpers.h"
#include "Crc.h"
#include "hex_utility.h"

namespace cbox {
/*
 * calculates 2 CRC characters to a hex string, used for testing
 */
std::string
crc(const std::string& in)
{
    std::string result;
    uint8_t crc = 0;
    for (auto it = in.begin(); it != in.end() && it + 1 != in.end();) {
        char char1 = *(it++);
        char char2 = *(it++);
        uint8_t data = (h2d(char1) << 4) | h2d(char2);
        crc = calc_crc(crc, data);
    }
    result.push_back(d2h(uint8_t(crc & 0xF0) >> 4));
    result.push_back(d2h(uint8_t(crc & 0xF)));

    return result;
}

/*
 * Appends 2 CRC characters to a hex string, used for testing
 */
std::string
addCrc(const std::string& in)
{
    return in + crc(in);
}

}