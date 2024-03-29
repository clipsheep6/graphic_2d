#include "test_common.h"

#define intToFloat(x) ((x) * 1.52587890625e-5f)

union drawFloatIntUnion {
    float   fFloat;
    int32_t fSignBitInt;
};

static inline float drawBits2Float(int32_t floatAsBits) {
    drawFloatIntUnion data;
    data.fSignBitInt = floatAsBits;
    return data.fFloat;
}

uint32_t TestRend::next(uint32_t seed) {
    return TMUL*seed + TADD;
}

void TestRend::init(uint32_t seed) {
    a = next(seed);
    if (0 == a) {
        a = next(a);
    }
    b = next(a);
    if (0 == b) {
        b = next(b);
    }
}

uint32_t TestRend::nextU() {
    a = 30345*(a & 0xffff) + (a >> 16);
    b = 18000*(b & 0xffff) + (b >> 16);
    return (((a << 16) | (a >> 16)) + b);
}

float_t TestRend::nextUScalar1()
{
    return intToFloat(this->nextU()>>16);
}

uint32_t TestRend::nextULessThan(uint32_t count)
{
    uint32_t min = 0;
    uint32_t max = count-1;
    uint32_t range = max-min+1;
    if(0 == range)
        return this->nextU();
    else{
        return (min+this->nextU()%range);
    }
}

float_t TestRend::nextF()
{
    int floatint = 0x3f800000 | (int)(this->nextU() >> 9);
    float f = drawBits2Float(floatint) - 1.0f;
    return f;    
}

float_t TestRend::nextRangeF(float_t min,float_t max)
{
    return this->nextUScalar1() * (max - min) + min;
}

uint32_t TestRend::nextBits(unsigned bitCount)
{
    return this->nextU() >> (32 - bitCount);
}

uint32_t color_to_565(uint32_t color)
{//not realize , for later use
    return color;
}