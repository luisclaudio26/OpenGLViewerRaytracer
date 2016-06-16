# CG-Project 
Raytracer Computer Graphics class in UFRN, 2016.

[https://github.com/luisclaudio26/CG-Project/]

1. Compiling

Once you have unzipped the code, simply MAKE it.

2. Running

To run, simply type ./render. The scene was hardcoded, so there is no parameter.

3. Remarks

Unfortunatelly I could not implement all primitives. I spent a lot of time trying to code the cubes with no success, and in the end I did spheres and cylinders only (and planes, which were not asked in the project but looked nice and easy).

The lighting model was not completely respected because all light components (specular, ambient, diffuse and reflection) have all the same behaviour for different wavelengths (that is, they all reflect one color only). This was done for simplicity, as these extra parameters would be too tedious to configure and would make code even larger with no great visual improvement.

Furthermore, supersampling and light attenuation were implemented so to compensate the lack of cones and cubes.

4. About [skip this if you don't care about how I did everything]

As OpenGL is NOT suited to do raytracing but rather to draw polygons, so I had to do some hacks. Code is really hard to understand because GLSL is a very limited language, so there are lots of structs, duplicated code, etc.

The raytracer itself is implemented in the fragment shader; to run this fragment shader, however, I need some primitive in the screen to render, and that's why I set up two triangles which spans across the entire window. We simply render these two triangles and the fragment shader "paint them" with the raytraced scene.

Again, OpenGL is not suited to do this kind of operation, so our scene need to be statically defined - no object can be created out of nowhere, but we can play with the ones we defined. Every object inside the scene is passed as UNIFORM to the shader: this is data which will be constant during the whole rendering pipeline. This means we have to predefine how many objects of each kind we'll have.

Materials and positions we're set empirically.

The INF1 and INF2 thing is horrible, but we need to know whether we had an intersection or not and this was a "nice" way I found to do it (we initially set T to INF1; if the intersection code returned INF2, which is greater than INF1, we consider no intersection took place).

Further stuff is commented inside the code.


