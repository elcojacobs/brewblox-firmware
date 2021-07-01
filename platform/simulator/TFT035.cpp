#include "TFT035.hpp"
#include "websocketserver.hpp"
#include <cassert>
#include <vector>
std::vector<uint64_t> graphicsBuffer;

TFT035::TFT035(std::function<void()> finishCallback)
    : finishCallback(finishCallback)
{
}

void TFT035::init()
{
}

void TFT035::aquire_spi()
{
}

void TFT035::release_spi()
{
}

bool TFT035::writePixels(unsigned int xs, unsigned int xe, unsigned int ys, unsigned int ye, uint8_t* pixels, uint16_t nPixels)
{
    graphicsBuffer.resize(320 * 480);
    for (uint32_t y = ys; y <= ye; y++) {
        for (uint32_t x = xs; x <= xe; x++) {
            auto r = *pixels;
            auto g = *(pixels + 1);
            auto b = *(pixels + 2);

            uint16_t Rgb565 = 0;
            Rgb565 = (r & 0b11111000) << 8;
            Rgb565 = Rgb565 + ((g & 0b11111100) << 3);
            Rgb565 = Rgb565 + ((b) >> 3);

            graphicsBuffer[((y * 480) + x)] = ((uint64_t)Rgb565 << 32) | ((y * 480) + x);
            assert(graphicsBuffer.size() != 0);

            pixels += 3;
        }
    }
    this->finishCallback();
    return true;
}