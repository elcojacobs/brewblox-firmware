// helper function for testing. Appends the CRC to a hex string, the same way CrcDataOut would do
#pragma once
#include <string>

namespace cbox {
std::string addCrc(const std::string& in);
std::string crc(const std::string& in);
}
