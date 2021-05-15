#include "TFT035.hpp"
#include "esp32/rom/ets_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "hal/hal_delay.h"
#include <esp_log.h>
#include <sys/time.h>

TFT035::TFT035()
    : spi(
        0, 20'000'000UL, 100, 4,
        SpiDevice::Mode::SPI_MODE0, SpiDevice::BitOrder::MSBFIRST,
        []() {}, []() {},
        [&](SpiTransaction& t) { // PRE
            bool val;
            // get bool that was stored in pointer address
            memcpy(&val, &t.user_cb_data, sizeof(bool));
            hal_gpio_write(dc, val);
            // hal_gpio_write(dc, true);

        },
        {} // POST
        )
    , dc(2)
{
}
void TFT035::ClearScreen(unsigned int bColor)
{
    unsigned int i, j;
    setPos(0, 49, 0, 49);
    for (i = 0; i < 50; i++) {

        for (j = 0; j < 50; j++) {
            Write_Data_U16(bColor);
        }
        vTaskDelay(1);
    }
}

hal_spi_err_t TFT035::writeCmd(const std::vector<uint8_t>& cmd)
{
    // hal_gpio_write(dc, false);
    auto err = spi.write(cmd, false);
    return err;
}
hal_spi_err_t TFT035::write(const std::vector<uint8_t>& cmd)
{
    // hal_gpio_write(dc, true);
    return spi.write(cmd, true);
}

hal_spi_err_t TFT035::writeCmd(uint8_t cmd)
{
    // hal_gpio_write(dc, false);
    auto err = spi.write(cmd, false);
    return err;
}
hal_spi_err_t TFT035::write(uint8_t cmd)
{
    // hal_gpio_write(dc, true);
    return spi.write(cmd, true);
}

void TFT035::init()
{
    spi.init();
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

    write(0xE8);

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
    write(0x02);       //Source,Gate scan dieection

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

    // writeCmd(0x23);

    // DispRGBGray();
    // writeCmd(0x00);
}
void TFT035::Write_Data(unsigned char DH, unsigned char DL)
{
    unsigned char R1, G1, B1, i;
    unsigned int LD = 0;

    // RGB565 TO RGB666
    LD = DH << 8;
    LD |= DL;

    R1 = (0x1f & (LD >> 11)) * 2;
    R1 <<= 2;
    G1 = 0x3f & (LD >> 5);
    G1 <<= 2;
    B1 = (0x1f & LD) * 2;
    B1 <<= 2;

    write({R1, G1, B1});
}
void TFT035::DispRGBGray(void)
{

    unsigned int COL = 320;
    unsigned char i, j, k, dbl, dbh;

    setPos(0, 319, 0, 479);

    // balck -> red
    for (k = 0; k < 80; k++) {
        for (i = 0; i < 32; i++) {
            for (j = 0; j < 10; j++) {
                dbh = i << 3;
                dbl = 0;
                Write_Data(dbh, dbl);
            }
        }
    }
    hal_delay_ms(1);
    // red -> black
    for (k = 0; k < 80; k++) {

        for (i = 31; i > 0; i--) {
            for (j = 0; j < 10; j++) {
                dbh = i << 3;
                dbl = 0;
                Write_Data(dbh, dbl);
            }
        }

        dbh = 0x00;
        dbl = 0x00;
        for (i = 0; i < 10; i++)
            Write_Data(dbh, dbl);
    }
    hal_delay_ms(1);
    // balck -> green
    for (k = 0; k < 80; k++) {

        for (i = 0; i < 64; i += 2) {
            for (j = 0; j < 10; j++) {
                dbh = i >> 3;
                dbl = i << 5;
                Write_Data(dbh, dbl);
            }
        }
    }
    hal_delay_ms(1);
    // green -> black
    for (k = 0; k < 80; k++) {
        for (i = 63; i != 1; i -= 2) {
            for (j = 0; j < 10; j++) {
                dbh = i >> 3;
                dbl = i << 5;
                Write_Data(dbh, dbl);
            }
        }
        dbh = 0x00;
        dbl = 0x00;
        for (i = 0; i < 10; i++)
            Write_Data(dbh, dbl);
    }

    hal_delay_ms(1);
    // balck -> blue
    for (k = 0; k < 80; k++) {
        for (i = 0; i < 32; i++) {
            for (j = 0; j < 10; j++) {
                dbh = 0;
                dbl = i;
                Write_Data(dbh, dbl);
            }
        }
    }
    hal_delay_ms(1);
    // blue -> black
    for (k = 0; k < 80; k++) {

        for (i = 31; i > 0; i--) {
            for (j = 0; j < 10; j++) {
                dbh = 0;
                dbl = i;
                Write_Data(dbh, dbl);
            }
        }
        dbh = 0x00;
        dbl = 0x00;
        for (i = 0; i < 10; i++)
            Write_Data(dbh, dbl);
    }
}
void TFT035::Write_Data_U16(unsigned int y)
{
    unsigned char m, n;
    m = y >> 8;
    n = y;
    Write_Data(m, n);
}

void TFT035::setPos(unsigned int xs, unsigned int xe, unsigned int ys, unsigned int ye)
{
    dmaWrite(0x2A, false);

    auto x = std::array<uint8_t, 4>{uint8_t(xs >> 8),
                                    uint8_t(xs & 0xFF),
                                    uint8_t(xe >> 8),
                                    uint8_t(xe & 0xFF)};
    spi.write(x, true, true);

    dmaWrite(0x2B, false);

    auto y = std::array<uint8_t, 4>{uint8_t(ys >> 8),
                                    uint8_t(ys & 0xFF),
                                    uint8_t(ye >> 8),
                                    uint8_t(ye & 0xFF)};

    spi.write(y, true, true);

    dmaWrite(0x2C, false);
}

bool TFT035::dmaWrite(uint8_t* tx_data, uint16_t tx_len, bool dc)
{
    spi.write(tx_data, tx_len, dc, true);

    return true;
}

bool TFT035::dmaWrite(uint8_t tx_val, bool dc)
{
    spi.write(tx_val, dc, true);

    return true;
}