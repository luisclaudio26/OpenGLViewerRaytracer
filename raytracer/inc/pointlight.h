#ifndef _POINT_LIGHT_H_
#define _POINT_LIGHT_H_

#include <glm/glm.hpp>

typedef struct {
	glm::vec3 pos;
	float k; //Intensity
	float falloff;
} PointLight;

#endif