#pragma once
#include "I2CDevice.hpp"


class FT6236 : public I2CDeviceBase<0x38> {
public:
    FT6236(uint8_t address): I2CDeviceBase(address) {}
    ~FT6236() = default;

    struct touch {
        uint8_t x;
        uint8_t y;
    };

    touch getFirstLoc(){
        i2c_write(0x06);
        auto y = i2c_read(1)[0];
        i2c_write(0x04);
        auto x = i2c_read(1)[0];
        return touch{x,y};
    }
    uint8_t getGesture(){
        i2c_write(0x01);
        return i2c_read(1)[0];
    }
    uint8_t getTouchPoints(){
        i2c_write(0x02);
        return i2c_read(1)[0];
    }




    


private:
   
};