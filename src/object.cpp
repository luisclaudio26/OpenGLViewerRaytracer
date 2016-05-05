#include "../inc/object.h"
#include "../inc/shaderloader.h"

Object::Object()
{
	this->shader_id = 0;
	this->vertex_array_id = 0;
	this->h_kA = this->h_kD = this->h_kS = 0;

	//Random default value
	this->kA = this->kD = this->kS = 0.3f;
}

void Object::load(const std::string& file, const std::string& shader)
{
	std::fstream in; in.open(file);

	while(!in.eof())
	{
		std::string buf;
		getline(in, buf);
		process_line(buf);
	}

	in.close();

	//Once data is stored inside object, load it into OpenGL
	this->load_into_opengl(shader);
}

void Object::load_into_opengl(const std::string& shader)
{
	//Load shader program
	this->shader_id = ShaderLoader::load(shader);

	//Build and bring new vertex array to context
	glGenVertexArrays(1, &this->vertex_array_id);
	glBindVertexArray(this->vertex_array_id);

	//Load data into buffer
	GLuint vertex_buffer;
	glGenBuffers(1, &vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);

	glBufferData(GL_ARRAY_BUFFER,			 
				this->out_vertices.size()*sizeof(Vertex),
				&this->out_vertices[0],
				GL_STATIC_DRAW);

	//with the current vertex array and data buffer loaded, 
	//load the parameters expected by the shader
	this->load_glsl_parameters();

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void Object::load_glsl_parameters()
{
	//Position
	GLuint id_pos = glGetAttribLocation(this->shader_id, "pos");
	glEnableVertexAttribArray(id_pos);
	glVertexAttribPointer(id_pos,			//Attribute we're setting
							3,				//how many elements this attribute is composed
							GL_FLOAT,		//type of the element
							GL_FALSE,		//element is not normalized
							sizeof(Vertex), //offset to next attribute in array. MAYBE THIS WILL CAUSE PROBLEMS
							0);				//offset to the first attribute

	//Normal
	GLuint id_normal = glGetAttribLocation(this->shader_id, "normal");
	glEnableVertexAttribArray(id_normal);
	glVertexAttribPointer(id_normal,			
							3,
							GL_FLOAT,
							GL_FALSE,
							sizeof(Vertex),
							(GLvoid*) (3*sizeof(GL_FLOAT)) );

	//Texture coordinates
	GLuint id_tex = glGetAttribLocation(this->shader_id, "tex");
	glEnableVertexAttribArray(id_tex);
	glVertexAttribPointer(id_tex,			
							3,
							GL_FLOAT,
							GL_FALSE,
							sizeof(Vertex),
							(GLvoid*) (6*sizeof(GL_FLOAT)) ) ;

	//Handles to uniform parameters (i.e., which are global to the model)
	this->h_kD = glGetUniformLocation(this->shader_id, "model.kD");
	this->h_kA = glGetUniformLocation(this->shader_id, "model.kA");
	this->h_kS = glGetUniformLocation(this->shader_id, "model.kS");
}

void Object::draw()
{
	//Install model shader
	glUseProgram(this->shader_id);

	//Set uniform data
	GLuint model = glGetUniformLocation(this->shader_id, "model.Matrix");
	glm::mat4 Model = glm::mat4(1.0f);
	glUniformMatrix4fv(model, 1, GL_FALSE, &Model[0][0]);

	glUniform1f(this->h_kD, 1.0f);
	glUniform1f(this->h_kA, 0.2f);
	glUniform1f(this->h_kS, 0.0f);
	
	//----------- This will removed from here after ------------
	glm::vec4 lightPos = glm::vec4(3.0f, 3.0f, -3.0f, 1.0f);

	GLuint light1 = glGetUniformLocation(this->shader_id, "light1");
	glUniform3f(light1, lightPos[0], lightPos[1], lightPos[2]);
	
	//View-Project matrix
	glm::mat4 Projection = glm::perspective(glm::radians(45.0f), 800.0f/600.0f, 0.01f, 20.0f);
	glm::mat4 View = glm::lookAt( glm::vec3(3.0f, 3.0f, 3.0f), //Position 
								glm::vec3(0.0f, 0.0f, 0.0f), 	//Look direction
								glm::vec3(0.0f, 1.0f, 0.0f) );	//Up

	//Pre-multiply projection and view
	glm::mat4 vpMatrix = Projection * View;

	GLuint vp = glGetUniformLocation(this->shader_id, "vp");
	glUniformMatrix4fv(vp, 1, GL_FALSE, &vpMatrix[0][0]);
	//----------------------------------------------------------

	//Load data for each model
	glBindVertexArray(this->vertex_array_id);
	glDrawArrays(GL_TRIANGLES, 0, this->out_vertices.size());
	glBindVertexArray(0);
	
	glUseProgram(0);
}



//Terrible function. It'll change after 
//(or not; we're waiting for Waldon's code)
void Object::process_line(const std::string& line)
{
	std::stringstream ss(line);
	std::string flag; ss>>flag;

	if(!flag.compare("v")) {
		Vec3 v;
		ss>>v.x>>v.y>>v.z;
		this->vertice.push_back(v);
	} 
	else if(!flag.compare("vt")) {
		Vec2 vt;
		ss>>vt.u>>vt.v;
		this->texture.push_back(vt);
	} 
	else if(!flag.compare("vn")) {
		Vec3 vn;
		ss>>vn.x>>vn.y>>vn.z;
		this->normal.push_back(vn);
	}
	else if(!flag.compare("f")) {

		//This code snipped won't work if texture coordinate is not informed!
		//Fix this after.
		int v1, vt1, vn1;
		int v2, vt2, vn2;
		int v3, vt3, vn3;

		//sscanf(line.c_str(), "f %d/%d/%d %d/%d/%d %d/%d/%d", &v1, &vt1, &vn1,
		//													&v2, &vt2, &vn2,
		//													&v3, &vt3, &vn3);

		sscanf(line.c_str(), "f %d//%d %d//%d %d//%d", &v1, &vn1,
														&v2, &vn2,
														&v3, &vn3);

		Vertex V1, V2, V3;
		V1.pos = vertice[v1-1];
		V1.n = normal[vn1-1];
		//V1.tex = texture[vt1-1];

		V2.pos = vertice[v2-1];
		V2.n = normal[vn2-1];
		//V2.tex = texture[vt2-1];

		V3.pos = vertice[v3-1];
		V3.n = normal[vn3-1];
		//V3.tex = texture[vt3-1];

		out_vertices.push_back(V1);
		out_vertices.push_back(V2);
		out_vertices.push_back(V3);
	}
}