#ifndef __MESH_H__
#define __MESH_H__

#pragma once
#include "Face.h"
#include "GL\glut.h"
#include "Texture.h"
#include <math.h>
#include "Vertex.h"
#include <vector>
#include<iostream>
#define SIZE 10
#define PI 3.14159265
using namespace std;

class Mesh
{
public:
	vector<Face*>	_faces;
	vector<Vertex*>	_vertices;
	vector<Texture*>_textureCoords;
	Vec3<double>	_minBound;
	Vec3<double>	_maxBound;
	GLuint			_textureIndex;
	float size;
	float x1, y1, z1;
	float angle;
	float move;
	float fish_angle;
	float oc_angle;

public:
	Mesh();
	Mesh(const char *obj,const char *texture,float x,float y,float z)
	{
		open(obj);
		loadTexture(texture, _textureIndex);
		x1 = x;
		y1 = y;
		z1 = z;
	}
	~Mesh();
public:
	void	open(const char *file);
	void	loadTexture(const char *file, GLuint &texture_index);
	void	computeNormal(void);
	void	makeAdjacencyList(void);
	void	moveToCenter(double scale = 1.0);
public:
	void	drawWire(void);
	void	drawPoint(void);
	void	drawSolid(bool smoothing);
	void	drawSolid2(bool smoothing);
	void	drawSolid3(bool smoothing);
};

#endif