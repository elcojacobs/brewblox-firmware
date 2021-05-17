#pragma once
#include "esp32/rom/ets_sys.h"
#include "hal/hal_gpio.h"
#include "hal/hal_spi.h"

class TFT035 {
public:
    // al deze parameters hardcoded in de klasse
    TFT035();
    ~TFT035() = default;

    hal_spi_err_t writeCmd(const std::vector<uint8_t>& cmd);
    hal_spi_err_t write(const std::vector<uint8_t>& cmd);
    hal_spi_err_t writeCmd(uint8_t cmd);
    hal_spi_err_t write(uint8_t cmd);

    // todo: spi should be internal
    void aquire_spi();
    void release_spi();

    void setPos(unsigned int xs, unsigned int xe, unsigned int ys, unsigned int ye);
    void init();
    void DispRGBGray();
    void Write_Data(unsigned char DH, unsigned char DL);
    void ClearScreen(unsigned int bColor);
    void Write_Data_U16(unsigned int y);
    bool dmaWrite(uint8_t* tx_data, uint16_t tx_len, bool dc);
    bool dmaWrite(uint8_t tx_data, bool dc);
    uint8_t status()
    {
        return _status;
    }

    enum command : uint8_t {
        PGAMCTRL = 0xE0,
        NGAMCTRL = 0xE1,
        PWCTRL1 = 0XC0,
        PWCTRL2 = 0xC1,
        PWCTRL3 = 0xC5,
        MADCTL = 0x36,
        COLMOD = 0x3A,
        IFMODE = 0XB0,
        FRMCTR1 = 0xB1,
        INVTR = 0xB4,
        DISCTRL = 0XB6,
        SETIMAGE = 0XE9,
        ADJCTRL3 = 0xF7,
        SLPOUT = 0x11,
        DISON = 0x29,
        ALLPOFF = 0x22,
        ALLPON = 0x23
    };

private:
    SpiDevice spi;
    const hal_pin_t dc;
    uint8_t _status;
};
