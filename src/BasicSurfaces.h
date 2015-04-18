#pragma once

#include "Object3D.h"

/*
    BasicSurfaces.h
    Defines some common analytic surfaces
    Author: Artyom Bishev
*/

// Absolutely empty surface
class EmptySurface : public Surface
{
public:
    EmptySurface() {}
    virtual Intersection Intersect(const Ray& ray, bool inverted = false) const override
    {
        return Intersection();
    }
    virtual ~EmptySurface() {}
};

// Sphere
class Sphere : public Surface
{
public:
    Sphere() {}
    virtual Intersection Intersect(const Ray& ray, bool inverted = false) const override;
    virtual ~Sphere() {}
};

// Plane
class Plane : public Surface
{
public:
    Plane() {}
    virtual Intersection Intersect(const Ray& ray, bool inverted = false) const override;
    virtual ~Plane() {}
};

// This function implements Muller-Trumbore algorithm 
// for finding ray-triangle intersections
inline Intersection IntersectTriangle(const Ray& ray,
    glm::dvec3 x, glm::dvec3 y, glm::dvec3 z, // coordinates
    glm::dvec3 nx, glm::dvec3 ny, glm::dvec3 nz // normals
    )
{
    glm::dvec3 edge[2];
    edge[0] = x - z;
    edge[1] = y - z;
    glm::dvec3 P, Q, T;
    double det, inv_det, u, v;

    P = glm::cross(ray.direction, edge[1]);
    det = glm::dot(edge[0], P);
    if (det > -0.01 && det < 0.01)
        return Intersection();
    inv_det = 1.0 / det;

    T = ray.origin - z;
    u = glm::dot(T, P) * inv_det;
    if (u < 0.0 || u > 1.0)
    {
        return Intersection();
    }

    Q = glm::cross(T, edge[0]);

    double t = inv_det * glm::dot(Q, edge[1]);
    if (t < 0.0)
        return Intersection();

    v = glm::dot(ray.direction, Q) * inv_det;
    if (v < 0.0 || u + v > 1.0)
    {
        return Intersection();
    }

    Intersection intersection;
    intersection.coord =
        x * u +
        y * v +
        z * (1.0 - u - v);

    intersection.normal = glm::normalize(
        nx * u +
        ny * v +
        nz * (1.0 - u - v)
        );

    intersection.is_intersected = true;
    intersection.distance = glm::length2(intersection.coord - ray.origin);
    return intersection;
}

// Triangle (or polygon)
class Poly : public Surface
{
public:
    glm::dvec3 vertices[3];
    glm::dvec3 normals[3];
    Intersection Intersect(const Ray& ray, bool inverted = false) const override
    {
        Intersection intersection = ::IntersectTriangle(ray,
            vertices[0], vertices[1], vertices[2],
            normals[0], normals[1], normals[2]
            );

        if ((glm::dot(ray.direction, intersection.normal) > 0.0) != inverted)
            return Intersection();
        return intersection;
    }
};