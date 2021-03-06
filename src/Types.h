#pragma once

/*
    Types.h
    Common types used in RayTracer
    Author: Artyom Bishev
*/

#include "glm/glm.hpp"
#include "glm/ext.hpp"
#include <vector>
#include <stack>

#define TRACER_EPSILON 0.0000004

class Object3D;

// Ray
struct Ray
{
    Ray() {}
    Ray(glm::dvec3 _origin, glm::dvec3 _direction)
        : direction(glm::normalize(_direction)), origin(_origin) {};

    glm::dvec3 origin;
    glm::dvec3 direction;

    std::stack<Object3D*> current_object_insides;
};

struct Camera
{
    glm::dvec3 position;          // Camera position
    glm::dvec3 orientation;       // Camera orientation

    glm::dmat3 GetRotateMatrix()
    {
        glm::dmat4 rotate_matrix(1.0f);
        for (int k = 0; k < 3; k++)
        {
            glm::dvec3 axis(0.0f);
            axis[k] = 1.0f;
            rotate_matrix = glm::rotate(rotate_matrix, orientation[k], axis);
        }
        return glm::dmat3(rotate_matrix);
    }

    double viewAngle = 3.14f / 3.0f;    // View angles (radians)
};

struct PointLight
{
    PointLight(glm::dvec3 _center = glm::dvec3(), glm::dvec3 _color = glm::dvec3())
        : center(_center), color(_color) {}
    glm::dvec3 center;
    glm::dvec3 color;
};

struct BoundingBox
{
    glm::dvec3 bounds[2];
    bool Intersect(const Ray& ray) const
    {
        glm::dvec3 tMin, tMax;
        for (int k = 0; k < 3; k++)
        {
            tMin[k] = (bounds[0][k] - ray.origin[k]) / ray.direction[k];
            tMax[k] = (bounds[1][k] - ray.origin[k]) / ray.direction[k];
            if (ray.direction[k] < 0) std::swap(tMin[k], tMax[k]);
            if (tMax[k] < 0) return false;
        }

        return glm::max(tMin.x, tMin.y, tMin.z) < glm::min(tMax.x, tMax.y, tMax.z);
    }
};

inline glm::dvec3 GammaCompression(glm::dvec3 physical_color, double gamma = 2.1)
{
    return glm::pow(physical_color, glm::dvec3(1 / gamma));
}
