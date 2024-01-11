#include <stdio.h>
#include <Windows.h>
#include "GL/glut.h"
#include "Mesh.h"
#include "Bitmap.h"
#include <mmsystem.h>
#pragma comment(lib,"winmm")

#define USE_MIPMAP

float _zoom = 15.0f;
float _rotate_x = 0.0f;
float _rotate_y = -90.0f;
float _translate_x = 0.0f;
float _translate_y = 0.0f;
int last_x = 0;
int last_y = 0;
float dir = 1; // 방향값
float time = 0; // 시간값
float dir2 = 1;
float time2 = 0;
float s_move = 0; // 상어움직임
float f1_move = 0; // 생선 1 움직임
float f2_move = 0;
float oc_move = 0;
unsigned char _btnStates[3] = { 0 };

Mesh *_mesh;
Mesh *_mesh2;
Mesh *_mesh3;
Mesh *_mesh4;
Mesh *_mesh5;
Mesh *_mesh6;
bool _smoothing = false; 

void init(void)
{
	PlaySound(TEXT("Aquarium2.wav"), 0, SND_FILENAME | SND_ASYNC | SND_LOOP | SND_NODEFAULT);
	_mesh = new Mesh("obj\\Jaws.obj","obj\\Jaws_body.png",0,4, s_move);
	_mesh2 = new Mesh("obj\\Fish10.obj","obj\\Fish10.jpg",0,0,f1_move);
	_mesh3 = new Mesh("obj\\Fish3.obj","obj\\Fish03.jpg",0,-9,f2_move);
	_mesh4 = new Mesh("obj\\Octo.obj","obj\\Octo.png",0,oc_move,0);
	_mesh5 = new Mesh("obj\\jellyfish.obj", "obj\\jellyfish.png", 0, -14, 0);
	_mesh6 = new Mesh("obj\\Octo.obj","obj\\Octo.png",0,oc_move,0);

	glEnable(GL_DEPTH_TEST);
}

void init2(const char *filename)
{
	Bitmap texture;
	texture.open(filename);
	glTexImage2D(GL_TEXTURE_2D, 0, 3, texture.getWidth(), texture.getHeight(), 0, GL_BGR_EXT, GL_UNSIGNED_BYTE, texture._image);

#ifdef USE_MIPMAP
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, texture.getWidth(), texture.getHeight(), GL_BGR_EXT, GL_UNSIGNED_BYTE, texture._image);
#endif

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);	// GL_REPEAT
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);	// GL_REPEAT
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

#ifdef USE_MIPMAP
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
#endif
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);	//GL_DECAL, GL_REPLACE, GL_MODULATE

#ifdef USE_MIPMAP
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);	// GL_REPEAT
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);	// GL_REPEAT
#endif

	glColor3f(1.0, 1.0, 0.0);

	glEnable(GL_TEXTURE_2D);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(40, 1, 0.1, 100);

	glClearColor(0.0, 0.0, 0.0, 0.0);
}

