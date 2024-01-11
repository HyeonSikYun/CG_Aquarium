#include "Mesh.h"
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
int index[3];
int index2[3];
int index3[3];
Mesh::Mesh()
{
	x1 = 0;
	y1 = 0;
	z1 = 0;
	angle = 0;
	move = 0;
	fish_angle = 180;
	oc_angle = 0;
}

Mesh::~Mesh()
{
}

void Mesh::loadTexture(const char *file, GLuint &texture_index)
{
	glGenTextures(1, &texture_index);
	FILE *fp;
	fopen_s(&fp, file, "rb");
	if (!fp)
	{
		printf("ERROR : NO %s.\n fail to bind %d\n", file, texture_index);
	}
	int width, height, channel;
	unsigned char *image = stbi_load_from_file(fp, &width, &height, &channel, 4);
	fclose(fp);

	glBindTexture(GL_TEXTURE_2D, texture_index);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_COLOR, GL_MODULATE);
}

void Mesh::open(const char *file)
{
	char buffer[1000] = { 0 };
	Vec3<double> pos;

	int id = 0;

	_minBound.Set(100000000.0);
	_maxBound.Set(-100000000.0);

	FILE *fp;
	fopen_s(&fp, file, "r");
	while (fscanf(fp, "%s %lf %lf %lf", &buffer, &pos[0], &pos[1], &pos[2]) != EOF) {
		if (buffer[0] == 'v' && buffer[1] == NULL) {
			for (int i = 0; i < 3; i++) {
				if (_minBound[i] > pos[i]) _minBound[i] = pos[i];
				if (_maxBound[i] < pos[i]) _maxBound[i] = pos[i];
			}
			_vertices.push_back(new Vertex(id++, pos));
		}
	}
	printf("num. of vertices : %d\n", _vertices.size());

	id = 0;
	fseek(fp, 0, SEEK_SET);
	while (fscanf(fp, "%s %lf %lf",&buffer, &pos[0], &pos[1]) != EOF)
	{
		if (!strcmp(buffer, "vt"))
		{
			_textureCoords.push_back(new Texture(pos[0], 1.0 - pos[1], 0.0));
		}
	}

	id = 0;
	fseek(fp, 0, SEEK_SET);
	while (fscanf(fp, "%s %d/%d/%d %d/%d/%d %d/%d/%d", &buffer, &index[0], &index[1], &index[2], &index2[0], &index2[1], &index2[2], &index3[0], &index3[1], &index3[2]) != EOF) {
		if (buffer[0] == 'f' && buffer[1] == NULL) {
			auto v0 = _vertices[index[0] - 1];
			auto v1 = _vertices[index2[0] - 1];
			auto v2 = _vertices[index3[0] - 1];
			_faces.push_back(new Face(id++, v0, v1, v2, index[1] - 1, index2[1] - 1, index3[1] - 1));
		}
	}
	fclose(fp);

	moveToCenter(10.0);
	makeAdjacencyList();
	computeNormal();
}

void Mesh::makeAdjacencyList(void)
{
	// v-f
	for (auto f : _faces) {
		for (auto v : f->_vertices) {
			v->_nbFaces.push_back(f);
		}
	}
}

void Mesh::computeNormal(void)
{
	// face normal
	for (auto f : _faces) {
		auto v1 = f->v(1)->_pos - f->v(0)->_pos;
		auto v2 = f->v(2)->_pos - f->v(0)->_pos;
		f->_normal = v1.Cross(v2);
		f->_normal.Normalize();
	}

	// vertex normal
	for (auto v : _vertices) {
		for (auto nf : v->_nbFaces) {
			v->_normal += nf->_normal;
		}
		v->_normal /= (double)v->_nbFaces.size();
	}
}

void Mesh::moveToCenter(double scale)
{
	double crossLen = (_minBound - _maxBound).Length();
	auto center = (_maxBound + _minBound) / 2.0;
	Vec3<double> origin(0.0, 0.0, 0.0);

	for (auto v : _vertices) {
		auto pos = v->_pos;
		auto vec = pos - center;
		vec /= crossLen;
		vec *= scale;
		v->_pos = origin + vec;
	}
}

void Mesh::drawWire(void)
{
	glPushMatrix();
	glEnable(GL_LIGHTING);
	for (auto f : _faces) {
		glBegin(GL_LINES);
		for (int i = 0; i < 3; i++) {
			auto v0 = f->v((i + 1) % 3); // 2
			auto v1 = f->v((i + 2) % 3); // 0
			glNormal3f(v0->_normal.x(), v0->_normal.y(), v0->_normal.z());
			glVertex3f(v0->x(), v0->y(), v0->z());
		
			glNormal3f(v1->_normal.x(), v1->_normal.y(), v1->_normal.z());
			glVertex3f(v1->x(), v1->y(), v1->z());
		}
		glEnd();
	}
	glDisable(GL_LIGHTING);
	glPopMatrix();
}

