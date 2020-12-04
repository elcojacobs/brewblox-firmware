cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

set(srcs 
  main.cpp
  App.cpp
  AppTicks.cpp
  BrewBlox.cpp
 )

file(GLOB network_srcs ./network/*.cpp)
file(GLOB http_srcs ./httpserver/*.cpp)

file(GLOB cbox_srcs ../../../controlbox/src/*.cpp)
set(cbox_inc "../../../controlbox/src/")

file(GLOB esp_srcs ../../../platform/esp/*.cpp)
set(esp_inc "../../../platform/esp/")

file(GLOB lib_srcs ../../../lib/src/*.cpp)
set(lib_inc "../../../lib/inc/")

set(cnl_inc "../../../lib/cnl/include")

file(GLOB nanopb_srcs platform/spark/device-os/third_party/nanopb/nanopb/*.c)
set(nanopb_inc "../../../platform/spark/device-os/third_party/nanopb/nanopb")


idf_component_register(
  SRCS
    ${srcs}
    ${cbox_srcs}
    ${lib_srcs}
    ${esp_srcs}
    ${nanopb_srcs}
    ${network_srcs}
    ${http_srcs}
  INCLUDE_DIRS 
    .
    ../../brewblox/proto/cpp
    ../../brewblox
    ${nanopb_inc}
    ${cbox_inc}
    ${lib_inc}
    ${cnl_inc}
    ${esp_inc}
  # REQUIRES
  #   smooth_component
)