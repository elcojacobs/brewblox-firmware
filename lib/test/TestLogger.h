#include <string>
#include <vector>

class TestLogger {
public:
    static void clear()
    {
        logs.clear();
    }

    static bool contains(const std::string& s)
    {
        for (const auto& log : logs) {
            if (s == log) {
                return true;
            }
        }
        return false;
    }

    static uint32_t count(const std::string& s)
    {
        uint32_t count = 0;
        for (const auto& log : logs) {
            if (s == log) {
                count++;
            }
        }
        return count;
    }

    static void add(std::string&& s)
    {
        logs.push_back(s);
    }

    static std::vector<std::string> logs;
};
