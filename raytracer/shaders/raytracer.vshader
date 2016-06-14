#version 330

in vec3 screen_coord;

void main()
{
	gl_Position.xyz = screen_coord;
	gl_Position.w = 1.0;
}