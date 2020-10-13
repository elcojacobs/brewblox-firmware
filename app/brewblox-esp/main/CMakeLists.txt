set(srcs "ethernet_example_main.cpp" "AppTicks.cpp")

file(GLOB cbox_srcs ../../../controlbox/src/*.cpp)
set(cbox_inc "../../../controlbox/src/")

file(GLOB esp_srcs ../../../platform/esp/*.cpp)
set(esp_inc "../../../platform/esp/")

idf_component_register(SRCS "${srcs}" "${cbox_srcs}" "${esp_srcs}"
                       INCLUDE_DIRS "." "../../../lib/inc/" "../../brewblox/proto/cpp" "../../../platform/nanopb" "${cbox_inc}" "${esp_inc}"
                       )

component_compile_options(-std=c++14)