#include <GL/glew.h>
#include <glfw3.h>
#include "object.h"
#include <iostream>

int load_shaders();

char* vertex_shader = 	"#version 150\n"
						"in vec3 pos;\n"
						"in vec3 normal;\n"
						"in vec3 tex;\n"
						"void main() {\n"
						"	gl_Position = vec4(pos, 1.0);\n"
						"}";

char* fragment_shader = "#version 150\n"
						"out vec4 fragColour;\n"
						"void main() {\n"
						"	fragColour = vec4(1.0, 0.0, 0.0, 1.0);\n"
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

	//--------------------------------------
	//---------- GEOMETRY LOADING ----------
	//--------------------------------------
	GLuint vertexArrayID;
	glGenVertexArrays(1, &vertexArrayID);
	glBindVertexArray(vertexArrayID);

	//Load shader code
	GLuint shader_program_id = load_shaders();

	//Load data into buffer
	Object obj; obj.load("./capsule.obj");
	GLuint vertex_buffer;
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, obj.faces.size()*sizeof(Vertex), (void*)obj.faces.data(), GL_STATIC_DRAW);
	
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

	glVertexAttribPointer(pos, 3, GL_FLOAT, GL_FALSE, 9*sizeof(GL_FLOAT), 0);
	glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, 9*sizeof(GL_FLOAT), (GLvoid*)(3*sizeof(GL_FLOAT)));
	glVertexAttribPointer(tex, 3, GL_FLOAT, GL_FALSE, 9*sizeof(GL_FLOAT), (GLvoid*)(6*sizeof(GL_FLOAT)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	//-------------------------------
	//---------- MAIN LOOP ----------
	//-------------------------------
	do
	{
		//Clear screen -> this function also clears stencil and depth buffer
		glClear( GL_COLOR_BUFFER_BIT );

		//Install our shader, draw geometry
		glUseProgram(shader_program_id);
		glBindVertexArray(vertexArrayID);
		glDrawArrays(GL_TRIANGLES, 0, 3);
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

	//Load fragment shader from hard coded string
	const GLchar* f_shader_code = fragment_shader;
	GLuint f_shader_id = glCreateShader(GL_FRAGMENT_SHADER); //Create shader id
	glShaderSource(f_shader_id, 1, &f_shader_code, NULL);
	glCompileShader(f_shader_id);

	//Link program
	GLuint program_id = glCreateProgram();
	glAttachShader(program_id, v_shader_id);
	glAttachShader(program_id, f_shader_id);
	glLinkProgram(program_id);

	return program_id;
}