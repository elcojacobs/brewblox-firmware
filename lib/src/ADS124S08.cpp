/*
 * Copyright 2020 BrewPi B.V.
 *
 * This file is part of the BrewBlox Control Library.
 *
 * BrewPi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BrewPi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with BrewPi.  If not, see <http://www.gnu.org/licenses/>.
 */

// #include <math.h>
// #include <assert.h>
#include "ADS124S08.h"
#include "esp_log.h"

//****************************************************************************
//
// Functions
//
//****************************************************************************

ADS124S08::ADS124S08(uint8_t spi_idx, int ss,
                     std::function<void(bool isHigh)> reset,
                     std::function<void(bool isHigh)> start,
                     std::function<void()> on_spi_aquire,
                     std::function<void()> on_spi_release)
    : spi(spi_idx, 100000, 1, ss,
          SpiDevice::Mode::SPI_MODE1, SpiDevice::BitOrder::MSBFIRST,
          on_spi_aquire, on_spi_release)
    , set_reset(reset)
    , set_start(start)
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
    // RegisterMap initRegisters = {0};
    RegStatus status;

    // // Provide additional delay time for power supply settling
    hal_delay_ms(3);

    // // Toggle nRESET pin to assure default register settings.
    set_start(false);
    set_reset(false);
    hal_delay_ms(1);
    set_reset(true);
    // // Must wait 4096 tCLK after reset
    hal_delay_ms(1000);

    spi.aquire_bus();
    hal_delay_ms(1);
    uint8_t result = readSingleRegister(RegStatus::address);
    ESP_LOGW("ADC", "%x", result);
    if ((status & status.ADS_nRDY_MASK)) {
        ESP_LOGW("ADC", "not ready");
        return (false); // Device not ready
    }

    // // Ensure internal register array is initialized
    // restoreRegisterDefaults();

    // // Configure initial device register settings here
    // initRegisterMap[REG_ADDR_STATUS] = 0x00; // Reset POR event
    // initRegisterMap[REG_ADDR_INPMUX] = adcChars->inputMuxConfReg;
    // initRegisterMap[REG_ADDR_PGA] = adcChars->pgaReg;
    // initRegisterMap[REG_ADDR_DATARATE] = adcChars->dataRateReg;
    // initRegisterMap[REG_ADDR_REF] = adcChars->refSelReg;
    // initRegisterMap[REG_ADDR_IDACMAG] = adcChars->IDACmagReg;
    // initRegisterMap[REG_ADDR_IDACMUX] = adcChars->IDACmuxReg;
    // initRegisterMap[REG_ADDR_VBIAS] = adcChars->VBIASReg;
    // initRegisterMap[REG_ADDR_SYS] = SYS_DEFAULT;

    // // Initialize ADC Characteristics
    // adcChars->resolution = ADS124S08_BITRES;
    // adcChars->VBIASReg = VBIAS_DEFAULT;
    // adcChars->offsetCalCoeff = 0;
    // adcChars->gainCalCoeff = 1;
    // adcChars->sampleRate = 20; // 20 samples per second
    // adcChars->pgaGain = pow(2, (adcChars->pgaReg & ADS_GAIN_MASK));

    // // Write to all modified registers
    // writeMultipleRegisters(RegStatus::address, RegSys::address - RegStatus::address + 1, initRegisterMap);

    //Read back all registers
    readMultipleRegisters(RegId::address, 18);
    uint8_t i = 0;
    for (auto& r : registers) {

        // if (i == REG_ADDR_STATUS)
        //     continue;
        // if (initRegisterMap[i] != registerMap[i])
        //     return (false);
        ESP_LOGI("reg ", "%d: %x", i++, r.value);
    }
    spi.release_bus();
    return true;
}

