#ifndef _MATERIAL_H_
#define _MATERIAL_H_

typedef struct {
	float kA, kD, kS, kR; 	//Ambient, diffuse, specular, reflectance
	float shininess; 	//Shininess for specular reflex
	float color[3];
} Material;

#endif