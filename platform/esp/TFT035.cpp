#include "TFT035.hpp"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/hal_delay.h"
#include "hal/hal_spi_types.h"
#include <cstring>
#include <esp_log.h>
#include <functional>
#include <sys/time.h>
using namespace spi;

auto callbackDcPinOn = StaticCallbacks{
    [](TransactionData& t) {
        hal_gpio_write(2, true);
    },
    nullptr};

auto callbackDcPinOnWithFree = StaticCallbacks{
    [](TransactionData& t) {
        hal_gpio_write(2, true);
    },
    [](TransactionData& t) {
        delete t.tx_data;
    }};

auto callbackDcPinOffWithFree = StaticCallbacks{
    [](TransactionData& t) {
        hal_gpio_write(2, false);
    },
    [](TransactionData& t) {
        delete t.tx_data;
    }};

auto callbackDcPinOff = StaticCallbacks{
    [](TransactionData& t) {
        hal_gpio_write(2, false);
    },
    nullptr};

TFT035::TFT035(std::function<void()> finishCallback)
    : spiDevice(spi::Settings{.spi_idx = 0, .speed = 20'000'000UL, .queueSize = 10, .ssPin = 4, .mode = Settings::Mode::SPI_MODE0, .bitOrder = Settings::BitOrder::MSBFIRST, .on_Aquire = []() {}, .on_Release = []() {}})
    , finishCallback(finishCallback)
    , dc(2)

{
    spiDevice.init();
}

error TFT035::writeCmd(const std::vector<uint8_t>& cmd)
{
    hal_gpio_write(2, false);
    return spiDevice.write(cmd);
}
error TFT035::write(const std::vector<uint8_t>& cmd)
{
    hal_gpio_write(2, true);
    return spiDevice.write(cmd);
}

error TFT035::writeCmd(uint8_t cmd)
{
    hal_gpio_write(2, false);
    return spiDevice.write(cmd);
}
error TFT035::write(uint8_t cmd)
{
    hal_gpio_write(2, true);
    return spiDevice.write(cmd);
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

error TFT035::setPos(unsigned int xs, unsigned int xe, unsigned int ys, unsigned int ye)
{
    if (auto error = dmaWrite(0x2A, false))
        return error;

    if (auto error = dmaWrite(uint8_t(xs >> 8), true))
        return error;
    if (auto error = dmaWrite(uint8_t(xs & 0xFF), true))
        return error;
    if (auto error = dmaWrite(uint8_t(xe >> 8), true))
        return error;
    if (auto error = dmaWrite(uint8_t(xe & 0xFF), true))
        return error;

    if (auto error = dmaWrite(0x2B, false))
        return error;

    if (auto error = dmaWrite(uint8_t(ys >> 8), true))
        return error;
    if (auto error = dmaWrite(uint8_t(ys & 0xFF), true))
        return error;
    if (auto error = dmaWrite(uint8_t(ye >> 8), true))
        return error;
    if (auto error = dmaWrite(uint8_t(ye & 0xFF), true))
        return error;

    return dmaWrite(0x2C, false);
}

error TFT035::dmaWrite(uint8_t* tx_data, uint16_t tx_len, bool dc)
{
    if (dc) {
        return spiDevice.dmaWrite(tx_data, tx_len, callbackDcPinOn);
    } else {
        return spiDevice.dmaWrite(tx_data, tx_len, callbackDcPinOff);
    }
}

error TFT035::dmaWrite(uint8_t tx_val, bool dc)
{
    auto alocatedVal = new uint8_t(tx_val);
    if (dc) {
        return spiDevice.dmaWrite(alocatedVal, 1, callbackDcPinOnWithFree);
    } else {
        return spiDevice.dmaWrite(alocatedVal, 1, callbackDcPinOffWithFree);
    }
}
bool TFT035::writePixels(unsigned int xs, unsigned int xe, unsigned int ys, unsigned int ye, uint8_t* pixels, uint16_t nPixels)
{
    if (auto error = this->setPos(xs, xe, ys, ye))
        return error;

    return spiDevice.dmaWrite(pixels, nPixels * 3,
                              Callbacks{[&](TransactionData& t) {
                                            hal_gpio_write(2, true);
                                        },
                                        [&](TransactionData& t) {
                                            this->finishCallback();
                                        }});
}

void TFT035::aquire_spi()
{
    spiDevice.aquire_bus();
}

void TFT035::release_spi()
{
    spiDevice.release_bus();
}