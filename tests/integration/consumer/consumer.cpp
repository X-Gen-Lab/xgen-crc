/**
 * \file            consumer.cpp
 * \brief           Verify that C++ consumers link the public C declarations
 * \author          X-Gen Lab
 */
#include <xgen/crc/crc.h>
#include <xgen/crc/version.h>

int main() {
    const uint8_t bytes[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
    static_assert(XGCRC_ABI_VERSION == 1, "Unexpected CRC ABI");
#if XGCRC_CONSUME_CRC8
    if (xgcrc_crc8_maxim_update(0U, bytes, sizeof(bytes)) != 0xa1U) {
        return 1;
    }
#endif
#if XGCRC_CONSUME_CRC16
    if (xgcrc_crc16_modbus_update(0xffffU, bytes, sizeof(bytes)) != 0x4b37U) {
        return 2;
    }
#endif
    return 0;
}
