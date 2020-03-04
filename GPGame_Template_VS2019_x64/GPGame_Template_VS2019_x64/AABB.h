#pragma once
#include <cmath>
#include <cassert>
#include <algorithm>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>


typedef struct AABB
{
    AABB() : c(), r() {}

    AABB(const glm::vec3 center, const glm::vec3 halfwidths) : c(center), r(halfwidths)
    {}

    glm::vec3 c;        // center glm::vec3
    glm::vec3 r;        // halfwidths
};


inline bool isCollisionCube(const AABB a, const AABB b)
{
    bool xOverlap = true;
    bool yOverlap = true;
    bool zOverlap = true;
    bool anyOverlap = false;

    if (std::fabs(a.c[0] - b.c[0]) > (a.r[0] + b.r[0]))
        xOverlap = false;
    if (std::fabs(a.c[1] - b.c[1]) > (a.r[1] + b.r[1]))
        yOverlap = false;
    if (std::fabs(a.c[2] - b.c[2]) > (a.r[2] + b.r[2]))
        zOverlap = false;

    anyOverlap = xOverlap && yOverlap && zOverlap;
    return anyOverlap;
}

inline bool isPointInsideSphere(const glm::vec3 point, const AABB sphere) {
    auto distance_sqr = (point.x - sphere.c.x) * (point.x - sphere.c.x) +
        (point.y - sphere.c.y) * (point.y - sphere.c.y) +
        (point.z - sphere.c.z) * (point.z - sphere.c.z);
    return distance_sqr < pow(sphere.r.x, 2);
}

inline bool isCollisionSphere(const AABB sphere, const AABB cube)
{
    bool anyOverlap = false;
    glm::vec3 p;
    p.x = std::max(cube.c.x - cube.r.x, std::min(sphere.c.x, cube.c.x + cube.r.x));
    p.y = std::max(cube.c.y - cube.r.y, std::min(sphere.c.y, cube.c.y + cube.r.y));
    p.z = std::max(cube.c.z - cube.r.z, std::min(sphere.c.z, cube.c.z + cube.r.z));
    anyOverlap = isPointInsideSphere(p, sphere);
    return anyOverlap;
}