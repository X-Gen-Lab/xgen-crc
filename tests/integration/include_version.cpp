/**
 * \file            include_version.cpp
 * \brief           Compile the generated version header independently as C++17
 * \author          X-Gen Lab
 */
#include <xgen/crc/version.h>

static_assert(XGCRC_ABI_VERSION == 1, "Unexpected CRC ABI");
