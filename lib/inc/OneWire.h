#pragma once

#include "OneWireAddress.h"
#include "OneWireLowLevelInterface.h"

class OneWire {
public:
    explicit OneWire(OneWireLowLevelInterface& driverImpl)
        : driver(driverImpl)
    {
        // base class OneWireLowLevelInterface configures pin or bus master IC
        init();
        reset_search();
    }
    OneWire(const OneWire&) = delete;
    OneWire(OneWire&&) = default;
    OneWire& operator=(const OneWire&) = delete;
    ~OneWire() = default;

private:
    OneWireLowLevelInterface& driver;
    // global search state
    OneWireAddress ROM_NO;
    uint8_t lastDiscrepancy;
    bool lastDeviceFlag;
    uint8_t lockedSearchBits;

public:
    // wrappers for low level functions
    bool init()
    {
        return driver.init();
    }

    uint8_t read()
    {
        return driver.read();
    }

    void write(uint8_t b)
    {
        driver.write(b);
    }

    void write_bit(uint8_t bit)
    {
        driver.write_bit(bit);
    }
    uint8_t read_bit()
    {
        return driver.read_bit();
    }

    bool reset()
    {
        return driver.reset();
    }

    // high level functions

    // Issue a 1-Wire rom select command, you do the reset first.
    void select(const OneWireAddress& rom);

    // Issue a 1-Wire rom skip command, to address all on bus.
    void skip(void);

    void write_bytes(const uint8_t* buf, uint16_t count);

    void read_bytes(uint8_t* buf, uint16_t count);

    // Clear the search state so that if will start from the beginning again.
    void reset_search();

    // Setup the search to find the device type 'family_code' on the next search
    void target_search(uint8_t family_code);

    // Look for the next device. Returns true if a new address has been
    // returned. A zero might mean that the bus is shorted, there are
    // no devices, or you have already retrieved all of them.  It
    // might be a good idea to check the CRC to make sure you didn't
    // get garbage.  The order is deterministic. You will always get
    // the same devices in the same order.
    // When the last device has been return, reset_search or target_search has to be called
    // to reset the search.
    bool search(OneWireAddress& newAddr);
};
