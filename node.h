#ifndef NODE_H
#define NODE_H

#include <memory>
#include <vector>

#include "bounding_box.h"
#include "hittable_list.h"
#include "rtweekend.h"
#include "tri.h"

class node {
   public:
    node(std::vector<shared_ptr<triangle>>& tris, int splitDepth) : bounds(), children(), splitDepth(splitDepth) {
        for (auto& tri : tris)
            children.add(tri);

        childA = NULL;
        childB = NULL;
        bounds = children.get_bounds();
        if (this->children.objects.size() > 1)
            split();

        bounds.calc_points();
    }

    node(hittable_list children, int splitDepth) : bounds(children.get_bounds()), children(children), splitDepth(splitDepth) {
        childA = NULL;
        childB = NULL;
        if (this->children.objects.size() > 1 && splitDepth < MAX_SPLIT_DEPTH)
            split();

        bounds.calc_points();
    }

    bounding_box bounds;
    hittable_list children;
    std::unique_ptr<node> childA;
    std::unique_ptr<node> childB;

    bool hit(const ray& r, interval ray_t, hit_record& rec) const {
        if (!bounds.hit(r))
            return false;

        bool hit_anything = false;
        double closest_so_far = ray_t.max;
        if (childA && childA->hit(r, interval(ray_t.min, closest_so_far), rec)) {
            hit_anything = true;
            closest_so_far = rec.t;
        }

        if (childB && childB->hit(r, interval(ray_t.min, closest_so_far), rec)) {
            hit_anything = true;
            closest_so_far = rec.t;
        }

        if (childA || childB)
            return hit_anything;

        return children.hit(r, ray_t, rec);

        if (children.hit(r, ray_t, rec)) return true;
        rec.mat = make_shared<lambertian>(color(1, 0, 1));
        return true;
    }

    void move_origin(vec3 offset) {
        bounds.offset(offset);

        if (childA) childA->move_origin(offset);
        if (childB) childB->move_origin(offset);

        if (!childA && !childB)
            children.move_origin(offset);
    }

   private:
    static const int MAX_SPLIT_DEPTH = 10;
    int splitDepth = 0;
    void split() {
        // Find longest axis
        int longestAxis = bounds.get_longest_axis();
        double splitPoint = (bounds.min.e[longestAxis] + bounds.max.e[longestAxis]) / 2;

        hittable_list aList = hittable_list();
        hittable_list bList = hittable_list();
        for (const auto& object : children.objects) {
            point3 center = object->origin;
            if (center.e[longestAxis] < splitPoint)
                aList.add(object);
            else
                bList.add(object);
        }

        // All nodes were in one child, this was not a useful split
        if (aList.objects.size() == 0 || bList.objects.size() == 0)
            return;

        childA = std::make_unique<node>(aList, splitDepth + 1);
        childB = std::make_unique<node>(bList, splitDepth + 1);
    }
};

#endif