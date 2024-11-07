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
        origin = point3();
    }

    mesh(std::vector<triangle> tris, shared_ptr<material> mat) : tris(tris), mat(mat) {
        origin = point3();
        for (auto& tri : tris) {
            tri.set_material(mat);
        }
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
        bounding_box box = bounding_box(origin);
        for (const auto& tri : tris) {
            box.expand_to_contain(tri.a);
            box.expand_to_contain(tri.b);
            box.expand_to_contain(tri.c);
        }

        return box;
    }

    void set_origin(point3 p) {
        point3 oldPos = origin;
        origin = p;
        point3 offset = origin - oldPos;
        // Move all triangles to the new origin
        for (auto& tri : tris) {
            tri.a += offset;
            tri.b += offset;
            tri.c += offset;
            tri.origin += offset;
        }
    }

   private:
    shared_ptr<material> mat;
};

#endif