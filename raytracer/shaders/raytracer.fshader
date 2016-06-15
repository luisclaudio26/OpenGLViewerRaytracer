#version 330

//----------------------------------------------------------
//---------------------- DATA TYPES ------------------------
//----------------------------------------------------------
struct _material {
	float kA; 
	float kD;
	float kS;
	vec3 color;
};

struct _sphere {
	float radius;
	vec3 pos;
	_material M;
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
uniform _sphere S[2];

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
void intersect_with_sphere(in vec3 o, in vec3 p, in _sphere S, out float t)
{
	//REMEMBER! One must really define the OUT variables here,
	//it won't retain the value it has outside this function
	t = 200000.0f;

	//Compute intersection with sphere
	float a = dot(p,p);
	float b = 2*dot(p, o-S.pos);
	float c = dot(S.pos-o, S.pos-o) - S.radius*S.radius;
	float D = b*b - 4*a*c;

	if(D > 0.0f)
	{
		//Compute closest intersection
		float t_ = -b - sqrt(D);

		//first root is negative, try the second one
		if(t_ < 0) t_ = -b + sqrt(D);

		//if we found a positive root, output it
		//otherwise, we'll output infinity
		if(t_ >= 0) t = t_/(2*a);
	}
}

void point_color(in vec3 intersection, in vec3 normal, in _sphere S, out vec3 color)
{
	//-------- Phong model --------
	//ambient light
	color = S.M.kA * S.M.color;

	//diffuse light
	vec3 inter2light = L1.pos - intersection;
	float diff = dot(normalize(inter2light), normal);

	diff = max(diff, 0.0f);

	color = color + (L1.k * diff * S.M.kD) * S.M.color;
}

void cast_ray(in vec3 o, in vec3 d, out int id, out float t)
{
	id = -1; t = 100000.0f; //Infinity
	for(int i = 0; i < 2; i++)
	{
		float t_aux;
		intersect_with_sphere(o, d, S[i], t_aux);

		if(t_aux < t) 
		{
			t = t_aux;
			id = i;
		}
	}
}

void main()
{
	sample_color = vec3(0.0f, 0.0f, 0.0f);

	//Compute position of the sample in camera space
	//we want samples to range from -W/2 to W/2 and -H/2 to H/2
	vec3 p;
	p.x = ( (gl_FragCoord.x/800.0f) - 0.5f) * filmW;
	p.y = ( (gl_FragCoord.y/600.0f) - 0.5f) * filmH;
	p.z = -filmD;

	//Compute first intersection
	int sphere_id; float t;
	cast_ray(vec3(0,0,0), p, sphere_id, t);

	//If we intersected something
	if(sphere_id != -1)
	{
		vec3 inter = t*p;
		vec3 normal = normalize(inter - S[sphere_id].pos);

		vec3 inter_color;
		point_color(inter, normal, S[sphere_id], inter_color);

		//cast ray on reflection direction
		int bounce_id; float bounce_t;
		vec3 ref_d = reflect(p, normal);
		cast_ray(inter+0.001f*ref_d, ref_d, bounce_id, bounce_t); //we displace it 0.001f units on
																	//the normal direction to avoid
																	//spurious autointersections

		vec3 bounce_color = vec3(0,0,0);
		if(bounce_id != -1)
		{
			vec3 bounce_inter = bounce_t*ref_d + inter;
			vec3 bounce_normal = normalize(bounce_inter - S[bounce_id].pos);

			point_color(bounce_inter, bounce_normal, S[bounce_id], bounce_color);
		}

		sample_color =  inter_color + S[sphere_id].M.kS*bounce_color;
	}
}