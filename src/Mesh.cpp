#include "Mesh.h"

bool Mesh::LoadFromFile(const std::string& filename, int mesh_name)
{
    std::cout << "Loading model: \"" << filename << "\"\n";
    Loader3ds::L3DS loader;
    if (!loader.LoadFile(filename.c_str()))
    {
        std::cout << "Error occured while loading .3ds file\n";
        return false;
    }

    int countOfMeshes = loader.GetMeshCount();
    std::cout << "Number of meshes: " << countOfMeshes << "\n";
    int polygonsCount = 0;
    int verticesCount = 0;

    Loader3ds::LMesh& mesh = loader.GetMesh(mesh_name);
    // Load vertex data
    int currentCountOfVertices = mesh.GetVertexCount();
    for (int j = 0; j < currentCountOfVertices; ++j)
    {
        const Loader3ds::LVector4& lvert = mesh.GetVertex(j);
        glm::dvec3 coord = LVectorTodvec3(lvert);
        if (j == 0)
        {
            boundingBox.lowerBound = boundingBox.upperBound = coord;
        }
        for (int k = 0; k < 3; k++)
        {
            boundingBox.lowerBound[k] = glm::min(coord[k], boundingBox.lowerBound[k]);
            boundingBox.upperBound[k] = glm::max(coord[k], boundingBox.upperBound[k]);
        }
        vertices.emplace_back(coord);
        normals.emplace_back(LVectorTodvec3(mesh.GetNormal(j)));
    }

    // Load indices data
    unsigned currentCountOfTriangles = mesh.GetTriangleCount();
    for (unsigned j = 0; j < currentCountOfTriangles; j++)
    {
        indices.emplace_back(
            mesh.GetTriangle(j).a,
            mesh.GetTriangle(j).b,
            mesh.GetTriangle(j).c
            );
    }
    verticesCount += currentCountOfVertices;
    polygonsCount += currentCountOfTriangles;

    std::cout << "Model was successfully loaded. Number of polys: " << polygonsCount << std::endl;
    return true;
}

Intersection Mesh::Intersect(const Ray& ray) const
{
	if (!boundingBox.Intersect(ray))
	{	
		return Intersection();
	}

    Intersection intersection;
    for (auto poly : indices)
    {
        Intersection currentPolyIntersection = IntersectTriangle(ray, poly);

        if (currentPolyIntersection && currentPolyIntersection.distance >= TRACER_EPSILON)
        {
            if (
                !intersection ||
                intersection.distance > currentPolyIntersection.distance
                )
            {
                intersection = currentPolyIntersection;
            }
        }
    }
    return intersection;
}