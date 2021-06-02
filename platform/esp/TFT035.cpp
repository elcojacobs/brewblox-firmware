#include "TFT035.hpp"
#include "esp32/rom/ets_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/hal_delay.h"
#include "hal/hal_spi_types.h"
#include <cstring>
#include <esp_log.h>
#include <functional>
#include <sys/time.h>
using namespace spi;

void callbackDcPinOn(TransactionData& t)
{
    hal_gpio_write(2, true);
}

void postFreeCallBack(TransactionData& t)
{
    delete t.tx_data;
}

void callbackDcPinOff(TransactionData& t)
{
    hal_gpio_write(2, false);
}

TFT035::TFT035(std::function<void()> finishCallback)
    : spiDevice(
        0, 20'000'000UL, 100, 4,
        Settings::Mode::SPI_MODE0, Settings::BitOrder::MSBFIRST,
        []() {}, []() {})
    , finishCallback(finishCallback)
    , dc(2)

{
    spiDevice.init();
}

hal_spi_err_t TFT035::writeCmd(const std::vector<uint8_t>& cmd)
{
    return spiDevice.write(cmd, false, callbackDcPinOff);
}
hal_spi_err_t TFT035::write(const std::vector<uint8_t>& cmd)
{
    return spiDevice.write(cmd, false, callbackDcPinOn);
}

hal_spi_err_t TFT035::writeCmd(uint8_t cmd)
{
    return spiDevice.write(cmd, callbackDcPinOff);
}
hal_spi_err_t TFT035::write(uint8_t cmd)
{
    return spiDevice.write(cmd, callbackDcPinOn);
}

void TFT035::init()
{
    writeCmd(PGAMCTRL);
    write({0x00,
           0x03,
           0x09,
           0x08,
           0x16,
           0x0A,
           0x3F,
           0x78,
           0x4C,
           0x09,
           0x0A,
           0x08,
           0x16,
           0x1A,
           0x0F});
    writeCmd(NGAMCTRL);
    write({0x00,
           0x16,
           0x19,
           0x03,
           0x0F,
           0x05,
           0x32,
           0x45,
           0x46,
           0x04,
           0x0E,
           0x0D,
           0x35,
           0x37,
           0x0F});

    writeCmd(PWCTRL1); //Power Control 1
    write(0x17);       //Vreg1out
    write(0x15);       //Verg2out

    writeCmd(PWCTRL2); //Power Control 2
    write(0x41);       //VGH,VGL

    writeCmd(PWCTRL3); //Power Control 3
    write(0x00);
    write(0x12); //Vcom
    write(0x80);

    writeCmd(MADCTL); //Memory Access

    write(0b00101000);

    writeCmd(COLMOD); // Interface Pixel Format
    write(0x66);      //18 bit

    writeCmd(IFMODE); // Interface Mode Control
    write(0x00);

    writeCmd(FRMCTR1); //Frame rate
    write(0xA0);       //60Hz

    writeCmd(INVTR); //Display Inversion Control
    write(0x02);     //2-dot

    writeCmd(DISCTRL); //Display Function Control  RGB/MCU Interface Control
    write(0x02);       //MCU
    write(0x02);       //Source,Gate scan direction

    writeCmd(SETIMAGE); // Set Image Function
    write(0x00);        // Disable 24 bit data

    writeCmd(ADJCTRL3); // Adjust Control
    write(0xA9);
    write(0x51);
    write(0x2C);
    write(0x82); // D7 stream, loose

    writeCmd(SLPOUT); //Sleep out
    hal_delay_ms(120);
    writeCmd(DISON);
}

hal_spi_err_t TFT035::setPos(unsigned int xs, unsigned int xe, unsigned int ys, unsigned int ye)
{
    if (auto error = dmaWrite(0x2A, false))
        return error;

    auto x = std::array<uint8_t, 4>{uint8_t(xs >> 8),
                                    uint8_t(xs & 0xFF),
                                    uint8_t(xe >> 8),
                                    uint8_t(xe & 0xFF)};
    if (auto error = spiDevice.write(x, true, callbackDcPinOn))
        return error;

    if (auto error = dmaWrite(0x2B, false))
        return error;

    auto y = std::array<uint8_t, 4>{uint8_t(ys >> 8),
                                    uint8_t(ys & 0xFF),
                                    uint8_t(ye >> 8),
                                    uint8_t(ye & 0xFF)};

    if (auto error = spiDevice.write(y, true, callbackDcPinOn))
        return error;

    return dmaWrite(0x2C, false);
}

hal_spi_err_t TFT035::dmaWrite(uint8_t* tx_data, uint16_t tx_len, bool dc)
{
    if (dc) {
        return spiDevice.write(tx_data, tx_len, true, callbackDcPinOn);
    } else {
        return spiDevice.write(tx_data, tx_len, true, callbackDcPinOff);
    }
}

hal_spi_err_t TFT035::dmaWrite(uint8_t tx_val, bool dc)
{
    auto alocatedVal = new uint8_t(tx_val);
    if (dc) {
        return spiDevice.write(alocatedVal, 1, true, callbackDcPinOn, postFreeCallBack);
    } else {
        return spiDevice.write(alocatedVal, 1, true, callbackDcPinOff, postFreeCallBack);
    }
}
bool TFT035::writePixels(unsigned int xs, unsigned int xe, unsigned int ys, unsigned int ye, uint8_t* pixels, uint16_t nPixels)
{
    if (auto error = this->setPos(xs, xe, ys, ye))
        return error;

    return spiDevice.write(pixels, nPixels * 3, true, callbackDcPinOn, [&](TransactionData& t) {
        this->finishCallback();
    });
}

void TFT035::aquire_spi()
{
    spiDevice.aquire_bus();
}

void TFT035::release_spi()
{
    spiDevice.release_bus();
}