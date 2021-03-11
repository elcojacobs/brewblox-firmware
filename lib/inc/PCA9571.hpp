#include "hal/hal_i2c.h"

class PCA9571 {
public:
    static constexpr uint8_t addr = 0x25;

    static void set_pins(uint8_t bits)
    {
        hal_i2c_master_write(addr, &bits, 1, true);
    }

    void set_pin(uint8_t pin, bool state)
    {
        uint8_t mask = uint8_t(0x1) << pin;
        if (state) {
            outputs |= mask;
        } else {
            outputs &= ~mask;
        }
        hal_i2c_master_write(addr, &outputs, 1, true);
    }

private:
    uint8_t outputs = 0xFF;
};
