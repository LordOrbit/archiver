#pragma once

#include <cmath>

// Round with precision
inline long double Round(long double x, int precision = 0) {
    long double p10 = 1;
    while (precision--) {
        p10 *= 10;
    }
    long double result = std::round(x * p10) / p10;
    return result;
}
