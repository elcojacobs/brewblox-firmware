#pragma once

#include <stdint.h>

uint8_t
OneWireCrc8(const uint8_t* addr, uint8_t len);

uint16_t
OneWireCrc16(const uint8_t* input, uint16_t len);
uint16_t
OneWireCrc16Update(uint8_t input, uint16_t crc);
