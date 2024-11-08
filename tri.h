#ifndef TRI_H
#define TRI_H

#include "hittable.h"
#include "material.h"
#include "rtweekend.h"

class triangle : public hittable {
   public:
    point3 a;
    point3 b;
    point3 c;

    triangle(const point3& a, const point3& b, const point3& c) : a(a), b(b), c(c) {
        origin = (a + b + c) / 3;
        mat = MISSING_TEXTURE_MAT;
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
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

        if (!ray_t.contains(dst))
            return false;

        rec.t = dst;
        rec.p = r.at(rec.t);
        rec.set_face_normal(r, normal);
        rec.mat = mat;

        return true;
    }

    bounding_box get_bounds() const override {
        bounding_box box = bounding_box(a, a);
        box.expand_to_contain(b);
        box.expand_to_contain(c);
        return box;
    }

    void set_material(shared_ptr<material> m) { mat = m; }

    void move_origin(const vec3& offset) override {
        a += offset;
        b += offset;
        c += offset;
        origin += offset;
    }

   private:
    shared_ptr<material> mat;
};

#endif
