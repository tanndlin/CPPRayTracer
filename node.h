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
        if (this->children.objects.size() > 1 && splitDepth < MAX_SPLIT_DEPTH)
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
        double dist = bounds.hit(r);
        if (dist < 0 || ray_t.max < dist)
            return false;

        if (childA && childB) {
            double distA = childA->bounds.hit(r);
            double distB = childB->bounds.hit(r);

            node* near = distA < distB ? childA.get() : childB.get();
            node* far = distA < distB ? childB.get() : childA.get();

            bool hit_near = false;
            double closest_so_far = ray_t.max;
            if (near->hit(r, ray_t, rec)) {
                hit_near = true;
                closest_so_far = rec.t;
            }

            if (closest_so_far < distB)
                return true;

            if (far->hit(r, interval(ray_t.min, closest_so_far), rec))
                return true;

            return hit_near;
        }

        return children.hit(r, ray_t, rec);
    }

    void move_origin(vec3 offset) {
        bounds.offset(offset);

        if (childA) childA->move_origin(offset);
        if (childB) childB->move_origin(offset);

        if (!childA && !childB)
            children.move_origin(offset);
    }

    int get_largest_bvh() const {
        if (childA && childB)
            return std::max(childA->get_largest_bvh(), childB->get_largest_bvh());

        return children.objects.size();
    }

   private:
    static const int MAX_SPLIT_DEPTH = 32;
    int splitDepth = 0;
    void split() {
        int longestAxis = bounds.get_longest_axis();
        double splitPoint = (bounds.min.e[longestAxis] + bounds.max.e[longestAxis]) / 2;

        hittable_list aList = hittable_list();
        hittable_list bList = hittable_list();
        for (const auto& object : children.objects) {
            if (object->origin[longestAxis] < splitPoint)
                aList.add(object);
            else
                bList.add(object);
        }

        // All nodes were in one child, this was not a useful split
        if (aList.objects.size() == 0 || bList.objects.size() == 0) {
            return;
        }

        childA = std::make_unique<node>(aList, splitDepth + 1);
        childB = std::make_unique<node>(bList, splitDepth + 1);
    }
};

#endif