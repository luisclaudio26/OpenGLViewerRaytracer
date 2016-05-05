#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdio>

#include <GL/glew.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "./pointlight.h"

typedef struct {
	float u, v;
} Vec2;

typedef struct {
	float x, y, z;
} Vec3;

typedef struct {
	float x, y, z, w;
} Vec4;

typedef struct {
	Vec3 pos, n;
	Vec2 tex;
} Vertex;

class Object
{
private:
	//-------------------------------
	//------- Transformations -------
	//-------------------------------
	glm::mat4 *vp;
	glm::mat4 model;

	//--------------------------------
	//------- Geometry storage -------
	//--------------------------------
	std::vector<Vec3> vertice;
	std::vector<Vec2> texture;
	std::vector<Vec3> normal;
	std::vector<Vertex> out_vertices;

	//--------------------------------
	//------- Material setting -------
	//--------------------------------
	float kA, kD, kS;
	glm::vec3 colorA, colorD, colorS;

	//------------------------------------
	//------- OpenGL related stuff -------
	//------------------------------------
	GLuint shader_id, vertex_array_id;
	
	GLint h_kA, h_kD, h_kS; //Handles to material settings
	GLint h_cA, h_cD, h_cS;

	GLint h_vp; //Handle to view-projection
	GLint h_model; //Handle to model transformation

	//-----------------------------------
	//------- Internal operations -------
	//-----------------------------------
	void process_line(const std::string& line);
	void load_into_opengl(const std::string& shader);
	void load_glsl_parameters();

public:
	Object();

	//This function expects to find two shader files with
	//extensions .vshader and .fshader, with the first name
	//being the one we passed as argument
	void load(const std::string& model, const std::string& shader);
	
	//Get an array with the point lights
	void draw(PointLight pl[], unsigned int n);

	//define View-Projection matrix of our scene
	void setViewProjection(glm::mat4* vp);
};

#endif