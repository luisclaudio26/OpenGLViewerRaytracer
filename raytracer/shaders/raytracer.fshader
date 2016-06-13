#version 150

//----------------------------------------------------------
//---------------------- DATA TYPES ------------------------
//----------------------------------------------------------
struct _material {
	float kA, kD, kS; 	//Ambient, diffuse, specular component
	float shininess; 	//Shininess for specular reflection
	vec3 color;
};

struct _sphere {
	_material M;
	float radius;
	vec3 pos;
};

struct _pointlight {
	vec3 pos;
	float k; //Intensity
};

//----------------------------------------------------------
//---------------------- INPUT DATA ------------------------
//----------------------------------------------------------
//Geometry info (we're rendering ONE sphere)
uniform _sphere S1;

//Light info (we're considering ONE light source)
uniform _pointlight L1;

//Camera info (for future work, apply transformation which
//sends all triangles to the camera space, perharps even
//apply projection transformation so to make ray casting
//easier)
uniform vec3 camPos;
uniform vec3 imgPlane; //This vector contains: {imageWidth, imageHeight, imageDistance}

//--------------------------------------------------
//---------------------- CODE ----------------------
//--------------------------------------------------
void main()
{
	gl_FragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
}