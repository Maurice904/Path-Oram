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