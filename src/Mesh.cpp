#include "Mesh.h"

Poly LTriangle2ToPoly(const Loader3ds::LTriangle2& tri)
{
    Poly poly;
    for (int k = 0; k < 3; ++k)
    {
        poly.vertices[k] = LVectorTodvec3(tri.vertices[k]);
        poly.normals[k] = LVectorTodvec3(tri.vertexNormals[k]);
    }
    return poly;
}

bool Mesh::LoadFromFile(const std::string& filename, int mesh_name)
{
    std::cout << "Loading model: \"" << filename << "\"\n";
    Loader3ds::L3DS loader;
    if (!loader.LoadFile(filename.c_str()))
    {
        std::cout << "Error occured while loading .3ds file\n";
        return false;
    }

    int count_of_meshes = loader.GetMeshCount();
    std::cout << "Number of meshes: " << count_of_meshes << "\n";
    triangles_count = 0;

    Loader3ds::LMesh& mesh = loader.GetMesh(mesh_name);

    // Obtain bounding box
    for (int j = 0; j < mesh.GetVertexCount(); ++j)
    {
        glm::dvec3 coord = LVectorTodvec3(mesh.GetVertex(j));
        if (j == 0)
        {
            bounding_box.bounds[0] = bounding_box.bounds[1] = coord;
        }
        for (int k = 0; k < 3; k++)
        {
            bounding_box.bounds[0][k] = glm::min(coord[k], bounding_box.bounds[0][k]);
            bounding_box.bounds[1][k] = glm::max(coord[k], bounding_box.bounds[1][k]);
        }
    }

    // Load mesh data to quad tree
    root_node = std::make_unique<MeshQuadTreeNode>();
    for (unsigned j = 0; j < mesh.GetTriangleCount(); j++)
    {
        AddPoly(LTriangle2ToPoly(mesh.GetTriangle2(j)));
    }

    std::cout << "Model was successfully loaded. Number of polys: " << triangles_count << std::endl;
    return true;
}

bool PolyInBox(const Poly& poly, const BoundingBox& bounding_box)
{
    for (const auto& v : poly.vertices)
    {
        for (int k = 0; k < 3; ++k)
        {
            if (v[k] > bounding_box.bounds[1][k] ||
                v[k] < bounding_box.bounds[0][k])
            {
                return false;
            }
        }
    }
    return true;
}