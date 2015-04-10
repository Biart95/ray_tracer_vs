#include "Renderer.h"
#include "atlimage.h"

using namespace glm;

Ray RayTracer::MakeRay(uvec2 pixelPos)
{
    dvec3 localDirection;
    localDirection.x = (pixelPos.x - resolution.x / 2.0f) / resolution.x;
	localDirection.y = (pixelPos.y - resolution.y / 2.0f) / resolution.x;
    localDirection.z = 0.5f / atan(camera.viewAngle / 2.0f);
    return Ray(camera.position, camera.GetRotateMatrix() * localDirection);
}

glm::dvec3 RayTracer::TraceRay(Ray ray, int step = 0)
{
	// Check whether recursive tracing is too deep to proceed calculations
    if (step >= maxRenderStep)
        return backgroundColor;
	// Find the nearest intersection of the ray and the scene
    Intersection intersection;
    Model* intersectedModel = nullptr;
    for (auto& model : scene->models)
    {
        Intersection currentIntersection = model.Intersect(ray);
        if (currentIntersection &&
            (!intersection || intersection.distance > currentIntersection.distance)) 
			// if there is no yet any intersections found or this intersection is closer than the previous one
        {
			// refresh the intersection data
            intersection = currentIntersection;
            intersectedModel = &model;
        }
    }

    dvec3 color = backgroundColor;
    if (!intersection)
        return color;

	// Calculate basic color
	// using Phong model
    for (auto& light : scene->lights)
    {
		// Diffuse shading
        color += intersectedModel->material->DiffuseColor(intersection, light);
		// Specular shading
        color += intersectedModel->material->SpecularColor(ray, intersection, light);
    }

    double k = intersectedModel->material->refractive_index;
    if (intersection.face_side < 0) k = 1.0 / k;
    double R = intersectedModel->material->FrenselReflectance(
        normalizeDot(-ray.direction, intersection.normal)
        );
    double T = 1.0 - R;

    dvec3 reflective_color = intersectedModel->material->refractive_color * R +
        intersectedModel->material->reflective_color;
    dvec3 refractive_color = intersectedModel->material->refractive_color * T;

	// If reflectance effect on the resulting pixel is sufficient,
	// trace the reflected ray further
    if (glm::length(reflective_color) > TRACER_EPSILON)
    {
        Ray reflected; // reflected ray
        reflected.direction = glm::reflect(ray.direction, intersection.normal);
        reflected.origin = intersection.coord;

        color += reflective_color * TraceRay(reflected, step + 1);
    }

    // If transparency effect on the resulting pixel is sufficient,
    // trace the refracted ray further
    if (glm::length(refractive_color) > TRACER_EPSILON)
    {
        Ray refracted; // refracted ray

        refracted.direction = glm::refract(ray.direction, intersection.normal, k);
        refracted.origin = intersection.coord;

        color += refractive_color * TraceRay(refracted, step + 1);
    }

    return color;
}

void RayTracer::Render(uvec2 res)
{
	// Set resolution
    resolution = res;
    pixels.resize(resolution.x * resolution.y);

	// For each image pixel, trace the corresponding ray
    #pragma omp parallel for schedule(guided)
    for (int i = 0; i < resolution.y; i++)
    {
        for (int j = 0; j < resolution.x; j++)
        {
            Ray ray = MakeRay(uvec2(j, i));
            pixels[i * resolution.x + j] = TraceRay(ray);
        }
    }
}

void RayTracer::SaveImageToFile(std::string fileName)
{
    CImage image;

    int width = resolution.x;
    int height = resolution.y;

    image.Create(width, height, 24);
    
	int pitch = image.GetPitch();
	unsigned char* imageBuffer = (unsigned char*)image.GetBits();

	if (pitch < 0)
	{
		imageBuffer += pitch * (height - 1);
		pitch =- pitch;
	}

	int i, j;
	int image_displacement = 0;
    int texture_displacement = 0;

	for (i = 0; i < height; i++)
	{
        for (j = 0; j < width; j++)
        {
            dvec3 color = pixels[texture_displacement + j];
            for (int k = 0; k < 3; k++)
                imageBuffer[image_displacement + 3*j + k] = clamp(color[2 - k], 0.0, 1.0) * 255.0;
        }

        image_displacement += pitch;
        texture_displacement += width;
	}

    image.Save(fileName.c_str());
	image.Destroy();
}