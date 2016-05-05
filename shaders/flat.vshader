#version 150

struct _model {
	mat4 transform;

	//Intensity and color for diffuse, 
	//ambient and specular components
	float kD; vec3 cD;
	float kA; vec3 cA;
	float kS; vec3 cS;
};
uniform _model model;


const int N_LIGHTS = 2;
struct _light {
	vec3 pos;
	float intensity;
	float falloff;
};
uniform _light light[N_LIGHTS];


//View-projection matrix
uniform mat4 vp;

in vec3 pos;
in vec3 normal;
in vec3 tex;

flat out vec3 vcolor;

void main() 
{
	//Apply Model-View-Projection matrix to vertice
	gl_Position = vp * model.transform * vec4(pos, 1.0);

	//------- Compute diffuse component --------
	float diffuse_acc = 0.0f;
	for(int i = 0; i < N_LIGHTS; i++)
	{
		//diffuse reflection for this light
		vec3 pos2light = light[i].pos - pos;
		float diffuse = max(0.0, dot(normalize(pos2light), normal) );
		float distance2 = dot(pos2light,pos2light);

		diffuse_acc += (diffuse * light[i].intensity * model.kD) / (distance2 * light[i].falloff);
	}

	vcolor = model.kA * model.cA;
	vcolor += diffuse_acc * model.cD;
}