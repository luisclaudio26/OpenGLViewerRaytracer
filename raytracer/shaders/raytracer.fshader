#version 330

//----------------------------------------------------------
//---------------------- DATA TYPES ------------------------
//----------------------------------------------------------
struct _material {
	vec3 color;

	float kA; 
	
	/*
	float kD;
	float kS;
	float shininess;*/
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
//uniform _pointlight L1;

//Camera info (for future work, apply transformation which
//sends all triangles to the camera space, perharps even
//apply projection transformation so to make ray casting
//easier)
//uniform vec3 camPos;
//uniform vec3 imgPlane; //This vector contains: {imageWidth, imageHeight, imageDistance}

//--------------------------------------------------------
//--------------------- OUTPUT DATA ----------------------
//--------------------------------------------------------
out vec3 sample_color;

//--------------------------------------------------
//---------------------- CODE ----------------------
//--------------------------------------------------
void main()
{
	sample_color = vec3(0.0, 0.0, 0.4);
}