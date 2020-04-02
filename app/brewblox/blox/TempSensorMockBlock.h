#pragma once

#include "TempSensorMock.h"

#include "blox/Block.h"
#include "blox/FieldTags.h"
#include "proto/cpp/TempSensorMock.pb.h"

class TempSensorMockBlock : public Block<BrewBloxTypes_BlockType_TempSensorMock> {
private:
    TempSensorMock sensor;

    using Fluctuation = TempSensorMock::Fluctuation;

protected:
    static bool streamFluctuationsOut(pb_ostream_t* stream, const pb_field_t* field, void* const* arg)
    {
        const std::vector<Fluctuation>* flucts = reinterpret_cast<std::vector<Fluctuation>*>(*arg);
        for (const auto& f : *flucts) {
            auto submsg = blox_TempSensorMock_Fluctuation();
            submsg.amplitude = cnl::unwrap(f.amplitude);
            submsg.period = f.period;
            if (!pb_encode_tag_for_field(stream, field)) {
                return false;
            }
            if (!pb_encode_submessage(stream, blox_TempSensorMock_Fluctuation_fields, &submsg)) {
                return false;
            }
        }
        return true;
    }

    static bool streamFluctuationsIn(pb_istream_t* stream, const pb_field_t*, void** arg)
    {
        std::vector<Fluctuation>* newFlucts = reinterpret_cast<std::vector<Fluctuation>*>(*arg);

        if (stream->bytes_left) {
            blox_TempSensorMock_Fluctuation submsg = blox_TempSensorMock_Fluctuation_init_zero;
            if (!pb_decode(stream, blox_TempSensorMock_Fluctuation_fields, &submsg)) {
                return false;
            }
            newFlucts->push_back(Fluctuation{
                cnl::wrap<decltype(Fluctuation::amplitude)>(submsg.amplitude), submsg.period});
        }
        return true;
    }

public:
    TempSensorMockBlock() = default;
    ~TempSensorMockBlock() = default;

    virtual cbox::CboxError streamFrom(cbox::DataIn& in) override final
    {
        blox_TempSensorMock newData = blox_TempSensorMock_init_zero;

        std::vector<Fluctuation> newFlucts;
        newData.fluctuations.funcs.decode = &streamFluctuationsIn;
        newData.fluctuations.arg = &newFlucts;
        cbox::CboxError result = streamProtoFrom(in, &newData, blox_TempSensorMock_fields, std::numeric_limits<size_t>::max() - 1);
        if (result == cbox::CboxError::OK) {
            sensor.fluctuations(std::move(newFlucts));
            sensor.setting(cnl::wrap<temp_t>(newData.setting));
            sensor.connected(newData.connected);
        }
        return result;
    }

    void writeMessage(blox_TempSensorMock& message) const
    {
        FieldTags stripped;

        message.fluctuations.funcs.encode = &streamFluctuationsOut;
        message.fluctuations.arg = const_cast<std::vector<Fluctuation>*>(&sensor.fluctuations());

        if (sensor.valid()) {
            message.value = cnl::unwrap((sensor.value()));
        } else {
            stripped.add(blox_TempSensorMock_value_tag);
        }

        message.setting = cnl::unwrap((sensor.setting()));
        message.connected = sensor.connected();
        stripped.copyToMessage(message.strippedFields, message.strippedFields_count, 1);
    }

    virtual cbox::CboxError streamTo(cbox::DataOut& out) const override final
    {
        blox_TempSensorMock message = blox_TempSensorMock_init_zero;
        writeMessage(message);

        return streamProtoTo(out, &message, blox_TempSensorMock_fields, std::numeric_limits<size_t>::max() - 1);
    }

    virtual cbox::CboxError streamPersistedTo(cbox::DataOut& out) const override final
    {
        blox_TempSensorMock message = blox_TempSensorMock_init_zero;
        writeMessage(message);
        message.value = 0; // value does not need persisting
        return streamProtoTo(out, &message, blox_TempSensorMock_fields, std::numeric_limits<size_t>::max() - 1);
    }

    virtual cbox::update_t update(const cbox::update_t& now) override final
    {
        sensor.update(now);
        return now + 100; // every 100ms
    }

    virtual void* implements(const cbox::obj_type_t& iface) override final
    {
        if (iface == BrewBloxTypes_BlockType_TempSensorMock) {
            return this; // me!
        }
        if (iface == cbox::interfaceId<TempSensor>()) {
            // return the member that implements the interface in this case
            TempSensor* ptr = &sensor;
            return ptr;
        }
        return nullptr;
    }

    TempSensorMock& get()
    {
        return sensor;
    }
};
