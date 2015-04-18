#include "Renderer.h"
#include "atlimage.h"

using namespace glm;

Ray RayTracer::MakeRay(uvec2 pixelPos)
{
    dvec3 localDirection;
    localDirection.x = (pixelPos.x - resolution.x / 2.0f) / resolution.x;
	localDirection.y = (pixelPos.y - resolution.y / 2.0f) / resolution.x;
    localDirection.z = 0.5f / atan(camera.viewAngle / 2.0f);
    Ray ray(camera.position, camera.GetRotateMatrix() * localDirection);
    ray.current_object_insides.push(&scene->empty_object);
    return ray;
}

glm::dvec3 RayTracer::TraceRay(Ray ray, int step = 0)
{
	// Check whether recursive tracing is too deep to proceed calculations
    if (step >= maxRenderStep)
        return backgroundColor;

	// Find the nearest intersection of the ray and the scene
    Intersection intersection;
    Object3D* intersected_object = nullptr;
    for (auto& object : scene->objects)
    {
        bool invert_model = (ray.current_object_insides.top() == &object);

        Intersection currentIntersection = object.surface->Intersect(ray, invert_model);
        if (currentIntersection &&
            (!intersection || intersection.distance > currentIntersection.distance))
            // if there is no yet any intersections found or this intersection is closer than the previous one
        {
            // refresh the intersection data
            intersection = currentIntersection;
            intersected_object = &object;
        }
    }

    // Set proper direction of the normal vector at the intersection point
    if (glm::dot(ray.direction, intersection.normal) > 0.0) 
        intersection.normal = -intersection.normal;

    // If no intersections occured, return default color
    dvec3 color = backgroundColor;
    if (!intersection)
        return color;

    // Push the intersected object to the stack if the ray went inside it
    // or pop the intersected object from the stack if the ray went outside of it
    bool outside = (ray.current_object_insides.top() == intersected_object);
    Object3D* prevous_object = ray.current_object_insides.top();
    Object3D* current_object;

    std::stack<Object3D*> new_object_insides = ray.current_object_insides;
    if (intersected_object->surface)
    {
        if (outside)
        {
            new_object_insides.pop();
        }
        else
        {
            new_object_insides.push(intersected_object);
        }
    }

	// Calculate color by summing intensities from all light sources 
    if (intersection.material)
    {
        for (auto& light : scene->lights)
        {
            color += intersection.material->Color(
                intersection.normal, intersection.coord, 
                ray.direction, light
                );
        }
    }

    double relative_refractive_index = 1.0;
    double R = 0.0, T = 0.0;

    if (new_object_insides.top()->material)
    {
        relative_refractive_index =
            new_object_insides.top()->material->refractive_index /
            ray.current_object_insides.top()->material->refractive_index;
        R = FrenselReflectance(
            normalizeDot(-ray.direction, intersection.normal),
            relative_refractive_index
            );
        T = 1.0 - R;
    }

    dvec3 reflective_color = intersection.material->transparency_color * R +
        intersection.material->reflective_color;
    dvec3 transparency_color = intersection.material->transparency_color * T;

	// If reflectance effect on the resulting pixel is sufficient,
	// trace the reflected ray further
    if (glm::length(reflective_color) > TRACER_EPSILON)
    {
        Ray reflected; // reflected ray
        reflected.direction = glm::reflect(ray.direction, intersection.normal);
        reflected.origin = intersection.coord;
        reflected.current_object_insides = ray.current_object_insides;

        color += reflective_color * TraceRay(reflected, step + 1);
    }

    // If transparency effect on the resulting pixel is sufficient,
    // trace the refracted ray further
    if (glm::length(transparency_color) > TRACER_EPSILON)
    {
        Ray refracted; // refracted ray

        refracted.direction = glm::refract(ray.direction, intersection.normal, relative_refractive_index);
        refracted.origin = intersection.coord;
        std::swap(refracted.current_object_insides, new_object_insides);

        color += transparency_color * TraceRay(refracted, step + 1);
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
            pixels[i * resolution.x + j] = GammaCompression(TraceRay(ray));
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
