#pragma once

#include "Types.h"
#include "Mesh.h"
#include "BasicSurfaces.h"
#include "Object3D.h"

#include <vector>
#include <map>
#include <memory>

/*
    Scene.h
    Scene that contains all rendered objects 
    and other stuff such as lights, cameras, e.t.c
    Author: Artyom Bishev
*/

class Scene
{
public:
    Scene() 
    {
        empty_object.material = &empty_material;
        empty_object.surface = &empty_surface;
    }
    std::vector<PointLight> lights;
    std::vector<Object3D> objects;
    std::map<std::string, std::unique_ptr<Surface>> surfaces;
    std::vector<std::unique_ptr<Model>> models;
    std::map<std::string, std::unique_ptr<SurfaceMaterial>> surface_materials;
    std::map<std::string, std::unique_ptr<InsideMaterial>> inside_materials;
    Object3D empty_object;
private:
    InsideMaterial empty_material;
    EmptySurface empty_surface;
};
