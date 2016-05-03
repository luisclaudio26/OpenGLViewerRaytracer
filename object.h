#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
using std::vector;
using std::fstream;
using std::string;
using std::stringstream;

#include <cstdio>

typedef struct {
	float x, y, z;
} Vec3;

typedef struct {
	float x, y, z, w;
} Vec4;

typedef struct {
	Vec3 pos, n, tex;
} Vertex;

typedef struct {
	Vertex v[3];
} Face;

class Object
{
private:
	void process_line(string line)
	{
		stringstream ss(line);

		string flag; ss>>flag;

		if(!flag.compare("v")) {
			Vec3 v;
			ss>>v.x>>v.y>>v.z;
			this->vertice.push_back(v);
		} 
		else if(!flag.compare("vt")) {
			Vec3 vt;
			ss>>vt.x>>vt.y;
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


			Face f;
			f.v[0].pos = vertice[v1-1];
			f.v[0].n = normal[vn1-1];
			//f.v[0].tex = texture[vt1-1];			

			f.v[1].pos = vertice[v2-1];
			f.v[1].n = normal[vn2-1];
			//f.v[1].tex = texture[vt2-1];

			f.v[2].pos = vertice[v3-1];
			f.v[2].n = normal[vn3-1];
			//f.v[2].tex = texture[vt3-1];

			faces.push_back(f);
		}
	}

public:
	vector<Vec3> vertice;
	vector<Vec3> texture;
	vector<Vec3> normal;
	vector<Face> faces;

	void load(std::string file)
	{
		fstream in; in.open(file);

		while(!in.eof())
		{
			string buf;
			getline(in, buf);

			process_line(buf);
		}

		in.close();
	}
};

#endif