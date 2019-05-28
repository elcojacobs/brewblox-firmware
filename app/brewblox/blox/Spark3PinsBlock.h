#pragma once

#include "Board.h"
#include "IoArray.h"
#include "IoArrayHelpers.h"
#include "blox/Block.h"
#include "proto/cpp/Spark3Pins.pb.h"

/*
 * Copyright 2015 BrewPi/Elco Jacobs.
 * Copyright 2015 Matthew McGowan.
 *
 * This file is part of BrewPi.
 *
 * BrewPi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BrewPi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with BrewPi.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include "ActuatorDigital.h"
#include "gpio_hal.h"
#include "pinmap_hal.h"

class Spark3PinsBlock : public IoArray, public Block<BrewbloxOptions_BlockType_Spark3Pins> {
private:
    std::array<uint8_t, 5> pins = {
        PIN_V3_TOP1,
        PIN_V3_TOP2,
        PIN_V3_TOP3,
        PIN_V3_BOTTOM1,
        PIN_V3_BOTTOM2,
    };

public:
    Spark3PinsBlock()
        : IoArray(5)
    {
    }

    virtual cbox::CboxError streamFrom(cbox::DataIn& in) override final
    {
        return cbox::CboxError::OK; // not directly writable
    }

    virtual cbox::CboxError streamTo(cbox::DataOut& out) const override final
    {
        blox_Spark3Pins message = blox_Spark3Pins_init_zero;

        readIoConfig(*this, 1, message.top1.config);
        readIoConfig(*this, 2, message.top2.config);
        readIoConfig(*this, 3, message.top3.config);
        readIoConfig(*this, 4, message.bottom1.config);
        readIoConfig(*this, 5, message.bottom2.config);

        return streamProtoTo(out, &message, blox_Spark3Pins_fields, blox_Spark3Pins_size);
    }

    virtual cbox::CboxError streamPersistedTo(cbox::DataOut& out) const override final
    {
        return cbox::CboxError::OK;
    }

    virtual cbox::update_t update(const cbox::update_t& now) override final
    {
        return update_never(now);
    }

    virtual void* implements(const cbox::obj_type_t& iface) override final
    {
        if (iface == BrewbloxOptions_BlockType_Spark3Pins) {
            return this; // me!
        }
        if (iface == cbox::interfaceId<IoArray>()) {
            // return the member that implements the interface in this case
            IoArray* ptr = this;
            return ptr;
        }
        return nullptr;
    }

    // generic ArrayIO interface
    virtual bool senseChannelImpl(uint8_t channel, State& result) const override final
    {
        if (validChannel(channel)) {
            result = HAL_GPIO_Read(pins[channel - 1]) != 0 ? State::Active : State::Inactive;
            return true;
        }
        return false;
    }

    virtual bool writeChannelImpl(uint8_t channel, const ChannelConfig& config) override final
    {
        if (validChannel(channel)) {
            auto pin = pins[channel - 1];
            switch (config) {
            case ChannelConfig::ACTIVE_HIGH:
                HAL_Pin_Mode(pin, OUTPUT);
                HAL_GPIO_Write(pin, true);
                break;
            case ChannelConfig::ACTIVE_LOW:
                HAL_Pin_Mode(pin, OUTPUT);
                HAL_GPIO_Write(pin, false);
                break;
            case ChannelConfig::INPUT:
            case ChannelConfig::UNUSED:
            case ChannelConfig::UNKNOWN:
                HAL_Pin_Mode(pin, INPUT);
                break;
            }
            return true;
        }
        return false;
    }

    virtual bool supportsFastIo() const override final
    {
        return true;
    }
};
