#ifndef _POINT_LIGHT_H_
#define _POINT_LIGHT_H_

typedef struct {
	float pos[3];
	float k; //Intensity
} PointLight;

#endif