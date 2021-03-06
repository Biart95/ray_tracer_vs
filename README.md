Ray Tracer

Author: Artyom Bishev

This is a simple ray tracer.

Available features in this release:
 - Loading meshes 3ds files, loading scenes from internal text format
 - Diffuse/Phong shading, reflection and refraction by Frensel formulas
 - OpenMP simple parallelization
 - Instancing
 - Octree for meshes

Just ready for release (can be seen on the images shown in img/):

- Simple shadows
- Simple global illumination using photon maps
- Extended material features, some texturing

In the nearest future:
- Mixing with backtracing techniques + extended photon maps
- Struggling with some very persistent artifacts
- Foggy object insides
- Postprocessing effects including tone mapping
- BSP-tree for objects
- OpenGL rendering with movable camera for taking a nice shot

Libraries used:
- atlimage (planning to move to lodepng)
- glm (of course it is used, http://glm.g-truc.net)
- L3DS (http://is.muni.cz/th/98745/fi_m/Calligraphy/l3ds/docs/index.html)

Models were taken from: http://pascal.leynaud.free.fr/3ds/

Project is now supported as Visual Studio solution
