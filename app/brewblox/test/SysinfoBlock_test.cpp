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
    std::string replyWithoutTrace = std::string("deviceId: \"999999999999\"")
                                    + " version: \"" + version + "\" platform: PLATFORM_GCC protocolVersion: \"" + protocolVersion
                                    + "\" releaseDate: \"" + releaseDate + "\" protocolDate: \"" + protocolDate + "\"";

    std::string emptyTrace = "";
    for (uint8_t i = 0; i < 10; i++) {
        emptyTrace += " trace { }";
    }

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
            CHECK(decoded.ShortDebugString() == replyWithoutTrace);
        }
    }

    SECTION("Tracing")
    {
        BrewBloxTestBox testBox;

        auto sendCmd = [&testBox](blox::SysInfo::SysInfoCommand cmd) {
            testBox.reset();

            testBox.put(uint16_t(0)); // msg id
            testBox.put(commands::WRITE_OBJECT);
            testBox.put(cbox::obj_id_t(2));
            testBox.put(uint8_t(0xFF));
            testBox.put(SysInfoBlock::staticTypeId());

            auto message = blox::SysInfo();
            message.set_command(cmd);
            testBox.put(message);

            auto decoded = blox::SysInfo();
            testBox.processInputToProto(decoded);
            return decoded;
        };

        WHEN("A READ_AND_SYS_CMD_TRACE_RESUME command is sent")
        {
            auto decoded = sendCmd(blox::SysInfo_SysInfoCommand_SYS_CMD_TRACE_READ_RESUME);

            THEN("Tracing is unpaused and the reply includes a trace (still empty in test)")
            {
                CHECK(testBox.lastReplyHasStatusOk());
                CHECK(decoded.ShortDebugString() == replyWithoutTrace + emptyTrace);
            }
        }

        WHEN("a read trace command is sent to the SysInfo block")
        {
            auto decoded = sendCmd(blox::SysInfo_SysInfoCommand_SYS_CMD_TRACE_READ);
            std::string emptyTrace = std::string("deviceId: \"999999999999\"") + std::string(" version: \"") + version + std::string("\" platform: PLATFORM_GCC") + std::string(" protocolVersion: \"") + protocolVersion + std::string("\" releaseDate: \"") + releaseDate + std::string("\" protocolDate: \"") + protocolDate + std::string("\"") + " trace { } trace { } trace { } trace { } trace { } trace { } trace { } trace { } trace { } trace { }";

            THEN("The last traced actions from previous run are included (from previous WHEN clause in test)")
            {
                CHECK(testBox.lastReplyHasStatusOk());
                CHECK(decoded.ShortDebugString() == replyWithoutTrace +
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

            AND_WHEN("A SYS_CMD_TRACE_RESUME command is sent")
            {
                auto decoded = sendCmd(blox::SysInfo_SysInfoCommand_SYS_CMD_TRACE_RESUME);
                THEN("The reply has no trace")
                {
                    CHECK(decoded.ShortDebugString() == replyWithoutTrace);
                }

                THEN("Tracing is unpaused and the next trace read includes a non-empty trace")
                {
                    auto decoded = sendCmd(blox::SysInfo_SysInfoCommand_SYS_CMD_TRACE_READ);

                    CHECK(testBox.lastReplyHasStatusOk());
                    CHECK(decoded.ShortDebugString() == replyWithoutTrace +
                                                            " trace { action: WRITE_BLOCK id: 2 type: 256 }"
                                                            " trace { action: PERSIST_BLOCK id: 2 type: 256 }"
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
    }
}