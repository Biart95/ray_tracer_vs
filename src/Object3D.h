#pragma once

/*
    Mesh.h
    Defines several classes of 3D shapes
    Author: Artyom Bishev
*/

#include "Types.h"
#include "Material.h"

// Base class for any renderable 3D object in scene
class Object3D
{
public:
    Object3D() {}
    virtual Intersection Intersect(const Ray& ray) const = 0;
    virtual ~Object3D() {};
};

// Sphere
class Sphere : public Object3D
{
public:
    Sphere() {}
    virtual Intersection Intersect(const Ray& ray) const;
    virtual ~Sphere() {}
};

// Plane
class Plane : public  Object3D
{
public:
    Plane() {}
    virtual Intersection Intersect(const Ray& ray) const;
    virtual ~Plane() {}
};

// This function implements Muller-Trumbore algorithm 
// for finding ray-triangle intersections
inline Intersection IntersectTriangle(const Ray& ray, 
    glm::dvec3 x , glm::dvec3 y , glm::dvec3 z , // coordinates
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

// 3D model with specified location and orientation and specified material
// Stores a pointer to the corresponding 3D object
struct Model
{
    const Object3D* object;

    glm::dvec3 position;
    glm::dvec3 orientation;
    glm::dvec3 scale;

    Material* material;

    // Get model matrix
    glm::dmat3 GetModelMatrix() const
    {
        glm::dmat4 model_matrix = glm::dmat4(1.0f);
        for (int k = 0; k < 3; k++)
        {
            glm::dvec3 axis(0.0f);
            axis[k] = 1.0f;
            model_matrix = glm::rotate(model_matrix, orientation[k], axis);
        }
        model_matrix = glm::scale(model_matrix, scale);
        return glm::dmat3(model_matrix);
    }

    // Calculate intersection
    Intersection Intersect(const Ray& ray) const
    {
        if (!object)
            return Intersection();

        // Calculate the matrices
        glm::dmat3 model_matrix = GetModelMatrix();
        glm::dmat3 model_matrix_inverse = glm::inverse(model_matrix);
        glm::dmat3 model_matrix_inverse_transpose = glm::inverseTranspose(model_matrix);

        // Calc intersection in the local space
        Ray localRay;
        localRay.direction = model_matrix_inverse * ray.direction;
        localRay.origin = model_matrix_inverse * (ray.origin - position);
        Intersection intersection = object->Intersect(localRay);
        if (!intersection) return intersection;

        // Transform intersection data to the global space
        intersection.coord = model_matrix * intersection.coord + position;
        intersection.normal = glm::normalize(model_matrix_inverse_transpose * intersection.normal);
        intersection.distance = glm::length2(intersection.coord - ray.origin);

        // If the intersection is too close to the ray origin, discard it
        if (intersection.distance < TRACER_EPSILON)
            return Intersection();
        // Set proper side of the intersected surface
        if (glm::dot(ray.direction, intersection.normal) > 0.0)
        {
            intersection.face_side = -intersection.face_side;
            intersection.normal = -intersection.normal;
        }

        return intersection;
    }
};
