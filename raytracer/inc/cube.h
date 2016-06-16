#ifndef _CUBE_H_
#define _CUBE_H_

#include "./plane.h"

//For this to work correctly, we'll need a transformation
//from world space to object space

typedef struct {
	glm::vec3 pos;
	float l;
} Cube;

void transform_cube(Cube& C, const glm::mat4& T)
{
	C.pos = glm::vec3( T * glm::vec4(C.pos, 1.0f) );
}

#endif