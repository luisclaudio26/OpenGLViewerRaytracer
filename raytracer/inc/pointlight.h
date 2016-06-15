#ifndef _POINT_LIGHT_H_
#define _POINT_LIGHT_H_

#include <glm/glm.hpp>

typedef struct {
	glm::vec3 pos;
	float k; //Intensity
	float falloff;
} PointLight;

void transform_pointlight(PointLight& p, const glm::mat4& m)
{
	p.pos = glm::vec3( m * glm::vec4(p.pos, 1.0f) );
}

#endif