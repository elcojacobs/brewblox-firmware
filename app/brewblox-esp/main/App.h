/*
Smooth - A C++ framework for embedded programming on top of Espressif's ESP-IDF
Copyright 2019 Per Malmberg (https://gitbub.com/PerMalmberg)

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
*/

#pragma once

#include "StreamingClient.h"
#include "StreamingProtocol.h"
#include "smooth/core/Application.h"
#include "smooth/core/ipc/IEventListener.h"
#include "smooth/core/ipc/TaskEventQueue.h"
#include "smooth/core/network/Ethernet.h"
#include "smooth/core/network/SecureSocket.h"
#include "smooth/core/network/ServerSocket.h"
#include "smooth/core/network/Socket.h"
#include <functional>

class App : public smooth::core::EarlyInit {
public:
    App();

    void init() override;

private:
    std::shared_ptr<smooth::core::network::ServerSocket<StreamingClient,
                                                        StreamingProtocol, void>>
        server{};

    smooth::core::network::Ethernet ethernet;
};
