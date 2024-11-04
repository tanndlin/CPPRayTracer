#ifndef BOUNDING_BOX_H
#define BOUNDING_BOX_H

#include "hittable.h"
#include "rtweekend.h"

class bounding_box {
   public:
    point3 a;
    point3 b;

    bounding_box() : a(point3()), b(point3()) {}
    bounding_box(const point3 a, const point3 b) : a(a), b(b) {}

    bool contains(point3 p) {
        auto xInterval = interval(a.x(), b.x());
        auto yInterval = interval(a.y(), b.y());
        auto zInterval = interval(a.z(), b.z());

        return xInterval.surrounds(p.x()) && yInterval.surrounds(p.y()) && zInterval.surrounds(p.z());
    }

    bool overlaps(bounding_box other) {
        bool overlapsX = std::min(a.x(), b.x()) < std::max(other.a.x(), other.b.x()) && std::max(a.x(), b.x()) > std::min(other.a.x(), other.b.x());
        bool overlapsY = std::min(a.y(), b.y()) < std::max(other.a.y(), other.b.y()) && std::max(a.y(), b.y()) > std::min(other.a.y(), other.b.y());
        bool overlapsZ = std::min(a.z(), b.z()) < std::max(other.a.z(), other.b.z()) && std::max(a.z(), b.z()) > std::min(other.a.z(), other.b.z());

        return overlapsX && overlapsY && overlapsZ;
    }

    // TODO: There has to be a less verbose way
    void expand_to_contain(bounding_box other) {
        point3* lowestX = a.x() < b.x() ? &a : &b;
        double otherLowestX = std::min(other.a.x(), other.b.x());
        if (lowestX->x() > otherLowestX)
            lowestX->setX(otherLowestX);

        point3* highestX = a.x() > b.x() ? &a : &b;
        double otherHighestX = std::max(other.a.x(), other.b.x());
        if (highestX->x() < otherHighestX)
            highestX->setX(otherHighestX);

        point3* lowestY = a.y() < b.y() ? &a : &b;
        double otherLowesty = std::min(other.a.y(), other.b.y());
        if (lowestY->y() > otherLowesty)
            lowestY->setY(otherLowesty);

        point3* highestY = a.y() > b.y() ? &a : &b;
        double otherHighestY = std::max(other.a.y(), other.b.y());
        if (highestY->y() < otherHighestY)
            highestY->setY(otherHighestY);

        point3* lowestZ = a.z() < b.z() ? &a : &b;
        double otherLowestZ = std::min(other.a.z(), other.b.z());
        if (lowestZ->z() > otherLowestZ)
            lowestZ->setZ(otherLowestZ);

        point3* highestZ = a.z() > b.z() ? &a : &b;
        double otherHighestZ = std::max(other.a.z(), other.b.z());
        if (highestZ->z() < otherHighestZ)
            highestZ->setZ(otherHighestZ);
    }

    int get_longest_axis() {
        double xDist = std::abs(a.x() - b.x());
        double yDist = std::abs(a.y() - b.y());
        double zDist = std::abs(a.z() - b.z());

        if (xDist > yDist && xDist > zDist)
            return -1;
        else if (yDist > xDist && yDist > zDist)
            return 0;
        else
            return 1;
    }
};

#endif