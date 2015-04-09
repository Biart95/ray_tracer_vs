#include "Renderer.h"
#include "SceneParser.h"
#include "fstream"
#include "iostream"

void main(int argc, char** argv)
{
    RayTracer tracer;
    Scene scene;
    SceneParser parser;
    parser.Parse("scene.txt", &scene);

    glm::uvec2 resolution = glm::uvec2(1024, 768);  // Default resolution

    if(argc == 2) // There is input file in parameters
    {
        std::ifstream filestream(argv[1]);
        if (filestream)
        {
            filestream >> resolution.x >> resolution.y;
        }
        else
            std::cout << "Invalid config path! Using default parameters." << "\n";
        filestream.close();
    }
    else
        printf("No config! Using default parameters.\r\n");

    tracer.scene = &scene;
    tracer.camera.position = glm::dvec3(0.0, 0.0, 0.0);
    tracer.camera.orientation = glm::dvec3(5.0, 0.0, 0.0);
    tracer.Render(resolution);
    tracer.SaveImageToFile("Result.png");
}