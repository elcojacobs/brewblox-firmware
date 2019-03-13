#include <cinttypes>
#include <functional>

class TimerInterrupts {
public:
    static void init();
    static int16_t add(std::function<void()>&& func);
    static void remove(uint8_t id);
};
