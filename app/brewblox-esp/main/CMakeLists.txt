cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 20)

set(srcs 
  main.cpp
  # App.cpp
  AppTicks.cpp
  brewblox_esp.cpp
  ExpansionGpio.cpp
  TimerInterrupts.cpp
  network/CboxConnection.cpp
  network/CboxConnectionManager.cpp
 )

 get_filename_component(ESP_ROOT
 ${CMAKE_CURRENT_LIST_DIR}/..
 ABSOLUTE)

 get_filename_component(REPO_ROOT
 ${ESP_ROOT}/../..
 ABSOLUTE)

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


execute_process (
    COMMAND git rev-parse --short=8 HEAD
    OUTPUT_VARIABLE GIT_VERSION
    WORKING_DIRECTORY ${REPO_ROOT}
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process (
    COMMAND git log -1 --format=%cd --date=short
    OUTPUT_VARIABLE GIT_DATE
    WORKING_DIRECTORY ${REPO_ROOT}
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process (
    COMMAND git rev-parse --short=8 HEAD
    OUTPUT_VARIABLE PROTO_VERSION
    WORKING_DIRECTORY ${REPO_ROOT}/brewblox/blox/proto
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

execute_process (
    COMMAND git log -1 --format=%cd --date=short
    OUTPUT_VARIABLE PROTO_DATE
    WORKING_DIRECTORY ${REPO_ROOT}/brewblox/blox/proto
    OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(SYSTEM_VERSION_STRING ${IDF_VERSION_MAJOR}.${IDF_VERSION_MINOR}.${IDF_VERSION_PATCH})
message(STATUS "Git version: " ${GIT_VERSION})
message(STATUS "Git date: " ${GIT_DATE})
message(STATUS "Proto version: " ${PROTO_VERSION})
message(STATUS "Proto date: " ${PROTO_DATE})
message(STATUS "IDF version: " ${SYSTEM_VERSION_STRING})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGIT_VERSION=${GIT_VERSION}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGIT_DATE=${GIT_DATE}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPROTO_VERSION=${PROTO_VERSION}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPROTO_DATE=${PROTO_DATE}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DPLATFORM_ID=100")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DSYSTEM_VERSION_STRING=${SYSTEM_VERSION_STRING}")

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
    ${REPO_ROOT}/brewblox/blox/proto/cpp
    ${brewblox_inc}
    ${nanopb_inc}
    ${cbox_inc}
    ${lib_inc}
    ${cnl_inc}
    ${esp_inc}
)
