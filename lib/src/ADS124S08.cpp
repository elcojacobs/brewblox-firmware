/*
 * Copyright 2020 BrewPi B.V.
 *
 * This file is part of the BrewBlox Control Library.
 *
 * Brewblox is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Brewblox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with Brewblox.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "ADS124S08.hpp"
#include "esp_log.h"

//****************************************************************************
//
// Functions
//
//****************************************************************************

ADS124S08::ADS124S08(uint8_t spi_idx, int ss,
                     std::function<void(bool isHigh)> reset,
                     std::function<void(bool isHigh)> start,
                     std::function<bool()> ready,
                     std::function<void()> on_spi_aquire,
                     std::function<void()> on_spi_release)
    : spi(spi_idx, 100000, 1, ss,
          SpiDevice::Mode::SPI_MODE1, SpiDevice::BitOrder::MSBFIRST,
          on_spi_aquire, on_spi_release)
    , set_reset(reset)
    , set_start(start)
    , sense_ready(ready)
{
    spi.init();
}

/************************************************************************************/ /**
 *
 * @brief adcStartupRoutine()
 *          Startup function to be called before communicating with the ADC
 *
 * @param[in]   *adcChars  ADC characteristics
 * @param[in]   *spiHdl    SPI_Handle pointer for TI Drivers
 *
 * @return      true for successful initialization
 *              false for unsuccessful initialization
 */

bool ADS124S08::startup()
{
    // // Provide additional delay time for power supply settling
    hal_delay_ms(3);

    if (set_start) {
        set_start(false); // set start pin low if setter is defined
    }
    reset(); // reset, uses reset pin or reset command

    // read status register and check ready bit
    bool success = false;
    spi.aquire_bus();
    if (updateReg(registers.byName.status) == 0) {
        success = registers.byName.status.ready();
    }

    if (success) {
        RegisterMap initRegs;
        // // Configure initial device register settings here
        initRegs.byName.status.value = 0x00; // Reset POR event
        initRegs.byName.inpMux.value = RegInpMux::ADS_P_AIN0 + RegInpMux::ADS_N_AINCOM;
        initRegs.byName.pga.value = RegPga::ADS_DELAY_14 | RegPga::ADS_PGA_ENABLED | RegPga::ADS_GAIN_4;
        initRegs.byName.dataRate.value = RegDataRate::ADS_GLOBALCHOP | RegDataRate::ADS_CONVMODE_SS | RegDataRate::ADS_DR_20;
        initRegs.byName.ref.value = RegRef::ADS_REFSEL_INT | RegRef::ADS_REFINT_ON_ALWAYS;
        initRegs.byName.sys.value = RegSys::ADS_CRC_ENABLE | RegSys::ADS_SENDSTATUS_ENABLE;

        writeRegs(initRegs.byName.status, initRegs.byName.sys);

        //Read back all registers
        updateRegs(registers.byName.id, registers.byName.gpiocon);
        if (success) {
            for (uint8_t i = 2; i < registers.byIdx.size(); i++) {
                if (initRegs[i] != registers[i]) {
                    ESP_LOGE("ADC", "Register mismatch: %d %02x %02x", i, initRegs[i], registers[i]);
                    success = false;
                    break;
                }
            }
        }
    }
    spi.release_bus();

    return success;
}

bool ADS124S08::ready()
{
    if (sense_ready) {
        return !sense_ready(); // ready pin is low when ready
    }
    // fall back to reading status register
    if (updateReg(registers.byName.status) == 0) {
        return registers.byName.status.ready();
    }
    return false; // SPI error
}

hal_spi_err_t ADS124S08::readSingleRegister(uint8_t& val, uint8_t address)
{
    uint8_t byte1 = address;
    byte1 |= OPCODE::RREG;
    uint8_t tx[3] = {byte1, 0, 0};
    uint8_t rx[3] = {0, 0, 0};

    SpiTransaction t{
        .tx_data = tx,
        .rx_data = rx,
        .tx_len = 3,
        .rx_len = 3,
        .user_cb_data = nullptr,
    };

    auto err = spi.transfer(t);
    if (err == 0) {
        /* Update register array and return read result*/
        val = rx[2];
    }
    return err;
}

hal_spi_err_t ADS124S08::readMultipleRegisters(uint8_t startAddress, uint8_t endAddress)
{
    const uint8_t count = endAddress - startAddress;
    const uint8_t len = 2 + count;

    uint8_t tx[20] = {0};
    uint8_t rx[20] = {0};

    SpiTransaction t{
        .tx_data = tx,
        .rx_data = rx,
        .tx_len = len,
        .rx_len = len,
        .user_cb_data = nullptr,
    };

    tx[0] = OPCODE::RREG | (startAddress & OPCODE::RWREG_MASK);
    tx[1] = count - 1;
    const auto err = spi.transfer(t);
    if (err == 0) {
        for (uint8_t i = 0; i < count; i++) {
            // Read register data bytes
            registers[i + startAddress] = rx[2 + i];
        }
    }
    return err;
}

