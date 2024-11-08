#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include <vector>

#include "../util/utils.h"
#include "bounding_box.h"
#include "hittable.h"

class hittable_list : public hittable {
   public:
    std::vector<shared_ptr<hittable>> objects;

    hittable_list() : objects() {}
    hittable_list(shared_ptr<hittable> object) { add(object); }

    void clear() { objects.clear(); }

    void add(shared_ptr<hittable> object) {
        objects.push_back(object);
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
        hit_record temp_rec;
        bool hit_anything = false;
        auto closest_so_far = ray_t.max;

        for (const auto& object : objects) {
            if (object->hit(r, interval(ray_t.min, closest_so_far), temp_rec)) {
                hit_anything = true;
                closest_so_far = temp_rec.t;
                rec = temp_rec;
            }
        }

        return hit_anything;
    }

    bounding_box get_bounds() const override {
        bounding_box box = objects[0]->get_bounds();
        for (const auto& object : objects)
            box.expand_to_contain(object->get_bounds());

        return box;
    }

    void move_origin(const vec3& offset) override {
        for (auto& object : objects)
            object->move_origin(offset);
    }
};

#endif