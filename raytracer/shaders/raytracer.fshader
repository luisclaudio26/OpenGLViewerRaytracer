#version 330

//----------------------------------------------------------
//---------------------- DATA TYPES ------------------------
//----------------------------------------------------------
struct _material {
	vec3 color;
	float kA; 
	float kD;

	/*
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
	float falloff;
	float k; //Intensity
};

//----------------------------------------------------------
//---------------------- INPUT DATA ------------------------
//----------------------------------------------------------
//Geometry info (we're rendering ONE sphere)
uniform _sphere S1;

//Light info (we're considering ONE light source)
uniform _pointlight L1;

//Camera info
uniform float filmW;
uniform float filmH;
uniform float filmD;

//Sample info
in vec4 gl_FragCoord;

//TODO: For some reason, passing the screen dimension
//as float (800.0f, 600.0f) is causing bizarre
//things. I won't deal with it now, so we'll just
//hardcode the dimension here and fix it after.

//--------------------------------------------------------
//--------------------- OUTPUT DATA ----------------------
//--------------------------------------------------------
out vec3 sample_color;

//--------------------------------------------------
//---------------------- CODE ----------------------
//--------------------------------------------------
void main()
{
	sample_color = vec3(0.2f, 0.2f, 0.2f);

	//Compute position of the sample in camera space
	//we want samples to range from -W/2 to W/2 and -H/2 to H/2
	vec3 p;
	p.x = ( (gl_FragCoord.x/800.0f) - 0.5f) * filmW;
	p.y = ( (gl_FragCoord.y/600.0f) - 0.5f) * filmH;
	p.z = -filmD;

	//Compute intersection with sphere
	float a = dot(p,p);
	float b = -2*dot(p, S1.pos);
	float c = dot(S1.pos, S1.pos) - S1.radius*S1.radius;
	float D = b*b - 4*a*c;

	if(D > 0.0f)
	{
		//Compute closest intersection
		float t1 = (-b - sqrt(D))/2*a;
		float t2 = (-b + sqrt(D))/2*a;
		float t = min(t1, t2);

		if(t > 0)
		{
			vec3 inter = t*p;
			vec3 normal = normalize(inter - S1.pos);

			//-------- Phong model --------
			//ambient light
			sample_color = S1.M.kA * S1.M.color;

			//diffuse light
			vec3 inter2light = L1.pos - inter;
			float diff = max( dot(normalize(inter2light), normal), 0.0f);
			float fo = L1.falloff / dot(inter2light, inter2light);

			sample_color = sample_color + (diff * fo * S1.M.kD) * S1.M.color;
		}
	}
}