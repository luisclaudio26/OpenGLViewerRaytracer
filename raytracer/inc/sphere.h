#ifndef _SPHERE_H_
#define _SPHERE_H_

#include "material.h"

typedef struct {
	Material M;
	float radius;
	float pos[3];
} Sphere;

#endif