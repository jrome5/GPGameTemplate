#pragma once
#include <cmath>
#include <cassert>

typedef struct Point
{
    Point() {}
    Point(double x, double y, double z) : x(x), y(y), z(z){}

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
    AABB(const Point center, const Point halfwidths) : c(center), r(halfwidths) {}

    Point c;        // center point
    Point r;        // halfwidths
};



double Abs(double a)
{
    return std::fabs(a);
}



bool isCollision(const AABB a, const AABB b)
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
  /*  if (anyOverlap)
        for (int i = 0; i < 3; i++)
        {
            cout << a.c[i] << " " << b.c[i] << "\n" << a.r[i] << " " << b.r[i] << "\n";
        }*/
    return anyOverlap;
};