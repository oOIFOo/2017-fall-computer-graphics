#include <glew.h>
#include "mesh.h"
#include "glut.h"
#include <fstream>
#include <cstdio>
#include <string>
#include <iostream>
#include <list>
#include <cmath>
#include <Freeimage.h>
#include<vector>

#define	num_texture 6
unsigned int texobject[num_texture];

using namespace std;
GLfloat light_specular[4] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_diffuse[4] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_ambient[4] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat light_position[4] = { 150.0, 150.0, 150.0, 0.3 };
GLfloat ambient[3];

string text[num_texture];
int windowSize[2];
int num = 0;
double trans[10][3];
double trans_trans[3];
double pi = 3.14;
double eyex = 500.0, eyey = 500.0, eyez = 500.0;
double cenx = 0.0, ceny = 0.0, cenz = 0.0;
double upx = 1.0, upy = 0.0, upz = 0.0;
double viewportx = 0.0, viewporty = 0.0, viewportz = 0.0, viewporth = 0.0;
double fovy = 1.0, near = 0.0, far = 0.0;
double pre_mousex, pre_mousey;
int flag = 0, mousepress = 0, first_in = 1;
double mousex, mousey;
double vectorx, vectory, vectorz;
list<mesh*> obj_list;
mesh *object;
int first = 0;

void light(int x);
void display();
void reshape(GLsizei, GLsizei);
void keyboardinput(unsigned char key, int x, int y);
void read_view_file();
void read_light_file();
void read_scene_input();
void show(string obj_in, int x);
void loadtexture(char* pfilename, int i);
void mutitexture();
void cubetexture();
void loadcube();
void singletexture();
void mapping(int i);

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("Mesh Example");
	read_view_file();
	glewInit();
	glutKeyboardFunc(keyboardinput);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMainLoop();

	system("pause");
	return 0;
}

void loadcube() {
	FIBITMAP* pimage;
	FIBITMAP *p32bitsimage;
	int iwidth;
	int iheight;

	for (int i = 0; i < 6; i++) {
		pimage = FreeImage_Load(FreeImage_GetFileType(text[i].data(), 0), text[i].data());
		p32bitsimage = FreeImage_ConvertTo32Bits(pimage);
		iwidth = FreeImage_GetWidth(p32bitsimage);
		iheight = FreeImage_GetHeight(p32bitsimage);

		glBindTexture(GL_TEXTURE_CUBE_MAP, texobject[0]);

		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, iwidth, iheight, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32bitsimage));

		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
		glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 3);

		glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
		FreeImage_Unload(p32bitsimage);
		FreeImage_Unload(pimage);
	}
}

void loadtexture(char* pfilename, int i) {
	FIBITMAP* pimage = FreeImage_Load(FreeImage_GetFileType(pfilename, 0), pfilename);
	FIBITMAP *p32bitsimage = FreeImage_ConvertTo32Bits(pimage);
	int iwidth = FreeImage_GetWidth(p32bitsimage);
	int iheight = FreeImage_GetHeight(p32bitsimage);

	glBindTexture(GL_TEXTURE_2D, texobject[i]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, iwidth, iheight,
		0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32bitsimage));
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

	FreeImage_Unload(p32bitsimage);
	FreeImage_Unload(pimage);
}

void read_scene_input() {
	string tmp1, tmp2;
	int i = 0;
	ifstream input("Chess.scene");
	double scale[3];
	double rotate[4];
	while (!input.eof()) {
		input >> tmp1;
		if (tmp1 == "no-texture") {
			i = 0;
		}
		else if (tmp1 == "single-texture") {
			input >> text[0];
			i = 1;
		}
		else if (tmp1 == "multi-texture") {
			input >> text[0];
			input >> text[1];
			i = 2;
		}
		else if (tmp1 == "cube-map") {
			input >> text[0];
			input >> text[1];
			input >> text[2];
			input >> text[3];
			input >> text[4];
			input >> text[5];
			i = 6;
		}
		else if (tmp1 == "model") {
			input >> tmp2;

			for (int i = 0; i < 3; i++) {
				input >> tmp1;
				scale[i] = atof(tmp1.data());
			}

			for (int i = 0; i < 4; i++) {
				input >> tmp1;
				rotate[i] = atof(tmp1.data());
			}

			for (int i = 0; i < 3; i++) {
				input >> tmp1;
				trans_trans[i] = atof(tmp1.data());
			}

			mapping(i);
			glPushMatrix();
			glTranslated(trans_trans[0], trans_trans[1], trans_trans[2]);
			glRotated(rotate[0], rotate[1], rotate[2], rotate[3]);
			glScaled(scale[0], scale[1], scale[2]);
			show(tmp2, i);
			glPopMatrix();
		}
	}
}

