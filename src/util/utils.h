#ifndef UTILS_H
#define UTILS_H

#include <cmath>
#include <cstdlib>
#include <iostream>
#include <limits>
#include <memory>
#include <random>

// C++ Std Usings

using std::make_shared;
using std::make_unique;
using std::shared_ptr;

// Constants

const double infinity = std::numeric_limits<double>::infinity();
const double pi = 3.1415926535897932385;

// Utility Functions

inline double degrees_to_radians(double degrees) {
    return degrees * pi / 180.0;
}

static std::mt19937& global_rng() {
    thread_local std::mt19937 rng{[] {
        std::random_device rd;
        std::seed_seq ss{rd(), rd(), rd(), rd()};
        return std::mt19937{ss};
    }()};
    return rng;
}

inline double random_double() {
    // Returns a random real in [0,1).
    static std::uniform_real_distribution<double> distribution(0.0, 1.0);  // Define the range
    return distribution(global_rng());
}

inline double random_double(double min, double max) {
    // Returns a random real in [min,max).
    return min + (max - min) * random_double();
}

// Common Headers
#include "../scene/color.h"
#include "../scene/ray.h"
#include "../util/interval.h"
#include "../util/vec3.h"

#endif