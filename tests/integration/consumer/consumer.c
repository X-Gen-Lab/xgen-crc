/**
 * \file            consumer.c
 * \brief           Exercise only the selected installed or source CRC targets
 * \author          X-Gen Lab
 */
#include <xgen/crc/crc.h>
#include <xgen/crc/version.h>

int main(void) {
    const uint8_t bytes[] = {'1', '2', '3', '4', '5', '6', '7', '8', '9'};
    if (XGCRC_VERSION_MAJOR != 0 || XGCRC_VERSION_MINOR != 1 ||
        XGCRC_VERSION_PATCH != 0 || XGCRC_ABI_VERSION != 1) {
        return 1;
    }
#if XGCRC_CONSUME_CRC8
    if (xgcrc_crc8_maxim(bytes, sizeof(bytes)) != UINT8_C(0xa1)) {
        return 2;
    }
#endif
#if XGCRC_CONSUME_CRC16
    if (xgcrc_crc16_modbus(bytes, sizeof(bytes)) != UINT16_C(0x4b37)) {
        return 3;
    }
#endif
    return 0;
}
