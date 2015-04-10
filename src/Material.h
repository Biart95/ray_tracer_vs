#pragma once

/*
    Material.h
    Different types of materials and texturing
    Author: Artyom Bishev
*/

#include "glm\glm.hpp"
#include "Types.h"

// Basic material
class Material
{
public:
    double shininess = 10.0f;
    glm::dvec3 specular;  // specular color
    glm::dvec3 diffuse;   // diffuse color
    glm::dvec3 reflective_color; // color of reflecions
    double refractive_index = 1.0; // refraction index
    glm::dvec3 refractive_color; // color of refraction

    // Calculate diffuse color
    glm::dvec3 DiffuseColor(const Intersection& s, const Light& light)
    {
        if (glm::length(diffuse) < FLT_EPSILON)
            return glm::dvec3(0.0f);
        double intensity = glm::dot(s.normal, glm::normalize(light.center - s.coord));
        if (intensity < FLT_EPSILON)
            return glm::dvec3(0.0f);
        return light.color * diffuse * intensity;
    }

    // Calculate specular color
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

    // Calculate percentage of reflected energy (opposite to the amount of refracted energy) 
    // using Frensel equations
    double FrenselReflectance(double cosine)
    {
        double n = 1.0 / refractive_index;

        if ((1.0 - glm::pow2(cosine)) * glm::pow2(refractive_index) >= 1.0)
            return 1.0;

        //Frensel equations
        double ci2 = glm::pow2(cosine);
        double si2 = 1.0 - ci2;
        double si4 = glm::pow2(si2);
        double a = ci2 + n * n - 1.0f;
        double sqa = 2 * glm::sqrt(a) * cosine;
        double b = ci2 + a;
        double c = ci2 * a + si4;
        double d = sqa * si2;
        double reflectance = (b - sqa) / (b + sqa) * (1.0 + (c - d) / (c + d)) * 0.5;

        return glm::clamp(reflectance, 0.0, 1.0);
    }
};
