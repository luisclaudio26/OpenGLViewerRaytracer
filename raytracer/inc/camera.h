#ifndef _CAMERA_H_
#define _CAMERA_H_

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

typedef struct {
	glm::vec3 pos;
	glm::vec3 look_at;
	glm::vec3 up;
	float d;	//distance from camera to film plane
	float w, h; //Width and height of film
} Camera;

glm::mat3 get_camera_coord_system(const Camera& cam)
{
	glm::vec3 W = glm::normalize( -cam.look_at);
	glm::vec3 V = glm::normalize( cam.up - glm::dot(cam.up, W)*W );
	glm::vec3 U = glm::normalize( cross(V, W) );

	return glm::mat3(U, V, W);
}

glm::mat4 world_to_camera(const Camera& cam)
{
	glm::mat3 uvw = get_camera_coord_system(cam);

	glm::vec4 Uh = glm::vec4(uvw[0], 0.0f);
	glm::vec4 Vh = glm::vec4(uvw[1], 0.0f);
	glm::vec4 Wh = glm::vec4(uvw[2], 0.0f);
	glm::vec4 Th = glm::vec4(cam.pos, 1.0f);

	return glm::inverse( glm::mat4(Uh, Vh, Wh, Th) );
}

#endif