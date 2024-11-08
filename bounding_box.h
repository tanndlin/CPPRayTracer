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

    void offset(vec3 offset) {
        min += offset;
        max += offset;

        calc_points();
    }

    double hit(const ray& r) const {
        double t1 = (min[0] - r.origin()[0]) * r.dir_inv[0];
        double t2 = (max[0] - r.origin()[0]) * r.dir_inv[0];

        double tmin = std::min(t1, t2);
        double tmax = std::max(t1, t2);

        for (int i = 1; i < 3; ++i) {
            t1 = (min[i] - r.origin()[i]) * r.dir_inv[i];
            t2 = (max[i] - r.origin()[i]) * r.dir_inv[i];

            tmin = std::max(tmin, std::min(std::min(t1, t2), tmax));
            tmax = std::min(tmax, std::max(std::max(t1, t2), tmin));
        }

        return tmax > std::max(tmin, 0.0) ? tmin : -1;
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