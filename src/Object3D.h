#pragma once

/*
    Object3D.h
    Defines base classes for 3D entities
    Author: Artyom Bishev
*/

#include "Types.h"
#include "Material.h"


// Contains all neccessary information about ray-object intersection
struct Intersection
{
    bool is_intersected = false;
    glm::dvec3 coord;
    glm::dvec3 normal;
    const SurfaceMaterial* material = nullptr;
    double distance;
    operator bool() const
    {
        return is_intersected;
    }
};


/*
    Base class for any intersectable 3D shape in scene
*/
class Surface
{
public:
    Surface() {}
    virtual Intersection Intersect(const Ray& ray, bool inverted = false) const = 0;
    virtual ~Surface() {};
};


/*
    3D surface with the specified location, scale and orientation and the specified surface material
    Stores a pointer to the corresponding 3D shape in local coordinate space
*/
class Model : public Surface
{
public:
    const Surface* surface; // contained surface
    const SurfaceMaterial* surface_material; // material of this surface
    bool inverted = false; // true if the object is turned inside out (So, it describes CSG NOT operation)

    // Calculate intersection
    Intersection Intersect(const Ray& ray, bool inverted = false) const override;

    // Getters & setters
    glm::dmat3 GetModelMatrix() const
    {
        return model_matrix;
    }
    glm::dmat3 GetModelMatrixInverse() const
    {
        return model_matrix_inverse;
    }
    glm::dmat3 GetNormalMatrix() const
    {
        return model_matrix_inverse_transpose;
    }
    const glm::dvec3& GetPosition()
    {
        return position;
    }
    const glm::dvec3& GetOrientation()
    {
        return orientation;
    }
    const glm::dvec3& GetScale()
    {
        return scale;
    }
    void SetPosition(const glm::dvec3& new_position)
    {
        position = new_position;
        RenewMatrices();
    }
    void SetOrientation(const glm::dvec3& new_orientation)
    {
        orientation = new_orientation;
        RenewMatrices();
    }
    void SetScale(const glm::dvec3& new_scale)
    {
        scale = new_scale;
        RenewMatrices();
    }

private:
    glm::dmat3 model_matrix;
    glm::dmat3 model_matrix_inverse;
    glm::dmat3 model_matrix_inverse_transpose;

    glm::dvec3 position;
    glm::dvec3 orientation;
    glm::dvec3 scale;

    void RenewMatrices()
    {
        glm::dmat4 model_mat4 = glm::dmat4(1.0f);
        for (int k = 0; k < 3; k++)
        {
            glm::dvec3 axis(0.0f);
            axis[k] = 1.0f;
            model_mat4 = glm::rotate(model_mat4, orientation[k], axis);
        }
        model_mat4 = glm::scale(model_mat4, scale);
        model_matrix = glm::dmat3(model_mat4);
        model_matrix_inverse = glm::inverse(model_matrix);
        model_matrix_inverse_transpose = glm::inverseTranspose(model_matrix);
    }
};


/*
    3D object
    Describes the surface plus the area inside
    It is assumed, that one object can only lie entirely inside another one, 
    or totally separate from the others.
*/
class Object3D
{
public:
    Surface* surface = nullptr;
    InsideMaterial* material = nullptr;
};
