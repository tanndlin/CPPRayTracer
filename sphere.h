#ifndef SPHERE_H
#define SPHERE_H

#include "hittable.h"
#include "rtweekend.h"

class sphere : public hittable {
   public:
    sphere(const point3& c, double radius, shared_ptr<material> mat)
        : radius(std::fmax(0, radius)), mat(mat) {
        origin = c;
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        vec3 oc = origin - r.origin();
        auto a = r.direction().length_squared();
        auto h = dot(r.direction(), oc);
        auto c = oc.length_squared() - radius * radius;

        auto discriminant = h * h - a * c;
        if (discriminant < 0)
            return false;

        auto sqrtd = std::sqrt(discriminant);

        // Find the nearest root that lies in the acceptable range.
        auto root = (h - sqrtd) / a;
        if (!ray_t.surrounds(root)) {
            root = (h + sqrtd) / a;
            if (!ray_t.surrounds(root))
                return false;
        }

        rec.t = root;
        rec.p = r.at(rec.t);
        vec3 outward_normal = (rec.p - origin) / radius;
        rec.set_face_normal(r, outward_normal);
        rec.mat = mat;

        return true;
    }

    bounding_box get_bounds() const override {
        point3 neg = point3(origin.x() - radius, origin.y() - radius, origin.z() - radius);
        point3 pos = point3(origin.x() + radius, origin.y() + radius, origin.z() + radius);

        return bounding_box(neg, pos);
    }

   private:
    double radius;
    shared_ptr<material> mat;
};

#endif