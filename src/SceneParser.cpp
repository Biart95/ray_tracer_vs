#include "SceneParser.h"

void SceneParser::Parse(const std::string& arg_filename, Scene* arg_scene)
{
    fin.open(arg_filename);
    scene = arg_scene;

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
    if (entity_name == "SurfaceMaterial")
    {
        ParseSurfaceMaterial();
    }
    else if (entity_name == "InsideMaterial")
    {
        ParseInsideMaterial();
    }
    else if (entity_name == "Mesh")
    {
        ParseMesh();
    }
    else if (entity_name == "Object")
    {
        ParseObject();
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

void SceneParser::ParseSurfaceMaterial()
{
    SurfaceMaterial* m = new SurfaceMaterial;
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
            scene->surface_materials[material_name] = std::unique_ptr<SurfaceMaterial>(m);
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
        else if (left == "transparency_color") {
            m->transparency_color = ParseVec();
        }
        else {
            delete m;
            throw SyntaxError(std::string("Wrong material parameter: ") + left);
        }
    }
    throw SyntaxError("Closed bracket '}' not found");
}

void SceneParser::ParseInsideMaterial()
{
    InsideMaterial* m = new InsideMaterial;
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
            scene->inside_materials[material_name] = std::unique_ptr<InsideMaterial>(m);
            return;
        }

        fin >> str;
        if (str != "=")
        {
            delete m;
            throw SyntaxError("Expression divided by '=' is expected");
        }

        if (left == "refractive_index") {
            fin >> m->refractive_index;
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
            scene->surfaces[mesh_name] = std::unique_ptr<Mesh>(m);
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

void SceneParser::ParseObject()
{
    auto m = std::make_unique<Model>();
    Object3D obj;
    m->surface_material = scene->surface_materials["default"].get();

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
            obj.surface = m.get();
            scene->objects.push_back(obj);
            scene->models.emplace_back(std::move(m));
            return;
        }

        fin >> str;
        if (str != "=")
        {
            throw SyntaxError("Expression divided by '=' is expected");
        }

        if (left == "position") {
            m->SetPosition(ParseVec());
        }
        else if (left == "orientation") {
            m->SetOrientation(ParseVec());
        }
        else if (left == "scale") {
            m->SetScale(ParseVec());
        }
        else if (left == "surface") {
            std::string obj_name;
            fin >> obj_name;
            if (scene->surfaces.find(obj_name) == scene->surfaces.end())
                throw SyntaxError("No surface named" + obj_name + "found");
            m->surface = scene->surfaces[obj_name].get();
        }
        else if (left == "surface_material") {
            std::string material_name;
            fin >> material_name;
            if (scene->surface_materials.find(material_name) == scene->surface_materials.end())
                throw SyntaxError("No material named" + material_name + "found");
            m->surface_material = scene->surface_materials[material_name].get();
        }
        else if (left == "inside_material") {
            std::string material_name;
            fin >> material_name;
            if (scene->inside_materials.find(material_name) == scene->inside_materials.end())
                throw SyntaxError("No material named" + material_name + "found");
            obj.material = scene->inside_materials[material_name].get();
        }
        else {
            throw SyntaxError(std::string("Wrong mesh parameter: ") + left);
        }
    }
    throw SyntaxError("Closed bracket '}' not found");
}

void SceneParser::ParseLight()
{
    PointLight light;

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
