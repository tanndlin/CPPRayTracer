#ifndef NODE_H
#define NODE_H

#include <memory>
#include <vector>

#include "bounding_box.h"
#include "hittable_list.h"
#include "rtweekend.h"

class node {
   public:
    node(hittable_list children, int splitDepth) : bounds(children.get_bounds()), children(children), splitDepth(splitDepth) {
        childA = NULL;
        childB = NULL;
        if (this->children.objects.size() > 1 && splitDepth < MAX_SPLIT_DEPTH)
            split();

        int total = 0;
        if (childA)
            total += childA->children.objects.size();
        if (childB)
            total += childB->children.objects.size();
        if ((childA || childB) && total != children.objects.size()) {
            std::cerr << "Splitting failed. Found " << total << " objects, expected " << children.objects.size() << std::endl;
            exit(1);
        }
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
        if (childA && childA->hit(r, ray_t, rec)) {
            hit_anything = true;
            closest_so_far = rec.t;
        }

        if (childB && childB->hit(r, interval(ray_t.min, closest_so_far), rec))
            hit_anything = true;

        if (childA || childB)
            return hit_anything;

        // bool hitChildren = children.hit(r, ray_t, rec);
        // if (hitChildren)
        //     return true;

        if (bounds.hit(r)) {
            rec.mat = make_shared<lambertian>(color(255, 0, 0));
            return true;
        }

        return false;
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
            point3 center = object->center;
            if (center.e[longestAxis] < splitPoint)
                aList.add(object);
            else
                bList.add(object);
        }

        if (aList.objects.size() > 0)
            childA = std::make_unique<node>(aList, splitDepth + 1);
        if (bList.objects.size() > 0)
            childB = std::make_unique<node>(bList, splitDepth + 1);
    }
};

#endif