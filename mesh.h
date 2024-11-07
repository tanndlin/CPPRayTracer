#ifndef MESH_H
#define MESH_H

#include <vector>

#include "hittable.h"
#include "material.h"
#include "node.h"
#include "rtweekend.h"
#include "tri.h"

class mesh : public hittable {
   public:
    node bvh;
    std::vector<triangle> tris;

    mesh(std::vector<triangle> tris) : bvh(tris, 0) {
        origin = point3();
    }

    mesh(std::vector<triangle>& tris, shared_ptr<material> mat) : bvh(tris, 0), mat(mat) {
        origin = point3();
        for (auto& tri : tris) {
            tri.set_material(mat);
        }
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        return bvh.hit(r, ray_t, rec);
    }

    bounding_box get_bounds() const override {
        bounding_box box = bounding_box(origin);
        for (const auto& tri : bvh.children.objects) {
            box.expand_to_contain(tri->get_bounds());
        }

        return box;
    }

    void set_origin(point3 p) {
        point3 oldPos = origin;
        origin = p;
        point3 offset = origin - oldPos;

        bvh.move_origin(offset);
    }

    void move_origin(const vec3& offset) override {
        bvh.move_origin(offset);
    }

   private:
    shared_ptr<material> mat;
};

#endif