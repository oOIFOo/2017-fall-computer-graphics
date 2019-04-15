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
#include <vector>

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
double right_v[3];
double pi = 3.14;
double eyex = 500.0, eyey = 500.0, eyez = 500.0;
double cenx = 0.0, ceny = 0.0, cenz = 0.0;
double upx = 1.0, upy = 0.0, upz = 0.0;
double viewportx = 0.0, viewporty = 0.0, viewportz = 0.0, viewporth = 0.0;
double fovy = 1.0, near = 0.0, far = 0.0;
double pre_mousex, pre_mousey;
double jitter[3] = {0};
double target[3] = { 0 };
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
void set_stncil(int i);
void show_scene(int i);
void show_mirror();
void show_all(int i);
void show_reflect(int i);
void right_vector(double objx, double objy, double objz);
void accum_scene();
void accum_reflect();

typedef struct {
	string	name;
	string	textname;
	int		textype;
	string  text[6];
	float	scale[3];
	float	rotate[4];
	float	trans[3];
}object_tmp;

vector<object_tmp>myobj;

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(1000, 530);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
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
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

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
	object_tmp tmp;
	string tmp1, tmp2;
	int i = 0;
	ifstream input("CornellBox.scene");
	while (!input.eof()) {
		input >> tmp.textname;
		if (tmp.textname == "no-texture") {
			tmp.textype = 0;
		}
		else if (tmp.textname == "single-texture") {
			input >> tmp.text[0];
			tmp.textype = 1;
		}
		else if (tmp.textname == "multi-texture") {
			input >> tmp.text[0];
			input >> tmp.text[1];
			tmp.textype = 2;
		}
		else if (tmp.textname == "cube-map") {
			input >> tmp.text[0];
			input >> tmp.text[1];
			input >> tmp.text[2];
			input >> tmp.text[3];
			input >> tmp.text[4];
			input >> tmp.text[5];
			tmp.textype = 6;
		}
		else if (tmp.textname == "model") {
			cout << "************" << endl;
			input >> tmp.name;

			for (int i = 0; i < 3; i++) {
				input >> tmp1;
				tmp.scale[i] = atof(tmp1.data());
			}

			for (int i = 0; i < 4; i++) {
				input >> tmp1;
				tmp.rotate[i] = atof(tmp1.data());
			}

			for (int i = 0; i < 3; i++) {
				input >> tmp1;
				tmp.trans[i] = atof(tmp1.data());
			}
			myobj.push_back(tmp);
			cout << myobj.size() << endl;
		}
	}
	/*glPushMatrix();
	read_light_file();
	for (int i = 0; i < myobj.size(); i++) {
		show_scene(i);
	}
	glPopMatrix();
	read_light_file();
	show_all();*/
}

void show_all(int i) {
	for (int i = 0; i < myobj.size(); i++) {
		mapping(i);
		glPushMatrix();
		glTranslated(myobj[i].trans[0], myobj[i].trans[1], myobj[i].trans[2]);
		glRotated(myobj[i].rotate[0], myobj[i].rotate[1], myobj[i].rotate[2], myobj[i].rotate[3]);
		glScaled(myobj[i].scale[0], myobj[i].scale[1], myobj[i].scale[2]);
		show(myobj[i].name, myobj[i].textype);
		glPopMatrix();
	}
}

void show_scene(int level) {
	for (int i = 0; i < myobj.size(); i++) {
		if (myobj[i].name != "Mirror.obj") {
			mapping(i);
			glPushMatrix();
			glTranslated(myobj[i].trans[0], myobj[i].trans[1], myobj[i].trans[2]);
			glRotated(myobj[i].rotate[0], myobj[i].rotate[1], myobj[i].rotate[2], myobj[i].rotate[3]);
			glScaled(myobj[i].scale[0], myobj[i].scale[1], myobj[i].scale[2]);
			show(myobj[i].name, myobj[i].textype);
			glPopMatrix();
		}
	}
}

void show_mirror() {
	glColorMask(0, 0, 0, 0);
	for (int i = 0; i < myobj.size(); i++) {
		if (myobj[i].name == "Mirror.obj") {
			mapping(i);
			glPushMatrix();
			glTranslated(myobj[i].trans[0], myobj[i].trans[1], myobj[i].trans[2]);
			glRotated(myobj[i].rotate[0], myobj[i].rotate[1], myobj[i].rotate[2], myobj[i].rotate[3]);
			glScaled(myobj[i].scale[0], myobj[i].scale[1], myobj[i].scale[2]);
			show(myobj[i].name, myobj[i].textype);
			glPopMatrix();
		}
	}
	glColorMask(1, 1, 1, 1);
}

void show_reflect(int level) {
	double	tmp, x;
	for (int i = 0; i < myobj.size(); i++) {
		if (myobj[i].name != "Mirror.obj") {

			mapping(i);
			glPushMatrix();

			x = myobj[i].trans[0];
			if (level == 1)tmp = -80 - x;
			if (level == 2)tmp = -160 + x;
			if (level == 3)tmp = -240 - x;
			if (level == 4)tmp = -320 + x;

			glTranslated(tmp, myobj[i].trans[1], myobj[i].trans[2]);
			if (level % 2) {
				glRotated(90, 0, 0, 1);
				glScaled(1, (-1), 1);
				glRotated(-90, 0, 0, 1);
			}
			glRotated(myobj[i].rotate[0], myobj[i].rotate[1], myobj[i].rotate[2], myobj[i].rotate[3]);
			glScaled(myobj[i].scale[0], myobj[i].scale[1], myobj[i].scale[2]);
			show(myobj[i].name, myobj[i].textype);
			glPopMatrix();
		}
	}
}

