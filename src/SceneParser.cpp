#include "SceneParser.h"

void SceneParser::Parse(const std::string& arg_filename, Scene* arg_scene)
{
    fin.open(arg_filename);
    scene = arg_scene;

    scene->meshes["sphere"] = std::make_unique<Sphere>();
    scene->meshes["plane"] = std::make_unique<Plane>();
    scene->materials["default"] = std::make_unique<Material>();

    while (fin)
    {
        ParseEntity();
    }
}

void SceneParser::ParseEntity()
{
    std::string entity_name;
    fin >> entity_name;
    if (entity_name.empty())
        return;
    Material material;
    if (entity_name == "Material")
    {
        ParseMaterial();
    }
    else if (entity_name == "Mesh")
    {
        ParseMesh();
    }
    else if (entity_name == "Model")
    {
        ParseModel();
    }
    else if (entity_name == "Light")
    {
        ParseLight();
    }
    else
    {
        throw SyntaxError(std::string("Wrong entity name: ") + entity_name);
    }
}

glm::dvec3 SceneParser::ParseVec()
{
    glm::dvec3 vec;
    fin >> vec.x >> vec.y >> vec.z;
    return vec;
}

void SceneParser::ParseMaterial()
{
    Material* m = new Material;
    std::string material_name;
    fin >> material_name;

    std::string str;
    fin >> str;
    if (str != "{")
    {
        delete m;
        throw SyntaxError("Open bracket '{' expected");
    }
    while (fin)
    {
        std::string left;
        fin >> left;
        if (left == "}")
        {
            scene->materials[material_name] = std::unique_ptr<Material>(m);
            return;
        }

        fin >> str;
        if (str != "=")
        {
            delete m;
            throw SyntaxError("Expression divided by '=' is expected");
        }

        if (left == "shininess") {
            fin >> m->shininess;
        }
        else if (left == "specular") {
            m->specular = ParseVec();
        }
        else if (left == "diffuse") {
            m->diffuse = ParseVec();
        }
        else if (left == "reflective_color") {
            m->reflective_color = ParseVec();
        }
        else if (left == "refractive_index") {
            fin >> m->refractive_index;
        }
        else if (left == "refractive_color") {
            m->refractive_color = ParseVec();
        }
        else {
            delete m;
            throw SyntaxError(std::string("Wrong material parameter: ") + left);
        }
    }
    throw SyntaxError("Closed bracket '}' not found");
}

void SceneParser::ParseMesh()
{
    Mesh* m = new Mesh;
    std::string mesh_name;
    fin >> mesh_name;

    std::string str;
    fin >> str;
    if (str != "{")
    {
        delete m;
        throw SyntaxError("Open bracket '{' expected");
    }
    while (fin)
    {
        std::string left;
        fin >> left;
        if (left == "}")
        {
            scene->meshes[mesh_name] = std::unique_ptr<Mesh>(m);
            return;
        }

        fin >> str;
        if (str != "=")
        {
            delete m;
            throw SyntaxError("Expression divided by '=' is expected");
        }

        if (left == "filename") {
            std::string filename;
            fin >> filename;
            int index;
            fin >> index;
            m->LoadFromFile("Models\\" + filename, index);
        }
        else {
            delete m;
            throw SyntaxError(std::string("Wrong mesh parameter: ") + left);
        }
    }
    throw SyntaxError("Closed bracket '}' not found");
}

void SceneParser::ParseModel()
{
    Model m;
    m.material = scene->materials["default"].get();

    std::string str;
    fin >> str;
    if (str != "{")
    {
        throw SyntaxError("Open bracket '{' expected");
    }
    while (fin)
    {
        std::string left;
        fin >> left;
        if (left == "}")
        {
            scene->models.emplace_back(std::move(m));
            return;
        }

        fin >> str;
        if (str != "=")
        {
            throw SyntaxError("Expression divided by '=' is expected");
        }

        if (left == "position") {
            m.position = ParseVec();
        }
        else if (left == "orientation") {
            m.orientation = ParseVec();
        }
        else if (left == "scale") {
            m.scale = ParseVec();
        }
        else if (left == "object") {
            std::string obj_name;
            fin >> obj_name;
            if (scene->meshes.find(obj_name) == scene->meshes.end())
                throw SyntaxError("No mesh named" + obj_name + "found");
            m.object = scene->meshes[obj_name].get();
        }
        else if (left == "material") {
            std::string material_name;
            fin >> material_name;
            if (scene->materials.find(material_name) == scene->materials.end())
                throw SyntaxError("No material named" + material_name + "found");
            m.material = scene->materials[material_name].get();
        }
        else {
            throw SyntaxError(std::string("Wrong mesh parameter: ") + left);
        }
    }
    throw SyntaxError("Closed bracket '}' not found");
}

void SceneParser::ParseLight()
{
    Light light;

    std::string str;
    fin >> str;
    if (str != "{")
    {
        throw SyntaxError("Open bracket '{' expected");
    }
    while (fin)
    {
        std::string left;
        fin >> left;
        if (left == "}")
        {
            scene->lights.push_back(light);
            return;
        }

        fin >> str;
        if (str != "=")
        {
            throw SyntaxError("Expression divided by '=' is expected");
        }

        if (left == "position") {
            light.center = ParseVec();
        } 
        else if (left == "color") {
            light.color = ParseVec();
        }
        else {
            throw SyntaxError(std::string("Wrong light parameter: ") + left);
        }
    }
    throw SyntaxError("Closed bracket '}' not found");
}
