#include "Utils.h"

#include <zlib.h>

namespace df3d {

std::mt19937_64& RandomUtils::rng()
{
    static std::mt19937_64 gen(std::chrono::high_resolution_clock::now().time_since_epoch().count());
    return gen;
}

float RandomUtils::randRange(float a, float b)
{
    std::uniform_real_distribution<> dis(a, b);
    return static_cast<float>(dis(rng()));
}

int RandomUtils::randRange(int a, int b)
{
    std::uniform_int_distribution<> dis(a, b);
    return dis(rng());
}

namespace utils {

bool inflateUncompress(uint8_t *dest, size_t destLen, const uint8_t *source, size_t sourceLen)
{
    uLongf tmp = destLen;

    if (uncompress(dest, &tmp, source, sourceLen) != Z_OK)
        return false;

    return tmp == destLen;
}

} }
