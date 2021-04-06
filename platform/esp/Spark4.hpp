#pragma once
#include "SX1508.hpp"

class Spark4 {
public:
    Spark4() = default;
    ~Spark4() = default;

    static void hw_init();
    static void hw_deinit();
    static void startup_beep();

    static SX1508 expander;
};
