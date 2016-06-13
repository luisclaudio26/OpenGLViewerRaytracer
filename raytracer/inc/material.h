#ifndef _MATERIAL_H_
#define _MATERIAL_H_

typedef struct {
	float kA, kD, kS; 	//Ambient, diffuse, specular component
	float shininess; 	//Shininess for specular reflex
	float color[3];
} Material;

#endif