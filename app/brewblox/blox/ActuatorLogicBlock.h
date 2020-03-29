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
    std::vector<cbox::CboxPtr<ProcessValue<fp12_t>>> processValueLookups;

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
            processValueLookups.clear();
            // first create all lookups to avoid realloating the vector after creating the functors
            // This would invalidate the this pointer passed to std::bind in the created functor
            for (pb_size_t s = 0; s < newData.section_count; s++) {
                auto msgSection = newData.section[s];
                if (msgSection.which_section == blox_ActuatorLogic_Section_actuators_tag) {
                    for (pb_size_t i = 0; i < msgSection.section.actuators.actuator_count; i++) {
                        cbox::obj_id_t id = msgSection.section.actuators.actuator[i];
                        actuatorLookups.emplace_back(objectsRef, id);
                    }
                } else if (msgSection.which_section == blox_ActuatorLogic_Section_comparison_tag) { // compare section
                    cbox::obj_id_t id = msgSection.section.comparison.compared;
                    processValueLookups.emplace_back(objectsRef, id);
                }
            }
            // Lookup order stays in sync, is flattened version of all actuator lookups used by logic actuator sections
            auto actuatorLookupsIt = actuatorLookups.cbegin();
            auto processValueLookupsIt = processValueLookups.cbegin();
            for (pb_size_t s = 0; s < newData.section_count; s++) {
                auto msgSection = newData.section[s];
                if (msgSection.which_section == blox_ActuatorLogic_Section_actuators_tag) {
                    auto newSection = std::make_unique<ADLogic::ActuatorSection>(
                        ADLogic::SectionOp(msgSection.sectionOp),
                        ADLogic::CombineOp(msgSection.combineOp));
                    for (pb_size_t i = 0; i < msgSection.section.actuators.actuator_count; i++) {
                        newSection->add(actuatorLookupsIt->lockFunctor());
                        actuatorLookupsIt++;
                    }
                    logic.addSection(std::move(newSection));
                } else if (msgSection.which_section == blox_ActuatorLogic_Section_comparison_tag) {
                    auto newSection = std::make_unique<ADLogic::CompareSection>(
                        ADLogic::SectionOp(msgSection.sectionOp),
                        ADLogic::CombineOp(msgSection.combineOp),
                        processValueLookupsIt->lockFunctor(),
                        msgSection.section.comparison.useSetting,
                        cnl::wrap<fp12_t>(msgSection.section.comparison.threshold));
                    processValueLookupsIt++;
                    logic.addSection(std::move(newSection));
                }
            }
        }
        return result;
    }

    void
    writeMessage(blox_ActuatorLogic& message) const
    {
        message.targetId = target.getId();
        message.enabled = enabled;
        message.result = blox_DigitalState(logic.result());
        // vector lookups stay in sync, are flattened version of nested lookups in same order
        auto actuatorLookupsIt = actuatorLookups.cbegin();
        auto processValueLookupsIt = processValueLookups.cbegin();
        for (const auto& section : logic.sectionsList()) {
            auto& msgSection = message.section[message.section_count];
            msgSection.sectionOp = blox_ActuatorLogic_SectionOp(section->sectionOp());
            msgSection.combineOp = blox_ActuatorLogic_CombineOp(section->combineOp());

            if (auto ptr = section->asActuatorSection()) {
                msgSection.which_section = blox_ActuatorLogic_Section_actuators_tag;
                for (uint8_t i = 0; i < ptr->lookupsList().size(); i++) {
                    auto& msgInput = msgSection.section.actuators.actuator[msgSection.section.actuators.actuator_count];
                    msgInput = actuatorLookupsIt->getId();
                    ++actuatorLookupsIt;
                    msgSection.section.actuators.actuator_count = msgSection.section.actuators.actuator_count + 1;
                }
            } else if (auto ptr = section->asCompareSection()) {
                msgSection.which_section = blox_ActuatorLogic_Section_comparison_tag;
                msgSection.section.comparison.compared = processValueLookupsIt->getId();
                msgSection.section.comparison.useSetting = ptr->useSetting();
                msgSection.section.comparison.threshold = cnl::unwrap(ptr->threshold());
                ++processValueLookupsIt;
            }

            message.section_count = message.section_count + 1;
        }
    }

    virtual cbox::CboxError
    streamTo(cbox::DataOut& out) const override final
    {
        blox_ActuatorLogic message = blox_ActuatorLogic_init_zero;
        writeMessage(message);

        return streamProtoTo(out, &message, blox_ActuatorLogic_fields, blox_ActuatorLogic_size);
    }

    virtual cbox::CboxError
    streamPersistedTo(cbox::DataOut& out) const override final
    {
        blox_ActuatorLogic message = blox_ActuatorLogic_init_zero;
        writeMessage(message);
        message.result = blox_DigitalState(0); // result doesn't need to be persisted

        return streamProtoTo(out, &message, blox_ActuatorLogic_fields, blox_ActuatorLogic_size);
    }

    virtual cbox::update_t
    update(const cbox::update_t& now) override final
    {
        if (enabled) {
            logic.update();
        }
        return now + 100; // update every 100ms
    }

    virtual void*
    implements(const cbox::obj_type_t& iface) override final
    {
        if (iface == BrewBloxTypes_BlockType_ActuatorLogic) {
            return this; // me!
        }
        return nullptr;
    }
};
