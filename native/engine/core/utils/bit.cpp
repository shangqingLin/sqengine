#include <cstdint>
#include <cstring>
#include "bit.h"


float Bit::uintToFloatBits(unsigned int u) {
    float f;
    std::memcpy(&f, &u, sizeof(f));
    return f;
}

unsigned int Bit::floatToUintBits(float f) {
    unsigned int u;
    std::memcpy(&u, &f, sizeof(u));
    return u;
}