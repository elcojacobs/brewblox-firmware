#pragma once
#include "I2CDevice.hpp"
#include "esp_err.h"

class TCA9538 : public I2CDeviceBase<0x70> {
public:
    TCA9538(uint8_t lower_address)
        : I2CDeviceBase(lower_address){};
    ~TCA9538() = default;

    void set_outputs(uint8_t bits)
    {
        outputs = bits;
        command(0x01, outputs);
    }

    void set_output(uint8_t pin, bool state)
    {
        uint8_t mask = uint8_t(0x1) << pin;
        if (state) {
            set_outputs(outputs | mask);
        } else {
            set_outputs(outputs & ~mask);
        }
    }

    // 1 for input, 0 for output
    void set_config(uint8_t inputs_mask)
    {
        command(0x03, inputs_mask);
    }

private:
    void command(uint8_t op, uint8_t data)
    {
        auto t = i2cTransaction();
        t.start_write();
        t.write(op);
        t.write(data);
        t.stop();
        ESP_ERROR_CHECK_WITHOUT_ABORT(t.process());
    }

    uint8_t inputs = 0xFF;
    uint8_t outputs = 0xFF;
};