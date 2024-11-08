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
            tri->a = origin + (tri->a - origin) * v;
            tri->b = origin + (tri->b - origin) * v;
            tri->c = origin + (tri->c - origin) * v;
            tri->origin = origin + (tri->origin - origin) * v;
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