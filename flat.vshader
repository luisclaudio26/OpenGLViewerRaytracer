#version 150

struct _model {
	mat4 Matrix;
	float kD;
	float kA;
	float kS;
};

uniform _model model;
uniform mat4 vp;
uniform vec3 light1;

in vec3 pos;
in vec3 normal;
in vec3 tex;

out vec3 vcolor;

void main() 
{
	gl_Position = vp * model.Matrix * vec4(pos, 1.0);
	vec3 v2light = normalize(light1 - pos);
	float d = max( 0.0, dot(v2light, normal) );
	vcolor = model.kA*vec3(0.0,0.0,1.0) + model.kD*d*vec3(0.0,0.0,1.0);
}