#pragma once

#include "glm\glm.hpp"
#include "Types.h"

class Material
{
public:
    double shininess = 10.0f;
    glm::dvec3 specular;
    glm::dvec3 diffuse;
    glm::dvec3 reflective_color;
    double refractive_index = 1.0;
    glm::dvec3 refractive_color;

    glm::dvec3 DiffuseColor(const Intersection& s, const Light& light)
    {
        if (glm::length(diffuse) < FLT_EPSILON)
            return glm::dvec3(0.0f);
        double intensity = glm::dot(s.normal, glm::normalize(light.center - s.coord));
        if (intensity < FLT_EPSILON)
            return glm::dvec3(0.0f);
        return light.color * diffuse * intensity;
    }

    glm::dvec3 SpecularColor(const Ray& ray, const Intersection& s, const Light& light)
    {
        if (glm::length(specular) < FLT_EPSILON)
            return glm::dvec3(0.0f);
        glm::dvec3 reflected = glm::reflect(glm::normalize(light.center - s.coord), s.normal);
        double intensity = glm::dot(glm::normalize(ray.direction), reflected);
        if (intensity < FLT_EPSILON)
            return glm::dvec3(0.0f);
        return light.color * specular * glm::pow(intensity, shininess);
    }
};