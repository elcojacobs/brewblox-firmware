#pragma once

#include "ActuatorLogic.h"
#include "BrewBlox.h"
#include "DigitalActuatorBlock.h"
#include "blox/Block.h"
#include "cbox/CboxPtr.h"
#include "proto/cpp/ActuatorLogic.pb.h"

class ActuatorLogicBlock : public Block<BrewBloxTypes_BlockType_ActuatorLogic> {
private:
    cbox::ObjectContainer& objectsRef; // remember object container reference to create constraints
    cbox::CboxPtr<ActuatorDigitalConstrained> target;
    ActuatorLogic logic;
    bool enabled = false;
    std::vector<cbox::CboxPtr<ActuatorDigitalConstrained>> actuatorLookups;

public:
    ActuatorLogicBlock(cbox::ObjectContainer& objects)
        : objectsRef(objects)
        , target(objects)
        , logic(target.lockFunctor())
    {
    }
    virtual ~ActuatorLogicBlock() = default;

    virtual cbox::CboxError streamFrom(cbox::DataIn& dataIn) override final
    {
        blox_ActuatorLogic newData = blox_ActuatorLogic_init_zero;
        cbox::CboxError result = streamProtoFrom(dataIn, &newData, blox_ActuatorLogic_fields, blox_ActuatorLogic_size);
        if (result == cbox::CboxError::OK) {
            target.setId(newData.targetId);
            enabled = newData.enabled;
            logic.clear();
            actuatorLookups.clear();
            // first create all lookups to avoid realloating the vector after creating the functors
            // This would invalidate the this pointer passed to std::bind in the created functor
            for (pb_size_t s = 0; s < newData.sections_count; s++) {
                auto msgSection = newData.sections[s];
                for (pb_size_t i = 0; i < msgSection.inputs_count; i++) {
                    cbox::obj_id_t id = msgSection.inputs[i];
                    actuatorLookups.emplace_back(objectsRef, id);
                }
            }
            // Lookup order stays in sync, is flattened version of all actuator lookups used by logic actuator sections
            auto cboxLookupsIt = actuatorLookups.cbegin();
            for (pb_size_t s = 0; s < newData.sections_count; s++) {
                auto msgSection = newData.sections[s];
                auto newSection = ActuatorLogic::makeSection(ADLogic::LogicOp(msgSection.sectionOp));
                for (pb_size_t i = 0; i < msgSection.inputs_count; i++) {
                    newSection->add(cboxLookupsIt->lockFunctor());
                    cboxLookupsIt++;
                }
                logic.addSection(ADLogic::LogicOp(newData.sections[s].combineOp), std::move(newSection));
            }
        }
        return result;
    }

    void writeMessage(blox_ActuatorLogic& message) const
    {
        message.targetId = target.getId();
        message.enabled = enabled;
        message.result = blox_DigitalState(logic.result());
        auto cboxLookupsIt = actuatorLookups.cbegin(); // stays in sync, is flattened version of all actuators
        for (const auto& section : logic.sectionsList()) {
            auto& msgSection = message.sections[message.sections_count];
            if (section.section) {
                for (uint8_t i = 0; i < section.section->lookupsList().size(); i++) {
                    auto& msgInput = msgSection.inputs[msgSection.inputs_count];
                    msgInput = cboxLookupsIt->getId();
                    cboxLookupsIt++;
                    msgSection.inputs_count = msgSection.inputs_count + 1;
                }
            }
            msgSection.combineOp = blox_ActuatorLogic_LogicOp(section.combineOp);
            message.sections_count = message.sections_count + 1;
        }
    }

    virtual cbox::CboxError streamTo(cbox::DataOut& out) const override final
    {
        blox_ActuatorLogic message = blox_ActuatorLogic_init_zero;
        writeMessage(message);

        return streamProtoTo(out, &message, blox_ActuatorLogic_fields, blox_ActuatorLogic_size);
    }

    virtual cbox::CboxError streamPersistedTo(cbox::DataOut& out) const override final
    {
        blox_ActuatorLogic message = blox_ActuatorLogic_init_zero;
        writeMessage(message);
        message.result = blox_DigitalState(0); // result doesn't need to be persisted

        return streamProtoTo(out, &message, blox_ActuatorLogic_fields, blox_ActuatorLogic_size);
    }

    virtual cbox::update_t update(const cbox::update_t& now) override final
    {
        if (enabled) {
            logic.update();
        }
        return now + 100; // update every 100ms
    }

    virtual void* implements(const cbox::obj_type_t& iface) override final
    {
        if (iface == BrewBloxTypes_BlockType_ActuatorLogic) {
            return this; // me!
        }
        return nullptr;
    }
};
