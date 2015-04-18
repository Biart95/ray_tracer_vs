#include "Object3D.h"

using namespace glm;

Intersection Model::Intersect(const Ray& ray, bool inverted) const
{
    if (!surface)
        return Intersection();

    // Calc intersection in the local space
    Ray localRay;
    localRay.direction = GetModelMatrixInverse() * ray.direction;
    localRay.origin = GetModelMatrixInverse() * (ray.origin - position);
    Intersection intersection = surface->Intersect(localRay, inverted);
    if (!intersection) return Intersection();
    
    // Transform intersection data to the global space
    intersection.coord = GetModelMatrix() * intersection.coord + position;
    intersection.normal = glm::normalize(GetNormalMatrix() * intersection.normal);
    intersection.distance = glm::length2(intersection.coord - ray.origin);

    // If the intersection is too close to the ray origin, discard it
    if (intersection.distance < TRACER_EPSILON) return Intersection();

    // Set intersection material
    intersection.material = surface_material;

    return intersection;
}
