/*
 * Copyright (c) 2024 Huawei Device Co., Ltd.
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#ifndef GRAPHICS_EFFECT_GE_HASH_REFER_H
#define GRAPHICS_EFFECT_GE_HASH_REFER_H
#include <cerrno>
#include <cstdint>
#include <cstring>
#include <memory>
#include <securec.h>

namespace OHOS {
namespace GraphicsEffectEngine {

enum Constants {
    CONST_0 = 0,
    CONST_1 = 1,
    CONST_2 = 2,
    CONST_3 = 3,
    CONST_4 = 4,
    CONST_8 = 8,
    CONST_16 = 16,
    CONST_24 = 24,
    CONST_32 = 32,
    CONST_40 = 40,
    CONST_48 = 48,
    CONST_64 = 64
};

// the default secret parameters
static const uint64_t _secret[CONST_4] = { 0xa0761d6478bd642full, 0xe7037ed1a0b428dbull,
    0x8ebc6af09c88c6e3ull, 0x589965cc75374cc3ull };

// Reference: https://skia.googlesource.com/skia/+/2e551697dc56/src/core/SkChecksum.cpp?autodive=0%2F%2F
// wyhash, a fast and good hash function, from https://github.com/wangyi-fudan/wyhash
// Likely and Unlikely macros
#if defined(__GNUC__) || defined(__INTEL_COMPILER) || defined(__clang__)
inline bool Likely(bool expression)
{
    return __builtin_expect(static_cast<bool>(expression), true);
}

inline bool Unlikely(bool expression)
{
    return __builtin_expect(static_cast<bool>(expression), false);
}
#else
inline bool Likely(bool expression)
{
    return expression;
}

inline bool Unlikely(bool expression)
{
    return expression;
}
#endif /*  */

// 128bit multiply function
#if defined(__SIZEOF_INT128__)
static inline void Multiply128bit(uint64_t* num1, uint64_t* num2)
{
    __uint128_t r = *num1;
    r *= *num2;
    *num1 = (uint64_t)r;
    *num2 = (uint64_t)(r >> CONST_64);
}
#elif defined(_MSC_VER) && defined(_M_X64)
static inline void Multiply128bit(uint64_t* num1, uint64_t* num2)
{
    *num1 = _umul128(*num1, *num2, num2);
}
#else
static void Multiply128bitInner(uint64_t* lowerBits, uint64_t* higherBits)
{
    uint64_t higherA = *lowerBits >> CONST_32;
    uint64_t higherB = *higherBits >> CONST_32;
    uint64_t lowerA = (uint32_t)*lowerBits;
    uint64_t lowerB = (uint32_t)*higherBits;
    uint64_t higherResult;
    uint64_t lowerResult;
    uint64_t higherProduct = higherA * higherB;
    uint64_t midProduct0 = higherA * lowerB;
    uint64_t midProduct1 = higherB * lowerA;
    uint64_t lowerProduct = lowerA * lowerB;
    uint64_t temp = lowerProduct + (midProduct0 << CONST_32);
    uint64_t carry = temp < lowerProduct;
    lowerResult = temp + (midProduct1 << CONST_32);
    carry += lowerResult < temp;
    higherResult = higherProduct + (midProduct0 >> CONST_32) + (midProduct1 >> CONST_32) + carry;
    *lowerBits = lowerResult;
    *higherBits = higherResult;
}

static inline void Multiply128bit(uint64_t* lowerBits, uint64_t* higherBits)
{
    Multiply128bitInner(lowerBits, higherBits);
}
#endif

// multiply and xor mix function, aka MUM
static inline uint64_t MultiplyXorMix(uint64_t num1, uint64_t num2)
{
    Multiply128bit(&num1, &num2);
    return num1 ^ num2;
}

// read functions
static inline uint64_t Read8(const uint8_t* p)
{
    uint64_t value;
    errno_t err = memcpy_s(&value, CONST_8, p, CONST_8);
    if (err != 0) {
        // Handle error if needed, here we could set value to 0 or handle it differently
        value = 0;
    }
    return value;
}

static inline uint64_t Read4(const uint8_t* p)
{
    uint32_t value;
    errno_t err = memcpy_s(&value, CONST_4, p, CONST_4);
    if (err != 0) {
        // Handle error if needed, here we could set value to 0 or handle it differently
        value = 0;
    }
    return value;
}

static inline uint64_t Read3(const uint8_t* p, size_t k)
{
    return (((uint64_t)p[CONST_0]) << CONST_16) | (((uint64_t)p[k >> CONST_1]) << CONST_8) | p[k - CONST_1];
}

// begin GEHash main function
static uint64_t GEHashInner(const void* key, size_t length, uint64_t seed, const uint64_t* secret)
{
    const uint8_t* data = (const uint8_t*)key;
    seed ^= MultiplyXorMix(seed ^ secret[CONST_0], secret[CONST_1]);
    uint64_t firstValue;
    uint64_t secondValue;
    if (Likely(length <= CONST_16)) {
        if (Likely(length >= CONST_4)) {
            firstValue = (Read4(data) << CONST_32) | Read4(data + ((length >> CONST_3) << CONST_2));
            secondValue = (Read4(data + length - CONST_4) << CONST_32) |
                          Read4(data + length - CONST_4 - ((length >> CONST_3) << CONST_2));
        } else if (Likely(length > CONST_0)) {
            firstValue = Read3(data, length);
            secondValue = CONST_0;
        } else
            firstValue = secondValue = CONST_0;
    } else {
        size_t remainingLength = length;
        if (Unlikely(remainingLength > CONST_48)) {
            uint64_t seed1 = seed;
            uint64_t seed2 = seed;
            do {
                seed = MultiplyXorMix(Read8(data) ^ secret[CONST_1], Read8(data + CONST_8) ^ seed);
                seed1 = MultiplyXorMix(Read8(data + CONST_16) ^ secret[CONST_2], Read8(data + CONST_24) ^ seed1);
                seed2 = MultiplyXorMix(Read8(data + CONST_32) ^ secret[CONST_3], Read8(data + CONST_40) ^ seed2);
                data += CONST_48;
                remainingLength -= CONST_48;
            } while (Likely(remainingLength > CONST_48));
            seed ^= seed1 ^ seed2;
        }
        while (Unlikely(remainingLength > CONST_16)) {
            seed = MultiplyXorMix(Read8(data) ^ secret[CONST_1], Read8(data + CONST_8) ^ seed);
            remainingLength -= CONST_16;
            data += CONST_16;
        }
        firstValue = Read8(data + remainingLength - CONST_16);
        secondValue = Read8(data + remainingLength - CONST_8);
    }
    firstValue ^= secret[CONST_1];
    secondValue ^= seed;
    Multiply128bit(&firstValue, &secondValue);
    return MultiplyXorMix(firstValue ^ secret[CONST_0] ^ length, secondValue ^ secret[CONST_1]);
}

static inline uint64_t GEHash(const void* key, size_t len, uint64_t seed, const uint64_t* secret)
{
    return GEHashInner(key, len, seed, secret);
}
// end get_hash main function

} // namespace GraphicsEffectEngine
} // namespace OHOS
#endif // GRAPHICS_EFFECT_GE_HASH_REFER_H
