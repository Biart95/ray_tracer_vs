#pragma once

/*
    SceneParsing.h
    Contains scene loader
    Author: Artyom Bishev
*/

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

// This class provides methods for reading 3D scenes from file
class SceneParser
{
private:
    std::ifstream fin;
    Scene* scene;

public:
    ~SceneParser() {}
    SceneParser() {}

    // Load scene from file to Scene*
    void Parse(const std::string& arg_filename, Scene* arg_scene);

private:
    void ParseEntity();

    glm::dvec3 ParseVec();

    void ParseMaterial();

    void ParseMesh();

    void ParseModel();

    void ParseLight();
};


