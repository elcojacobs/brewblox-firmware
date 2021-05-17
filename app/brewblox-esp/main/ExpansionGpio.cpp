#include "ExpansionGpio.hpp"
#include "esp_log.h"

using ChanBits = ExpansionGpio::ChanBits;
using ChanBitsInternal = ExpansionGpio::ChanBitsInternal;
using PinState = ExpansionGpio::PinState;
using FlexChannel = ExpansionGpio::FlexChannel;

ChanBits::ChanBits(const ChanBitsInternal& internal)
{
    this->c1 = internal.c1;
    this->c2 = internal.c2;
    this->c3 = internal.c3;
    this->c4 = internal.c4;
    this->c5 = internal.c5;
    this->c6 = internal.c6;
    this->c7 = internal.c7;
    this->c8 = internal.c8;
}

ChanBitsInternal::ChanBitsInternal(const ChanBits& external)
{
    this->c1 = external.c1;
    this->c2 = external.c2;
    this->c3 = external.c3;
    this->c4 = external.c4;
    this->c5 = external.c5;
    this->c6 = external.c6;
    this->c7 = external.c7;
    this->c8 = external.c8;
}

PinState ChanBits::get(uint8_t chan)
{
    // numbering board pins doesn't match driver's bits
    switch (chan) {
    case 1:
        return PinState(c1);
    case 2:
        return PinState(c2);
    case 3:
        return PinState(c3);
    case 4:
        return PinState(c4);
    case 5:
        return PinState(c5);
    case 6:
        return PinState(c6);
    case 7:
        return PinState(c7);
    case 8:
        return PinState(c8);
    default:
        return PinState(0x00);
    }
}

void ChanBits::set(uint8_t chan, PinState state)
{
    // numbering board pins doesn't match driver's bits
    switch (chan) {
    case 1:
        c1 = state;
        return;
    case 2:
        c2 = state;
        return;
    case 3:
        c3 = state;
        return;
    case 4:
        c4 = state;
        return;
    case 5:
        c5 = state;
        return;
    case 6:
        c6 = state;
        return;
    case 7:
        c7 = state;
        return;
    case 8:
        c8 = state;
        return;
    default:
        return;
    }
}

void FlexChannel::configure(
    const ChanBits& pins,
    const ChanBits& when_active_state,
    const ChanBits& when_inactive_state)
{

    pins_mask = pins;
    when_active_mask = when_active_state;
    when_inactive_mask = when_inactive_state;
    ESP_LOGW("configure", "%x %x", pins.all, pins_mask.all);
}

void FlexChannel::apply(ChannelConfig& config, ChanBitsInternal& op_ctrl)
{
    if (config == ChannelConfig::ACTIVE_HIGH) {
        op_ctrl.apply(pins_mask, when_active_mask);
    } else if (config == ChannelConfig::ACTIVE_LOW) {
        op_ctrl.apply(pins_mask, when_inactive_mask);
    } else {
        op_ctrl.apply(pins_mask, ChanBitsInternal());
    }
}

bool ExpansionGpio::senseChannelImpl(uint8_t channel, State& result) const
{
    return false;
}
bool ExpansionGpio::writeChannelImpl(uint8_t channel, IoArray::ChannelConfig config)
{
    if (!channel || channel > 8) {
        return false;
    }
    uint8_t idx = channel - 1;

    flexChannels[idx].apply(config, op_ctrl);
    ESP_LOGW("OP", "%x", op_ctrl.all);
    ESP_ERROR_CHECK_WITHOUT_ABORT(drv.writeRegister(DRV8908::RegAddr::OP_CTRL_1, op_ctrl.byte1));
    ESP_ERROR_CHECK_WITHOUT_ABORT(drv.writeRegister(DRV8908::RegAddr::OP_CTRL_2, op_ctrl.byte2));
    return true;
}

void ExpansionGpio::drv_status()
{
    // uint8_t result = 0xFF;
    // ESP_ERROR_CHECK_WITHOUT_ABORT(drv.readRegister(DRV8908::RegAddr::IC_STAT, result));
    ESP_LOGI("drv status", "%x", drv.status());
}

void ExpansionGpio::test()
{
    ChanBits pins;
    ChanBits whenActive;
    ChanBits whenInactive;
    pins.all = 0xFFFF;
    // pins.set(7, PinState::BOTH);
    // pins.set(2, PinState::BOTH);
    // pins.set(3, PinState::BOTH);
    // pins.set(4, PinState::BOTH);
    ChanBitsInternal pins2 = pins;
    ChanBits pins3 = pins2;
    // pins2.set(1, PinState::BOTH);
    // pins2.set(2, PinState::BOTH);
    ESP_LOGW("test", "%x %x %x", pins.all, pins2.all, pins3.all);

    whenActive.set(1, PinState::PULL_DOWN);
    whenActive.set(2, PinState::PULL_DOWN);
    whenActive.set(3, PinState::PULL_DOWN);
    whenActive.set(4, PinState::PULL_DOWN);
    whenActive.set(5, PinState::PULL_UP);
    whenActive.set(6, PinState::PULL_UP);
    whenActive.set(7, PinState::PULL_UP);
    whenActive.set(8, PinState::PULL_UP);

    whenInactive.set(1, PinState::PULL_UP);
    whenInactive.set(2, PinState::PULL_UP);
    whenInactive.set(3, PinState::PULL_UP);
    whenInactive.set(4, PinState::PULL_UP);
    whenInactive.set(5, PinState::PULL_DOWN);
    whenInactive.set(6, PinState::PULL_DOWN);
    whenInactive.set(7, PinState::PULL_DOWN);
    whenInactive.set(8, PinState::PULL_DOWN);

    flexChannels[0].configure(pins, whenActive, whenInactive);
    auto f = flexChannels[0];
    ESP_LOGW("converted_Back", "%x %x", f.when_active().all, f.when_inactive().all);
}