#ifndef _CYLINDER_H_
#define _CYLINDER_H_

//For this to work correctly, we'll need a transformation
//from world space to object space

typedef struct {
	glm::vec3 pos;
	float a, b;
} Cylinder;

//this is not entirely correct; if T was not orthogonal,
//scale wouldn't be correct.
void transform_cylinder(Cylinder& C, const glm::mat4& T)
{
	C.pos = glm::vec3( T * glm::vec4(C.pos, 1.0f) );
}

#endif