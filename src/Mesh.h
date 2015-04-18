#pragma once

/*
    Mesh.h
    Defines polygonal 3D models as they are represented in RayTracer
    Author: Artyom Bishev
*/

#include "glm\glm.hpp"
#include "glm\ext.hpp"
#include "Types.h"
#include "Object3D.h"
#include "BasicSurfaces.h"
#include "L3DS\l3ds.h"
#include <vector>
#include <memory>
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

bool PolyInBox(const Poly& poly, const BoundingBox& bounding_box);

struct MeshOctreeNode
{
    std::vector<MeshOctreeNode> subtrees;
    std::vector<Poly> triangles;
    void AddPoly(const Poly& poly, const BoundingBox& bounding_box)
    {
        glm::dvec3 center = glm::mix(bounding_box.bounds[0], bounding_box.bounds[1], 0.5);
        for (int i = 0; i < 8; ++i)
        {
            BoundingBox subbox = bounding_box;
            for (int j = 0; j < 3; ++j)
            {
                subbox.bounds[(i >> j) & 1][j] = center[j];
            }
            if (PolyInBox(poly, subbox))
            {
                if (subtrees.empty())
                    subtrees.resize(8);
                subtrees[i].AddPoly(poly, subbox);
                return;
            }
        }
        triangles.push_back(poly);
    }
    Intersection Intersect(const Ray& ray, const BoundingBox& bounding_box, bool inverted)
    {
        Intersection intersection;
        for (const auto& poly : triangles)
        {
            auto current_intersection = poly.Intersect(ray, inverted);
            if (current_intersection && current_intersection.distance >= TRACER_EPSILON)
            {
                if (!intersection || current_intersection.distance < intersection.distance)
                {
                    intersection = current_intersection;
                }
            }
        }

        if (subtrees.empty())
            return intersection;

        glm::dvec3 center = glm::mix(bounding_box.bounds[0], bounding_box.bounds[1], 0.5);
        for (int i = 0; i < 8; ++i)
        {
            BoundingBox subbox = bounding_box;
            for (int j = 0; j < 3; ++j)
            {
                subbox.bounds[(i >> j) & 1][j] = center[j];
            }
            if (subbox.Intersect(ray))
            {
                auto current_intersection = subtrees[i].Intersect(ray, subbox, inverted);
                if (current_intersection)
                {
                    if (!intersection || current_intersection.distance < intersection.distance)
                    {
                        intersection = current_intersection;
                    }
                }
            }
        }

        return intersection;
    }
};

// 3D mesh of polygonal object
class Mesh : public Surface
{
public:
    // Load mesh from .3ds
    bool LoadFromFile(const std::string& filename, int mesh_name = 0);

    Intersection Intersect(const Ray& ray, bool inverted = false) const override
    {
        if (!bounding_box.Intersect(ray))
            return Intersection();
        return root_node->Intersect(ray, bounding_box, inverted);
    }

	Mesh() {};
	~Mesh() {};

private:
    void AddPoly(const Poly& poly)
    {
        assert(root_node);
        assert(PolyInBox(poly, bounding_box));
        root_node->AddPoly(poly, bounding_box);
        ++triangles_count;
    }

    BoundingBox bounding_box;
    std::unique_ptr<MeshOctreeNode> root_node;
    int triangles_count;
};
