#pragma once

#include "Scene.h"
#include <string>
#include <fstream>
#include <exception>
#include "Material.h"
class SyntaxError : std::exception
{
public:
    SyntaxError(const std::string& msg) : msg(msg) {};
    virtual const const char* what() const throw()
    {
        return msg.c_str();
    }
    std::string msg;
};

class SceneParser
{
private:
    std::ifstream fin;
    Scene* scene;

public:
    ~SceneParser() {}
    SceneParser() {}

    void Parse(const std::string& arg_filename, Scene* arg_scene);

private:
    void ParseEntity();

    glm::dvec3 ParseVec();

    void ParseMaterial();

    void ParseMesh();

    void ParseModel();

    void ParseLight();
};


