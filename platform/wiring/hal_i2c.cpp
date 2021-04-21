#include "hal/hal_i2c.h"
#include "spark_wiring.h"
#include "spark_wiring_i2c.h"

hal_i2c_err_t to_hal_err(uint8_t err)
{
    // 0: success
    // 1: busy timeout upon entering endTransmission()
    // 2: START bit generation timeout
    // 3: end of address transmission timeout
    // 4: data byte transfer timeout
    // 5: data byte transfer succeeded, busy timeout immediately after
    return err; // TODO: convert errors to platform independent enum
}

hal_i2c_err_t hal_i2c_master_init()
{
    Wire.reset();
    Wire.setTimeout(1);
    Wire.begin();

    return to_hal_err(0);
}

hal_i2c_err_t hal_i2c_write(uint8_t address, const uint8_t* data, size_t len, bool stop)
{
    Wire.beginTransmission(address);
    Wire.write(data, len);
    auto err = Wire.endTransmission(stop);
    return to_hal_err(err);
}

hal_i2c_err_t hal_i2c_read(uint8_t address, uint8_t* data, size_t len, bool stop)
{
    auto num_bytes_received = Wire.requestFrom(address, len, stop);
    if (num_bytes_received == 0) {
        return to_hal_err(4);
    }
    return 0;
}