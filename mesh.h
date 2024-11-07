#ifndef MESH_H
#define MESH_H

#include <vector>

#include "hittable.h"
#include "material.h"
#include "rtweekend.h"
#include "tri.h"

class mesh : public hittable {
   public:
    std::vector<triangle> tris;

    mesh(std::vector<triangle> tris) : tris(tris) {
        center = point3();
        for (const auto& tri : tris)
            center += tri.center;

        center /= tris.size();
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        hit_record temp_rec;
        bool hit_anything = false;
        auto closest_so_far = ray_t.max;

        for (const auto& tri : tris) {
            if (tri.hit(r, interval(ray_t.min, closest_so_far), temp_rec)) {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }

    bounding_box get_bounds() const override {
        bounding_box box = bounding_box(center);
        for (const auto& tri : tris) {
            box.expand_to_contain(tri.a);
            box.expand_to_contain(tri.b);
            box.expand_to_contain(tri.c);
        }

        return box;
    }
};

#endif