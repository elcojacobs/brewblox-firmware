cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

set(srcs 
  main.cpp
  # App.cpp
  AppTicks.cpp
  brewblox_esp.cpp
  TimerInterrupts.cpp
 )

file(GLOB network_srcs ./network/*.cpp)
file(GLOB http_srcs ./httpserver/*.cpp)

file(GLOB cbox_srcs ../../../controlbox/src/cbox/*.cpp)
set(cbox_inc "../../../controlbox/src/")

file(GLOB brewblox_srcs ../../../brewblox/*.cpp ../../../brewblox/blox/*.cpp ../../../brewblox/blox/proto/cpp/*.c)
set(brewblox_inc "../../../brewblox/")

file(GLOB esp_srcs ../../../platform/esp/*.cpp ../../../platform/esp/*.c)
set(esp_inc "../../../platform/esp/")

file(GLOB lib_srcs ../../../lib/src/*.cpp)
set(lib_inc "../../../lib/inc/")

set(cnl_inc "../../../lib/cnl/include")

file(GLOB nanopb_srcs ../../../platform/spark/device-os/third_party/nanopb/nanopb/*.c)
set(nanopb_inc "../../../platform/spark/device-os/third_party/nanopb/nanopb")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGIT_VERSION=TODO")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPROTO_VERSION=TODO")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGIT_DATE=TODO")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPROTO_DATE=TODO")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPLATFORM_ID=100")

idf_component_register(
  SRCS
    ${srcs}
    ${cbox_srcs}
    ${brewblox_srcs}
    ${lib_srcs}
    ${esp_srcs}
    ${nanopb_srcs}
    ${network_srcs}
    ${http_srcs}
  INCLUDE_DIRS 
    .
    ../../../brewblox/blox/proto/cpp
    ${brewblox_inc}
    ${nanopb_inc}
    ${cbox_inc}
    ${lib_inc}
    ${cnl_inc}
    ${esp_inc}
)