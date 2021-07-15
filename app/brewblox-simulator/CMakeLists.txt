cmake_minimum_required(VERSION 3.10)

# set the project name
project(simulator)    
set(CMAKE_CXX_STANDARD 17)
find_package(Threads REQUIRED)

add_definitions(-DLV_CONF_INCLUDE_SIMPLE -DLV_LVGL_H_INCLUDE_SIMPLE)



file(GLOB SOURCES 
main/*.cpp
../../lib/inc/graphics/fonts/*.cpp
../../platform/simulator/*.cpp
../../brewblox/blox/proto/cpp/*.c
../../platform/spark/device-os/third_party/nanopb/nanopb/*.c
../../brewblox/blox
../../controlbox/src/
)

include_directories(
    ../../platform/simulator
    ../../platform/all
    ../../lib/inc
    ../../lib/cnl/include
    ../../brewblox/blox/proto/cpp
    ../../brewblox
    ../../platform/spark/device-os/third_party/nanopb/nanopb
    ../../controlbox/src/
    main
    components/lvgl/src
    )


add_subdirectory (components/lvgl)

# add the executable
add_executable(simulator ${SOURCES})
target_link_libraries (simulator LINK_PUBLIC lvgl)
target_link_libraries(simulator PRIVATE Threads::Threads)
