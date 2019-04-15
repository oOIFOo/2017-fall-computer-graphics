#include "mesh.h"
#include "glut.h"
#include <fstream>
#include <cstdio>
#include <string>
#include <iostream>
#include <list>
#include <cmath>
using namespace std;
GLfloat light_specular[4] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_diffuse[4] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_ambient[4] = { 0.0, 0.0, 0.0, 1.0 };
GLfloat light_position[4] = { 150.0, 150.0, 150.0, 0.3 };
GLfloat ambient[3];

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
int quent = 100;

void light(int x);
void display();
void reshape(GLsizei, GLsizei);
void keyboardinput(unsigned char key, int x, int y);
void read_view_file();
void read_light_file();
void read_scene_input(int x);
void show(string obj_in);
void mouseinput(int x, int y);

int main(int argc, char** argv)
{
	object = new mesh("assassins-creed-altairx.obj");

	glutInit(&argc, argv);
	glutInitWindowSize(8787, 8787);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("Mesh Example");
	read_view_file();
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboardinput);
	//glutMouseFunc(mouseinput);
	glutMotionFunc(mouseinput);
	glutMainLoop();

	system("pause");
	return 0;
}

void read_scene_input(int x) {
	string tmp1, tmp2;
	int time = 0;
	ifstream input("scene2.scene");
	double scale[3];
	double rotate[4];
	while (!input.eof()) {
		input >> tmp1;
		if (tmp1 == "model") {
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

			if (first_in == 1) {
				trans[time][0] = trans_trans[0];
				trans[time][1] = trans_trans[1];
				trans[time][2] = trans_trans[2];
			}

			if (time + 1 == x) {
				trans[time][0] += mousex*eyex/5;
				trans[time][1] += mousey*eyey/5;
				trans[time][2] -= mousex*eyex/5;
			}


			glPushMatrix();
			glTranslated(trans[time][0], trans[time][1], trans[time][2]);
			glRotated(rotate[0], rotate[1], rotate[2], rotate[3]);
			glScaled(scale[0], scale[1], scale[2]);
			show(tmp2);
			glPopMatrix();
			time++;
		}
	}
//	glPopMatrix();
	mousepress = 0;
	first_in = 0;
}

void read_view_file() {
	string tmp;
	ifstream input("scene2.view");
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
	ifstream input("scene2.light");
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
	/*GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat light_ambient[] = { 0.0, 0.0, 0.0, 1.0 };
	GLfloat light_position[] = { 150.0, 150.0, 150.0, 1.0 };*/

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
	read_scene_input(num);
	glutSwapBuffers();
}

void show(string obj_in) {
	object = new mesh(obj_in.data());
	int lastMaterial = -1;
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

		glBegin(GL_TRIANGLES);
		for (size_t j = 0; j<3; ++j)
		{
			//textex corrd. object->tList[object->faceList[i][j].t].ptr
			glNormal3fv(object->nList[object->faceList[i][j].n].ptr);
			glVertex3fv(object->vList[object->faceList[i][j].v].ptr);
		}
		glEnd();
	}
}

void reshape(GLsizei w, GLsizei h)
{
	windowSize[0] = w;
	windowSize[1] = h;
}

void keyboardinput(unsigned char key, int x, int y) {
	vectorx = cenx - eyex; vectory = ceny - eyey; vectorz = cenz - eyez;
	double factor;
	factor = sqrt((vectorx*vectorx) + (vectory*vectory) + (vectorz*vectorz));
	vectorx = vectorx / factor; vectory = vectory / factor; vectorz = vectorz / factor;

	switch (key) {
	case 'w':
		glMatrixMode(GL_MODELVIEW);
		eyex += vectorx;
		eyey += vectory;
		eyez += vectorz;
		quent -= 10;
		if (quent <= 0) quent = 10;
		glutPostRedisplay();
		break;
	case 'a':
		glMatrixMode(GL_MODELVIEW);
		eyex = cos(-pi / 180)*eyex - sin(-pi / 180)*eyez;
		eyez = sin(-pi / 180)*eyex + cos(-pi / 180)*eyez;
		glutPostRedisplay();
		break;
	case 's':
		glMatrixMode(GL_MODELVIEW);
		eyex -= vectorx;
		eyey -= vectory;
		eyez -= vectorz;
		quent += 10;
		glutPostRedisplay();
		break;
	case 'd':
		glMatrixMode(GL_MODELVIEW);
		eyex = cos(pi / 180)*eyex - sin(pi / 180)*eyez;
		eyez = sin(pi / 180)*eyex + cos(pi / 180)*eyez;
		glutPostRedisplay();
		break;
	case '1':
		num = 1;
		pre_mousex = 0;
		pre_mousey = 0;
		glutPostRedisplay();
		break;
	case '2':
		num = 2;
		pre_mousex = 0;
		pre_mousey = 0;
		glutPostRedisplay();
		break;
	case '3':
		num = 3;
		pre_mousex = 0;
		pre_mousey = 0;
		glutPostRedisplay();
		break;
	case '4':
		num = 4;
		pre_mousex = 0;
		pre_mousey = 0;
		glutPostRedisplay();
		break;
	case '5':
		num = 5;
		pre_mousex = 0;
		pre_mousey = 0;
		glutPostRedisplay();
		break;
	case '6':
		num = 6;
		pre_mousex = 0;
		pre_mousey = 0;
		glutPostRedisplay();
		break;
	case '7':
		num = 7;
		pre_mousex = 0;
		pre_mousey = 0;
		glutPostRedisplay();
		break;
	case '8':
		num = 8;
		pre_mousex = 0;
		pre_mousey = 0;
		glutPostRedisplay();
		break;
	case '9':
		num = 9;
		pre_mousex = 0;
		pre_mousey = 0;
		glutPostRedisplay();
		break;
	}
}

void mouseinput(int x, int y) {
	/*switch (button) {
	case GLUT_LEFT_BUTTON:
		if (flag == 0) {
			pre_mousex = x;
			pre_mousey = y;
			flag = 1;
		}
		else if (flag == 1) {
			mousex = eyex*(x - pre_mousex) / quent;
			mousey = eyey*(pre_mousey - y) / quent;
			flag = 0;
			mousepress = 1;
			cout << eyex << "   " << eyey << endl;
			glutPostRedisplay();
			break;
		}
	}*/
	if (pre_mousex == 0) pre_mousex = x;
	if (pre_mousey == 0) pre_mousey = y;

	mousex = sin((x - pre_mousex) / quent);
	mousey = sin((pre_mousey - y) / quent);
	pre_mousex = x;
	pre_mousey = y;
	glutPostRedisplay();
}
