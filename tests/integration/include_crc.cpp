/**
 * \file            include_crc.cpp
 * \brief           Compile the public CRC header independently as C++17
 * \author          X-Gen Lab
 */
#include <xgen/crc/crc.h>

static_assert(sizeof(uint8_t) == 1, "The CRC API exposes byte input");
