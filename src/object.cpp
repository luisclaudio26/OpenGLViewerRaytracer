#include "../inc/object.h"

void Object::load(const std::string& file)
{
	std::fstream in; in.open(file);

	while(!in.eof())
	{
		std::string buf;
		getline(in, buf);
		process_line(buf);
	}

	in.close();
}

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