void TextureMap()
{
	glClear(GL_COLOR_BUFFER_BIT);
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-30.0, -30, 45.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(-30.0, -30, -45.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(30.0, -30, -45.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(30.0, -30, 45.0);
	glEnd();
}
void TextureMap2()
{
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-30.0, -30.0, -45.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(-30.0, 30.0, -45.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(30.0, 30.0, -45.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(30.0, -30.0, -45.0);
	glEnd();
}
void TextureMap3()
{
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-30.0, -30.0, 45.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(-30.0, 30.0, 45.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(30.0, 30.0, 45.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(30.0, -30.0, 45.0);
	glEnd();
}
void TextureMap4()
{
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(30.0, -30.0, 45.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(30.0, 30.0, 45.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(30.0, 30.0, -45.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(30.0, -30.0, -45.0);
	glEnd();
}
void TextureMap5()
{
	glBegin(GL_QUADS);
	glTexCoord2f(0.0, 0.0); glVertex3f(-30.0, -30.0, 45.0);
	glTexCoord2f(0.0, 1.0); glVertex3f(-30.0, 30.0, 45.0);
	glTexCoord2f(1.0, 1.0); glVertex3f(-30.0, 30.0, -45.0);
	glTexCoord2f(1.0, 0.0); glVertex3f(-30.0, -30.0, -45.0);
	glEnd();
}


void draw(void)
{	

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glPushMatrix();
	glTranslatef(0, 0, s_move);
	_mesh->drawSolid(_smoothing);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 0, f1_move);
	_mesh2->drawSolid2(_smoothing);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, 0, f2_move);
	_mesh3->drawSolid2(_smoothing);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, oc_move, -8);
	_mesh4->drawSolid3(_smoothing);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(0, -2, 9);
	_mesh5->drawSolid3(_smoothing);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(1000, oc_move, -1080);
	_mesh6->drawSolid3(_smoothing);
	glPopMatrix();

	glDisable(GL_LIGHTING);
}

void GL_Display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (time > 10.0f)
	{ 
  		dir = -1;
		_mesh->angle += 180;
		_mesh2->fish_angle += 180;
	}

	if (time < -10.0f)
	{
		dir = 1;
		_mesh->angle += 180;
		_mesh2->fish_angle += 180;
	}
	time += dir * 0.5;
	s_move += dir * 0.2;
	f1_move += dir * 0.4;

	if (time2 > 10.0f)
	{
		dir2 = -1;
		_mesh3->fish_angle += 180;
	}
	if (time2 < -10.0f)
	{
		dir2 = 1;
		_mesh3->fish_angle += 180;
	}
	time2 += dir2 * 0.3;
	f2_move += dir2 * 0.2;
	oc_move += dir2 * 0.15;
	_mesh4->oc_angle += 5;
	_mesh5->oc_angle += 2;
	TextureMap();
	TextureMap2();
	TextureMap3();
	TextureMap4();
	TextureMap5();

	glLoadIdentity();

	glTranslatef(0.0f, 0.0f, -_zoom); 
	glTranslatef(_translate_x, _translate_y, 0.0);
	glRotatef(_rotate_x, 1, 0, 0);
	glRotatef(_rotate_y, 0, 1, 0);
	draw();

	glutPostRedisplay();
	glutSwapBuffers();
}

void GL_Reshape(int w, int h)
{
	if (w == 0) {
		h = 1;
	}
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45.0f, (float)w / (float)h, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
}

void GL_Motion(int x, int y)
{
	int diff_x = x - last_x;
	int diff_y = y - last_y;

	last_x = x;
	last_y = y;

	if (_btnStates[2]) {
		_zoom -= (float) 0.05f * diff_x;
	}
	else if (_btnStates[0]) {
		_rotate_x += (float)0.5f * diff_y;
		_rotate_y += (float)0.5f * diff_x;
	}
	else if (_btnStates[1]) {
		_translate_x += (float)0.05f * diff_x;
		_translate_y -= (float)0.05f * diff_y;
	}
	glutPostRedisplay();
}

void GL_Mouse(int button, int state, int x, int y)
{
	last_x = x;
	last_y = y;
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		_btnStates[0] = ((GLUT_DOWN == state) ? 1 : 0);
		break;
	case GLUT_MIDDLE_BUTTON:
		_btnStates[1] = ((GLUT_DOWN == state) ? 1 : 0);
		break;
	case GLUT_RIGHT_BUTTON:
		_btnStates[2] = ((GLUT_DOWN == state) ? 1 : 0);
		break;
	default:
		break;
	}
	glutPostRedisplay();
}

void GL_Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'Q':
	case 'q':
		exit(0);
	case 'S':
	case 's':
		_smoothing = !_smoothing;
		break;
	default:
		break;
	}
	glutPostRedisplay();
}


void main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(1500, 800);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Aquarium Project");
	init();
	init2("underwater.bmp");
	glutDisplayFunc(GL_Display);
	glClearColor(0.4, 0.2, 0.8, 0.0);
	glutReshapeFunc(GL_Reshape);
	glutMouseFunc(GL_Mouse);
	glutMotionFunc(GL_Motion);
	glutKeyboardFunc(GL_Keyboard);

	glutMainLoop();
}