#ifndef _SPHERE_H_
#define _SPHERE_H_

#include <glm/glm.hpp>
#include "material.h"

typedef struct {
	Material M;
	float radius;
	glm::vec3 pos;
} Sphere;

void transform_sphere(Sphere& s, const glm::mat4& m)
{
	s.pos = glm::vec3( m * glm::vec4(s.pos, 1.0f) );
}

#endif