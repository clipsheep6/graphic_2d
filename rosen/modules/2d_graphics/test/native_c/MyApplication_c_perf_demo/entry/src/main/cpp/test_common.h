#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include "napi/native_api.h"
#include <string>
#include <bits/alltypes.h>

#define TMUL  1664525
#define TADD 1013904223

class TestRend {
public:
    TestRend() {init(0);}
    TestRend(uint32_t seed) { init(seed); }
    ~TestRend() = default;
    
    uint32_t nextU();
    float_t nextUScalar1();
    uint32_t nextULessThan(uint32_t count);
    float_t nextF();
    float_t nextRangeF(float_t min,float_t max);
    uint32_t nextBits(unsigned bitCount);
protected:
    void init(uint32_t seed);
    uint32_t next(uint32_t seed);

    uint32_t a;
    uint32_t b;
};
#endif // TEST_COMMON_H