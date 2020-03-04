#pragma once
#include <cmath>
#include <cassert>
#include <algorithm>


typedef struct Point
{
    Point() {}
    Point(double r) : x(r), y(r), z(r) {}
    Point(double x, double y, double z) : x(x), y(y), z(z)
    {}
    double x = 0.0;
    double y = 0.0;
    double z = 0.0;
    double w = 0.0;

    const double operator[](const int idx) const
    {
        if (idx == 0) return x;
        if (idx == 1) return y;
        if (idx == 2) return z;
        if (idx == 3) return w;

        assert(0);
    }
};

typedef struct AABB
{
    AABB() : c(), r() {}

    AABB(const Point center, const Point halfwidths) : c(center), r(halfwidths)
    {}

    Point c;        // center point
    Point r;        // halfwidths
};

double Abs(double a)
{
    return std::fabs(a);
}

bool isCollisionCube(const AABB a, const AABB b)
{
    bool xOverlap = true;
    bool yOverlap = true;
    bool zOverlap = true;
    bool anyOverlap = false;

    if (Abs(a.c[0] - b.c[0]) > (a.r[0] + b.r[0])) 
        xOverlap = false;
    if (Abs(a.c[1] - b.c[1]) > (a.r[1] + b.r[1]))
        yOverlap = false;
    if (Abs(a.c[2] - b.c[2]) > (a.r[2] + b.r[2]))
        zOverlap = false;

    anyOverlap = xOverlap && yOverlap && zOverlap;
    return anyOverlap;
}

bool isPointInsideSphere(const Point point, const AABB sphere) {
    auto distance_sqr = (point.x - sphere.c.x) * (point.x - sphere.c.x) +
        (point.y - sphere.c.y) * (point.y - sphere.c.y) +
        (point.z - sphere.c.z) * (point.z - sphere.c.z);
    return distance_sqr < pow(sphere.r.x, 2);
}

bool isCollisionSphere(const AABB sphere, const AABB cube)
{
    bool anyOverlap = false;
    Point p;
    p.x = std::max(cube.c.x - cube.r.x, std::min(sphere.c.x, cube.c.x + cube.r.x));
    p.y = std::max(cube.c.y - cube.r.y, std::min(sphere.c.y, cube.c.y + cube.r.y));
    p.z = std::max(cube.c.z - cube.r.z, std::min(sphere.c.z, cube.c.z + cube.r.z));
    anyOverlap = isPointInsideSphere(p, sphere);
    return anyOverlap;
}