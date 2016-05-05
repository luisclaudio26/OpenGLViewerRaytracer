#include <GL/glew.h>
#include <glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "object.h"
#include "shaderloader.h"
#include <iostream>

int load_shaders();

char* vertex_shader = 	"#version 150\n"
						"struct _model {\n"
						"	mat4 Matrix;\n"
						"	float kD;\n"
						"	float kA;\n"
						"	float kS;\n"
						"};\n"
						"uniform _model model;"
						"uniform mat4 vp;\n"
						"uniform vec3 light1;\n"
						"in vec3 pos;\n"
						"in vec3 normal;\n"
						"in vec3 tex;\n"
						"out vec3 vcolor;\n"
						"void main() {\n"
						"	gl_Position = vp * model.Matrix * vec4(pos, 1.0);\n"
						"	vec3 v2light = normalize(light1 - pos);\n"
						"	float d = max( 0.0, dot(v2light, normal) );\n"
						"	vcolor = model.kA*vec3(0.0,0.0,1.0) + model.kD*d*vec3(0.0,0.0,1.0);\n"
						"}";

char* fragment_shader = "#version 150\n"
						"in vec3 vcolor;\n"
						"out vec4 fragColour;\n"
						"void main() {\n"
						"	fragColour = vec4(vcolor, 1.0);\n"
						"}";

int main(int argc, char** args)
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

	//--------------------------------------
	//---------- GEOMETRY LOADING ----------
	//--------------------------------------
	GLuint vertexArrayID;
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	//Load shader code
	GLuint shader_program_id = load_shaders();

	//Load data into buffer
	Object obj; obj.load("./obj/lowPolySphere/lpSphere.obj");
	GLuint vertex_buffer;
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, obj.out_vertices.size()*sizeof(Vertex), &obj.out_vertices[0], GL_STATIC_DRAW);
	
	//Bind data to variables inside shader!!!
	GLuint pos = glGetAttribLocation(shader_program_id, "pos");
	GLuint normal = glGetAttribLocation(shader_program_id, "normal");
	GLuint tex = glGetAttribLocation(shader_program_id, "tex");

	//This is information of how to iterate over vertex data. posColor is in the positions
	//3,6,9,... so we need to define this offset. Also, vertice data are packed in 6 floats,
	//so we inform this also
	glEnableVertexAttribArray(pos);
	glEnableVertexAttribArray(normal);
	glEnableVertexAttribArray(tex);

	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GL_FLOAT), 0);
	glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, 8*sizeof(GL_FLOAT), (GLvoid*)(3*sizeof(GL_FLOAT)));
	glVertexAttribPointer(tex, 2, GL_FLOAT, GL_FALSE, 8*sizeof(GL_FLOAT), (GLvoid*)(6*sizeof(GL_FLOAT)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//material
	GLuint kD_id = glGetUniformLocation(shader_program_id, "model.kD");
	GLuint kA_id = glGetUniformLocation(shader_program_id, "model.kA");
	GLuint kS_id = glGetUniformLocation(shader_program_id, "model.kS");

	//visualization
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.01f, 20.0f);
	glm::mat4 View = glm::lookAt( glm::vec3(3.0f, 3.0f, 3.0f), //Position 
								glm::vec3(0.0f, 0.0f, 0.0f), 	//Look direction
								glm::vec3(0.0f, 1.0f, 0.0f) );	//Up

	//Pre-multiply projection and view
	glm::mat4 vpMatrix = Projection * View;

	//model angle
	float angle = 0.0f;

	GLuint vp = glGetUniformLocation(shader_program_id, "vp");
	GLuint model = glGetUniformLocation(shader_program_id, "model.Matrix");

	//lighting model
	glm::vec4 point_light1 = glm::vec4(3.0f, 1.0f, 0.0f, 1.0f);
	GLuint light1 = glGetUniformLocation(shader_program_id, "light1");

	//-------------------------------
	//---------- MAIN LOOP ----------
	//-------------------------------
	do
	{
		//Clear screen -> this function also clears stencil and depth buffer
		glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

		//Install our shader, draw geometry
		glUseProgram(shader_program_id);

		//---------- Set uniform data -----------
		//Model matrix
		glm::mat4 Model = glm::mat4(2.0f);
		glUniformMatrix4fv(model, 1, GL_FALSE, &Model[0][0]);

		glUniform1f(kD_id, 1.0f);
		glUniform1f(kA_id, 0.2f);
		glUniform1f(kS_id, 0.0f);
		
		//Point light
		glm::mat4 LightMat = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));
		glm::vec4 lightPos = LightMat * point_light1;
		glUniform3f(light1, lightPos[0], lightPos[1], lightPos[2]);
		
		//View-Project matrix
		glUniformMatrix4fv(vp, 1, GL_FALSE, &vpMatrix[0][0]);

		angle += 0.02f; if(angle >= 6.28) angle = 0.0;

		//Load data for each model
		glBindVertexArray(vertexArrayID);
		glDrawArrays(GL_TRIANGLES, 0, obj.out_vertices.size());
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

int load_shaders()
{
	//Effectively, this function create shader objects, load shader code,
	//compile it, create a program, attach the shaders to it then link the program

	//Load vertex shader from hard coded string
	const GLchar* v_shader_code = vertex_shader;
	GLuint v_shader_id = glCreateShader(GL_VERTEX_SHADER); //Create shader id
	glShaderSource(v_shader_id, 1, &v_shader_code, NULL);
	glCompileShader(v_shader_id);

	char buffer[1000];
	glGetShaderInfoLog(v_shader_id, 1000, NULL, buffer);
	std::cout<<"Vertex shader log:\n"<<buffer<<std::endl;

	//Load fragment shader from hard coded string
	const GLchar* f_shader_code = fragment_shader;
	GLuint f_shader_id = glCreateShader(GL_FRAGMENT_SHADER); //Create shader id
	glShaderSource(f_shader_id, 1, &f_shader_code, NULL);
	glCompileShader(f_shader_id);

	char buffer2[1000];
	glGetShaderInfoLog(f_shader_id, 1000, NULL, buffer2);
	std::cout<<"Fragment shader log:\n"<<buffer2<<std::endl;

	//Link program
	GLuint program_id = glCreateProgram();
	glAttachShader(program_id, v_shader_id);
	glAttachShader(program_id, f_shader_id);
	glLinkProgram(program_id);

	return program_id;
}