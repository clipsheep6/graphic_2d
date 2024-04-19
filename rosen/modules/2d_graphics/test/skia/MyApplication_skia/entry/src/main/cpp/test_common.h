#ifndef TEST_COMMON_H
#define TEST_COMMON_H

#include "napi/native_api.h"
#include <native_drawing/drawing_types.h>
#include <string>
#include <bits/alltypes.h>

#define TMUL  1664525
#define TADD 1013904223

#define DRAW_ColorWHITE 0xFFFFFFFF
#define DRAW_ColorBLACK 0xFF000000
#define DRAW_ColorRED 0xFFFF0000
#define DRAW_ColorGREEN 0xFF00FF00
#define DRAW_ColorBLUE 0xFF0000FF
#define DRAW_ColorYELLOW 0xFFFFFF00
#define DRAW_ColorCYAN 0xFF00FFFF
#define DRAW_ColorMAGENTA 0xFFFF00FF
#define DRAW_ColorGRAY 0xFF808080
#define DRAW_ColorTRANSPARENT 0x00000000

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