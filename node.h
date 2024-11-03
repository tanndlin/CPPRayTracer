#ifndef NODE_H
#define NODE_H

#include <memory>
#include <vector>

#include "bounding_box.h"
#include "hittable_list.h"
#include "rtweekend.h"

class node {
   public:
    node(std::shared_ptr<hittable> children) : bounds(children->get_bounds()), children(children) {
        bounds = children->get_bounds();
    }

    bounding_box bounds;
    std::shared_ptr<hittable> children;
    std::unique_ptr<node> childA;
    std::unique_ptr<node> childB;
};

#endif