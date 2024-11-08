#ifndef INTERVAL_H
#define INTERVAL_H

#include "../util/utils.h"

class interval {
   public:
    double min, max;

    interval() : min(+infinity), max(-infinity) {}  // Default interval is empty

    interval(double min, double max) : min(min), max(max) {
        if (min > max) {
            double temp = max;
            max = min;
            min = temp;
        }
    }

    double size() const {
        return max - min;
    }

    bool contains(double x) const {
        return min <= x && x <= max;
    }

    bool surrounds(double x) const {
        return min < x && x < max;
    }

    bool surrounds(interval i) const {
        return min < i.min && max > i.max;
    }

    double clamp(double x) const {
        if (x < min) return min;
        if (x > max) return max;
        return x;
    }

    static const interval empty, universe;
};

const interval interval::empty = interval(+infinity, -infinity);
const interval interval::universe = interval(-infinity, +infinity);

#endif