void mapping(int i){

	if (myobj[i].textype == 1) {
		singletexture();
	}
	else if (myobj[i].textype == 2) {
		mutitexture();
	}
	else if (myobj[i].textype == 6) {
		cubetexture();
	}
}

void read_view_file() {
	string tmp;
	ifstream input("CornellBox.view");
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
			target[0] = cenx;
			input >> tmp;
			ceny = atof(tmp.data());
			target[1] = ceny;
			input >> tmp;
			cenz = atof(tmp.data());
			target[2] = cenz;
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
	ifstream input("CornellBox.light");
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
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_MODELVIEW);
	glDepthFunc(GL_LEQUAL);                    // The Type Of Depth Test To Do
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);//這行把畫面清成黑色並且清除z buffer

													   // viewport transformation
	glViewport(viewportx, viewporty, windowSize[0], windowSize[1]);

	// projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (GLfloat)windowSize[0] / (GLfloat)windowSize[1], near, far);
	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	if (first == 0) {
		read_scene_input();
		first = 1;
	}

	gluLookAt(eyex, eyey, eyez,
			  cenx, ceny, cenz,
			  upx, upy, upz);
	read_light_file();

	right_vector(target[0], target[1], target[2]);
	set_stncil(0);
	accum_scene();
	accum_reflect();
		

	glDisable(GL_MODELVIEW);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);

	glutSwapBuffers();
}

void show(string obj_in, int x) {
	object = new mesh(obj_in.data());
	int lastMaterial = -1;
	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.5f);
	glEnable(GL_TEXTURE_2D);

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
		cout << "**********************" << endl;
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
	case '1':
		cenx = -10;
		ceny = 12;
		cenz = 0;
		glutPostRedisplay();
		break;
	case '2':
		cenx = -50;
		ceny = 12;
		cenz = 0;
		glutPostRedisplay();
		break;
	case '3':
		cenx = -400;
		ceny = 12;
		cenz = 0;
		glutPostRedisplay();
		break;
	}
}

void set_stncil(int i){
	glClearStencil(0x00);
	glStencilMask(0xFF);

	glDisable(GL_DEPTH_TEST);
	glStencilFunc(GL_ALWAYS, 1, 0xff);
	glStencilOp(GL_KEEP, GL_KEEP, GL_INCR);
	show_mirror();
	glEnable(GL_DEPTH_TEST);

	glStencilFunc(GL_EQUAL, 1, 0xFF);
	glStencilOp(GL_KEEP, GL_KEEP, GL_DECR);
	show_scene(0);
}

void right_vector(double tarx, double tary, double tarz){
	double vx = eyex - tarx;
	double vy = eyey - tary;
	double vz = eyez - tarz;
	right_v[0] = vy * upz - vz * upy;
	right_v[1] = vz * upx - vx * upz;
	right_v[2] = vx * upy - vy * upx;
}

void accum_scene() {
	double factor;
	double up_fac[3];
	factor = pow((upx*upx) + (upy*upy) + (upz*upz), 1 / 3);
	up_fac[0] = upx / factor;
	up_fac[1] = upy / factor;
	up_fac[2] = upz / factor;

	for (int i = 0; i < 8; i++) {
		for (int j = 0; j < 3; j++)
			jitter[j] = (right_v[j] * cos(45 * i*pi / 180) + up_fac[j] * sin(45 * i*pi / 180));

		factor = sqrt((jitter[0] * jitter[0]) + (jitter[1] * jitter[1])
			+ (jitter[2] * jitter[2]));
		for (int j = 0; j < 3; j++) jitter[j] /= factor * 4;
		cout << "*******" << jitter[0] << "*******" << jitter[1] << "*******" << jitter[2] << endl;

		glLoadIdentity();
		gluLookAt(eyex + jitter[0], eyey + jitter[1], eyez + jitter[2],
			cenx, ceny, cenz,
			upx, upy, upz);

		glStencilFunc(GL_EQUAL, 0, 0xFF);
		glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
		show_scene(0);
		glAccum(i ? GL_ACCUM : GL_LOAD, 1.0 / 8);
	}
	glAccum(GL_RETURN, 1.0);
}

void accum_reflect() {
	double factor;
	double up_fac[3];
	factor = pow((upx*upx) + (upy*upy) + (upz*upz), 1 / 3);
	up_fac[0] = upx / factor;
	up_fac[1] = upy / factor;
	up_fac[2] = upz / factor;

	for (int level = 1; level <= 4; level++) {
		for (int i = 0; i < 8; i++) {
			for (int j = 0; j < 3; j++)
				jitter[j] = (right_v[j] * cos(45 * i*pi / 180) + up_fac[j] * sin(45 * i*pi / 180));

			factor = sqrt((jitter[0] * jitter[0]) + (jitter[1] * jitter[1])
				+ (jitter[2] * jitter[2]));
			for (int j = 0; j < 3; j++) jitter[j] /= factor * 4;

			glLoadIdentity();
			gluLookAt(eyex + jitter[0], eyey + jitter[1], eyez + jitter[2],
					  cenx, ceny, cenz,
					  upx, upy, upz);
			if(level%2)glCullFace(GL_FRONT);
			else glCullFace(GL_BACK);
			glStencilFunc(GL_EQUAL, 1, 0xFF);
			glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);
			show_reflect(level);
			glAccum(i ? GL_ACCUM : GL_LOAD, 1.0 / 8);
		}
		glAccum(GL_RETURN, 1.0);
	}
}