uint8_t ADS124S08::readSingleRegister(uint8_t address)
{
    /* Initialize arrays */
    address &= OPCODE::RWREG_MASK;
    address |= OPCODE::RREG;
    uint8_t tx[3] = {address, 0, 0};
    uint8_t rx[3] = {0, 0, 0};

    SpiTransaction t{
        .tx_data = tx,
        .rx_data = rx,
        .tx_len = 3,
        .rx_len = 3,
        .user_cb_data = nullptr,
    };

    if (spi.transmit(t) == 0) {
        /* Update register array and return read result*/
        registers[address] = rx[2];
        return rx[2];
    }
    return 0xFF;
}

uint8_t ADS124S08::readMultipleRegisters(uint8_t startAddress, uint8_t count)
{
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
    if (spi.transmit(t) == 0) {
        for (uint8_t i = 0; i < count; i++) {
            // Read register data bytes
            registers[i + startAddress] = rx[2 + i];
        }
        return 0;
    }
    return 1;
}

uint8_t ADS124S08::writeMultipleRegisters(uint8_t startAddress, uint8_t count, uint8_t data[])
{
    const uint8_t len = 2 + count;

    uint8_t tx[20] = {0};
    uint8_t rx[20] = {0};

    tx[0] = uint8_t(OPCODE::WREG) | (startAddress & uint8_t(OPCODE::RWREG_MASK));
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

    if (spi.transmit(t) == 0) {
        return 0;
    }
    return 1;
}

#if 0
/************************************************************************************/ /**
 *
 * @brief changeADCParameters()
 *          Change ADC parameters 
 *
 * @param[in]   *adcChars  ADC characteristics
 * @param[in]   *spiHdl    SPI_Handle pointer for TI Drivers
 *
 * @return      true for successful initialization
 *              false for unsuccessful initialization
 */
bool
changeADCParameters(ADCchar_Set* adcChars, SPI_Handle spiHdl)
{
    uint8_t initRegisterMap[NUM_REGISTERS] = {0};
    uint8_t i;

    // Reconfigure initial device register settings here
    initRegisterMap[REG_ADDR_INPMUX] = adcChars->inputMuxConfReg;
    initRegisterMap[REG_ADDR_PGA] = adcChars->pgaReg;
    initRegisterMap[REG_ADDR_DATARATE] = adcChars->dataRateReg;
    initRegisterMap[REG_ADDR_REF] = adcChars->refSelReg;
    initRegisterMap[REG_ADDR_IDACMAG] = adcChars->IDACmagReg;
    initRegisterMap[REG_ADDR_IDACMUX] = adcChars->IDACmuxReg;
    initRegisterMap[REG_ADDR_VBIAS] = adcChars->VBIASReg;

    // Initialize ADC Characteristics based
    adcChars->resolution = ADS124S08_BITRES;
    adcChars->VBIASReg = VBIAS_DEFAULT;
    adcChars->offsetCalCoeff = 0;
    adcChars->gainCalCoeff = 1;
    adcChars->sampleRate = 20; // 20 samples per second
    adcChars->pgaGain = pow(2, (adcChars->pgaReg & ADS_GAIN_MASK));

    // Write to all modified registers
    writeMultipleRegisters(spiHdl, REG_ADDR_INPMUX, REG_ADDR_VBIAS - REG_ADDR_INPMUX + 1, initRegisterMap);

    // Read back all registers
    readMultipleRegisters(spiHdl, REG_ADDR_ID, NUM_REGISTERS);
    for (i = REG_ADDR_INPMUX; i < REG_ADDR_VBIAS - REG_ADDR_INPMUX + 1; i++) {
        if (i == REG_ADDR_STATUS)
            continue;
        if (initRegisterMap[i] != registerMap[i])
            return (false);
    }
    return (true);
}


/************************************************************************************/ /**
 *
 * @brief writeSingleRegister()
 *          Write data to a single register at the specified address
 *
 * @param[in]   spiHdl      SPI_Handle from TI Drivers
 * @param[in]	address 	Register address to write
 * @param[in]	data 		8-bit data to write
 *
 * @return 		None
 */
