#ifndef _SPHERE_H_
#define _SPHERE_H_

#include <glm/glm.hpp>
#include "material.h"

typedef struct {
	Material M;
	float radius;
	glm::vec3 pos;
} Sphere;

#endif