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
    TestRend(uint32_t seed) {init(seed);}
    ~TestRend() = default;
    
    uint32_t nextU();
protected:
    void init(uint32_t seed);
    uint32_t next(uint32_t seed);

    uint32_t a;
    uint32_t b;
};
#endif // TEST_COMMON_H