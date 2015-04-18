#pragma once

/*
    Material.h
    Different types of materials and texturing
    Author: Artyom Bishev
*/

#include "glm\glm.hpp"
#include "Types.h"

// Basic surface material
class SurfaceMaterial
{
public:
    double shininess = 10.0f;      // shininess coefficient for the hotspots
    glm::dvec3 specular;           // specular color
    glm::dvec3 diffuse;            // diffuse color
    glm::dvec3 reflective_color;   // color of reflecions
    glm::dvec3 transparency_color; // color of transparency

    glm::dvec3 Color(
        const glm::dvec3& normal,
        const glm::dvec3& point,
        const glm::dvec3& view_vector,
        const PointLight& light
        ) const
    {
        return DiffuseColor(normal, point, light) + SpecularColor(normal, point, view_vector, light);
    }

    // Calculate diffuse color
    glm::dvec3 DiffuseColor(
        const glm::dvec3& normal,
        const glm::dvec3& point,
        const PointLight& light
        ) const
    {
        if (glm::length(diffuse) < FLT_EPSILON)
            return glm::dvec3(0.0f);
        double dist = glm::distance(light.center, point);
        double intensity = glm::dot(normal, (light.center - point) / glm::pow3(dist));
        if (intensity < FLT_EPSILON)
            return glm::dvec3(0.0f);
        return light.color * diffuse * intensity;
    }

    // Calculate specular color
    glm::dvec3 SpecularColor(
        const glm::dvec3& normal, 
        const glm::dvec3& point,
        const glm::dvec3& view_vector,
        const PointLight& light
        ) const
    {
        if (glm::length(specular) < FLT_EPSILON)
            return glm::dvec3(0.0f);
        double dist = glm::distance(light.center, point);
        glm::dvec3 reflected = glm::reflect((light.center - point) / dist, normal);
        double intensity = glm::dot(glm::normalize(view_vector), reflected);
        if (intensity < FLT_EPSILON)
            return glm::dvec3(0.0f);
        return light.color * specular * glm::pow(intensity, shininess) / glm::pow2(dist);
    }
};

// Basic inside material
class InsideMaterial
{
public:
    double refractive_index = 1.0; // refractive index
    //glm::dvec3 color; // color of inside
};

// Calculate percentage of reflected energy (opposite to the amount of refracted energy) 
// using Frensel equations
inline double FrenselReflectance(double cosine, double relative_refractive_index)
{
    if ((1.0 - glm::pow2(cosine)) * glm::pow2(relative_refractive_index) >= 1.0)
        return 1.0;

    double n = 1.0 / relative_refractive_index;

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
