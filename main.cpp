#include <GL/glew.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <cstring>

#include "inc/object.h"
#include "inc/pointlight.h"

GLFWwindow* setup();

int main(int argc, char** args)
{
	GLFWwindow* window = setup();

	//------------------------------------------
	//-------- View-Projection settings --------
	//------------------------------------------
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.01f, 20.0f);
	glm::mat4 View = glm::lookAt( glm::vec3(5.0f, 1.0f, 0.0f), //Position 
								glm::vec3(0.0f, 0.0f, 0.0f), 	//Look direction
								glm::vec3(0.0f, 1.0f, 0.0f) );	//Up

	//Pre-multiply projection and view
	glm::mat4 vpMatrix = Projection * View;

	//---------------------------
	//-------- Lighting ---------
	//---------------------------
	PointLight p[2];
	p[0].pos = glm::vec3(0.0f, 0.0f, +3.0f);
	p[0].intensity = 2.0f;
	p[0].falloff = 1.0f;

	p[1].pos = glm::vec3(0.0f, 0.0f, -3.0f);
	p[1].intensity = 2.0f;
	p[1].falloff = 1.0f;

	glm::vec3 p0 = glm::vec3(0.0f, 0.0f, +3.0f);
	glm::vec3 p1 = glm::vec3(0.0f, +4.0f, 0.0f);

	float angle = 0.0f;

	//----------------------------------
	//-------- Geometry setting --------
	//----------------------------------
	Object obj;
	obj.load("./obj/suzanne.obj", "./shaders/flat");
	obj.setViewProjection(&vpMatrix);

	do
	{
		//Clear screen -> this function also clears stencil and depth buffer
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glm::mat4 rot1 = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::mat4 rot2 = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 0.0f, 1.0f));
		
		p[0].pos = glm::vec3( rot1 * glm::vec4(p0, 1.0f) );
		p[1].pos = glm::vec3( rot2 * glm::vec4(p1, 1.0f) );
		
		angle += 0.05f; if(angle >= 6.28) angle = 0.0f;

		obj.draw(p, 2);

		//Swap buffer and query events
		glfwSwapBuffers(window);
		glfwPollEvents();

	} while(glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && 
			!glfwWindowShouldClose(window));

	//Drop GLFW device
	glfwTerminate();

	return 0;
}

GLFWwindow* setup()
{
	//This will setup a new window and OpenGL context.
	//GLFW or GLUT may be used for this kind of stuff.
	glfwInit();

	glfwWindowHint(GLFW_SAMPLES, 1); //Supersampling, 4 samples/pixel
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3); //Make sure OpenGL
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2); //version is 3.0

	//Something about which kind of OpenGL will run here
	//This says to glfw that we're using only OpenGL core functions, not
	//vendor-specific
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	//Create window and set it as the context
	GLFWwindow *window = glfwCreateWindow(800, 600, "Project 1", NULL, NULL);
	glfwMakeContextCurrent(window);

	//Initialize GLEW. Don't what the fuck is this.
	glewExperimental = true; //Where the fuck is this coming from?
	glewInit();

	//Get keyboard input
	glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

	//Enable z-buffering
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	return window;
}