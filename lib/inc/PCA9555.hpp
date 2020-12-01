#include "hal/hal_i2c.h"

class PCA9555 {
public:
    enum class RegAddr : uint8_t {
        INPUT = 0x00,
        OUTPUT = 0x02,
        INVERT = 0x04,
        CONFIG = 0x06,
    };

    PCA9555(uint8_t address)
        : addr(address)
    {
    }

    hal_i2c_err_t writeRegister(RegAddr target, uint16_t data)
    {
        uint8_t* asBytes = reinterpret_cast<uint8_t*>(&data);
        uint8_t bytes[3] = {uint8_t(target), asBytes[0], asBytes[1]};
        return hal_i2c_master_write(addr, bytes, 3, true);
    }

    hal_i2c_err_t readRegister(RegAddr target, uint16_t& data)
    {
        uint8_t bytes[2] = {uint8_t(target), 0};
        hal_i2c_err_t err = hal_i2c_master_write(addr, bytes, 1, true);
        if (err == 0) {
            err = hal_i2c_master_read(addr, bytes, 2, hal_i2c_ack_type_t::I2C_MASTER_ACK, true);
            uint8_t* resultAsBytes = reinterpret_cast<uint8_t*>(&data);
            resultAsBytes[0] = bytes[0];
            resultAsBytes[1] = bytes[1];
        }
        return err;
    }

    hal_i2c_err_t set_directions(uint16_t v)
    {
        auto err = writeRegister(RegAddr::CONFIG, v);
        if (err == 0) {
            direction = v;
        }
        return err;
    }

    hal_i2c_err_t get_directions(uint16_t& v)
    {
        auto err = readRegister(RegAddr::CONFIG, v);
        if (err == 0) {
            direction = v;
        }
        return err;
    }

    hal_i2c_err_t set_inverts(uint16_t v)
    {
        auto err = writeRegister(RegAddr::INVERT, v);
        if (err == 0) {
            invert = v;
        }
        return err;
    }

    hal_i2c_err_t get_inverts(uint16_t& v)
    {
        auto err = readRegister(RegAddr::INVERT, v);
        if (err == 0) {
            invert = v;
        }
        return err;
    }

    hal_i2c_err_t get_inputs(uint16_t& v)
    {
        auto err = readRegister(RegAddr::INPUT, v);
        if (err == 0) {
            inputs = v;
        }
        return err;
    }

    hal_i2c_err_t set_output(uint8_t pin, bool state)
    {
        uint16_t mask = uint16_t(0x1) << pin;
        if (state) {
            outputs |= mask;
        } else {
            outputs &= ~mask;
        }
        return writeRegister(RegAddr::OUTPUT, outputs);
    }

    hal_i2c_err_t get_input(uint8_t pin, bool& state)
    {
        uint16_t values = 0;
        auto err = get_inputs(values);
        if (err == 0) {
            uint16_t mask = uint16_t(0x1) << pin;
            state = (values & mask) != 0;
        }
        return err;
    }

private:
    uint16_t inputs = 0xFFFF;    // logic high
    uint16_t outputs = 0xFFFF;   // logic high
    uint16_t invert = 0x0000;    // not inverted
    uint16_t direction = 0xFFFF; // all inputs
    const uint8_t addr = 0x20;
};
