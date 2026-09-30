/**
 * \file            test_crc.cpp
 * \brief           CRC vectors, streaming contracts and bitwise references
 * \author          X-Gen Lab
 */

#include <xgen/crc/crc.h>

#include <array>
#include <gtest/gtest.h>

namespace {

constexpr std::array<uint8_t, 9> check_bytes = {'1', '2', '3', '4', '5',
                                                '6', '7', '8', '9'};

/** \brief           Independent bitwise MAXIM reference, without a table. */
uint8_t reference_crc8(uint8_t seed, const uint8_t* data, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        seed ^= data[i];
        for (size_t bit = 0; bit < 8; ++bit) {
            seed = static_cast<uint8_t>((seed >> 1U) ^
                                        ((seed & 1U) != 0U ? 0x8cU : 0U));
        }
    }
    return seed;
}

/** \brief           Independent bitwise MODBUS reference, without a table. */
uint16_t reference_crc16(uint16_t seed, const uint8_t* data, size_t size) {
    for (size_t i = 0; i < size; ++i) {
        seed ^= data[i];
        for (size_t bit = 0; bit < 8; ++bit) {
            seed = static_cast<uint16_t>((seed >> 1U) ^
                                         ((seed & 1U) != 0U ? 0xa001U : 0U));
        }
    }
    return seed;
}

TEST(Crc8Test, MatchesStandardCheckVector) {
    EXPECT_EQ(xgcrc_crc8_maxim(check_bytes.data(), check_bytes.size()), 0xa1U);
}

TEST(Crc16Test, MatchesStandardCheckVector) {
    EXPECT_EQ(xgcrc_crc16_modbus(check_bytes.data(), check_bytes.size()),
              0x4b37U);
}

TEST(Crc8Test, AcceptsEveryStandardVectorSplit) {
    for (size_t cut = 0; cut <= check_bytes.size(); ++cut) {
        SCOPED_TRACE(cut);
        const uint8_t first = xgcrc_crc8_maxim(check_bytes.data(), cut);
        EXPECT_EQ(xgcrc_crc8_maxim_update(first, check_bytes.data() + cut,
                                          check_bytes.size() - cut),
                  0xa1U);
    }
}

TEST(Crc16Test, AcceptsEveryStandardVectorSplit) {
    for (size_t cut = 0; cut <= check_bytes.size(); ++cut) {
        SCOPED_TRACE(cut);
        const uint16_t first = xgcrc_crc16_modbus(check_bytes.data(), cut);
        EXPECT_EQ(xgcrc_crc16_modbus_update(first, check_bytes.data() + cut,
                                            check_bytes.size() - cut),
                  0x4b37U);
    }
}

TEST(Crc8Test, NullInputPreservesSeedForEveryLength) {
    for (const size_t size : {size_t{0}, size_t{1}, size_t{100}, SIZE_MAX}) {
        SCOPED_TRACE(size);
        EXPECT_EQ(xgcrc_crc8_maxim(nullptr, size), 0U);
        EXPECT_EQ(xgcrc_crc8_maxim_update(7U, nullptr, size), 7U);
        EXPECT_EQ(xgcrc_crc8_maxim_update(0xffU, nullptr, size), 0xffU);
    }
}

TEST(Crc16Test, NullInputPreservesSeedForEveryLength) {
    for (const size_t size : {size_t{0}, size_t{1}, size_t{100}, SIZE_MAX}) {
        SCOPED_TRACE(size);
        EXPECT_EQ(xgcrc_crc16_modbus(nullptr, size), 0xffffU);
        EXPECT_EQ(xgcrc_crc16_modbus_update(123U, nullptr, size), 123U);
        EXPECT_EQ(xgcrc_crc16_modbus_update(0U, nullptr, size), 0U);
    }
}

TEST(Crc8Test, ZeroLengthPreservesSeedWithoutReadingInput) {
    const uint8_t* end = check_bytes.data() + check_bytes.size();
    EXPECT_EQ(xgcrc_crc8_maxim(end, 0U), 0U);
    EXPECT_EQ(xgcrc_crc8_maxim_update(0xabU, end, 0U), 0xabU);
}

