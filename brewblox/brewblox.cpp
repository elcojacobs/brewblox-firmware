/*
 * Copyright 2021 BrewPi B.V.
 *
 * This file is part of Brewblox.
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

#include "brewblox.hpp"
#include "blox/ActuatorAnalogMockBlock.h"
#include "blox/ActuatorLogicBlock.h"
#include "blox/ActuatorOffsetBlock.h"
#include "blox/ActuatorPwmBlock.h"
#include "blox/BalancerBlock.h"
#include "blox/DS2408Block.h"
#include "blox/DS2413Block.h"
#include "blox/DigitalActuatorBlock.h"
#include "blox/MockPinsBlock.h"
#include "blox/MotorValveBlock.h"
#include "blox/MutexBlock.h"
#include "blox/PidBlock.h"
#include "blox/SetpointProfileBlock.h"
#include "blox/SetpointSensorPairBlock.h"
#include "blox/TempSensorCombiBlock.h"
#include "blox/TempSensorMockBlock.h"
#include "blox/TempSensorOneWireBlock.h"
#include "blox/stringify.h"
#include <memory>

namespace brewblox {

cbox::ObjectFactory combine_factories(std::vector<cbox::ObjectFactoryEntry>&& a,
                                      std::vector<cbox::ObjectFactoryEntry>&& b)
{
    a.insert(std::end(a),
             std::make_move_iterator(std::begin(b)),
             std::make_move_iterator(std::end(b)));
    return cbox::ObjectFactory(std::move(a));
}

cbox::Box& make_box(cbox::ObjectContainer&& systemObjects,
                    std::vector<cbox::ObjectFactoryEntry>&& platformFactories,
                    cbox::ObjectStorage& storage,
                    cbox::ConnectionPool& connectionPool,
                    std::vector<std::unique_ptr<cbox::ScanningFactory>>&& scanners)
{

    static cbox::ObjectContainer objects = std::move(systemObjects);

    static const cbox::ObjectFactory factories = combine_factories(
        std::vector<cbox::ObjectFactoryEntry>{
            //{TempSensorOneWireBlock::staticTypeId(), std::make_shared<TempSensorOneWireBlock>},
            {SetpointSensorPairBlock::staticTypeId(), []() { return std::make_shared<SetpointSensorPairBlock>(objects); }},
            {TempSensorMockBlock::staticTypeId(), std::make_shared<TempSensorMockBlock>},
            {ActuatorAnalogMockBlock::staticTypeId(), []() { return std::make_shared<ActuatorAnalogMockBlock>(objects); }},
            {PidBlock::staticTypeId(), []() { return std::make_shared<PidBlock>(objects); }},
            {ActuatorPwmBlock::staticTypeId(), []() { return std::make_shared<ActuatorPwmBlock>(objects); }},
            {ActuatorOffsetBlock::staticTypeId(), []() { return std::make_shared<ActuatorOffsetBlock>(objects); }},
            {BalancerBlock::staticTypeId(), std::make_shared<BalancerBlock>},
            {MutexBlock::staticTypeId(), std::make_shared<MutexBlock>},
            {SetpointProfileBlock::staticTypeId(), []() { return std::make_shared<SetpointProfileBlock>(objects); }},
            //{DS2413Block::staticTypeId(), std::make_shared<DS2413Block>},
            {DigitalActuatorBlock::staticTypeId(), []() { return std::make_shared<DigitalActuatorBlock>(objects); }},
            //{DS2408Block::staticTypeId(), std::make_shared<DS2408Block>},
            {MotorValveBlock::staticTypeId(), []() { return std::make_shared<MotorValveBlock>(objects); }},
            {ActuatorLogicBlock::staticTypeId(), []() { return std::make_shared<ActuatorLogicBlock>(objects); }},
            {MockPinsBlock::staticTypeId(), []() { return std::make_shared<MockPinsBlock>(); }},
            {TempSensorCombiBlock::staticTypeId(), []() { return std::make_shared<TempSensorCombiBlock>(objects); }},
        },
        std::move(platformFactories));

    static cbox::Box box(factories, objects, storage, connectionPool, std::move(scanners));

    return box;
}

} // end namespace brewlbox

const std::string&
versionCsv()
{
#if PLATFORM_ID == 3
#define PLATFORM_STRING "gcc"
#elif PLATFORM_ID == 6
#define PLATFORM_STRING "photon"
#elif PLATFORM_ID == 8
#define PLATFORM_STRING "p1"
#elif PLATFORM_ID == 100
#define PLATFORM_STRING "esp32"
#else
#define PLATFORM_STRING "unkown"
#endif

    static const std::string version = stringify(GIT_VERSION) "," stringify(PROTO_VERSION) "," stringify(GIT_DATE) "," stringify(PROTO_DATE) "," stringify(SYSTEM_VERSION_STRING) "," PLATFORM_STRING;
    return version;
}
