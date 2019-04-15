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
#include <glew.h>
#include "../SharedCode/ShaderLoader.h"

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
void singletexture();
void mapping(int i);
bool ShaderLoad(GLhandleARB programId, char* shaderSrc, GLenum shaderType);
void LoadShaders();
char *textFileRead(char *fn);
void printProgramInfoLog(GLuint obj);
void printShaderInfoLog(GLuint obj);

GLhandleARB MyShader;
int level = 0;

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitWindowSize(800, 600);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_SINGLE | GLUT_RGBA | GLUT_DEPTH);
	glutCreateWindow("Mesh Example");
	glewInit();
	read_view_file();
	LoadShaders();
	glutKeyboardFunc(keyboardinput);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutMainLoop();

	return 0;
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
	ifstream input("as3.scene");
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

void mapping(int i) {
	singletexture();
}

void read_view_file() {
	string tmp;
	ifstream input("as3.view");
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
	ifstream input("as3.light");
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
			light_specular[3] = 1.0; 
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

	if(first == 0) read_light_file();

	glUseProgram(MyShader);

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

void keyboardinput(unsigned char key, int x, int y) {
	float tmpx, tmpy, tmpz;
	vectorx = cenx - eyex; vectory = ceny - eyey; vectorz = cenz - eyez;
	double factor;
	factor = sqrt((vectorx*vectorx) + (vectory*vectory) + (vectorz*vectorz));
	vectorx = vectorx / factor; vectory = vectory / factor; vectorz = vectorz / factor;

	switch (key) {
	case 'w':
		glMatrixMode(GL_MODELVIEW);
		eyex += vectorx*0.1;
		eyey += vectory*0.1;
		eyez += vectorz*0.1;
		break;
	case 'a':
		glMatrixMode(GL_MODELVIEW);
		tmpx = cos(15 * pi / 180)*(eyex - cenx) - sin(15 * pi / 180)*(eyez - cenz) + cenx;
		tmpz = sin(15 * pi / 180)*(eyex - cenx) + cos(15 * pi / 180)*(eyez - cenz) + cenz;
		eyex = tmpx;
		eyez = tmpz;
		break;
	case 's':
		glMatrixMode(GL_MODELVIEW);
		eyex -= vectorx*0.1;
		eyey -= vectory*0.1;
		eyez -= vectorz*0.1;
		break;
	case 'd':
		glMatrixMode(GL_MODELVIEW);
		tmpx = cos(-15 * pi / 180)*(eyex - cenx) - sin(-15 * pi / 180)*(eyez - cenz) + cenx;
		tmpz = sin(-15 * pi / 180)*(eyex - cenx) + cos(-15 * pi / 180)*(eyez - cenz) + cenz;
		eyex = tmpx;
		eyez = tmpz;
		break;
	case 'r':
		glMatrixMode(GL_MODELVIEW);
		light_position[1] += 5;
		light(0);
		break;
	case 'f':
		glMatrixMode(GL_MODELVIEW);
		light_position[1] -= 5;
		light(0);
		break;
	case '0':
		glMatrixMode(GL_MODELVIEW);
		level = 0;
		glUseProgram(MyShader);
		glUniform1i(glGetUniformLocation(MyShader, "level"), level);
		glUseProgram(0);
		break;
	case '1':
		glMatrixMode(GL_MODELVIEW);
		level = 1;
		glUseProgram(MyShader);
		glUniform1i(glGetUniformLocation(MyShader, "level"), level);
		glUseProgram(0);
		break;
	case '2':
		glMatrixMode(GL_MODELVIEW);
		level = 2;
		glUseProgram(MyShader);
		glUniform1i(glGetUniformLocation(MyShader, "level"), level);
		glUseProgram(0);
		break;
	}

	glutPostRedisplay();
}

void LoadShaders()
{
	MyShader = glCreateProgram();
	if (MyShader != 0)
	{
		ShaderLoad(MyShader, "../include/PhongShading.vert", GL_VERTEX_SHADER);
		ShaderLoad(MyShader, "../include/PhongShading.geom", GL_GEOMETRY_SHADER);
		ShaderLoad(MyShader, "../include/PhongShading.frag", GL_FRAGMENT_SHADER);
	}
}

bool ShaderLoad(GLhandleARB programId, char* shaderSrc, GLenum shaderType)
{
	FILE *fp;
	GLhandleARB h_shader;
	GLcharARB *shader_string;
	GLint str_length, maxLength;
	GLint isCompiled = GL_FALSE, isLinked = GL_FALSE;
	GLcharARB *pInfoLog;

	// open the file of shader source code
	if ((fp = fopen(shaderSrc, "r")) == NULL)
	{
		fprintf(stderr, "Error : Failed to read the OpenGL shader source \"%s\".\n", shaderSrc);
		return false;
	}

	// allocate memory for program string and load it.
	shader_string = (GLcharARB*)malloc(sizeof(GLcharARB) * 65536);
	str_length = (GLint)fread(shader_string, 1, 65536, fp);
	fclose(fp);

	// Create and load shader string.
	h_shader = glCreateShader(shaderType);
	if (h_shader == 0)
	{
		fprintf(stderr, "Error : Failed to create OpenGL shader object \"%s\".\n", shaderSrc);
		return false;
	}
	glShaderSource(h_shader, 1, (const GLcharARB**)&shader_string, &str_length);
	free(shader_string);

	// Compile the vertex shader, print out the compiler log message.
	glCompileShader(h_shader);

	// get compile state information
	glGetObjectParameterivARB(h_shader, GL_OBJECT_COMPILE_STATUS_ARB, &isCompiled);

	if (!isCompiled)
	{
		fprintf(stderr, "Error : Failed to compile OpenGL shader source \"%s\".\n", shaderSrc);
		glGetObjectParameterivARB(h_shader, GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);
		pInfoLog = (GLcharARB *)malloc(maxLength * sizeof(GLcharARB));
		glGetInfoLogARB(h_shader, maxLength, &str_length, pInfoLog);
		fprintf(stderr, "%s\n", pInfoLog);
		free(pInfoLog);
		return false;
	}
	glAttachShader(programId, h_shader);

	// delete the shader object, since we have attached it with the program object.
	glDeleteShader(h_shader);

	// Link the program and print out the linker log message
	glLinkProgram(programId);
	glGetObjectParameterivARB(programId, GL_OBJECT_LINK_STATUS_ARB, &isLinked);

	if (!isLinked)
	{
		fprintf(stderr, "Error : Failed to link OpenGL shader \"%s\".\n", shaderSrc);
		glGetObjectParameterivARB(programId, GL_OBJECT_INFO_LOG_LENGTH_ARB, &maxLength);
		pInfoLog = (GLcharARB *)malloc(maxLength * sizeof(GLcharARB));
		glGetInfoLogARB(programId, maxLength, &str_length, pInfoLog);
		fprintf(stderr, "%s\n", pInfoLog);
		free(pInfoLog);
		return false;
	}

	return true;
}

char *textFileRead(char *fn)
{
	FILE *fp;
	char *content = NULL;

	int count = 0;

	if (fn != NULL) {

		fp = fopen(fn, "rt");

		if (fp != NULL) {

			fseek(fp, 0, SEEK_END);
			count = ftell(fp);
			rewind(fp);

			if (count > 0) {
				content = (char *)malloc(sizeof(char) * (count + 1));
				count = fread(content, sizeof(char), count, fp);
				content[count] = '\0';
			}
			fclose(fp);

		}
	}

	return content;
}

void printProgramInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten = 0;
	char *infoLog;
	glGetProgramiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetProgramInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("printProgramInfoLog: %s\n", infoLog);
		free(infoLog);
	}
	else {
		printf("Program Info Log: OK\n");
	}
}

void printShaderInfoLog(GLuint obj)
{
	int infologLength = 0;
	int charsWritten = 0;
	char *infoLog;
	glGetShaderiv(obj, GL_INFO_LOG_LENGTH, &infologLength);
	if (infologLength > 0)
	{
		infoLog = (char *)malloc(infologLength);
		glGetShaderInfoLog(obj, infologLength, &charsWritten, infoLog);
		printf("printShaderInfoLog: %s\n", infoLog);
		free(infoLog);
	}
	else {
		printf("Shader Info Log: OK\n");
	}
}
