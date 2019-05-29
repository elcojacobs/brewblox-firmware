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
    std::array<uint8_t, 5> pins{{
        PIN_V3_TOP1,
        PIN_V3_TOP2,
        PIN_V3_TOP3,
        PIN_V3_BOTTOM1,
        PIN_V3_BOTTOM2,
    }};

public:
    Spark3PinsBlock()
        : IoArray(5)
    {
    }

    virtual cbox::CboxError streamFrom(cbox::DataIn& in) override final
    {
        blox_Spark3Pins message = blox_Spark3Pins_init_zero;
        cbox::CboxError result = streamProtoFrom(in, &message, blox_Spark3Pins_fields, blox_Spark3Pins_size);

        if (result == cbox::CboxError::OK) {
            // io pins are not writable through this block. They are configured by creating Digital Actuators
            HAL_GPIO_Write(PIN_LCD_BACKLIGHT, message.enableLcdBacklight);
            HAL_GPIO_Write(PIN_ALARM, message.soundAlarm);
#if defined(PIN_5V_ENABLE)
            HAL_GPIO_Write(PIN_5V_ENABLE, message.enableIoSupply5V);
#endif
#if defined(PIN_12V_ENABLE)
            HAL_GPIO_Write(PIN_12V_ENABLE, message.enableIoSupply12V);
#endif
        }
        return result;
    }

    virtual cbox::CboxError
    streamTo(cbox::DataOut& out) const override final
    {
        blox_Spark3Pins message = blox_Spark3Pins_init_zero;

        readIoConfig(*this, 1, message.io.top1.config);
        readIoConfig(*this, 2, message.io.top2.config);
        readIoConfig(*this, 3, message.io.top3.config);
        readIoConfig(*this, 4, message.io.bottom1.config);
        readIoConfig(*this, 5, message.io.bottom2.config);

        message.enableLcdBacklight = HAL_GPIO_Read(PIN_LCD_BACKLIGHT);
        message.soundAlarm = HAL_GPIO_Read(PIN_ALARM);
#if defined(PIN_5V_ENABLE)
        message.enableIoSupply5V = HAL_GPIO_Read(PIN_5V_ENABLE);
#endif
#if defined(PIN_12V_ENABLE)
        message.enableIoSupply12V = HAL_GPIO_Read(PIN_12V_ENABLE);
#endif

        return streamProtoTo(out, &message, blox_Spark3Pins_fields, blox_Spark3Pins_size);
    }

    virtual cbox::CboxError
    streamPersistedTo(cbox::DataOut& out) const override final
    {
        blox_Spark3Pins message = blox_Spark3Pins_init_zero;

        message.enableLcdBacklight = HAL_GPIO_Read(PIN_LCD_BACKLIGHT);
        message.soundAlarm = HAL_GPIO_Read(PIN_ALARM);
#if defined(PIN_5V_ENABLE)
        message.enableIoSupply5V = HAL_GPIO_Read(PIN_5V_ENABLE);
#endif
#if defined(PIN_12V_ENABLE)
        message.enableIoSupply12V = HAL_GPIO_Read(PIN_12V_ENABLE);
#endif

        return streamProtoTo(out, &message, blox_Spark3Pins_fields, blox_Spark3Pins_size);
    }

    virtual cbox::update_t
    update(const cbox::update_t& now) override final
    {
        return update_never(now);
    }

    virtual void*
    implements(const cbox::obj_type_t& iface) override final
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
    virtual bool
    senseChannelImpl(uint8_t channel, State& result) const override final
    {
        if (validChannel(channel)) {
            result = HAL_GPIO_Read(pins[channel - 1]) != 0 ? State::Active : State::Inactive;
            return true;
        }
        return false;
    }

    virtual bool
    writeChannelImpl(uint8_t channel, const ChannelConfig& config) override final
    {
        if (validChannel(channel)) {
            auto pin = pins[channel - 1];
#ifdef PIN_V3_TOP1_DIR
            if (pin == PIN_V3_TOP1) {
                bool isOutput = config == ChannelConfig::ACTIVE_HIGH || config == ChannelConfig::ACTIVE_LOW;
                HAL_GPIO_Write(PIN_V3_TOP1_DIR, isOutput);
            }
#endif
#ifdef PIN_V3_TOP1_DIR
            if (pin == PIN_V3_TOP2) {
                bool isOutput = config == ChannelConfig::ACTIVE_HIGH || config == ChannelConfig::ACTIVE_LOW;
                HAL_GPIO_Write(PIN_V3_TOP2_DIR, isOutput);
            }
#endif
            switch (config) {
            case ChannelConfig::ACTIVE_HIGH:
                HAL_GPIO_Write(pin, true);
                break;
            case ChannelConfig::ACTIVE_LOW:
                HAL_GPIO_Write(pin, false);
                break;
            case ChannelConfig::INPUT:
            case ChannelConfig::UNUSED:
            case ChannelConfig::UNKNOWN:
                break;
            }
            return true;
        }
        return false;
    }

    virtual bool
    supportsFastIo() const override final
    {
        return true;
    }
};
