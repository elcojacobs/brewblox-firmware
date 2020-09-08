#pragma once

#include "ActuatorAnalogConstraintsProto.h"
#include "Balancer.h"
#include "blox/Block.h"
#include "cbox/CboxPtr.h"

class BalancerBlock : public Block<BrewBloxTypes_BlockType_Balancer> {
public:
    using Balancer_t = Balancer<blox_AnalogConstraint_balanced_tag>;

private:
    Balancer_t balancer;

public:
    BalancerBlock() = default;
    virtual ~BalancerBlock() = default;

    virtual cbox::CboxError
    streamFrom(cbox::DataIn&) override final
    {
        return cbox::CboxError::OK; // no settings to write (actuators register themselves)
    }

    virtual cbox::CboxError
    streamTo(cbox::DataOut& out) const override final;

    virtual cbox::CboxError
    streamPersistedTo(cbox::DataOut&) const override final
    {
        return cbox::CboxError::OK; // no settings to persist
    }

    virtual cbox::update_t
    update(const cbox::update_t& now) override final
    {
        balancer.update();
        return now + 1000;
    }

    virtual void*
    implements(const cbox::obj_type_t& iface) override final;

    Balancer_t&
    getBalancer()
    {
        return balancer;
    }
};
