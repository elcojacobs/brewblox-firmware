/*
 * Copyright 2018 BrewPi B.V.
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

#include <catch.hpp>

#include "BrewBloxTestBox.h"
#include "blox/SysInfoBlock.h"
#include "blox/stringify.h"
#include "proto/test/cpp/SysInfo_test.pb.h"

using namespace cbox;

SCENARIO("SysInfo Block")
{
    std::string version = stringify(GIT_VERSION);
    std::string protocolVersion = stringify(PROTO_VERSION);
    std::string releaseDate = stringify(GIT_DATE);
    std::string protocolDate = stringify(PROTO_DATE);
    using commands = cbox::Box::CommandID;

    WHEN("The SysInfo block is read")
    {
        BrewBloxTestBox testBox;

        testBox.reset();

        testBox.put(uint16_t(0)); // msg id
        testBox.put(commands::READ_OBJECT);
        testBox.put(cbox::obj_id_t(2));

        auto decoded = blox::SysInfo();
        testBox.processInputToProto(decoded);

        THEN("The system info is serialized correctly")
        {
            CHECK(testBox.lastReplyHasStatusOk());
            CHECK(decoded.ShortDebugString() == std::string("deviceId: \"999999999999\"") + std::string(" version: \"") + version + std::string("\" platform: gcc") + std::string(" protocolVersion: \"") + protocolVersion + std::string("\" releaseDate: \"") + releaseDate + std::string("\" protocolDate: \"") + protocolDate + std::string("\""));
        }
    }

    WHEN("A read trace command is sent to the SysInfo block")
    {
        BrewBloxTestBox testBox;
        testBox.reset();

        testBox.put(uint16_t(0)); // msg id
        testBox.put(commands::WRITE_OBJECT);
        testBox.put(cbox::obj_id_t(2));
        testBox.put(uint8_t(0xFF));
        testBox.put(SysInfoBlock::staticTypeId());

        auto message = blox::SysInfo();
        message.set_command(blox::SysInfo_Command::SysInfo_Command_READ_TRACE);
        testBox.put(message);

        auto decoded = blox::SysInfo();
        testBox.processInputToProto(decoded);

        THEN("The sysinfo includes a trace of end of the previous run (empty for this test)")
        {
            CHECK(testBox.lastReplyHasStatusOk());
            CHECK(decoded.ShortDebugString() == std::string("deviceId: \"999999999999\"") + std::string(" version: \"") + version + std::string("\" platform: gcc") + std::string(" protocolVersion: \"") + protocolVersion + std::string("\" releaseDate: \"") + releaseDate + std::string("\" protocolDate: \"") + protocolDate + std::string("\"") + " trace { } trace { } trace { } trace { } trace { } trace { } trace { } trace { } trace { } trace { }");
        }

        AND_THEN("Tracing is unpaused and the next trace read includes a non-empty trace")
        {
            testBox.put(uint16_t(0)); // msg id
            testBox.put(commands::WRITE_OBJECT);
            testBox.put(cbox::obj_id_t(2));
            testBox.put(uint8_t(0xFF));
            testBox.put(SysInfoBlock::staticTypeId());

            auto message = blox::SysInfo();
            message.set_command(blox::SysInfo_Command::SysInfo_Command_READ_TRACE);
            testBox.put(message);

            auto decoded = blox::SysInfo();
            testBox.processInputToProto(decoded);

            CHECK(testBox.lastReplyHasStatusOk());
            CHECK(decoded.ShortDebugString() == std::string("deviceId: \"999999999999\"") + std::string(" version: \"") + version + std::string("\" platform: gcc") + std::string(" protocolVersion: \"") + protocolVersion + std::string("\" releaseDate: \"") + releaseDate + std::string("\" protocolDate: \"") + protocolDate + std::string("\"") +
                                                    " trace { }"
                                                    " trace { }"
                                                    " trace { action: UPDATE_BLOCK id: 1 type: 65534 }"
                                                    " trace { action: UPDATE_BLOCK id: 2 type: 256 }"
                                                    " trace { action: UPDATE_BLOCK id: 3 type: 257 }"
                                                    " trace { action: UPDATE_BLOCK id: 4 type: 258 }"
                                                    " trace { action: UPDATE_BLOCK id: 7 type: 314 }"
                                                    " trace { action: UPDATE_BLOCK id: 19 type: 319 }"
                                                    " trace { action: WRITE_BLOCK id: 2 type: 256 }"
                                                    " trace { action: PERSIST_BLOCK id: 2 type: 256 }");
        }
    }
}
