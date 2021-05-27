#include "DRV8908.hpp"
#include "IoArray.h"
#include "TCA9538.hpp"
#include "esp_err.h"
#include "hal/hal_delay.h"
#include <array>

class ExpansionGpio : public IoArray {
public:
    ExpansionGpio(uint8_t lower_address)
        : IoArray(8)
        , expander(lower_address)
        , drv(
              0, -1,
              [this]() {
                  expander.set_output(0, false);
                  hal_delay_ms(1);
              },
              [this]() {
                  expander.set_output(0, true);
              })
    {
        init();
    }

    void init()
    {
        expander.set_outputs(0b11111101); // 24V uit, OneWire powered
        expander.set_config(0b11101000);
        // disable OLD
        ESP_ERROR_CHECK_WITHOUT_ABORT(drv.writeRegister(DRV8908::RegAddr::OLD_CTRL_2, 0b01000000));
        // set overvoltage threshold to 33V and clear all faults
        ESP_ERROR_CHECK_WITHOUT_ABORT(drv.writeRegister(DRV8908::RegAddr::CONFIG_CTRL, 0b00000011));
        expander.set_outputs(0b11111111); // 24V aan
    }

    void drv_status();

    void test();

    virtual bool supportsFastIo()
    {
        return false;
    }

    void power_cycle_onewire()
    {
        expander.set_output(5, false);
        hal_delay_ms(100);
        expander.set_output(5, true);
    }

public:
    enum PinState : uint8_t {
        DISCONNECTED = 0x0,
        PULL_DOWN = 0x01,
        PULL_UP = 0x02,
        BOTH = 0x03,
    };

    struct ChanBitsInternal;
    struct ChanBits {
        ChanBits()
        {
            this->all = 0;
        }

        // explicit ChanBits(ChanBits& other)
        // {
        //     this->all = other.all;
        // }
        // explicit ChanBits(const ChanBits& other)
        // {
        //     this->all = other.all;
        // }

        ChanBits(const ChanBitsInternal& internal);
        // ChanBits(ChanBitsInternal& internal);

        union {
            struct {
                uint8_t c1 : 2;
                uint8_t c2 : 2;
                uint8_t c3 : 2;
                uint8_t c4 : 2;
                uint8_t c5 : 2;
                uint8_t c6 : 2;
                uint8_t c7 : 2;
                uint8_t c8 : 2;
            };

            uint16_t all;
        };

        PinState get(uint8_t chan);
        void set(uint8_t chan, PinState state);
    };

    struct ChanBitsInternal {
        ChanBitsInternal()
        {
            this->all = 0;
        }

        ChanBitsInternal(const ChanBits& external);

        union {
            struct {
                // uint8_t c5 : 2;
                // uint8_t c3 : 2;
                // uint8_t c6 : 2;
                // uint8_t c2 : 2;
                // uint8_t c7 : 2;
                // uint8_t c8 : 2;
                // uint8_t c4 : 2;
                // uint8_t c1 : 2;
                uint8_t c1 : 2;
                uint8_t c4 : 2;
                uint8_t c8 : 2;
                uint8_t c7 : 2;
                uint8_t c2 : 2;
                uint8_t c6 : 2;
                uint8_t c3 : 2;
                uint8_t c5 : 2;
            };
            struct {
                uint8_t byte1 : 8;
                uint8_t byte2 : 8;
            };
            uint16_t all;
        };

        void apply(const ChanBitsInternal& mask, const ChanBitsInternal& state)
        {
            ESP_LOGW("apply_before", "%x %x", state.all, mask.all);
            this->all &= ~mask.all;
            this->all |= state.all;
            ESP_LOGW("apply_after", "%x", this->all);
        }
    };

    class FlexChannel {
    public:
        ChanBitsInternal pins_mask;          // pins controlled by this channel
        ChanBitsInternal when_active_mask;   // state when active
        ChanBitsInternal when_inactive_mask; // state when inactive

        ChanBits when_active() const
        {
            ChanBits converted = when_active_mask;
            return converted;
        }
        ChanBits when_inactive() const
        {
            ChanBits converted = when_inactive_mask;
            return converted; // convert to external order
        }

        // uint8_t pins() const; // convert to 1 bit per pin instead of 2

        void configure(
            const ChanBits& pins,
            const ChanBits& when_active_state,
            const ChanBits& when_inactive_state);

        void apply(ChannelConfig& config, ChanBitsInternal& op_ctrl);
    };

    virtual bool senseChannelImpl(uint8_t channel, State& result) const override final;
    virtual bool writeChannelImpl(uint8_t channel, ChannelConfig config) override final;
    virtual bool supportsFastIo() const override final
    {
        return false;
    }

private:
    TCA9538 expander;
    DRV8908 drv;
    std::array<FlexChannel, 8> flexChannels;
    ChanBitsInternal op_ctrl;
};