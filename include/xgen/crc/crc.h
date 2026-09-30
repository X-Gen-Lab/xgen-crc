/**
 * \file            crc.h
 * \brief           Allocation-free MAXIM CRC8 and MODBUS CRC16 calculations
 * \author          X-Gen Lab
 */

#ifndef XGCRC_CRC_H
#define XGCRC_CRC_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * \brief           Calculate reflected MAXIM CRC8 with initial value zero.
 * \param[in]       data: Borrowed input bytes; NULL returns the initial value.
 * \param[in]       len: Readable byte count when data is not NULL.
 * \return          CRC value without a final XOR or byte encoding.
 * \note            Polynomial 0x31, reflected input/output, initial value 0.
 *                  NULL is accepted for every len; a zero len reads no bytes.
 *                  Non-NULL input must contain at least len readable bytes.
 * \note            The synchronous call retains no pointers, allocates no
 *                  memory, and performs work proportional to len. It is
 *                  reentrant; callers must prevent concurrent input writes.
 *                  ISR callers must provide a suitable processing budget.
 */
uint8_t xgcrc_crc8_maxim(const uint8_t* data, size_t len);

/**
 * \brief           Continue a reflected MAXIM CRC8 calculation.
 * \param[in]       crc: Previous CRC value, or an explicit initial seed.
 * \param[in]       data: Borrowed input bytes; NULL leaves crc unchanged.
 * \param[in]       len: Readable byte count when data is not NULL.
 * \return          Updated CRC, or crc unchanged for NULL or zero length.
 * \note            The input, lifetime, concurrency and execution contracts
 *                  of xgcrc_crc8_maxim() also apply. There is no final XOR;
 *                  updating adjacent chunks equals one contiguous update.
 */
uint8_t xgcrc_crc8_maxim_update(uint8_t crc, const uint8_t* data, size_t len);

/**
 * \brief           Calculate reflected MODBUS CRC16 with initial value 0xffff.
 * \param[in]       data: Borrowed input bytes; NULL returns the initial value.
 * \param[in]       len: Readable byte count when data is not NULL.
 * \return          CRC value without a final XOR or byte encoding.
 * \note            Polynomial 0x8005, reflected input/output, seed 0xffff.
 *                  NULL is accepted for every len; a zero len reads no bytes.
 *                  Non-NULL input must contain at least len readable bytes.
 * \note            The synchronous call retains no pointers, allocates no
 *                  memory, and performs work proportional to len. It is
 *                  reentrant; callers must prevent concurrent input writes.
 *                  ISR callers must provide a suitable processing budget.
 */
uint16_t xgcrc_crc16_modbus(const uint8_t* data, size_t len);

/**
 * \brief           Continue a reflected MODBUS CRC16 calculation.
 * \param[in]       crc: Previous CRC value, or an explicit initial seed.
 * \param[in]       data: Borrowed input bytes; NULL leaves crc unchanged.
 * \param[in]       len: Readable byte count when data is not NULL.
 * \return          Updated CRC, or crc unchanged for NULL or zero length.
 * \note            The input, lifetime, concurrency and execution contracts
 *                  of xgcrc_crc16_modbus() also apply. There is no final XOR;
 *                  updating adjacent chunks equals one contiguous update.
 */
uint16_t xgcrc_crc16_modbus_update(uint16_t crc, const uint8_t* data,
                                   size_t len);

#ifdef __cplusplus
}
#endif

#endif
