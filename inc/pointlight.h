#ifndef _POINT_LIGHT_H_
#define _POINT_LIGHT_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class PointLight
{
public:
	glm::vec3 pos, color;
	float intensity;
};

#endif