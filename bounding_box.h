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
    bounding_box(point3 p) : min(p), max(p) {}
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

    void expand_to_contain(point3 p) {
        min.setX(std::fmin(min.x(), p.x()));
        min.setY(std::fmin(min.y(), p.y()));
        min.setZ(std::fmin(min.z(), p.z()));

        max.setX(std::fmax(max.x(), p.x()));
        max.setY(std::fmax(max.y(), p.y()));
        max.setZ(std::fmax(max.z(), p.z()));
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
        // There are 3 faces that the ray can see
        // The dot product of the ray direction and the normal of the face will be negative
        // if the ray is facing the face
        if (dot(r.direction(), bottomNormal) < 0) {
            // Calc how far the ray is from the face
            double targetY = a.y();
            double t = (targetY - r.origin().y()) / r.direction().y();
            point3 target = r.at(t);
            // Check if target is within the face
            double minX = a.x();
            double maxX = c.x();
            double minZ = c.z();
            double maxZ = a.z();
            if (target.x() >= minX && target.x() <= maxX && target.z() >= minZ && target.z() <= maxZ) {
                return true;
            }
        }

        if (dot(r.direction(), topNormal) < 0) {
            // Calc how far the ray is from the face
            double targetY = e.y();
            double t = (targetY - r.origin().y()) / r.direction().y();
            point3 target = r.at(t);
            // Check if target is within the face
            double minX = h.x();
            double maxX = f.x();
            double minZ = h.z();
            double maxZ = f.z();
            if (target.x() >= minX && target.x() <= maxX && target.z() >= minZ && target.z() <= maxZ) {
                return true;
            }
        }

        if (dot(r.direction(), maxZNormal) < 0) {
            // Calc how far the ray is from the face
            double targetZ = a.z();
            double t = (targetZ - r.origin().z()) / r.direction().z();
            point3 target = r.at(t);
            // Check if target is within the face
            double minX = a.x();
            double maxX = f.x();
            double minY = a.y();
            double maxY = f.y();
            if (target.x() >= minX && target.x() <= maxX && target.y() >= minY && target.y() <= maxY) {
                return true;
            }
        }

        if (dot(r.direction(), minZNormal) < 0) {
            // Calc how far the ray is from the face
            double targetZ = c.z();
            double t = (targetZ - r.origin().z()) / r.direction().z();
            point3 target = r.at(t);
            // Check if target is within the face
            double minX = d.x();
            double maxX = g.x();
            double minY = d.y();
            double maxY = g.y();
            if (target.x() >= minX && target.x() <= maxX && target.y() >= minY && target.y() <= maxY) {
                return true;
            }
        }

        if (dot(r.direction(), maxXNormal) < 0) {
            // Calc how far the ray is from the face
            double targetX = b.x();
            double t = (targetX - r.origin().x()) / r.direction().x();
            point3 target = r.at(t);
            // Check if target is within the face
            double minY = b.y();
            double maxY = g.y();
            double minZ = g.z();
            double maxZ = b.z();
            if (target.y() >= minY && target.y() <= maxY && target.z() >= minZ && target.z() <= maxZ) {
                return true;
            }
        }

        if (dot(r.direction(), minXNormal) < 0) {
            // Calc how far the ray is from the face
            double targetX = d.x();
            double t = (targetX - r.origin().x()) / r.direction().x();
            point3 target = r.at(t);
            // Check if target is within the face
            double minY = a.y();
            double maxY = h.y();
            double minZ = h.z();
            double maxZ = a.z();
            if (target.y() >= minY && target.y() <= maxY && target.z() >= minZ && target.z() <= maxZ) {
                return true;
            }
        }

        return false;
    }

    void calc_points() {
        a = point3(min.x(), min.y(), max.z());
        b = point3(max.x(), min.y(), max.z());
        c = point3(max.x(), min.y(), min.z());
        d = point3(min.x(), min.y(), min.z());

        // Top face CCW... E is on top of A
        e = point3(min.x(), max.y(), max.z());
        f = point3(max.x(), max.y(), max.z());
        g = point3(max.x(), max.y(), min.z());
        h = point3(min.x(), max.y(), min.z());
    }

   private:
    point3 a;
    point3 b;
    point3 c;
    point3 d;

    // Top face CCW... E is on top of A
    point3 e;
    point3 f;
    point3 g;
    point3 h;

    vec3 bottomNormal = vec3(0, -1, 0);
    vec3 topNormal = vec3(0, 1, 0);
    vec3 maxZNormal = vec3(0, 0, 1);
    vec3 minZNormal = vec3(0, 0, -1);
    vec3 maxXNormal = vec3(1, 0, 0);
    vec3 minXNormal = vec3(-1, 0, 0);
};

#endif