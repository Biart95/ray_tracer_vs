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
        surfaces["sphere"] = std::make_unique<Sphere>();
        surfaces["plane"] = std::make_unique<Plane>();
        surface_materials["default"] = std::make_unique<SurfaceMaterial>();
        inside_materials["default"] = std::make_unique<InsideMaterial>();

        empty_object.material = inside_materials["default"].get();
        empty_object.surface = &empty_surface;
    }

    // Lights
    std::vector<PointLight> lights;

    // Objects (all of them will be rendered)
    std::vector<Object3D> objects;

    // Surfaces
    std::map<std::string, std::unique_ptr<Surface>> surfaces;

    // Models
    std::vector<std::unique_ptr<Model>> models;

    // Materials
    std::map<std::string, std::unique_ptr<SurfaceMaterial>> surface_materials;
    std::map<std::string, std::unique_ptr<InsideMaterial>> inside_materials;

    // Object representing the empty space outside of all of the other objects
    Object3D empty_object;

private:
    // Surface of the empty_object
    Surface empty_surface;
};
