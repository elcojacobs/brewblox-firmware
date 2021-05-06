/*
 * Copyright 2019 BrewPi B.V.
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

#if PLATFORM_ID == 8 || PLATFORM_ID == 3

#include "blox/particle/Spark3PinsBlock.h"
#include "Board.h"
#include "blox/IoArrayHelpers.h"
#include "blox/proto/cpp/Spark3Pins.pb.h"

#if PLATFORM_ID != 3
#include "BrewPiTouch.h"
extern BrewPiTouch touch;
#endif

pin_t Spark3PinsBlock::channelToPin(uint8_t channel) const
{
    auto pins = std::array<pin_t, numPins>{
#ifdef PIN_V3_TOP1
        PIN_V3_TOP1,
#else
        pin_t(-1),
#endif
        PIN_V3_TOP2,
        PIN_V3_TOP3,
        PIN_V3_BOTTOM1,
        PIN_V3_BOTTOM2,
    };

    if (validChannel(channel)) {
        return pins[channel - 1];
    }
    return -1;
}

cbox::CboxError
Spark3PinsBlock::streamFrom(cbox::DataIn& in)
{
    blox_Spark3Pins message = blox_Spark3Pins_init_zero;
    cbox::CboxError result = streamProtoFrom(in, &message, blox_Spark3Pins_fields, blox_Spark3Pins_size);

    if (result == cbox::CboxError::OK) {
        // io pins are not writable through this block. They are configured by creating Digital Actuators
        digitalWriteFast(PIN_ALARM, message.soundAlarm);
#if defined(PIN_5V_ENABLE)
        digitalWriteFast(PIN_5V_ENABLE, message.enableIoSupply5V);
#endif
#if defined(PIN_12V_ENABLE)
        digitalWriteFast(PIN_12V_ENABLE, message.enableIoSupply12V);
#endif
    }
    return result;
}

cbox::CboxError
Spark3PinsBlock::streamTo(cbox::DataOut& out) const
{
    blox_Spark3Pins message = blox_Spark3Pins_init_zero;

    message.pins_count = numPins;
    message.pins[0].which_Pin = blox_Spark3IoPin_top1_tag;
    readIo(*this, 1, message.pins[0].Pin.top1);
    message.pins[1].which_Pin = blox_Spark3IoPin_top2_tag;
    readIo(*this, 2, message.pins[1].Pin.top2);
    message.pins[2].which_Pin = blox_Spark3IoPin_top3_tag;
    readIo(*this, 3, message.pins[2].Pin.top3);
    message.pins[3].which_Pin = blox_Spark3IoPin_bottom1_tag;
    readIo(*this, 4, message.pins[3].Pin.bottom1);
    message.pins[4].which_Pin = blox_Spark3IoPin_bottom2_tag;
    readIo(*this, 5, message.pins[4].Pin.bottom2);

    message.soundAlarm = HAL_GPIO_Read(PIN_ALARM);
#if defined(PIN_5V_ENABLE)
    message.enableIoSupply5V = HAL_GPIO_Read(PIN_5V_ENABLE);
#endif
#if defined(PIN_12V_ENABLE)
    message.enableIoSupply12V = HAL_GPIO_Read(PIN_12V_ENABLE);
#endif

#if PLATFORM_ID != 3
    message.voltage5 = touch.read5V();
    message.voltage12 = touch.read12V();
#else
    message.voltage5 = 5 * 410;
    message.voltage12 = 12 * 149;
#endif
    return streamProtoTo(out, &message, blox_Spark3Pins_fields, blox_Spark3Pins_size);
}

cbox::CboxError
Spark3PinsBlock::streamPersistedTo(cbox::DataOut& out) const
{
    blox_Spark3Pins message = blox_Spark3Pins_init_zero;

    message.soundAlarm = HAL_GPIO_Read(PIN_ALARM);
#if defined(PIN_5V_ENABLE)
    message.enableIoSupply5V = HAL_GPIO_Read(PIN_5V_ENABLE);
#endif
#if defined(PIN_12V_ENABLE)
    message.enableIoSupply12V = HAL_GPIO_Read(PIN_12V_ENABLE);
#endif

    return streamProtoTo(out, &message, blox_Spark3Pins_fields, blox_Spark3Pins_size);
}

void* Spark3PinsBlock::implements(const cbox::obj_type_t& iface)
{
    if (iface == BrewBloxTypes_BlockType_Spark3Pins) {
        return this; // me!
    }
    if (iface == cbox::interfaceId<IoArray>()) {
        // return the member that implements the interface in this case
        IoArray* ptr = this;
        return ptr;
    }
    return nullptr;
}
#endif