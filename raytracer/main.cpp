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
#include "./inc/plane.h"
#include "./inc/cylinder.h"

GLFWwindow* setup();

int main(int argc, char** args)
{
	GLFWwindow* window = setup();

	//------------------------------------------
	//-------- View-Projection settings --------
	//------------------------------------------
	Camera cam;
	cam.pos = glm::vec3(40.0f, 7.0f, 0.0f);
	cam.look_at = glm::vec3(0.0, 0.0, 6.0);
	cam.up = glm::vec3(0.0f, 1.0f, 0.0f);
	cam.d = 1.0f;
	cam.w = 1.3f;
	cam.h = 1.0f;

	glm::mat4 cam_align = world_to_camera(cam);

	//---------------------------
	//-------- Lighting ---------
	//---------------------------
	PointLight PL[2]; const int N_LIGHTS = 2;
	PL[0].k = 0.5f;
	PL[0].falloff = 0.005f;
	PL[0].pos = glm::vec3(-4.0f, 10.0f, 10.0f);
 
	PL[1].k = 0.5f;
	PL[1].falloff = 0.01f;
	PL[1].pos = glm::vec3(8.0f, 10.0f, -10.0f);

	//----------------------------------
	//-------- Geometry setting --------
	//----------------------------------
	Sphere S[2]; const int N_SPHERES = 2;

	S[0].radius = 3.0f;
	S[0].pos = glm::vec3(0.0f, -2.0f, -12.0f);

	S[1].radius = 5.0f;
	S[1].pos = glm::vec3(0.0f, 0.0f, 0.0f);

	Plane P[2]; const int N_PLANES = 2;
	P[0].normal = glm::vec3(0.0f, 1.0f, 0.0f);
	P[0].d = 5.0f;

	P[1].normal = glm::vec3(0.0f, 0.0f, 1.0f);
	P[1].d = 17.0f;

	Cylinder C[1]; const int N_CYLINDER = 1;
	C[0].pos = glm::vec3(-9.0f, 0.0f, 15.0f);
	C[0].a = C[0].b = 3.0f;

	Material M[5]; 	

	//spheres
	M[0].color[0] = 0.7f;
	M[0].color[1] = 0.3f;
	M[0].color[2] = 0.3f;
	M[0].kA = 0.1f;
	M[0].kD = 0.8f;
	M[0].kS = 0.02f;
	M[0].kR = 1.0f;
	M[0].shininess = 1.5f;

	M[1].color[0] = 0.0f;
	M[1].color[1] = 1.0f;
	M[1].color[2] = 0.0f;
	M[1].kA = 0.1f;
	M[1].kD = 0.0f;
	M[1].kS = 0.01f;
	M[1].kR = 1.0f;
	M[1].shininess = 1.0f;

	//planes
	M[2].color[0] = 1.0f;
	M[2].color[1] = 1.0f;
	M[2].color[2] = 1.0f;
	M[2].kA = 0.3f;
	M[2].kD = 0.6f;
	M[2].kS = 0.0f;
	M[2].kR = 0.0f;
	M[2].shininess = 1.0f;

	M[3].color[0] = 0.4f;
	M[3].color[1] = 0.3f;
	M[3].color[2] = 0.7f;
	M[3].kA = 0.3f;
	M[3].kD = 0.6f;
	M[3].kS = 0.0f;
	M[3].kR = 0.0f;
	M[3].shininess = 1.0f;

	//cylinder
	M[4].color[0] = 0.4f;
	M[4].color[1] = 0.8f;
	M[4].color[2] = 0.7f;
	M[4].kA = 0.4f;
	M[4].kD = 0.6f;
	M[4].kS = 0.0f;
	M[4].kR = 0.0f;
	M[4].shininess = 1.0f;

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
	for(int i = 0; i < N_SPHERES; i++) transform_sphere(S[i], cam_align);
	for(int i = 0; i < N_LIGHTS; i++) transform_pointlight(PL[i], cam_align);
	for(int i = 0; i < N_CYLINDER; i++) transform_cylinder(C[i], cam_align);
	for(int i = 0; i < N_PLANES; i++) transform_plane(P[i], cam_align);

	//----- Animation stuff ------
	//this is not correct, as we're rotating in the camera space.
	//we should rotate it in world space and then send to camera space,
	//but this would kill performance for no great reason
	glm::vec3 _pl0 = glm::vec3(S[1].pos.x, S[1].pos.y, S[1].pos.z);
	glm::mat4 t1 = glm::translate(glm::mat4(1.0f), S[1].pos);
	glm::mat4 t2 = glm::translate(glm::mat4(1.0f), -S[1].pos);
	float angle = 0.0f;

	do
	{
		//Clear screen -> this function also clears stencil and depth buffer
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		glUseProgram(raytracer);

		//------ Animate ball ------
		glm::mat4 rot = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));
		S[1].pos = glm::vec3( rot * glm::vec4(_pl0, 1.0f));
		angle += 0.007f; if(angle >= 6.28f) angle = 0.0f;

		//------ Load uniform data ------
		//There's no better way to do this. If we transform each one in
		//a function, we'll introduce the overhead of calling one function
		//(or the compiler itself would just inline all of them).
		GLuint sphere_base = glGetUniformLocation(raytracer, "S[0].radius");
		for(int i = 0; i < N_SPHERES; i++)
		{
			GLuint sphere_id = sphere_base + i*8; //8 parameters per struct Sphere

			glUniform1f(sphere_id, S[i].radius);
			glUniform3f(sphere_id+1, S[i].pos[0], S[i].pos[1], S[i].pos[2]);
			glUniform1f(sphere_id+2, M[i].kA);
			glUniform1f(sphere_id+3, M[i].kD);
			glUniform1f(sphere_id+4, M[i].kS);
			glUniform1f(sphere_id+5, M[i].kR);
			glUniform1f(sphere_id+6, M[i].shininess);
			glUniform3f(sphere_id+7, M[i].color[0], M[i].color[1], M[i].color[2]);
		}

		GLuint plane_base = glGetUniformLocation(raytracer, "P[0].d");
		for(int i = 0; i < N_PLANES; i++)
		{
			GLuint plane_id = plane_base + i*8;

			glUniform1f(plane_id, P[i].d);
			glUniform3f(plane_id+1, P[i].normal[0], P[i].normal[1], P[i].normal[2]);
			glUniform1f(plane_id+2, M[i+2].kA);
			glUniform1f(plane_id+3, M[i+2].kD);
			glUniform1f(plane_id+4, M[i+2].kS);
			glUniform1f(plane_id+5, M[i+2].kR);
			glUniform1f(plane_id+6, M[i+2].shininess);
			glUniform3f(plane_id+7, M[i+2].color[0], M[i+2].color[1], M[i+2].color[2]);
		}

		GLuint light_base = glGetUniformLocation(raytracer, "L[0].k");
		for(int i = 0; i < N_LIGHTS; i++)
		{
			GLuint light_id = light_base + i*3;

			glUniform1f(light_id, PL[i].k);
			glUniform1f(light_id+1, PL[i].falloff);
			glUniform3f(light_id+2, PL[i].pos[0], PL[i].pos[1], PL[i].pos[2]);
		}

		GLuint cylinder_base = glGetUniformLocation(raytracer, "C[0].pos");
		for(int i = 0; i < N_CYLINDER; i++)
		{
			GLuint cylinder_id = cylinder_base + i*9;

			glUniform3f(cylinder_id, C[i].pos[0], C[i].pos[1], C[i].pos[2]);
			glUniform1f(cylinder_id+1, C[i].a);
			glUniform1f(cylinder_id+2, C[i].b);

			//set material
			glUniform1f(cylinder_id+3, M[4].kA);
			glUniform1f(cylinder_id+4, M[4].kD);
			glUniform1f(cylinder_id+5, M[4].kS);
			glUniform1f(cylinder_id+6, M[4].kR);
			glUniform1f(cylinder_id+7, M[4].shininess);
			glUniform3f(cylinder_id+8, M[4].color[0], M[4].color[1], M[4].color[2]);
		}

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
	GLFWwindow *window = glfwCreateWindow(800,600, "Project 2", NULL, NULL);
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