hal_spi_err_t ADS124S08::writeMultipleRegisters(uint8_t firstAddress, uint8_t lastAddress, uint8_t data[])
{
    const uint8_t count = lastAddress - firstAddress + 1;
    const uint8_t len = 2 + count;

    uint8_t tx[20] = {0};
    uint8_t rx[20] = {0};

    tx[0] = uint8_t(OPCODE::WREG) | (firstAddress & uint8_t(OPCODE::RWREG_MASK));
    tx[1] = count - 1;

    for (uint8_t i = 0; i < count; i++) {
        tx[2 + i] = data[i];
        registers[i] = data[i];
    }

    SpiTransaction t{
        .tx_data = tx,
        .rx_data = rx,
        .tx_len = len,
        .rx_len = len,
        .user_cb_data = nullptr,
    };

    return spi.transfer(t);
}

hal_spi_err_t ADS124S08::sendCommand(OPCODE opcode)
{
    uint8_t tx = opcode;
    SpiTransaction t{
        .tx_data = &tx,
        .rx_data = nullptr,
        .tx_len = 1,
        .rx_len = 0,
        .user_cb_data = nullptr,
    };

    return spi.transfer(t);
}

void ADS124S08::start()
{
    if (set_start) {
        set_start(true);
    } else {
        sendCommand(OPCODE::START);
    }
}

void ADS124S08::pulse_start()
{
    if (set_start) {
        set_start(false);
        set_start(true);
        set_start(false);
    } else {
        sendCommand(OPCODE::START);
    }
}

void ADS124S08::stop()
{
    if (set_start) {
        set_start(false);
    } else {
        sendCommand(OPCODE::STOP);
    }
}

void ADS124S08::reset()
{
    if (set_reset) {
        set_reset(false);
        hal_delay_ms(1);
        set_reset(true);
    } else {
        sendCommand(OPCODE::RESET);
    }
    // Must wait 4096 tCLK after reset (= 1ms $ 4.096 Mhz internal oscillator)
    hal_delay_ms(1);
    registers.reset();
}

uint8_t ADS124S08::calculateCrc(const uint8_t dataBytes[], uint8_t numBytes)
{
    uint64_t result = 0;
    // polynomical X^8 + X^2 + X^2 + X + 1
    uint64_t poly = uint64_t{0x107} << (8 * numBytes - 1);
    for (uint8_t i = 0; i < numBytes; i++) {
        result += uint64_t(dataBytes[i]) << (8 * (numBytes - i));
    }
    uint8_t msbPos = (numBytes * 8 + 7);
    uint64_t msbMask = uint64_t(0x1) << msbPos;

    while (result >= 0x100) {
        if (msbMask & result) {
            result ^= poly;
        }
        msbMask = msbMask >> 1;
        poly = poly >> 1;
    }

    return result;
}

int32_t ADS124S08::waitAndReadData()
{
    return readData(ReadMode::DIRECT);
}

int32_t ADS124S08::readLastData()
{
    return readData(ReadMode::COMMAND);
}

// read data using command mode to not need syncing to DRDY and avoid corruption
int32_t ADS124S08::readData(ReadMode mode)
{
    spi.aquire_bus();

    uint8_t tx[6] = {0};
    uint8_t rx[6] = {0};

    uint8_t numBytes = 3;
    uint8_t rxPos = 0;
    uint8_t dataPos = 0;
    bool statusEnabled = registers.byName.sys.value & RegSys::ADS_SENDSTATUS_MASK;
    bool crcEnabled = registers.byName.sys.value & RegSys::ADS_CRC_MASK;

    if (mode == ReadMode::COMMAND) {
        tx[0] = OPCODE::RDATA;
        numBytes += 1;
        rxPos = 1;
        dataPos += 1;
    }
    if (statusEnabled) {
        numBytes += 1;
        dataPos += 1;
    }
    if (crcEnabled) {
        numBytes += 1;
    }

    SpiTransaction t{
        .tx_data = tx,
        .rx_data = rx,
        .tx_len = numBytes,
        .rx_len = numBytes,
        .user_cb_data = nullptr,
    };
    auto err = spi.transfer(t);
    spi.release_bus();

    if (err != 0) {
        ESP_LOGE("ADC", "SPI ERROR %d", err);
        return SPI_ERROR_RESULT;
    }
    if (crcEnabled) {
        uint8_t crc = calculateCrc(&rx[rxPos], numBytes - rxPos - 1);
        if (crc != rx[numBytes - 1]) {
            ESP_LOGE("ADC", "CRC ERROR");
            return CRC_ERROR_RESULT;
        }
    }
    if (statusEnabled) {
        registers.byName.status.value = rx[rxPos];
    }
    int32_t result = rx[dataPos] & 0x80u ? 0xFF000000 : 0x0;
    result |= uint32_t(rx[dataPos]) << 16;
    result |= uint32_t(rx[dataPos + 1]) << 8;
    result |= uint32_t(rx[dataPos + 2]);

    return result;
}
