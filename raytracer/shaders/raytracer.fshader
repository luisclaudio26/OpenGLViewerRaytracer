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
	float shininess;
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
//as float is causing bizarre things. I won't deal 
//with it now, so we'll just hardcode the dimension 
//here and fix it after.

//--------------------------------------------------------
//--------------------- OUTPUT DATA ----------------------
//--------------------------------------------------------
out vec3 sample_color;

//--------------------------------------------------
//---------------------- CODE ----------------------
//--------------------------------------------------
void intersect_with_sphere(in vec3 o, in vec3 p, in _sphere S, out float t);
void intersect_with_plane(in vec3 o, in vec3 d, in _plane P, out float t);

void cast_ray(in vec3 o, in vec3 d, out int id, out int type, out float t);
void point_color(in vec3 inter, in vec3 normal, in vec3 eye2inter, in _material obj_mat, out vec3 inter_color);

void compute_normal(in vec3 intersection, in int obj_id, in int obj_type, out vec3 normal);
void get_material(in int obj_id, in int obj_type, out _material obj_mat);

void main()
{
	sample_color = vec3(0.0f, 0.0f, 0.0f);

	//Compute position of the sample in camera space
	//we want samples to range from -W/2 to W/2 and -H/2 to H/2
	vec3 p;
	p.x = ( (gl_FragCoord.x/800.0f) - 0.5f) * filmW;
	p.y = ( (gl_FragCoord.y/600.0f) - 0.5f) * filmH;
	p.z = -filmD;

	//this is computed from first to last intersection
	int max_depth = 3;
	vec3 ray_origin = vec3(0,0,0);
	vec3 ray_dir = p;
	float specularity = 1.0f;

	while(max_depth > 0)
	{
		//Compute closest intersection
		int obj_id; int obj_type; float t;
		cast_ray(ray_origin, ray_dir, obj_id, obj_type, t);

		//if we intersected nothing, stop recursing;
		//otherwise, compute pixel color and set next
		//recursion level
		if(obj_id == -1)
			max_depth = 0;
		else
		{
			//needed data
			_material obj_mat; get_material(obj_id, obj_type, obj_mat);
			vec3 inter = ray_origin + t*ray_dir;
			vec3 normal; compute_normal(inter, obj_id, obj_type, normal);
			vec3 eye2inter = inter - ray_origin;

			//compute final color
			vec3 color;
			point_color(inter, normal, eye2inter, obj_mat, color);

			//add computed color to accumulator
			sample_color += specularity * color;

			//set parameters for the next level of recursion
			ray_dir = reflect(ray_dir, normal);
			ray_origin = inter + normal * 0.001f; //avoid spurious intersections
			specularity = obj_mat.kS;

			//decrease depth counter
			max_depth -= 1;
		}
	}
}

void get_material(in int obj_id, in int obj_type, out _material obj_mat)
{
	if(obj_type == SPHERE)
		obj_mat = S[obj_id].M;
	else if(obj_type == PLANE)
		obj_mat = P[obj_id].M;
}

void compute_normal(in vec3 intersection, in int obj_id, in int obj_type, out vec3 normal)
{
	normal = vec3(0,0,0);

	if(obj_type == SPHERE)
		normal = normalize(intersection - S[obj_id].pos);
	else if(obj_type == PLANE)
		normal = normalize(P[obj_id].normal);
}

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

void point_color(in vec3 inter, in vec3 normal, in vec3 eye2inter, in _material obj_mat, out vec3 inter_color)
{
	//Ambient light
	inter_color = obj_mat.kA * obj_mat.color;

	for(int i = 0; i < N_LIGHTS; i++)
	{
		vec3 inter2light = L[i].pos - inter;
	
		//cast shadow ray	
		int occluder_id; int dummy; float t;
		cast_ray(inter + 0.0001f * inter2light, inter2light, occluder_id, dummy, t);

		//diffuse and specular only if point of intersection is
		//not occluded by another object
		if(occluder_id == -1)
		{
			//light attenuation, proportional to 1/(a.d²)
			float falloff = 1.0f / (L[i].falloff * dot(inter2light, inter2light));

			//diffuse light
			float diff = dot(normalize(inter2light), normal);
			diff = max(diff, 0.0f);

			inter_color += (L[i].k * diff * obj_mat.kD) * obj_mat.color;

			//specular light
			vec3 ref_ray = reflect(-normalize(inter2light), normal);
			float spec = dot(ref_ray, -eye2inter);
			spec = max(spec, 0.0f);

			inter_color += (L[i].k * spec * falloff * obj_mat.kS) * obj_mat.color;
		}
	}

}