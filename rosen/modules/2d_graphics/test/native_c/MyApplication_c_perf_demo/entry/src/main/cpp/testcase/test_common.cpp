#include "test_common.h"

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