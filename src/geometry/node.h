#ifndef NODE_H
#define NODE_H

#include <climits>
#include <memory>
#include <vector>

#include "../util/utils.h"
#include "bounding_box.h"
#include "hittable_list.h"
#include "tri.h"

class node {
   public:
    node(std::vector<shared_ptr<triangle>>& tris, int splitDepth) : bounds(), children(), splitDepth(splitDepth) {
        for (auto& tri : tris)
            children.add(tri);

        bounds = children.get_bounds();
        init();
    }

    node(hittable_list children, int splitDepth) : bounds(children.get_bounds()), children(children), splitDepth(splitDepth) {
        init();
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

        if (children.hit(r, ray_t, rec))
            return true;

        return false;
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

    void init() {
        // Make sure the bounds are not a plane
        if (bounds.min.x() == bounds.max.x())
            bounds.max.setX(bounds.max.x() + 0.0001);
        if (bounds.min.y() == bounds.max.y())
            bounds.max.setY(bounds.max.y() + 0.0001);
        if (bounds.min.z() == bounds.max.z())
            bounds.max.setZ(bounds.max.z() + 0.0001);

        childA = NULL;
        childB = NULL;
        if (this->children.objects.size() > 1 && splitDepth < MAX_SPLIT_DEPTH)
            split();

        bounds.calc_points();
    }

    void split() {
        // Try all axis
        int longestAxis = -1;
        int difference = INT_MAX;
        int winnerLeftCount = 0;
        int winnerRightCount = 0;
        for (int i = 0; i < 3; i++) {
            double splitPoint = (bounds.min[i] + bounds.max[i]) / 2;
            int leftCount = 0;
            int rightCount = 0;

            for (const auto& object : children.objects)
                if (object->origin[i] < splitPoint)
                    leftCount++;
                else
                    rightCount++;

            if (leftCount > 0 && rightCount > 0) {
                int diff = std::abs(leftCount - rightCount);
                if (diff < difference) {
                    difference = diff;
                    longestAxis = i;
                    winnerLeftCount = leftCount;
                    winnerRightCount = rightCount;
                }
            }
        }

        if (longestAxis == -1) {
            return;
        }

        double splitPoint = (bounds.min[longestAxis] + bounds.max[longestAxis]) / 2;
        hittable_list aList;
        hittable_list bList;

        for (const auto& object : children.objects)
            if (object->origin[longestAxis] < splitPoint)
                aList.add(object);
            else
                bList.add(object);

        if (aList.objects.size() == 0 || bList.objects.size() == 0)
            return;

        childA = std::make_unique<node>(aList, splitDepth + 1);
        childB = std::make_unique<node>(bList, splitDepth + 1);
    }
};

#endif