#ifndef _OBJECT_H_
#define _OBJECT_H_

#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cstdio>

typedef struct {
	float u, v;
} Vec2;

typedef struct {
	float x, y, z;
} Vec3;

typedef struct {
	float x, y, z, w;
} Vec4;

typedef struct {
	Vec3 pos, n;
	Vec2 tex;
} Vertex;

class Object
{
private:
	std::vector<Vec3> vertice;
	std::vector<Vec2> texture;
	std::vector<Vec3> normal;

	std::vector<Vertex> out_vertices;

	void process_line(const std::string& line);

public:
	void load(const std::string& file);
	std::vector<Vertex>& getVertices() {
		return out_vertices;
	}
};

#endif