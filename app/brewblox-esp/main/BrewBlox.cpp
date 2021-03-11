/*
 * Copyright 2020 BrewPi B.V.
 *
 * This file is part of BrewBlox.
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

#include "AppTicks.h"
#include "Logger.h"
//#include "blox/AcuatorAnalogMockBlock.h"
//#include "blox/TempSensorMock.h"
#include "blox/TicksBlock.h"
#include "cbox/ArrayEepromAccess.h"
#include "cbox/Box.h"
#include "cbox/Connections.h"
#include "cbox/EepromObjectStorage.h"
#include "cbox/ObjectContainer.h"
#include "cbox/ObjectFactory.h"
#include "cbox/Tracing.h"
#include <memory>

// #include "cbox/spark/ConnectionsSerial.h"
// #include "cbox/spark/ConnectionsTcp.h"

cbox::ConnectionPool&
theConnectionPool()
{
    static cbox::ConnectionPool connections = {};
    return connections;
}

cbox::Box&
makeBrewBloxBox()
{
    static cbox::ObjectContainer objects({
        // groups will be at position 1
        // cbox::ContainedObject(2, 0x80, std::make_shared<SysInfoBlock>()),
        cbox::ContainedObject(3, 0x80, std::make_shared<TicksBlock<TicksClass>>(ticks)),
    });

    static const cbox::ObjectFactory objectFactory{
        //{TempSensorMockBlock::staticTypeId(), std::make_shared<TempSensorMockBlock>},
        //{ActuatorAnalogMockBlock::staticTypeId(), []() { return std::make_shared<ActuatorAnalogMockBlock>(objects); }},
        // {PidBlock::staticTypeId(), []() { return std::make_shared<PidBlock>(objects); }},
        // {ActuatorPwmBlock::staticTypeId(), []() { return std::make_shared<ActuatorPwmBlock>(objects); }},
        // {ActuatorOffsetBlock::staticTypeId(), []() { return std::make_shared<ActuatorOffsetBlock>(objects); }},
        // {BalancerBlock::staticTypeId(), std::make_shared<BalancerBlock>},
        // {MutexBlock::staticTypeId(), std::make_shared<MutexBlock>},
        // {SetpointProfileBlock::staticTypeId(), []() { return std::make_shared<SetpointProfileBlock>(objects); }},
        // {DS2413Block::staticTypeId(), std::make_shared<DS2413Block>},
        // {DigitalActuatorBlock::staticTypeId(), []() { return std::make_shared<DigitalActuatorBlock>(objects); }},
        // {DS2408Block::staticTypeId(), std::make_shared<DS2408Block>},
        // {MotorValveBlock::staticTypeId(), []() { return std::make_shared<MotorValveBlock>(objects); }},
        // {ActuatorLogicBlock::staticTypeId(), []() { return std::make_shared<ActuatorLogicBlock>(objects); }},
        //{MockPinsBlock::staticTypeId(), []() { return std::make_shared<MockPinsBlock>(); }},
        //{TempSensorCombiBlock::staticTypeId(), []() { return std::make_shared<TempSensorCombiBlock>(objects); }},
    };

    static cbox::ArrayEepromAccess<2000> eeprom;
    static cbox::EepromObjectStorage objectStore(eeprom);
    static cbox::ConnectionPool& connections = theConnectionPool();

    std::vector<std::unique_ptr<cbox::ScanningFactory>> scanningFactories;
    //scanningFactories.reserve(1);
    //scanningFactories.push_back(std::make_unique<OneWireScanningFactory>(objects, theOneWire()));

    static cbox::Box box(objectFactory, objects, objectStore, connections, std::move(scanningFactories));

    return box;
}

cbox::Box&
brewbloxBox()
{
    static cbox::Box& box = makeBrewBloxBox();
    return box;
}

Logger&
logger()
{
    static Logger logger([](Logger::LogLevel level, const std::string& log) {
        cbox::DataOut& out = theConnectionPool().logDataOut();
        out.write('<');
        const char debug[] = "DEBUG";
        const char info[] = "INFO";
        const char warn[] = "WARNING";
        const char err[] = "ERROR";

        switch (level) {
        case Logger::LogLevel::DEBUG:
            out.writeBuffer(debug, strlen(debug));
            break;
        case Logger::LogLevel::INFO:
            out.writeBuffer(info, strlen(info));
            break;
        case Logger::LogLevel::WARN:
            out.writeBuffer(warn, strlen(warn));
            break;
        case Logger::LogLevel::ERROR:
            out.writeBuffer(err, strlen(err));
            break;
        }
        out.write(':');
        for (const auto& c : log) {
            out.write(c);
        }
        out.write('>');
    });
    return logger;
}

void
logEvent(const std::string& event)
{
    cbox::DataOut& out = theConnectionPool().logDataOut();
    out.write('<');
    out.write('!');
    for (const auto& c : event) {
        out.write(c);
    }
    out.write('>');
}

void
updateBrewbloxBox()
{
    brewbloxBox().update(ticks.millis());
#if PLATFORM_ID == 3
    ticks.delayMillis(10); // prevent 100% cpu usage
#endif
}

const char*
versionCsv()
{
    static const char version[] = "esp"; //stringify(GIT_VERSION) "," stringify(PROTO_VERSION) "," stringify(GIT_DATE) "," stringify(PROTO_DATE) "," stringify(SYSTEM_VERSION_STRING) "," PLATFORM_STRING;
    return version;
}

namespace cbox {
void
connectionStarted(DataOut& out)
{
    char header[] = "<!BREWBLOX>";

    out.writeBuffer(header, strlen(header));
    //     out.writeBuffer(versionCsv(), strlen(versionCsv()));
    //     out.write(',');
    //     cbox::EncodedDataOut hexOut(out);

    // #if PLATFORM_ID == 3
    //     int resetReason = 0;
    // #else
    //     auto resetReason = System.resetReason();
    // #endif
    //     hexOut.write(resetReason);
    //     out.write(',');
    // #if PLATFORM_ID == 3
    //     int resetData = 0;
    // #else
    //     auto resetData = System.resetReasonData();
    // #endif
    //     hexOut.write(resetData);
    //     out.write(',');

    //     uint8_t deviceId[12];
    //     HAL_device_ID(deviceId, 12);
    //     hexOut.writeBuffer(deviceId, 12);
    //     out.write('>');
}

// handler for custom commands outside of controlbox
bool
applicationCommand(uint8_t cmdId, cbox::DataIn& in, cbox::EncodedDataOut& out)
{
    switch (cmdId) {
        //     case 100: // firmware update
        //     {
        //         CboxError status = CboxError::OK;
        //         in.spool();
        //         if (out.crc()) {
        //             status = CboxError::CRC_ERROR_IN_COMMAND;
        //         }
        //         out.writeResponseSeparator();
        //         out.write(asUint8(status));
        //         out.endMessage();
        //         ticks.delayMillis(10);
        //         if (status == CboxError::OK) {
        //             cbox::tracing::add(AppTrace::FIRMWARE_UPDATE_STARTED);
        //             changeLedColor();
        //             brewbloxBox().disconnect();
        //             ticks.delayMillis(10);
        // #if PLATFORM_ID != PLATFORM_GCC
        //             updateFirmwareFromStream(in.streamType());
        //             // reset in case the firmware update failed
        //             System.reset(RESET_USER_REASON::FIRMWARE_UPDATE_FAILED);
        // #endif
        //         }
        //         return true;
        //     }
    }
    return false;
}
} // end namespace cbox