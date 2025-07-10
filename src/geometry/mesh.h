#ifndef MESH_H
#define MESH_H

#include <vector>

#include "../scene/material.h"
#include "../util/utils.h"
#include "hittable.h"
#include "node.h"
#include "tri.h"

class mesh : public hittable {
   public:
    node bvh;
    std::vector<shared_ptr<triangle>> tris;

    mesh(std::vector<shared_ptr<triangle>>& tris) : bvh(tris, 0), tris(tris) {
        origin = point3();
    }

    mesh(std::vector<shared_ptr<triangle>>& tris, shared_ptr<material> mat) : bvh(tris, 0), tris(tris), mat(mat) {
        origin = point3();
        for (auto& tri : tris) {
            tri->set_material(mat);
        }
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        return bvh.hit(r, ray_t, rec);
    }

    bounding_box get_bounds() const override {
        bounding_box box = bounding_box(origin);
        for (const auto& tri : tris) {
            box.expand_to_contain(tri->min);
            box.expand_to_contain(tri->max);
        }

        return box;
    }

    void set_origin(const point3& p) {
        point3 oldPos = origin;
        origin = p;
        point3 offset = origin - oldPos;

        bvh.move_origin(offset);
    }

    void move_origin(const vec3& offset) override {
        bvh.move_origin(offset);
    }

    void set_material(shared_ptr<material> m) {
        mat = m;
        for (auto& tri : tris) {
            tri->set_material(m);
        }
    }

    void scale(double factor) {
        scale(vec3(factor, factor, factor));
    }

    void scale(const vec3& v) {
        for (auto& tri : tris) {
            tri->scale(origin, v);
        }

        calculate_bvh();
    }

    void rotate(double angle, const vec3& axis) {
        for (auto& tri : tris) {
            tri->rotate(angle, origin, axis);
        }

        calculate_bvh();
    }

   private:
    shared_ptr<material> mat;

    void calculate_bvh() {
        bvh = node(tris, 0);
    }
};

#endif