void Mesh::drawSolid(bool smoothing)
{
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _textureIndex);
	glEnable(GL_LIGHTING);
	smoothing == true ? glEnable(GL_SMOOTH) : glEnable(GL_FLAT);
	glEnable(GL_SMOOTH);
	glScalef(0.8, 0.8, 0.8);
	glRotatef(angle, 0, 1.0, 0);
	for (auto f : _faces) {


		glBegin(GL_POLYGON);
		if (!smoothing) {
			glNormal3f(f->_normal.x(), f->_normal.y(), f->_normal.z());
			for (int i = 0; i < 3; i++)
			{
				auto t = _textureCoords[f->_texelPos[i]];
				auto v = f->_vertices[i];

				glTexCoord2f((GLfloat)t->x(), (GLfloat)t->y());
				glVertex3f(v->x()+x1, v->y()+y1, v->z()+z1);
			}
		}
		for (auto v : f->_vertices) {
			if (smoothing) {
				glNormal3f(v->_normal.x(), v->_normal.y(), v->_normal.z());
			}
			glVertex3f(v->x()+x1, v->y()+y1, v->z()+z1);
		}
		glEnd();
	}
	glDisable(GL_LIGHTING);
	glPopMatrix();
}
void Mesh::drawSolid2(bool smoothing)
{
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _textureIndex);
	glEnable(GL_LIGHTING);
	smoothing == true ? glEnable(GL_SMOOTH) : glEnable(GL_FLAT);
	glEnable(GL_SMOOTH);
	glScalef(0.2, 0.2, 0.2);
	glRotatef(fish_angle, 0, 1.0, 0);
	for (auto f : _faces) {


		glBegin(GL_POLYGON);
		if (!smoothing) {
			glNormal3f(f->_normal.x(), f->_normal.y(), f->_normal.z());
			for (int i = 0; i < 3; i++)
			{
				auto t = _textureCoords[f->_texelPos[i]];
				auto v = f->_vertices[i];

				glTexCoord2f((GLfloat)t->x(), (GLfloat)t->y());
				glVertex3f(v->x()+x1, v->y()+y1, v->z()-5); 
			}
		}
		for (auto v : f->_vertices) {
			if (smoothing) {
				glNormal3f(v->_normal.x(), v->_normal.y(), v->_normal.z());
			}
			glVertex3f(v->x()+x1, v->y()+y1, v->z()-5);
		}
		glEnd();
	}
	glDisable(GL_LIGHTING);
	glPopMatrix();
}

void Mesh::drawSolid3(bool smoothing)
{
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _textureIndex);
	glEnable(GL_LIGHTING);
	smoothing == true ? glEnable(GL_SMOOTH) : glEnable(GL_FLAT);
	glEnable(GL_SMOOTH);
	glScalef(0.2, 0.2, 0.2);
	glRotatef(oc_angle, 0, 1.0, 0);
	for (auto f : _faces) {


		glBegin(GL_POLYGON);
		if (!smoothing) {
			glNormal3f(f->_normal.x(), f->_normal.y(), f->_normal.z());
			for (int i = 0; i < 3; i++)
			{
				auto t = _textureCoords[f->_texelPos[i]];
				auto v = f->_vertices[i];

				glTexCoord2f((GLfloat)t->x(), (GLfloat)t->y());
				glVertex3f(v->x() + x1, v->y() + y1, v->z() - 5); 
			}
		}
		for (auto v : f->_vertices) {
			if (smoothing) {
				glNormal3f(v->_normal.x(), v->_normal.y(), v->_normal.z());
			}
			glVertex3f(v->x() + x1, v->y() + y1, v->z() - 5);
		}
		glEnd();
	}
	glDisable(GL_LIGHTING);
	glPopMatrix();
}

void Mesh::drawPoint(void)
{
	glPushMatrix();
	glDisable(GL_LIGHTING);
	glPointSize(2.0f);
	glColor3f(1.0f, 1.0f, 1.0f);
	glBegin(GL_POINTS);
	for (auto v : _vertices) {
		glVertex3f(v->x(), v->y(), v->z());
	}
	glEnd();
	glPointSize(1.0f);
	glPopMatrix();
}