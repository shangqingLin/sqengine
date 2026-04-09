#include "math.h"

unsigned int hash(unsigned int x)
{
    x = ((x >> (unsigned int)(16)) ^ x) * (unsigned int)(73244475);
    x = ((x >> (unsigned int)(16)) ^ x) * (unsigned int)(73244475);
    x = (x >> (unsigned int)(16)) ^ x;
    return x;
}

/**
 * LCG伪随机算法
 */
float rand_from_seed(unsigned int& seed)
{
    int k;
    int s = int(seed);
    if (s == 0)
    {
        s = 305420679;
    };
    k = s / 127773;
    s = 16807 * (s - k * 127773) - 2836 * k;
    if (s < 0)
    {
        s += 2147483647;
    };
    seed = (unsigned int)(s);
    return float(seed % 65536u) / 65535.0;
}

float rand_from_seed_m1_p1(unsigned int& seed)
{
    return rand_from_seed(seed) * 2.0 - 1.0;
}

