#ifndef _PLANE_H_
#define _PLANE_H_

#include <glm/glm.hpp>

typedef struct {
	glm::vec3 normal;
	float d;
} Plane;

#endif