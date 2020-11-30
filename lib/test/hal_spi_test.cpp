#include "hal/hal_spi.h"

hal_spi_err_t SpiDevice::init()
{
    return 0;
}

void SpiDevice::deinit()
{
}

hal_spi_err_t SpiDevice::queue_transfer(const SpiTransaction&, uint32_t)
{
    return 0;
}

hal_spi_err_t SpiDevice::transfer_impl(const SpiTransaction&, uint32_t)
{
    return 0;
}

void SpiDevice::aquire_bus_impl()
{
}

void SpiDevice::release_bus_impl()
{
}
