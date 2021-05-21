#pragma once
#include "hal/hal_spi.h"

class DRV8908 {
public:
    DRV8908(uint8_t spi_idx, int ss,
            std::function<void()> on_spi_aquire,
            std::function<void()> on_spi_release);
    ~DRV8908() = default;

    enum class RegAddr : uint8_t {
        IC_STAT = 0x00,         // IC Status
        OCP_STAT_1 = 0x01,      // Overcurrent Protection Status 1
        OCP_STAT_2 = 0x02,      // Overcurrent Protection Status 2
        OCP_STAT_3 = 0x03,      // Overcurrent Protection Status 3
        OLD_STAT_1 = 0x04,      // Open Load Detection Status 1
        OLD_STAT_2 = 0x05,      // Open Load Detection Status 2
        OLD_STAT_3 = 0x06,      // Open Load Detection Status 3
        CONFIG_CTRL = 0x07,     // Configuration
        OP_CTRL_1 = 0x08,       // Operation Control 1
        OP_CTRL_2 = 0x09,       // Operation Control 2
        OP_CTRL_3 = 0x0A,       // Operation Control 3
        PWM_CTRL_1 = 0x0B,      // PWM Control 1
        PWM_CTRL_2 = 0x0C,      // PWM Control 2
        FW_CTRL_1 = 0x0D,       // Free-Wheeling Control 1
        FW_CTRL_2 = 0x0E,       // Free-Wheeling Control 2
        PWM_MAP_CTRL_1 = 0x0F,  // PWM Map Control 1
        PWM_MAP_CTRL_2 = 0x10,  // PWM Map Control 2
        PWM_MAP_CTRL_3 = 0x11,  // PWM Map Control 3
        PWM_MAP_CTRL_4 = 0x12,  // PWM Map Control 4
        PWM_FREQ_CTRL_1 = 0x13, // PWM Frequency Control 1
        PWM_FREQ_CTRL_2 = 0x14, // PWM Frequency Control 2
        PWM_DUTY_CTRL_1 = 0x15, // PWM Duty Control 1
        PWM_DUTY_CTRL_2 = 0x16, // PWM Duty Control 2
        PWM_DUTY_CTRL_3 = 0x17, // PWM Duty Control 3
        PWM_DUTY_CTRL_4 = 0x18, // PWM Duty Control 4
        PWM_DUTY_CTRL_5 = 0x19, // PWM Duty Control 5
        PWM_DUTY_CTRL_6 = 0x1A, // PWM Duty Control 6
        PWM_DUTY_CTRL_7 = 0x1B, // PWM Duty Control 7
        PWM_DUTY_CTRL_8 = 0x1C, // PWM Duty Control 8
        SR_CTRL_1 = 0x1D,       // Slew Rate Control 1
        SR_CTRL_2 = 0x1E,       // Slew Rate Control 2
        OLD_CTRL_1 = 0x1F,      // Open Load Detect 1
        OLD_CTRL_2 = 0x20,      // Open Load Detect 2
        OLD_CTRL_3 = 0x21,      // Open Load Detect 3
        OLD_CTRL_4 = 0x22,      // Open Load Detect 4
        OLD_CTRL_5 = 0x23,      // Open Load Detect 5
        OLD_CTRL_6 = 0x24,      // Open Load Detect 6
    };

    hal_spi_err_t readRegister(RegAddr address, uint8_t& val);
    hal_spi_err_t writeRegister(RegAddr address, uint8_t val);

    uint8_t status()
    {
        return _status;
    }

private:
    SpiDevice<> spi;
    uint8_t _status;
};