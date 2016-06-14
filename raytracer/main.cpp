#include <GL/glew.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>

#include <iostream>
#include <cstring>
using std::cout;
using std::endl;

#include "./inc/sphere.h"
#include "./inc/pointlight.h"
#include "./inc/shaderloader.h"
#include "./inc/camera.h"

GLFWwindow* setup();

int main(int argc, char** args)
{
	GLFWwindow* window = setup();

	//------------------------------------------
	//-------- View-Projection settings --------
	//------------------------------------------
	Camera cam;
	cam.pos = glm::vec3(15.0f, 2.0f, 0.0f);
	cam.look_at = glm::vec3(0.0f, 0.0f, 0.0f) - cam.pos;
	cam.up = glm::vec3(0.0f, 1.0f, 0.0f);
	cam.d = 5.0f;
	cam.w = 1.0f;
	cam.h = 1.0f;

	glm::mat4 cam_align = world_to_camera(cam);

	//---------------------------
	//-------- Lighting ---------
	//---------------------------
	PointLight PL1; PL1.k = 1.0f;
					PL1.pos = glm::vec3(1.0f, 1.0f, 1.0f);

	//----------------------------------
	//-------- Geometry setting --------
	//----------------------------------
	Sphere S1; 	S1.radius = 1.0f;
				S1.pos = glm::vec3(0.0f, 0.0f, 0.0f);

	Material M1; 	M1.color[0] = 0.5f;
					M1.color[1] = 0.3f;
					M1.color[2] = 0.0f;
					M1.kA = 0.2f;
					M1.kD = 0.4f;
					M1.kS = 0.4f;
					M1.shininess = 10.0f;

	//------------------------------
	//------- Shader setting -------
	//------------------------------
	GLuint raytracer = ShaderLoader::load("./shaders/raytracer");

	//---------------------------
	//------- Ugly stuff --------
	//---------------------------
	//We'll create a simple rectangle which spans across
	//the entire window. The pixel shader will be responsible
	//for drawing the actual image onto it
	GLfloat screen[] = {-1.0f, -1.0f, 0.0f,
		 				1.0f, -1.0f, 0.0f,
		 				1.0f,  1.0f, 0.0f,
		 				-1.0f,  1.0f, 0.0f};
	
	GLuint screen_vao;
	glGenVertexArrays(1, &screen_vao);
	glBindVertexArray(screen_vao);
	
	//Copy stuff to geometry buffer
	GLuint screen_buffer;
	glGenBuffers(1, &screen_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, screen_buffer);
	glBufferData(GL_ARRAY_BUFFER, 
				sizeof(screen),
				screen,
				GL_STATIC_DRAW);

	//Set attributes
	GLuint id_pos = glGetAttribLocation(raytracer, "screen_coord");
	glEnableVertexAttribArray(id_pos);
	glVertexAttribPointer(id_pos,
							3,
							GL_FLOAT,
							GL_TRUE,
							0,
							(GLvoid*)0);

	//finish setting stuff
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//-----------------------------------
	//------- Preprocessing stage -------
	//-----------------------------------
	//Send objects to camera space (camera is origin,
	//with up = y, look_at = -z and x = up x look_at)
	PL1.pos = glm::vec3( cam_align*glm::vec4(PL1.pos, 1.0f) );
	S1.pos = glm::vec3( cam_align*glm::vec4(S1.pos, 1.0f) );

	std::cout<<glm::to_string(PL1.pos)<<endl;

	do
	{
		//Clear screen -> this function also clears stencil and depth buffer
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glUseProgram(raytracer);

		//Load uniform data
		GLuint sphere_color = glGetUniformLocation(raytracer, "S1.M.color");
		glUniform3f(sphere_color, M1.color[0], M1.color[1], M1.color[2]);

		GLuint sphere_kA = glGetUniformLocation(raytracer, "S1.M.kA");
		glUniform1f(sphere_kA, M1.kA);

		GLuint sphere_kD = glGetUniformLocation(raytracer, "S1.M.kD");
		glUniform1f(sphere_kD, M1.kD);

		GLuint sphere_radius = glGetUniformLocation(raytracer, "S1.radius");
		glUniform1f(sphere_radius, S1.radius);

		GLuint sphere_pos = glGetUniformLocation(raytracer, "S1.pos");
		glUniform3f(sphere_pos, S1.pos[0], S1.pos[1], S1.pos[2]);

		GLuint light_pos = glGetUniformLocation(raytracer, "L1.pos");
		glUniform3f(light_pos, PL1.pos[0], PL1.pos[1], PL1.pos[2]);

		GLuint light_k = glGetUniformLocation(raytracer, "L1.k");
		glUniform1f(light_k, PL1.k);

		GLuint film_w = glGetUniformLocation(raytracer, "filmW");
		glUniform1f(film_w, cam.w);

		GLuint film_h = glGetUniformLocation(raytracer, "filmH");
		glUniform1f(film_h, cam.h);

		GLuint film_d = glGetUniformLocation(raytracer, "filmD");
		glUniform1f(film_d, cam.d);

		//Draw
		glBindVertexArray(screen_vao);
		glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
		glBindVertexArray(0);
		
		glUseProgram(0);

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