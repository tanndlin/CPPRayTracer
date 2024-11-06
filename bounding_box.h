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

        point3 bottomFace[4] = {a, b, c, d};
        point3 topFace[4] = {e, f, g, h};
        point3 maxZFace[4] = {a, b, f, e};
        point3 minZFace[4] = {c, d, h, g};
        point3 maxXFace[4] = {b, c, g, f};
        point3 minXFace[4] = {d, h, e, a};

        // Get normals
        vec3 bottomNormal = cross(a - b, c - a);
        vec3 topNormal = cross(f - e, g - e);
        vec3 maxZNormal = cross(a - f, b - a);
        vec3 minZNormal = cross(d - c, g - c);
        vec3 maxXNormal = cross(f - b, b - g);
        vec3 minXNormal = cross(e - a, d - a);

        // There are 3 faces that the ray can see
        // The dot product of the ray direction and the normal of the face will be negative
        // if the ray is facing the face
        if (dot(r.direction(), bottomNormal)) {
            // Calc how far the ray is from the face
            double targetY = bottomFace[0].y();
            double t = (targetY - r.origin().y()) / r.direction().y();
            point3 target = r.at(t);
            // Check if target is within the face
            double minX = std::fmin(bottomFace[0].x(), bottomFace[1].x());
            double maxX = std::fmax(bottomFace[0].x(), bottomFace[1].x());
            double minZ = std::fmin(bottomFace[0].z(), bottomFace[2].z());
            double maxZ = std::fmax(bottomFace[0].z(), bottomFace[2].z());
            if (target.x() >= minX && target.x() <= maxX && target.z() >= minZ && target.z() <= maxZ) {
                return true;
            }
        }

        if (dot(r.direction(), topNormal)) {
            // Calc how far the ray is from the face
            double targetY = topFace[0].y();
            double t = (targetY - r.origin().y()) / r.direction().y();
            point3 target = r.at(t);
            // Check if target is within the face
            double minX = std::fmin(topFace[0].x(), topFace[1].x());
            double maxX = std::fmax(topFace[0].x(), topFace[1].x());
            double minZ = std::fmin(topFace[0].z(), topFace[2].z());
            double maxZ = std::fmax(topFace[0].z(), topFace[2].z());
            if (target.x() >= minX && target.x() <= maxX && target.z() >= minZ && target.z() <= maxZ) {
                return true;
            }
        }

        if (dot(r.direction(), maxZNormal)) {
            // Calc how far the ray is from the face
            double targetZ = maxZFace[0].z();
            double t = (targetZ - r.origin().z()) / r.direction().z();
            point3 target = r.at(t);
            // Check if target is within the face
            double minX = std::fmin(maxZFace[0].x(), maxZFace[1].x());
            double maxX = std::fmax(maxZFace[0].x(), maxZFace[1].x());
            double minY = std::fmin(maxZFace[0].y(), maxZFace[2].y());
            double maxY = std::fmax(maxZFace[0].y(), maxZFace[2].y());
            if (target.x() >= minX && target.x() <= maxX && target.y() >= minY && target.y() <= maxY) {
                return true;
            }
        }

        if (dot(r.direction(), minZNormal)) {
            // Calc how far the ray is from the face
            double targetZ = minZFace[0].z();
            double t = (targetZ - r.origin().z()) / r.direction().z();
            point3 target = r.at(t);
            // Check if target is within the face
            double minX = std::fmin(minZFace[0].x(), minZFace[1].x());
            double maxX = std::fmax(minZFace[0].x(), minZFace[1].x());
            double minY = std::fmin(minZFace[0].y(), minZFace[2].y());
            double maxY = std::fmax(minZFace[0].y(), minZFace[2].y());
            if (target.x() >= minX && target.x() <= maxX && target.y() >= minY && target.y() <= maxY) {
                return true;
            }
        }

        if (dot(r.direction(), maxXNormal)) {
            // Calc how far the ray is from the face
            double targetX = maxXFace[0].x();
            double t = (targetX - r.origin().x()) / r.direction().x();
            point3 target = r.at(t);
            // Check if target is within the face
            double minY = std::fmin(maxXFace[0].y(), maxXFace[2].y());
            double maxY = std::fmax(maxXFace[0].y(), maxXFace[2].y());
            double minZ = std::fmin(maxXFace[0].z(), maxXFace[1].z());
            double maxZ = std::fmax(maxXFace[0].z(), maxXFace[1].z());
            if (target.y() >= minY && target.y() <= maxY && target.z() >= minZ && target.z() <= maxZ) {
                return true;
            }
        }

        if (dot(r.direction(), minXNormal)) {
            // Calc how far the ray is from the face
            double targetX = minXFace[0].x();
            double t = (targetX - r.origin().x()) / r.direction().x();
            point3 target = r.at(t);
            // Check if target is within the face
            double minY = std::fmin(minXFace[0].y(), minXFace[2].y());
            double maxY = std::fmax(minXFace[0].y(), minXFace[2].y());
            double minZ = std::fmin(minXFace[0].z(), minXFace[1].z());
            double maxZ = std::fmax(minXFace[0].z(), minXFace[1].z());
            if (target.y() >= minY && target.y() <= maxY && target.z() >= minZ && target.z() <= maxZ) {
                return true;
            }
        }

        return false;
    }
};

#endif