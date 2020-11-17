#include "hal/hal_i2c.h"

class PCA9571 {
public:
    static constexpr uint8_t addr = 0x25;

    static void write_io(uint8_t bits)
    {
        hal_i2c_master_write(addr, &bits, 1, true);
    }
};
