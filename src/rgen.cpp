#include "rgen.h"

size_t randomSizeT(size_t min, size_t max) {
    static std::random_device rd;
    static std::mt19937_64 gen(rd());
    std::uniform_int_distribution<size_t> dis(min, max);
    return dis(gen);
}

double randomDouble(double min, double max) {
    static std::random_device rd;
    static std::mt19937_64 gen(rd());
    std::uniform_real_distribution<double> dis(min, max);
    return dis(gen);
}

size_t reverseBits(size_t val, size_t bits) {
    size_t reversed = 0;
    for (size_t i = 0; i < bits; i ++) {
        reversed |= (val & 1) << (bits - 1 - i);
        val >>= 1;
    }
    return reversed;
}