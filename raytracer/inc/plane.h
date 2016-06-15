#ifndef _PLANE_H_
#define _PLANE_H_

#include <glm/glm.hpp>

typedef struct {
	glm::vec3 normal;
	float d;
} Plane;

void transform_plane(Plane& p, const glm::mat4& m)
{
	//I honestly can't understand exactly this method,
	//but I had no time to read about it.
	glm::vec4 _p = glm::vec4(p.normal, p.d);

	_p = glm::inverse( glm::transpose(m) ) * _p;
	
	p.normal = glm::vec3( _p[0], _p[1], _p[2] );
	p.d = _p[3];
}

#endif