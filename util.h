#ifndef UTIL_H
#define UTIL_H

#include "rtweekend.h"

inline bool RayHitsTri(ray r, point3 a, point3 b, point3 c) {
    point3 edgeAB = b - a;
    point3 edgeAC = c - a;
    point3 normal = cross(edgeAB, edgeAC);
    point3 ao = r.origin() - a;
    point3 dao = cross(ao, r.direction());

    double determinant = -dot(r.direction(), normal);
    double invDet = 1 / determinant;

    // Calculate dst to triangle
    double dst = dot(ao, normal) * invDet;
    double u = dot(edgeAC, dao) * invDet;
    double v = -dot(edgeAB, dao) * invDet;
    double w = 1 - u - v;

    if (!(determinant >= 1e-6 && dst >= 0 && u >= 0 && v >= 0 && w >= 0))
        return false;

    return true;
}

#endif