void mapping(int i){
	if (i == 1) {
		singletexture();
	}
	else if (i == 2) {
		mutitexture();
	}
	else if (i == 6) {
		cubetexture();
	}
}

void read_view_file() {
	string tmp;
	ifstream input("Chess.view");
	while (!input.eof()) {
		input >> tmp;
		if (tmp == "eye") {
			input >> tmp;
			eyex = atof(tmp.data());
			input >> tmp;
			eyey = atof(tmp.data());
			input >> tmp;
			eyez = atof(tmp.data());
		}

		if (tmp == "vat") {
			input >> tmp;
			cenx = atof(tmp.data());
			input >> tmp;
			ceny = atof(tmp.data());
			input >> tmp;
			cenz = atof(tmp.data());
		}

		if (tmp == "vup") {
			input >> tmp;
			upx = atof(tmp.data());
			input >> tmp;
			upy = atof(tmp.data());
			input >> tmp;
			upz = atof(tmp.data());
		}

		if (tmp == "fovy") {
			input >> tmp;
			fovy = atof(tmp.data());
		}

		if (tmp == "dnear") {
			input >> tmp;
			near = atof(tmp.data());
		}

		if (tmp == "dfar") {
			input >> tmp;
			far = atof(tmp.data());
		}

		if (tmp == "viewport") {
			input >> tmp;
			viewportx = atof(tmp.data());
			input >> tmp;
			viewporty = atof(tmp.data());
			input >> tmp;
			viewportz = atof(tmp.data());
			input >> tmp;
			viewporth = atof(tmp.data());
		}
	}
}

void read_light_file() {
	string tmp;
	ifstream input("Chess.light");
	int x = 0;
	while (!input.eof()) {
		input >> tmp;
		if (tmp == "light") {
			for (int i = 0; i < 3; i++) {
				input >> tmp;
				light_position[i] = atof(tmp.data());
			}
			light_position[3] = 0.3;

			for (int i = 0; i < 3; i++) {
				input >> tmp;
				light_ambient[i] = atof(tmp.data());
			}
			light_ambient[3] = 1.0;

			for (int i = 0; i < 3; i++) {
				input >> tmp;
				light_diffuse[i] = atof(tmp.data());
			}
			light_diffuse[3] = 1.0;

			for (int i = 0; i < 3; i++) {
				input >> tmp;
				light_specular[i] = atof(tmp.data());
			}
			light_specular[3] = 1.0;  //???
			light(x);
			x++;
		}

		if (tmp == "ambient") {
			for (int i = 0; i < 3; i++) {
				input >> tmp;
				ambient[i] = atof(tmp.data());
			}
			glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
		}
	}
}

void light(int x)
{

	glShadeModel(GL_SMOOTH);

	// z buffer enable
	glEnable(GL_DEPTH_TEST);

	// enable lighting
	glEnable(GL_LIGHTING);
	// set light property
	glEnable(GL_LIGHT0 + x);
	glLightfv(GL_LIGHT0 + x, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0 + x, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0 + x, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0 + x, GL_AMBIENT, light_ambient);
}

void display()
{
	// clear the buffer

	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);      //清除用color
	glClearDepth(1.0f);                        // Depth Buffer (就是z buffer) Setup
	glEnable(GL_DEPTH_TEST);                   // Enables Depth Testing
	glDepthFunc(GL_LEQUAL);                    // The Type Of Depth Test To Do
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//這行把畫面清成黑色並且清除z buffer

													   // viewport transformation
	glViewport(viewportx, viewporty, windowSize[0], windowSize[1]);

	// projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (GLfloat)windowSize[0] / (GLfloat)windowSize[1], near, far);
	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	gluLookAt(eyex, eyey, eyez,// eye
		cenx, ceny, cenz,     // center
		upx, upy, upz);    // up

	read_light_file();
	read_scene_input();
	glutSwapBuffers();

	first = 1;
}

