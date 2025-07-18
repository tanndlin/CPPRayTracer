#ifndef HITTABLE_H
#define HITTABLE_H

#include "../util/utils.h"
#include "bounding_box.h"

class material;

class hit_record {
   public:
    point3 p;
    vec3 normal;
    shared_ptr<material> mat;
    double t;
    bool front_face;
    double u, v;  // UV coordinates for texture mapping

    void set_face_normal(const ray& r, const vec3& outward_normal) {
        // Sets the hit record normal vector
        // NOTE: the parameter `outward_nromal` is assumed to have unit length
        front_face = dot(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal : -outward_normal;
    }
};

class hittable {
   public:
    virtual ~hittable() = default;
    virtual bool hit(const ray& r, interval ray_t, hit_record& rec) const = 0;
    virtual bounding_box get_bounds() const = 0;
    virtual void move_origin(const vec3& offset) = 0;

    point3 origin;
};

#endif