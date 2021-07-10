// helper function for testing. Appends the CRC to a hex string, the same way CrcDataOut would do
#pragma once
#include <sstream>
#include <string>

namespace cbox {
std::string addCrc(const std::string& in);
std::string crc(const std::string& in);

/* Wrapper class around std::stringstream that clears error flags when new data is added.
 * Otherwise EOF flag is still set after data has been added to the stream
 */
class StringStreamAutoClear : public std::stringstream {
public:
    template <typename T>
    StringStreamAutoClear& operator<<(T& v)
    {
        std::stringstream& s = *this;
        s.clear(); // clear error flags
        s << v;
        return *this;
    }
};
}
