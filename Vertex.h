#ifndef __VERTEX_H__
#define __VERTEX_H__

#pragma once
#include "Vec3.h"
#include <vector>

using namespace std;

class Face;
class Vertex
{
public:
	int				_index;
	bool    hasNbVertex(Vertex *v);
	Vec3<double>	_pos;
	Vec3<double>	_normal;
	vector<Face*>	_nbFaces; // nb(neighbor faces)
	vector<Vertex*>    _nbVertices; // Neighbor vertex
public:
	Vertex();
	Vertex(int index, Vec3<double> pos)
	{
		_index = index;
		_pos = pos;
	}
	~Vertex();
public:
	inline double x(void) { return _pos.x(); }
	inline double y(void) { return _pos.y(); }
	inline double z(void) { return _pos.z(); }
};

#endif