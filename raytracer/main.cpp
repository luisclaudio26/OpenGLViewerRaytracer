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
	cam.pos = glm::vec3(8.0f, 2.0f, 0.0f);
	cam.look_at = glm::vec3(0.0f, 0.0f, 0.0f) - cam.pos;
	cam.up = glm::vec3(0.0f, 1.0f, 0.0f);
	cam.d = 1.0f;
	cam.w = 1.0f;
	cam.h = 1.0f;

	glm::mat4 cam_align = world_to_camera(cam);

	//---------------------------
	//-------- Lighting ---------
	//---------------------------
	PointLight PL1; PL1.k = 1.0f;
					PL1.falloff = 5.0f;
					PL1.pos = glm::vec3(5.0f, 3.0f, 0.0f);

	//----------------------------------
	//-------- Geometry setting --------
	//----------------------------------
	Sphere S[2]; 	

	S[0].radius = 3.0f;
	S[0].pos = glm::vec3(0.0f, 0.0f, 0.0f);

	S[1].radius = 3.0f;
	S[1].pos = glm::vec3(5.0f, 5.0f, 0.0f);

	Material M1; 	M1.color[0] = 0.5f;
					M1.color[1] = 0.3f;
					M1.color[2] = 0.0f;
					M1.kA = 0.2f;
					M1.kD = 0.8f;
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
	S[0].pos = glm::vec3( cam_align*glm::vec4(S[0].pos, 1.0f) );
	S[1].pos = glm::vec3( cam_align*glm::vec4(S[1].pos, 1.0f) );

	std::cout<<glm::to_string(S[0].pos)<<", "<<glm::to_string(S[1].pos)<<endl;

	do
	{
		//Clear screen -> this function also clears stencil and depth buffer
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glUseProgram(raytracer);

		//Load uniform data
		GLuint sphere_base = glGetUniformLocation(raytracer, "S[0].radius");
		for(int i = 0; i < 2; i++)
		{
			GLuint sphere_id = sphere_base + i*5; //5 parameters per struct Sphere

			glUniform1f(sphere_id, S[i].radius);
			glUniform3f(sphere_id+1, S[i].pos[0], S[i].pos[1], S[i].pos[2]);
			glUniform1f(sphere_id+2, M1.kA);
			glUniform1f(sphere_id+3, M1.kD);
			glUniform3f(sphere_id+4, M1.color[0], M1.color[1], M1.color[2]);
		}

		GLuint light_pos = glGetUniformLocation(raytracer, "L1.pos");
		glUniform3f(light_pos, PL1.pos[0], PL1.pos[1], PL1.pos[2]);

		GLuint light_k = glGetUniformLocation(raytracer, "L1.k");
		glUniform1f(light_k, PL1.k);

		GLuint light_falloff = glGetUniformLocation(raytracer, "L1.falloff");
		glUniform1f(light_falloff, PL1.falloff);

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