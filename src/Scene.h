#pragma once

#include "Types.h"
#include "Mesh.h"
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
    Scene() {}
    std::vector<Light> lights;
    std::vector<Model> models;
    std::map<std::string, std::unique_ptr<Object3D>> meshes;
    std::map<std::string, std::unique_ptr<Material>> materials;
};
