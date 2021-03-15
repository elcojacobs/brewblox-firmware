#include "Spark4.hpp"

#include "SX1508.hpp"
#include "hal/hal_delay.h"
#include "hal/hal_gpio.h"
#include "hal/hal_i2c.h"
#include "network/network.hpp"
#include <asio.hpp>

extern "C" {
#ifdef ESP_PLATFORM
void app_main();
}
#else
int main(int /*argc*/, char** /*argv*/);
#endif

#ifdef ESP_PLATFORM
void app_main()
#else
int main(int /*argc*/, char** /*argv*/)
#endif
{
    Spark4::init();
    esp_event_loop_create_default();

    SX1508 expander(0);
    expander.write_reg(SX1508::RegAddr::clock, 0b01011011);
    hal_delay_ms(200);
    expander.write_reg(SX1508::RegAddr::clock, 0b01011010);
    hal_delay_ms(200);
    expander.write_reg(SX1508::RegAddr::clock, 0b01010000);

    // Expander pins:
    // 0,1,2,4 -> EX0, EX1, EX2, EX3 (interrupt inputs)
    // 3, 6, 7 -> LED B, R, G. (B and G support breathing)
    // 5 -> LCD backlight

    // Disable input for RGB LED and TFT backlight
    expander.write_reg(SX1508::RegAddr::inputDisable, 0b11101000);
    // Inverse polarity for TFT backlight
    expander.write_reg(SX1508::RegAddr::polarity, 0b00100000);
    // Set dir
    expander.write_reg(SX1508::RegAddr::dir, 0b00010111);
    // Disable pullup for RGB LED and TFT backlight
    expander.write_reg(SX1508::RegAddr::pullUp, 0b00010111);
    // Enable open drain for RGB LED, TFT backlight is push/pull
    expander.write_reg(SX1508::RegAddr::openDrain, 0b11001000);
    // logarithmic fading, PWM frequendy 250 Hz, reset is POR, auto increment register, auto clean nint on read
    expander.write_reg(SX1508::RegAddr::misc, 0b11101000);
    // enable led driver on RGB and backlight
    expander.write_reg(SX1508::RegAddr::ledDriverEnable, 0b11101000);

    // Configure Blue and Green for breathing
    expander.write_reg(SX1508::RegAddr::iOn3, 128);
    expander.write_reg(SX1508::RegAddr::tRise3, 3);
    expander.write_reg(SX1508::RegAddr::tOn3, 1);
    expander.write_reg(SX1508::RegAddr::tFall3, 3);
    expander.write_reg(SX1508::RegAddr::off3, 0b00001010); // 1 period off, intensity 2

    expander.write_reg(SX1508::RegAddr::iOn7, 128);
    expander.write_reg(SX1508::RegAddr::tRise7, 3);
    expander.write_reg(SX1508::RegAddr::tOn7, 1);
    expander.write_reg(SX1508::RegAddr::tFall7, 3);
    expander.write_reg(SX1508::RegAddr::off7, 0b00001010); // 1 period off, intensity 2

    // Configure Red for blinking, but disabled now
    expander.write_reg(SX1508::RegAddr::iOn6, 0);
    expander.write_reg(SX1508::RegAddr::tOn6, 15);
    expander.write_reg(SX1508::RegAddr::off6, 0b01111000); // 1 period off, intensity 0

    // Configure backlight PWM at 50%
    expander.write_reg(SX1508::RegAddr::iOn5, 128);

    // Enable outputs
    expander.write_reg(SX1508::RegAddr::data, 0x00);

    network_init();

    asio::io_context io;
    io.run();

#ifndef ESP_PLATFORM
    return 0;
#endif
}
