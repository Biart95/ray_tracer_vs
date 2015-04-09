#pragma once

#include "Types.h"
#include "Mesh.h"
#include <vector>
#include <map>
#include <memory>

class Scene
{
public:
    Scene() {}
    std::vector<Light> lights;
    std::vector<Model> models;
    std::map<std::string, std::unique_ptr<Object3D>> meshes;
    std::map<std::string, std::unique_ptr<Material>> materials;
};