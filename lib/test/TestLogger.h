#include <string>

class TestLogger {
public:
    static void clear();

    static bool contains(const std::string& s);

    static uint32_t count(const std::string& s);

    static void add(std::string&& s);
};
