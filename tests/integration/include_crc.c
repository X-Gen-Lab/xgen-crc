/**
 * \file            include_crc.c
 * \brief           Compile the public CRC header independently as C11
 * \author          X-Gen Lab
 */
#include <xgen/crc/crc.h>

_Static_assert(sizeof(uint8_t) == 1, "The CRC API exposes byte input");
