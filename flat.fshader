#version 150

in vec3 vcolor;

out vec4 fragColour;

void main() 
{
	fragColour = vec4(vcolor, 1.0);
}