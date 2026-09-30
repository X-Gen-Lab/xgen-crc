/**
 * \file            include_version.c
 * \brief           Compile the generated version header independently as C11
 * \author          X-Gen Lab
 */
#include <xgen/crc/version.h>

_Static_assert(XGCRC_ABI_VERSION == 1, "Unexpected CRC ABI");
