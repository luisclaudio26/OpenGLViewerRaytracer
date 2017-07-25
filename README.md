# Simple .OBJ viewer and Whitted raytracer

<img src="https://github.com/luisclaudio26/CG-Project/raw/master/Suzanne.png" width="400"> <img src="https://github.com/luisclaudio26/CG-Project/raw/master/Tyra.png" width="400">

Blender's Suzanne and a T-Rex using the OBJ viewer

<img src="https://github.com/luisclaudio26/CG-Project/raw/master/raytracer.png" width="800">

Raytracer output. ~3 fps.

## Overview

These are two assignments for my Computer Graphics class. I took this course on 2015 and it was a general overview about topics related to computer graphics (rendering techniques, illumination models, transformations, 3D object representation etc.).

The first is a simple .OBJ viewer, where I load a .obj mesh and display it using OpenGL, using Phong model for illumination and point lights. This exposed to the fundamentals of OpenGL pipeline and to its shading language, GLSL.

The second is somewhat more complex, because OpenGL's pipeline is not exactly suited to raytracing operations. The solution was to fit two triangles in the entire screen, passing the entire scene geometry as uniform variables and then the raytracing operations happen inside the fragment shader. The problem with this is that the final code is completely inflexible, as every array passed to the fragment shader must have fixed dimensions, known in compile time. One solution to this would be just passing entire arrays with a fixed, large dimension and fill only as many elements as we need, but this would be a waste of memory. This code was more of a defy and polished my skills in OpenGL, but I learnt that this is **not** a good approach to raytracing.
