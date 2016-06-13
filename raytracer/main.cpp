#include <GL/glew.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <iostream>
#include <cstring>

#include "./inc/sphere.h"
#include "./inc/pointlight.h"
#include "./inc/shaderloader.h"

GLFWwindow* setup();

int main(int argc, char** args)
{
	GLFWwindow* window = setup();

	//------------------------------------------
	//-------- View-Projection settings --------
	//------------------------------------------
	glm::vec3 cameraPos = glm::vec3(5.0f, 1.0f, 0.0f);

	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.01f, 20.0f);
	glm::mat4 View = glm::lookAt(cameraPos, //Position 
								glm::vec3(0.0f, 0.0f, 0.0f), 	//Look direction
								glm::vec3(0.0f, 1.0f, 0.0f) );	//Up

	//Pre-multiply projection and view
	glm::mat4 vpMatrix = Projection * View;

	//---------------------------
	//-------- Lighting ---------
	//---------------------------
	PointLight PL1; PL1.k = 1.0f;
					PL1.pos[0] = 0.0f;
					PL1.pos[1] = 5.0f;
					PL1.pos[2] = 0.0f;

	//----------------------------------
	//-------- Geometry setting --------
	//----------------------------------
	Sphere S1; 	S1.radius = 1.0f;
				S1.pos[0] = 0.0f;
				S1.pos[1] = 0.0f;
				S1.pos[2] = 0.0f;

	Material M1; 	M1.color[0] = 1.0f;
					M1.color[1] = 0.0f;
					M1.color[2] = 0.0f;
					M1.kA = 0.2f;
					M1.kD = 0.4f;
					M1.kS = 0.4f;
					M1.shininess = 10.0f;

	//------------------------------
	//------- Shader setting -------
	//------------------------------
	GLuint raytracer = ShaderLoader::load("./shaders/raytracer");

	do
	{
		//Clear screen -> this function also clears stencil and depth buffer
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

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
	GLFWwindow *window = glfwCreateWindow(800, 600, "Project 2", NULL, NULL);
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