void
writeSingleRegister(SPI_Handle spiHdl, uint8_t address, uint8_t data)
{
    /* Initialize arrays */
    uint8_t DataTx[COMMAND_LENGTH + 1] = {OPCODE_WREG | (address & OPCODE_RWREG_MASK), 0, data};
    uint8_t DataRx[COMMAND_LENGTH + 1] = {0};

    /* Check that the register address is in range */
    assert(address < NUM_REGISTERS);

    /* Build TX array and send it */
    spiSendReceiveArrays(spiHdl, DataTx, DataRx, COMMAND_LENGTH + 1);

    /* Update register array */
    registerMap[address] = DataTx[COMMAND_LENGTH];
}


/************************************************************************************/ /**
 *
 * @brief sendCommand()
 *          Sends the specified SPI command to the ADC
 *
 * @param[in]   spiHdl      SPI_Handle from TI Drivers
 * @param[in]	op_code 	SPI command byte
 *
 * @return 		None
 */
void
sendCommand(SPI_Handle spiHdl, uint8_t op_code)
{
    /* Assert if this function is used to send any of the following commands */
    assert(OPCODE_RREG != op_code); /* Use "readSingleRegister()"  or "readMultipleRegisters()"  */
    assert(OPCODE_WREG != op_code); /* Use "writeSingleRegister()" or "writeMultipleRegisters()" */

    /* SPI communication */
    spiSendReceiveByte(spiHdl, op_code);

    // Check for RESET command
    if (OPCODE_RESET == op_code) {
        /* Update register array to keep software in sync with device */
        restoreRegisterDefaults();
    }
}

/************************************************************************************/ /**
 *
 * @brief startConversions()
 *        	Wakes the device from power-down and starts continuous conversions
 *            by setting START pin high or sending START Command
 *
 * @param[in]   spiHdl      SPI_Handle from TI Drivers
 *
 * @return 		None
 */
void
startConversions(SPI_Handle spiHdl)
{
    // Wakeup device
    sendWakeup(spiHdl);

#ifdef START_PIN
    /* Begin continuous conversions */
    setSTART(HIGH);
#else
    sendSTART(spiHdl);
#endif
}

/************************************************************************************/ /**
 *
 * @brief stopConversions()
 *          Stops continuous conversions by setting START pin low or sending STOP Command
 *
 * @param[in]   spiHdl      SPI_Handle from TI Drivers
 *
 * @return      None
 */
void
stopConversions(SPI_Handle spiHdl)
{
    /* Stop continuous conversions */
#ifdef START_PIN
    setSTART(LOW);
#else
    sendSTOP(spiHdl);
#endif
}

/************************************************************************************/ /**
 *
 * @brief resetADC()
 *          Resets ADC by setting RESET pin low or sending RESET Command
 *
 * @param[in]   spiHdl      SPI_Handle from TI Drivers
 *
 * @return      None
 */
void
resetADC(SPI_Handle spiHdl)
{
    /* Reset ADC */
#ifdef RESET_PIN
    toggleRESET();
#else
    sendRESET(spiHdl);
#endif
}

/************************************************************************************/ /**
 *
 * @brief readData()
 *          Sends the read command and retrieves STATUS (if enabled) and data
 *          NOTE: Call this function after /DRDY goes low and specify the 
 *          the number of bytes to read and the starting position of data
 *          
 * @param[in]   spiHdl      SPI_Handle from TI Drivers
 * @param[in]	status[] 	Pointer to location where STATUS byte will be stored
 * @param[in]	mode 		Direct or Command read mode
 * 
 * @return 		32-bit sign-extended conversion result (data only)
 */
