#ifndef NODE_H
#define NODE_H

#include <memory>
#include <vector>

#include "bounding_box.h"
#include "hittable_list.h"
#include "rtweekend.h"

class node {
   public:
    node(hittable_list children, bounding_box bounds) : bounds(bounds), children(children) {
        if (this->children.objects.size() > 2)
            split();
    }
    node(hittable_list children) : bounds(children.get_bounds()), children(children) {
        if (this->children.objects.size() > 2)
            split();
    }

    bounding_box bounds;
    hittable_list children;
    std::unique_ptr<node> childA;
    std::unique_ptr<node> childB;

    bool hit(const ray& r, interval ray_t, hit_record& rec) const {
        if (childA)
            if (childA->hit(r, ray_t, rec))
                return true;

        if (childB)
            if (childB->hit(r, ray_t, rec))
                return true;

        // If no children nodes, check if the hittable list is hit
        if (bounds.hit(r, ray_t))
            if (children.hit(r, ray_t, rec))
                return true;

        return false;
    }

   private:
    void split() {
        // Find longest axis
        int longestAxis = bounds.get_longest_axis();
        bounding_box aBounds;
        bounding_box bBounds;
        // X axis
        if (longestAxis == -1) {
            double midpoint = (bounds.a.x() + bounds.b.x()) / 2;
            // Keep a where it is, move b to new midpoint
            aBounds = bounding_box(bounds.a, point3(midpoint, bounds.b.y(), bounds.b.z()));
            // Keep b where it is, move a to new midpoint
            bBounds = bounding_box(point3(midpoint, bounds.a.y(), bounds.a.z()), bounds.b);
        } else if (longestAxis == 0) {  // Y axis
            double midpoint = (bounds.a.y() + bounds.b.y()) / 2;
            // Keep a where it is, move b to new midpoint
            aBounds = bounding_box(bounds.a, point3(bounds.b.x(), midpoint, bounds.b.z()));
            // Keep b where it is, move a to new midpoint
            bBounds = bounding_box(point3(bounds.a.x(), midpoint, bounds.a.z()), bounds.b);
        } else if (longestAxis == 1) {  // Z axis
            double midpoint = (bounds.a.z() + bounds.b.z()) / 2;
            // Keep a where it is, move b to new midpoint
            aBounds = bounding_box(bounds.a, point3(bounds.b.x(), bounds.b.y(), midpoint));
            // Keep b where it is, move a to new midpoint
            bBounds = bounding_box(point3(bounds.a.x(), bounds.a.y(), midpoint), bounds.b);
        }

        hittable_list aList = hittable_list();
        hittable_list bList = hittable_list();
        for (const auto& object : children.objects) {
            if (object->partially_contained_by(aBounds)) {
                aList.add(object);
            } else if (object->partially_contained_by(bBounds)) {
                bList.add(object);
            } else {
                std::cerr << "Object was somehow not covered by either split box" << std::endl;
                exit(1);
            }
        }

        if (aList.objects.size() > 0)
            childA = std::make_unique<node>(aList, aBounds);
        if (bList.objects.size() > 0)
            childB = std::make_unique<node>(bList, bBounds);
    }
};

#endif