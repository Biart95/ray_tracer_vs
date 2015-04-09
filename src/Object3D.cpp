#include "Object3D.h"

using namespace glm;

Intersection Sphere::Intersect(const Ray& ray) const
{
    float a = dot(ray.direction, ray.direction);
    float b = dot(ray.direction, ray.origin);
    float c = dot(ray.origin, ray.origin) - 1.0;
    float D = b * b - a * c;
    if (D < 0.0f)
        return Intersection();

    Intersection intersection;
    intersection.is_intersected = true;
    double t;

    // Smaller root of quadratic equation
    t = (-b - sqrt(D)) / a;
    intersection.coord = ray.origin + ray.direction * t;
    if (t > 0.0 && glm::length2(intersection.coord - ray.origin) > TRACER_EPSILON)
    {
        intersection.normal = normalize(intersection.coord);
        return intersection;
    }
    // Bigger root of quadratic equation
    t = (-b + sqrt(D)) / a;
    intersection.coord = ray.origin + ray.direction * t;
    if (t > 0.0 && glm::length2(intersection.coord - ray.origin) > TRACER_EPSILON)
    {
        intersection.normal = normalize(intersection.coord);
        return intersection;
    }
    return Intersection();
}

Intersection Plane::Intersect(const Ray& ray) const
{
    Intersection intersection;
    dvec3 vertices[4] = {
        dvec3(-0.5, -0.5, 0.0),
        dvec3(-0.5, 0.5, 0.0),
        dvec3(0.5, 0.5, 0.0),
        dvec3(0.5, -0.5, 0.0)
    };
    dvec3 normal = dvec3(0.0, 0.0, -1.0);

    // First triangle
    intersection = IntersectTriangle(ray,
        vertices[0], vertices[1], vertices[2],
        normal, normal, normal
        );
    if (intersection) return intersection;
    // Second triangle
    intersection = IntersectTriangle(ray,
        vertices[0], vertices[2], vertices[3],
        normal, normal, normal
        );
    return intersection;
}