int32_t
readConvertedData(SPI_Handle spiHdl, uint8_t status[], readMode mode)
{
    uint8_t DataTx[STATUS_LENGTH + DATA_LENGTH + CRC_LENGTH + 1] = {0}; // Initialize all array elements to 0
    uint8_t DataRx[STATUS_LENGTH + DATA_LENGTH + CRC_LENGTH + 1] = {0};
    uint8_t byteLength;
    uint8_t dataPosition;
    uint8_t byte_options;
    bool status_byte_enabled = 0;
    int32_t signByte, upperByte, middleByte, lowerByte;

    // Status Byte is sent if SENDSTAT bit of SYS register is set
    byte_options = IS_SENDSTAT_SET << 1 | IS_CRC_SET;
    switch (byte_options) {
    case 0: // No STATUS and no CRC
        byteLength = DATA_LENGTH;
        dataPosition = 0;
        break;
    case 1: // No STATUS and CRC
        byteLength = DATA_LENGTH + CRC_LENGTH;
        dataPosition = 0;
        break;
    case 2: // STATUS and no CRC
        byteLength = STATUS_LENGTH + DATA_LENGTH;
        dataPosition = 1;
        status_byte_enabled = 1;
        break;
    case 3: // STATUS and CRC
        byteLength = STATUS_LENGTH + DATA_LENGTH + CRC_LENGTH;
        dataPosition = 1;
        status_byte_enabled = 1;
        break;
    }

    if (mode == COMMAND) {
        DataTx[0] = OPCODE_RDATA;
        byteLength += 1;
        dataPosition += 1;
    }
    spiSendReceiveArrays(spiHdl, DataTx, DataRx, byteLength);

    // Parse returned SPI data
    /* Check if STATUS byte is enabled and if we have a valid "status" memory pointer */
    if (status_byte_enabled && status) {
        status[0] = DataRx[dataPosition - 1];
    }

    /* Return the 32-bit sign-extended conversion result */
    if (DataRx[dataPosition] & 0x80u) {
        signByte = 0xFF000000;
    } else {
        signByte = 0x00000000;
    }

    upperByte = ((int32_t)DataRx[dataPosition] & 0xFF) << 16;
    middleByte = ((int32_t)DataRx[dataPosition + 1] & 0xFF) << 8;
    lowerByte = ((int32_t)DataRx[dataPosition + 2] & 0xFF);

    return (signByte + upperByte + middleByte + lowerByte);
}

/************************************************************************************/ /**
 *
 * @brief restoreRegisterDefaults()
 *          Updates the registerMap[] array to its default values
 *          NOTES: If the MCU keeps a copy of the ADC register settings in memory,
 *          then it is important to ensure that these values remain in sync with the
 *          actual hardware settings. In order to help facilitate this, this function
 *          should be called after powering up or resetting the device (either by
 *          hardware pin control or SPI software command).
 *          Reading back all of the registers after resetting the device will
 *          accomplish the same result.
 *
 * @return 		None
 */
void
restoreRegisterDefaults(void)
{
    /* Default register settings */
    registerMap[REG_ADDR_ID] = ID_DEFAULT;
    registerMap[REG_ADDR_STATUS] = STATUS_DEFAULT;
    registerMap[REG_ADDR_INPMUX] = INPMUX_DEFAULT;
    registerMap[REG_ADDR_PGA] = PGA_DEFAULT;
    registerMap[REG_ADDR_DATARATE] = DATARATE_DEFAULT;
    registerMap[REG_ADDR_REF] = REF_DEFAULT;
    registerMap[REG_ADDR_IDACMAG] = IDACMAG_DEFAULT;
    registerMap[REG_ADDR_IDACMUX] = IDACMUX_DEFAULT;
    registerMap[REG_ADDR_VBIAS] = VBIAS_DEFAULT;
    registerMap[REG_ADDR_SYS] = SYS_DEFAULT;
    registerMap[REG_ADDR_OFCAL0] = OFCAL0_DEFAULT;
    registerMap[REG_ADDR_OFCAL1] = OFCAL1_DEFAULT;
    registerMap[REG_ADDR_OFCAL2] = OFCAL2_DEFAULT;
    registerMap[REG_ADDR_FSCAL0] = FSCAL0_DEFAULT;
    registerMap[REG_ADDR_FSCAL1] = FSCAL1_DEFAULT;
    registerMap[REG_ADDR_FSCAL2] = FSCAL2_DEFAULT;
    registerMap[REG_ADDR_GPIODAT] = GPIODAT_DEFAULT;
    registerMap[REG_ADDR_GPIOCON] = GPIOCON_DEFAULT;
}
#endif