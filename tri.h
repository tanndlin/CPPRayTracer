#ifndef TRI_H
#define TRI_H

#include "hittable.h"
#include "rtweekend.h"

class triangle : public hittable {
   public:
    point3 a;
    point3 b;
    point3 c;

    triangle(const point3& a, const point3& b, const point3& c, shared_ptr<material> mat) : a(a), b(b), c(c), mat(mat) {}

    bool hit(const ray& r, interval ray_t, hit_record& rec) {
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

        rec.t = dst;
        rec.p = r.at(rec.t);
        rec.set_face_normal(r, normal);
        rec.mat = mat;
    }

   private:
    shared_ptr<material> mat;
};

#endif