void show(string obj_in, int x) {
	object = new mesh(obj_in.data());
	int lastMaterial = -1;
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.5f);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_DEPTH_TEST);

	for (size_t i = 0; i < object->fTotal; ++i)
	{
		// set material property if this face used different material
		if (lastMaterial != object->faceList[i].m)
		{
			lastMaterial = (int)object->faceList[i].m;
			glMaterialfv(GL_FRONT, GL_AMBIENT, object->mList[lastMaterial].Ka);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, object->mList[lastMaterial].Kd);
			glMaterialfv(GL_FRONT, GL_SPECULAR, object->mList[lastMaterial].Ks);
			glMaterialfv(GL_FRONT, GL_SHININESS, &object->mList[lastMaterial].Ns);

			//you can obtain the texture name by object->mList[lastMaterial].map_Kd
			//load them once in the main function before mainloop
			//bind them in display function here
		}


		if (x == 0) {
			glBegin(GL_TRIANGLES);
			for (size_t j = 0; j<3; ++j)
			{
				glNormal3fv(object->nList[object->faceList[i][j].n].ptr);
				glVertex3fv(object->vList[object->faceList[i][j].v].ptr);
			}
			glEnd();
		}
		else if (x == 1) {
			cout << i << endl;
			glBegin(GL_TRIANGLES);
			for (size_t j = 0; j<3; ++j)
			{
				glTexCoord2fv(object->tList[object->faceList[i][j].t].ptr);
				glNormal3fv(object->nList[object->faceList[i][j].n].ptr);
				glVertex3fv(object->vList[object->faceList[i][j].v].ptr);
			}
			glEnd();
			continue;
		}
		else if (x == 2) {
			glBegin(GL_TRIANGLES);
			for (size_t j = 0; j < 3; ++j)
			{
				glMultiTexCoord2fv(GL_TEXTURE0, object->tList[object->faceList[i][j].t].ptr);
				glMultiTexCoord2fv(GL_TEXTURE1, object->tList[object->faceList[i][j].t].ptr);
				glNormal3fv(object->nList[object->faceList[i][j].n].ptr);
				glVertex3fv(object->vList[object->faceList[i][j].v].ptr);
			}
			glEnd();
		}
		else if (x == 6) {
			glBegin(GL_TRIANGLES);
			for (size_t j = 0; j < 3; ++j)
			{
				//glTexCoord2fv(object->tList[object->faceList[i][j].t].ptr);
				glNormal3fv(object->nList[object->faceList[i][j].n].ptr);
				glVertex3fv(object->vList[object->faceList[i][j].v].ptr);
			}
			glEnd();
		}
	}

	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_TEXTURE_CUBE_MAP);
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	glDisable(GL_ALPHA_TEST);
	glDisable(GL_TEXTURE_GEN_R);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_S);

	glActiveTexture(GL_TEXTURE1);
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0);
	glDisable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, 0);


	glFlush();
}

void reshape(GLsizei w, GLsizei h)
{
	windowSize[0] = w;
	windowSize[1] = h;
}

void singletexture() {
	FreeImage_Initialise();
	glGenTextures(1, texobject);
	loadtexture(&text[0][0], 0);

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texobject[0]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	FreeImage_DeInitialise();
}

void mutitexture() {
	FreeImage_Initialise();
	glGenTextures(num_texture, texobject);
	loadtexture(&text[0][0], 0);
	loadtexture(&text[1][0], 1);

	glActiveTexture(GL_TEXTURE0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texobject[0]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);

	glActiveTexture(GL_TEXTURE1);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texobject[1]);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
	glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);

	FreeImage_DeInitialise();
}

void cubetexture() {
	FreeImage_Initialise();
	loadcube();
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
	glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);
	glEnable(GL_TEXTURE_CUBE_MAP);

	FreeImage_DeInitialise();
}

void keyboardinput(unsigned char key, int x, int y) {
	vectorx = cenx - eyex; vectory = ceny - eyey; vectorz = cenz - eyez;
	double factor;
	factor = sqrt((vectorx*vectorx) + (vectory*vectory) + (vectorz*vectorz));
	vectorx = vectorx / factor; vectory = vectory / factor; vectorz = vectorz / factor;

	switch (key) {
	case 'w':
		glMatrixMode(GL_MODELVIEW);
		eyex += vectorx *5;
		eyey += vectory*5;
		eyez += vectorz*5;
		glutPostRedisplay();
		break;
	case 'a':
		glMatrixMode(GL_MODELVIEW);
		eyex = cos(15*pi / 180)*eyex - sin(15*pi / 180)*eyez;
		eyez = sin(15*pi / 180)*eyex + cos(15*pi / 180)*eyez;
		glutPostRedisplay();
		break;
	case 's':
		glMatrixMode(GL_MODELVIEW);
		eyex -= vectorx*5;
		eyey -= vectory*5;
		eyez -= vectorz*5;
		glutPostRedisplay();
		break;
	case 'd':
		glMatrixMode(GL_MODELVIEW);
		eyex = cos(-15*pi / 180)*eyex - sin(-15*pi / 180)*eyez;
		eyez = sin(-15*pi / 180)*eyex + cos(-15*pi / 180)*eyez;
		glutPostRedisplay();
		break;
	}
}