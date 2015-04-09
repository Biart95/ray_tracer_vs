#pragma once

/*
    Mesh.h
    Defines 3D models as they are represented in RayTracer
    Author: Artyom Bishev
*/

#include "glm\glm.hpp"
#include "glm\ext.hpp"
#include "Types.h"
#include "Object3D.h"
#include "L3DS\l3ds.h"
#include <vector>
#include <string>
#include <iostream>

// Convert LVector structs from L3DS to vec3
template<typename T>
glm::dvec3 LVectorTodvec3(const T& input)
{
    glm::dvec3 result;
    result.x = input.x;
    result.y = input.y;
    result.z = input.z;
    return result;
}

inline std::ostream& operator<<(std::ostream& out, const glm::dvec3& vec)
{
    return out << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
}

// 3D mesh of polygonal object
class Mesh : public Object3D
{
public:
    std::vector<glm::uvec3> indices;
    std::vector<glm::dvec3> vertices;
    std::vector<glm::dvec3> normals;
    BoundingBox boundingBox;

    bool LoadFromFile(const std::string& filename, int mesh_name = 0);
    Intersection Intersect(const Ray& ray) const;

	Mesh() {};
	~Mesh() {};

private:
	Intersection IntersectTriangle(const Ray& ray, glm::uvec3 indices) const
	{  
        return ::IntersectTriangle(ray, 
            vertices[indices.x], vertices[indices.y], vertices[indices.z],
            normals[indices.x], normals[indices.y], normals[indices.z]
            );  
	}
};


