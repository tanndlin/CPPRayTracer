#ifndef TRI_H
#define TRI_H

#include "../scene/material.h"
#include "../util/utils.h"
#include "hittable.h"

class triangle : public hittable {
   public:
    point3 a;
    point3 b;
    point3 c;
    point3 min;
    point3 max;

    triangle(const point3& a, const point3& b, const point3& c) : a(a), b(b), c(c) {
        mat = MISSING_TEXTURE_MAT;
        calc_bounds();
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        point3 ao = r.origin() - a;
        point3 dao = cross(ao, r.direction());

        double determinant = -dot(r.direction(), normal);
        if (determinant < 1e-6)
            return false;

        double invDet = 1 / determinant;

        // Calculate dst to triangle
        double dst = dot(ao, normal) * invDet;
        if (dst < 0)
            return false;

        double u = dot(edgeAC, dao) * invDet;
        if (u < 0 || u > 1)
            return false;

        double v = -dot(edgeAB, dao) * invDet;
        if (v < 0 || u + v > 1)
            return false;

        if (!ray_t.contains(dst))
            return false;

        rec.t = dst;
        rec.p = r.at(rec.t);
        rec.set_face_normal(r, normal);
        rec.mat = mat;

        return true;
    }

    bounding_box get_bounds() const override {
        return bounds;
    }

    void calc_bounds() {
        min = vec_min(a, vec_min(b, c));
        max = vec_max(a, vec_max(b, c));

        bounds = bounding_box(min, max);

        origin = (min + max) / 2;
        edgeAB = b - a;
        edgeAC = c - a;
        normal = cross(edgeAB, edgeAC);
    }

    void set_material(shared_ptr<material> m) { mat = m; }

    void move_origin(const vec3& offset) override {
        a += offset;
        b += offset;
        c += offset;

        calc_bounds();
    }

    void scale(const point3& origin, const vec3& v) {
        a = origin + v * (a - origin);
        b = origin + v * (b - origin);
        c = origin + v * (c - origin);

        calc_bounds();
    }

   private:
    shared_ptr<material> mat;
    bounding_box bounds;

    point3 edgeAB;
    point3 edgeAC;
    point3 normal;
};

#endif
