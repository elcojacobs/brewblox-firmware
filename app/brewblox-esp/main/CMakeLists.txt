cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

set(srcs 
  main.cpp
  App.cpp
  AppTicks.cpp
  BrewBlox.cpp
 )

file(GLOB cbox_srcs ../../../controlbox/src/*.cpp)
set(cbox_inc "../../../controlbox/src/")

file(GLOB esp_srcs ../../../platform/esp/*.cpp)
set(esp_inc "../../../platform/esp/")

file(GLOB nanopb_srcs platform/spark/device-os/third_party/nanopb/nanopb/*.c)
set(nanopb_inc "../../../platform/spark/device-os/third_party/nanopb/nanopb")


idf_component_register(
  SRCS
    ${srcs}
    ${cbox_srcs}
    ${esp_srcs}
    ${nanopb_srcs}
  INCLUDE_DIRS 
    .
    ../../../lib/inc/
    ../../brewblox/proto/cpp
    ../../brewblox
    ${nanopb_inc}
    ${cbox_inc}
    ${esp_inc}
  REQUIRES
    smooth_component
)