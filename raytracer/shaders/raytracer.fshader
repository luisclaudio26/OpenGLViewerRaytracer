#version 330

#define EPS 0.000001f
#define SPHERE 1
#define PLANE 2

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

struct _plane {
	float d;
	vec3 normal;
	_material M;
};

struct _pointlight {
	float k; //Intensity
	float falloff;
	vec3 pos;
};

//----------------------------------------------------------
//---------------------- INPUT DATA ------------------------
//----------------------------------------------------------
//Geometry info
uniform _sphere S[2];
#define N_SPHERES

uniform _plane P[1];
#define N_PLANES

//Light info (we're considering ONE light source)
uniform _pointlight L[2];
#define N_LIGHTS 2

//Camera info
uniform float filmW;
uniform float filmH;
uniform float filmD;

//Sample info
in vec4 gl_FragCoord;

//TODO: For some reason, passing the screen dimension
//as float (1024.0f, 600.0f) is causing bizarre
//things. I 768't deal with it now, so we'll just
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

void intersect_with_plane(in vec3 o, in vec3 d, in _plane P, out float t)
{
	t = 200000.0f;

	//compute intersection with plane
	float a = dot(P.normal, d);
	float b = dot(P.normal, o) + P.d;

	//intersection only if a != 0, i.e., the ray is not parallel
	//to the plane (perpendicular to the normal)
	if( abs(a) > EPS )
	{
		float _t = -b/a;
		if(_t > 0) t = _t;
	}
}

void cast_ray(in vec3 o, in vec3 d, out int id, out int type, out float t)
{
	id = -1; t = 100000.0f; //Infinity
	float t_aux;
	
	//test intersection with the spheres
	for(int i = 0; i < 2; i++)
	{
		intersect_with_sphere(o, d, S[i], t_aux);

		if(t_aux < t) 
		{
			t = t_aux;
			id = i;
			type = SPHERE;
		}
	}

	//test intersection with the planes
	for(int i = 0; i < 1; i++)
	{
		intersect_with_plane(o, d, P[i], t_aux);

		if(t_aux < t)
		{
			t = t_aux;
			id = i;
			type = PLANE;
		}
	}
}

void point_color_sphere(in vec3 intersection, in vec3 normal, in _sphere S, out vec3 color)
{
	//-------- Phong model --------
	//ambient light
	color = S.M.kA * S.M.color;
	
	for(int i = 0; i < N_LIGHTS; i++)
	{
		//cast shadow ray
		vec3 inter2light = L[i].pos - intersection;

		int occluder_id; float t; int dummy;
		cast_ray(intersection+0.0001f*inter2light, inter2light, occluder_id, dummy, t);

		//if no object is occluding the point
		if(occluder_id == -1)
		{
			//diffuse light
			float diff = dot(normalize(inter2light), normal);
			diff = max(diff, 0.0f);

			color += (L[i].k * diff * S.M.kD) * S.M.color;
		}
	}
}

void point_color_plane(in vec3 inter, in _plane P, out vec3 inter_color)
{
	//------ Phong model -------
	//Ambient light
	inter_color = P.M.kA * P.M.color;

	for(int i = 0; i < N_LIGHTS; i++)
	{
		//cast shadow ray
		vec3 inter2light = L[i].pos - inter;
		
		int occluder_id; int dummy; float t;
		cast_ray(inter+0.0001f*inter2light, inter2light, occluder_id, dummy, t);

		if(occluder_id == -1)
		{
			//diffuse light
			float diff = dot(normalize(inter2light), P.normal);
			diff = max(diff, 0.0f);

			if(diff > 0.0f)
			{
				float falloff = L[i].falloff / dot(inter2light, inter2light);
				inter_color += (L[i].k * diff *falloff * P.M.kD) * P.M.color;
			}
		}
	}
}

void compute_normal(in vec3 intersection, in int obj_id, in int obj_type, out vec3 normal)
{
	normal = vec3(0,0,0);

	if(obj_type == SPHERE)
		normal = normalize(intersection - S[obj_id].pos);
	else if(obj_type == PLANE)
		normal = normalize(P[obj_id].normal);
}

void point_color(in vec3 inter, in vec3 normal, in int obj_id, in int obj_type, out vec3 inter_color)
{
	if(obj_type == SPHERE)
		point_color_sphere(inter, normal, S[obj_id], inter_color);
	else if(obj_type == PLANE)
		point_color_plane(inter, P[obj_id], inter_color);
}

void main()
{
	sample_color = vec3(0.0f, 0.0f, 0.0f);

	//Compute position of the sample in camera space
	//we want samples to range from -W/2 to W/2 and -H/2 to H/2
	vec3 p;
	p.x = ( (gl_FragCoord.x/1024.0f) - 0.5f) * filmW;
	p.y = ( (gl_FragCoord.y/768.0f) - 0.5f) * filmH;
	p.z = -filmD;

	//Compute first intersection
	int obj_id; int obj_type; float t;
	cast_ray(vec3(0,0,0), p, obj_id, obj_type, t);

	//If we intersected something
	if(obj_id != -1)
	{
		vec3 inter = t*p;
		vec3 normal; compute_normal(inter, obj_id, obj_type, normal);

		vec3 inter_color;
		point_color(inter, normal, obj_id, obj_type, inter_color);

		//cast ray on reflection direction
		int bounce_id; int bounce_type; float bounce_t;
		vec3 ref_d = reflect(p, normal);

		//We displace it 0.001f units on the normal direction to avoid
		//spurious autointersections
		cast_ray(inter+0.001f*ref_d, ref_d, bounce_id, bounce_type, bounce_t);

		vec3 bounce_color = vec3(0,0,0);
		if(bounce_id != -1)
		{
			vec3 bounce_inter = bounce_t*ref_d + inter;
			vec3 bounce_normal; compute_normal(bounce_inter, bounce_id, bounce_type, bounce_normal);

			point_color(bounce_inter, bounce_normal, bounce_id, bounce_type, bounce_color);
		}

		sample_color = inter_color;

		if(obj_type == SPHERE)
			sample_color += S[obj_id].M.kS * bounce_color;
		else if(obj_type == PLANE)
			sample_color += P[obj_id].M.kS * bounce_color;
	}
}