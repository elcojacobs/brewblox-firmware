cmake_minimum_required(VERSION 3.10)

# set the project name
project(simulator)    

find_package(Threads REQUIRED)

add_definitions(-DLV_CONF_INCLUDE_SIMPLE -DLV_LVGL_H_INCLUDE_SIMPLE)



file(GLOB SOURCES 
"main/*.cpp"
"main/graphics/fonts/*.cpp"
"../../platform/simulator/*.cpp"
)

include_directories(
    ../../platform/simulator
    main
    components/lvgl/src
    )


add_subdirectory (components/lvgl)

# add the executable
add_executable(simulator ${SOURCES})
target_link_libraries (simulator LINK_PUBLIC lvgl)
target_link_libraries(simulator PRIVATE Threads::Threads)