TEST(Crc16Test, ZeroLengthPreservesSeedWithoutReadingInput) {
    const uint8_t* end = check_bytes.data() + check_bytes.size();
    EXPECT_EQ(xgcrc_crc16_modbus(end, 0U), 0xffffU);
    EXPECT_EQ(xgcrc_crc16_modbus_update(0x1234U, end, 0U), 0x1234U);
}

TEST(Crc8Test, MatchesAllOneByteInputsAndSeeds) {
    for (unsigned int seed = 0; seed < 256U; ++seed) {
        for (unsigned int value = 0; value < 256U; ++value) {
            const uint8_t byte = static_cast<uint8_t>(value);
            const uint8_t initial = static_cast<uint8_t>(seed);
            ASSERT_EQ(xgcrc_crc8_maxim_update(initial, &byte, 1U),
                      reference_crc8(initial, &byte, 1U))
                << "seed=" << seed << ", value=" << value;
        }
    }
}

TEST(Crc16Test, MatchesAllOneByteInputsAndRepresentativeSeeds) {
    for (const uint16_t seed : {uint16_t{0}, uint16_t{1}, uint16_t{0x1234},
                                uint16_t{0x8000}, uint16_t{0xffff}}) {
        for (unsigned int value = 0; value < 256U; ++value) {
            const uint8_t byte = static_cast<uint8_t>(value);
            ASSERT_EQ(xgcrc_crc16_modbus_update(seed, &byte, 1U),
                      reference_crc16(seed, &byte, 1U))
                << "seed=" << seed << ", value=" << value;
        }
    }
}

TEST(CrcStreamingTest, MatchesEveryRandomPrefixAndMidpointSplit) {
    std::array<uint8_t, 1024> bytes{};
    uint32_t random_state = UINT32_C(0x31415926);
    for (uint8_t& byte : bytes) {
        random_state ^= random_state << 13U;
        random_state ^= random_state >> 17U;
        random_state ^= random_state << 5U;
        byte = static_cast<uint8_t>(random_state);
    }
    const auto original = bytes;
    for (size_t size = 0; size <= bytes.size(); ++size) {
        SCOPED_TRACE(size);
        const uint8_t crc8 = reference_crc8(0U, bytes.data(), size);
        const uint16_t crc16 = reference_crc16(0xffffU, bytes.data(), size);
        EXPECT_EQ(xgcrc_crc8_maxim(bytes.data(), size), crc8);
        EXPECT_EQ(xgcrc_crc16_modbus(bytes.data(), size), crc16);
        const size_t cut = size / 2U;
        EXPECT_EQ(xgcrc_crc8_maxim_update(xgcrc_crc8_maxim(bytes.data(), cut),
                                          bytes.data() + cut, size - cut),
                  crc8);
        EXPECT_EQ(
            xgcrc_crc16_modbus_update(xgcrc_crc16_modbus(bytes.data(), cut),
                                      bytes.data() + cut, size - cut),
            crc16);
    }
    EXPECT_EQ(bytes, original);
}

TEST(CrcStreamingTest, PreservesIndependentStreamsAndNondefaultSeeds) {
    uint8_t first_crc8 = 0x42U;
    uint8_t second_crc8 = 0xe7U;
    uint16_t first_crc16 = 0x1234U;
    uint16_t second_crc16 = 0xabcdU;
    for (size_t i = 0; i < check_bytes.size(); ++i) {
        first_crc8 = xgcrc_crc8_maxim_update(first_crc8, &check_bytes[i], 1U);
        second_crc8 = xgcrc_crc8_maxim_update(second_crc8, &check_bytes[i], 1U);
        first_crc16 =
            xgcrc_crc16_modbus_update(first_crc16, &check_bytes[i], 1U);
        second_crc16 =
            xgcrc_crc16_modbus_update(second_crc16, &check_bytes[i], 1U);
    }
    EXPECT_EQ(first_crc8, reference_crc8(0x42U, check_bytes.data(), 9U));
    EXPECT_EQ(second_crc8, reference_crc8(0xe7U, check_bytes.data(), 9U));
    EXPECT_EQ(first_crc16, reference_crc16(0x1234U, check_bytes.data(), 9U));
    EXPECT_EQ(second_crc16, reference_crc16(0xabcdU, check_bytes.data(), 9U));
}

} /* namespace */
