#ifndef BOUNDING_BOX_H
#define BOUNDING_BOX_H

#include "hittable.h"
#include "rtweekend.h"
#include "util.h"

class bounding_box {
   public:
    point3 min;
    point3 max;

    bounding_box() : min(point3()), max(point3()) {}
    bounding_box(point3 a, point3 b) : min(a), max(b) {
        // Make sure a is the minimum point and b is the maximum point
        if (min.x() > max.x()) {
            double temp = min.x();
            min.setX(max.x());
            max.setX(temp);
        }

        if (min.y() > max.y()) {
            double temp = min.y();
            min.setY(max.y());
            max.setY(temp);
        }

        if (min.z() > max.z()) {
            double temp = min.z();
            min.setZ(max.z());
            max.setZ(temp);
        }
    }

    bool contains(point3 p) {
        auto xInterval = interval(min.x(), max.x());
        auto yInterval = interval(min.y(), max.y());
        auto zInterval = interval(min.z(), max.z());

        return xInterval.contains(p.x()) && yInterval.contains(p.y()) && zInterval.contains(p.z());
    }

    bool overlaps(bounding_box other) {
        bool overlapsX = min.x() <= other.max.x() && max.x() >= other.min.x();
        bool overlapsY = min.y() <= other.max.y() && max.y() >= other.min.y();
        bool overlapsZ = min.z() <= other.max.z() && max.z() >= other.min.z();

        return overlapsX && overlapsY && overlapsZ;
    }

    void expand_to_contain(bounding_box other) {
        min.setX(std::fmin(min.x(), other.min.x()));
        min.setY(std::fmin(min.y(), other.min.y()));
        min.setZ(std::fmin(min.z(), other.min.z()));

        max.setX(std::fmax(max.x(), other.max.x()));
        max.setY(std::fmax(max.y(), other.max.y()));
        max.setZ(std::fmax(max.z(), other.max.z()));
    }

    int get_longest_axis() {
        double xDist = max.x() - min.x();
        double yDist = max.y() - min.y();
        double zDist = max.z() - min.z();

        if (xDist > yDist && xDist > zDist)
            return 0;
        else if (yDist > xDist && yDist > zDist)
            return 1;

        return 2;
    }

    bool hit(const ray& r) const {
        // Triangulate the box into 12 triangles and check if the ray intersects any of them
        // Bottom face CCW
        point3 a = point3(min.x(), min.y(), max.z());
        point3 b = point3(max.x(), min.y(), max.z());
        point3 c = point3(min.x(), min.y(), min.z());
        point3 d = min;

        // Top face CCW... E is on top of A
        point3 e = point3(min.x(), max.y(), max.z());
        point3 f = max;
        point3 g = point3(min.x(), max.y(), min.z());
        point3 h = point3(max.x(), max.y(), min.z());

        // Check if the ray intersects any of the triangles
        // Bottom face
        if (RayHitsTri(r, c, b, a) || RayHitsTri(r, a, d, c))
            return true;

        // Top face
        if (RayHitsTri(r, e, f, g) || RayHitsTri(r, e, g, h))
            return true;

        // Max Z face
        if (RayHitsTri(r, a, b, f) || RayHitsTri(r, a, f, e))
            return true;

        // Min Z face
        if (RayHitsTri(r, c, d, h) || RayHitsTri(r, c, h, g))
            return true;

        // Max X face
        if (RayHitsTri(r, b, c, g) || RayHitsTri(r, b, g, f))
            return true;

        // Min X face
        if (RayHitsTri(r, a, e, h) || RayHitsTri(r, a, h, d))
            return true;

        return false;
    }
};

#endif