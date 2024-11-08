#ifndef RAY_H
#define RAY_H

#include "../util/vec3.h"

class ray {
   public:
    vec3 dir_inv;
    ray() {}

    ray(const point3& origin, const vec3& direction) : orig(origin), dir(direction) {
        dir_inv = vec3(1 / direction.x(), 1 / direction.y(), 1 / direction.z());
    }

    const point3& origin() const { return orig; }
    const vec3& direction() const { return dir; }

    point3 at(double t) const {
        return orig + t * dir;
    }

   private:
    point3 orig;
    vec3 dir;
};

#endif