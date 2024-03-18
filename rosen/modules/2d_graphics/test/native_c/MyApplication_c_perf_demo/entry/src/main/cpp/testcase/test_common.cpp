#include "test_common.h"

#define intToFloat(x) ((x) * 1.52587890625e-5f)

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