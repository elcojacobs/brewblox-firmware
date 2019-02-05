#pragma once

#include "ActuatorDigitalConstrained.h"
#include "blox/Block.h"
#include "nanopb_callbacks.h"
#include "proto/cpp/Mutex.pb.h"

class MutexBlock : public Block<BrewbloxOptions_BlockType_Mutex> {
private:
    TimedMutex m_mutex;

public:
    MutexBlock() = default;
    virtual ~MutexBlock() = default;

    virtual cbox::CboxError
    streamFrom(cbox::DataIn& dataIn) override final
    {
        blox_Mutex newData = blox_Mutex_init_zero;
        cbox::CboxError result = streamProtoFrom(dataIn, &newData, blox_Mutex_fields, blox_Mutex_size);
        if (result == cbox::CboxError::OK) {
            m_mutex.differentActuatorWait(newData.differentActuatorWait);
        }
        return result;
    }

    virtual cbox::CboxError
    streamTo(cbox::DataOut& out) const override final
    {
        blox_Mutex message = blox_Mutex_init_zero;
        message.differentActuatorWait = m_mutex.differentActuatorWait();
        message.waitRemaining = m_mutex.waitRemaining();

        return streamProtoTo(out, &message, blox_Mutex_fields, blox_Mutex_size);
    }

    virtual cbox::CboxError
    streamPersistedTo(cbox::DataOut& out) const override final
    {
        return streamTo(out);
    }

    virtual cbox::update_t
    update(const cbox::update_t& now) override final
    {
        // ensure mutex always has recent system time
        m_mutex.update(now);
        return now + 1;
    }

    virtual void*
    implements(const cbox::obj_type_t& iface) override final
    {
        if (iface == BrewbloxOptions_BlockType_Mutex) {
            return this; // me!
        }
        if (iface == cbox::interfaceId<TimedMutex>()) {
            // return the member that implements the interface in this case
            TimedMutex* ptr = &m_mutex;
            return ptr;
        }
        return nullptr;
    }

    TimedMutex&
    getMutex()
    {
        return m_mutex;
    }
};
