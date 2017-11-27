/*
---------------------------------------------------------------------------------------------
Nome: Frederico Balcão
Data: 02-02-2017
Curso: Engenharia Informática
U.C: Computação Gráfica
Ano/Semestre: 2ºAno/1ºSemestre
Cubo - OpenGL
---------------------------------------------------------------------------------------------
*/


#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <GL/glut.h>
#include <stdbool.h>

#define CELL_SIZE 1.2 
#define TAB_SIZE 13 

GLfloat vertices[][3] = { 
	{ -1.0,-1.0,-1.0 },
	{ 1.0,-1.0,-1.0 },
	{ 1.0,1.0,-1.0 },
	{ -1.0,1.0,-1.0 },
	{ -1.0,-1.0,1.0 },
	{ 1.0,-1.0,1.0 },
	{ 1.0,1.0,1.0 },
	{ -1.0,1.0,1.0 } 
};

GLfloat normals[][3] = { 
	{ -1.0,-1.0,-1.0 },
	{ 1.0,-1.0,-1.0 },
	{ 1.0,1.0,-1.0 },
	{ -1.0,1.0,-1.0 },
	{ -1.0,-1.0,1.0 },
	{ 1.0,-1.0,1.0 },
	{ 1.0,1.0,1.0 },
	{ -1.0,1.0,1.0 } 
};

GLfloat colors[][3] = { 
	{ 0.0,0.0,0.0 },
	{ 1.0,0.0,0.0 },
	{ 1.0,1.0,0.0 },
	{ 0.0,1.0,0.0 },
	{ 0.0,0.0,1.0 },
	{ 1.0,0.0,1.0 },
	{ 1.0,1.0,1.0 },
	{ 0.0,1.0,1.0 } 
};

GLfloat verticesOctaedro[][3] = { 
	{ -0.5,-0.5,-0.5 },  //0
	{ 0.5,-0.5,-0.5 },   //1
	{ 0.5,-0.5,0.5 },    //2
	{ -0.5,-0.5,0.5 },   //3
	{ 0.0,0.0,0.0 },     //4
	{ 0.0,-1.0,0.0 }     //5
};

GLfloat normalsOctaedro[][3] = {
	{ -0.5,-0.5,-0.5 },  //0
	{ 0.5,-0.5,-0.5 },   //1
	{ 0.5,-0.5,0.5 },    //2
	{ -0.5,-0.5,0.5 },   //3
	{ 0.0,0.0,0.0 },     //4
	{ 0.0,-1.0,0.0 }     //5
};

typedef struct cubo * Cubo;
struct cubo {
	float x, y, z;
	float scale;
	float xRot, yRot;   // Rotação necessária em x e em y em graus multiplos de 90
	float xRoti, yRoti; // Rotação actual em x e em y até atingir xRot e yRot, respectivamente
	float zRot, zRoti; 
						// Matriz de rotação que define a posição actual do cubo
	GLfloat m[16];

	float r, g, b;
};

typedef struct octaedro *Octaedro;
struct octaedro {
	float x, y, z;
	float scale;
	float xRot, yRot;
	float xRoti, yRoti;
	float zRot, zRoti;

	GLfloat m[16];

	float r, g, b;
};

Cubo cubo;     // cubo actual 
Cubo *aCubos;  // array de cubos
Octaedro *aOct; //array de octaedros

//para detetar colisao
Cubo *cuboMov;

int nCubos = 4;  
int nOctaedros = 4;
//int nPassos = 0;

bool camPanMode = false;
bool camZoomMode = false;
float distCamara = 4; 
float dDistCamara = 0.5; 
float alfaLongitude = 0;
float alfaLatitude = 10;

float xRato, yRato;   // guarda a última posição do rato (para calcular o deslocamento na Lat. e Long.)
float angRato = 0.40;  // precisão do rato: número de angulos por pixel. 


int timeUpdate = 20;     // intervalo de tempo (ms) para o update
float velRot = 500.0;     // velocidade de rotação dos cubos em graus/s
float angRotMax = 90;

int nUpdate = 0;           // debug

float dAlfa = 5.0;
static GLfloat theta[] = { 0.0,0.0,0.0 };

void polygon(int a, int b, int c, int d)
{
	/* draw a polygon via list of vertices */

	glBegin(GL_POLYGON);
	//glColor3fv(colors[a]);
	glNormal3fv(normals[a]);
	glVertex3fv(vertices[a]);
	//glColor3fv(colors[b]);
	glNormal3fv(normals[b]);
	glVertex3fv(vertices[b]);
	//glColor3fv(colors[c]);
	glNormal3fv(normals[c]);
	glVertex3fv(vertices[c]);
	//glColor3fv(colors[d]);
	glNormal3fv(normals[d]);
	glVertex3fv(vertices[d]);
	glEnd();
}

void lineloop(int a, int b, int c, int d)
{

	/* draw a lineloop via list of vertices */
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_LINE_LOOP);
	glVertex3fv(vertices[a]);
	glVertex3fv(vertices[b]);
	glVertex3fv(vertices[c]);
	glVertex3fv(vertices[d]);
	glEnd();
}

void colorcube(void)
{
	polygon(0, 3, 2, 1);
	polygon(2, 3, 7, 6);
	polygon(0, 4, 7, 3);
	polygon(1, 2, 6, 5);
	polygon(4, 5, 6, 7);
	polygon(0, 1, 5, 4);

	lineloop(0, 3, 2, 1);
	lineloop(2, 3, 7, 6);
	lineloop(0, 4, 7, 3);
	lineloop(1, 2, 6, 5);
	lineloop(4, 5, 6, 7);
	lineloop(0, 1, 5, 4);
}

void polygon_Oct(int a, int b, int c)
{
	glBegin(GL_POLYGON);

	glNormal3fv(normalsOctaedro[a]);
	glVertex3fv(verticesOctaedro[a]);

	glNormal3fv(normalsOctaedro[b]);
	glVertex3fv(verticesOctaedro[b]);

	glNormal3fv(normalsOctaedro[c]);
	glVertex3fv(verticesOctaedro[c]);
	glEnd();
}

void lineloop_Oct(int a, int b, int c)
{
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_LINE_LOOP);
	glVertex3fv(verticesOctaedro[a]);
	glVertex3fv(verticesOctaedro[b]);
	glVertex3fv(verticesOctaedro[c]);
	glEnd();
}

void octaedro() {
	//triangulo1 cima
	polygon_Oct(0, 1, 4);
	polygon_Oct(1, 2, 4);
	polygon_Oct(2, 3, 4);
	polygon_Oct(3, 0, 4);
	//triangulo1 baixo
	polygon_Oct(0, 1, 5);
	polygon_Oct(1, 2, 5);
	polygon_Oct(2, 3, 5);
	polygon_Oct(3, 0, 5);
	

	//triangulo2 cima
	lineloop_Oct(0, 1, 4);
	lineloop_Oct(1, 2, 4);
	lineloop_Oct(2, 3, 4);
	lineloop_Oct(3, 0, 4);
	//triangulo2 baixo
	lineloop_Oct(0, 1, 5);
	lineloop_Oct(1, 2, 5);
	lineloop_Oct(2, 3, 5);
	lineloop_Oct(3, 0, 5);
}

Cubo criaCubo(float x, float y, float z, float scale)
{
	Cubo c = (Cubo)malloc(sizeof(*c));
	c->x = x;
	c->y = y;
	c->z = z;
	c->scale = scale;
	c->xRot = c->yRot = c->zRot = c->xRoti = c->yRoti = c->zRoti = 0.0;

	c->r = 1;
	c->g = 1;
	c->b = 0;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glGetFloatv(GL_MODELVIEW_MATRIX, c->m);
	glPopMatrix();

	return c;
}

Octaedro criaOctaedro(float x, float y, float z, float scale)
{
	Octaedro o = (Octaedro)malloc(sizeof(*o));
	o->x = x;
	o->y = y;
	o->z = z;
	o->scale = scale;
	o->xRot = o->yRot = o->xRoti = o->yRoti = 0.0;

	o->r = 0;
	o->g = 1;
	o->b = 0;

	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glGetFloatv(GL_MODELVIEW_MATRIX, o->m);
	glPopMatrix();

	return o;
}

void actualizaRotOct(Octaedro c, GLfloat ang, GLfloat x, GLfloat y, GLfloat z)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glRotatef(ang, x, y, z);
	glMultMatrixf(c->m);
	glGetFloatv(GL_MODELVIEW_MATRIX, c->m);
	glPopMatrix();
}

void actualizaRotCubo(Cubo c, GLfloat ang, GLfloat x, GLfloat y, GLfloat z)
{
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
	glLoadIdentity();
	glRotatef(ang, x, y, z);
	glMultMatrixf(c->m);
	glGetFloatv(GL_MODELVIEW_MATRIX, c->m);
	glPopMatrix();
}

void rodaCuboCima(Cubo c)
{
	if (c->zRot == c->zRoti)     
		c->xRot += -angRotMax;
}

void rodaCuboBaixo(Cubo c)
{
	if (c->zRot == c->zRoti)    
		c->xRot += angRotMax;
}

void rodaCuboEsq(Cubo c)
{
	if (c->xRot == c->xRoti)     
		c->zRot += -angRotMax;
}

void rodaCuboDir(Cubo c)
{
	if (c->xRot == c->xRoti)     
		c->zRot += angRotMax;
}

void rodaOctaedro(Octaedro o) {
	if (o->yRot == o->yRoti) {
		o->xRot += angRotMax;
	}
}

void select(Cubo c)
{
	c->scale *= 1.4;
}

void unselect(Cubo c)
{
	c->scale /= 1.4;
}

void desenhaOctaedro(Octaedro o) {
	if (o == NULL) return;

	glPushMatrix();

	glTranslatef(o->x, o->y, o->z);
	glScalef(o->scale, o->scale, o->scale);

	glRotatef(o->xRoti, 0, 1, 0);
	glRotatef(o->yRoti*-1, 0, 0, 1);

	glMultMatrixf(o->m);

	glColor3f(o->r, o->g, o->b);
	octaedro();

	glPopMatrix();
}

void desenhaTabuleiro()
{
	int i, j;


	glPushMatrix();

	glScalef(0.3, 0.3, 0.3);
	if (TAB_SIZE % 2 != 0) {
		glTranslatef(-TAB_SIZE + 1, 0, -TAB_SIZE + 1);
	}
	else
	{
		glTranslatef(-TAB_SIZE, 0, -TAB_SIZE);
	}

	for (i = 0; i < TAB_SIZE; i++)
		for (j = 0; j < TAB_SIZE; j++) {
			glPushMatrix();
			glTranslatef(j*2.0, -2.0, i*2.0);
			glScalef(0.95, 0.95, 0.95);
			glColor3f(0.0, 1.0, 0.0);
			colorcube();
			glPopMatrix();
		}

	glPopMatrix();
}
void desenhaCubo(Cubo c)
{

	if (c == NULL) return;

	glPushMatrix();

	glTranslatef(c->x, c->y, c->z);
	glScalef(c->scale, c->scale, c->scale);

	glRotatef(c->xRoti, 1, 0, 0);
	//glRotatef(c->yRoti, 0, 1, 0);
	glRotatef(c->zRoti, 0, 0, 1);

	glMultMatrixf(c->m);

	glColor3f(c->r, c->g, c->b);
	colorcube();

	glPopMatrix();
}

void updateCubo(Cubo c, int t)
{
	float angMov = velRot*(t / 1000.0);

	if (c->xRot != 0 || c->xRoti != 0) {
		//DOWN
		if (c->xRot > c->xRoti) {
			//if (c->z < 0.5*-CELL_SIZE*((TAB_SIZE / CELL_SIZE) / 2)) {
			c->xRoti += angMov; c->z -= (0.5*CELL_SIZE) / (angRotMax / angMov) + 0.0007;
			//}
		}
		//UP
		if (c->xRot < c->xRoti) {
			//if (c->z < 0.5*CELL_SIZE*((TAB_SIZE / CELL_SIZE) / 2)) {
			c->xRoti -= angMov; c->z += (0.5*CELL_SIZE) / (angRotMax / angMov) + 0.0007;
			//}
		}
		//COLOCA A ROTAÇÃO DO EIXO A ZERO
		if (fabs(c->xRot - c->xRoti) < angMov) {
			actualizaRotCubo(c, c->xRot, 1, 0, 0);
			c->xRot = c->xRoti = 0.0;
		}
	}
	//LEFT
	if (c->zRot != 0 || c->zRoti != 0) {
		//if (c->x > 0.5*CELL_SIZE*((TAB_SIZE / CELL_SIZE) / 2)) {
		if (c->zRot > c->zRoti) {
			c->zRoti += angMov; c->x += (0.5*CELL_SIZE) / (angRotMax / angMov) + 0.0007;
			//}
		}
		//RIGHT
		if (c->zRot < c->zRoti) {
			//if (c->x > 0.5*-CELL_SIZE*((TAB_SIZE / CELL_SIZE) / 2)) {
			c->zRoti -= angMov; c->x -= (0.5*CELL_SIZE) / (angRotMax / angMov) + 0.0007;
			//}
		}

		//COLOCA A ROTAÇÃO DO EIXO A ZERO
		if (fabs(c->zRot - c->zRoti) < angMov) {
			actualizaRotCubo(c, c->zRot, 0, 0, 1);
			c->zRot = c->zRoti = 0.0;
		}
	}
}


void updateOctaedro(Octaedro o, int t) {
	float angMov = velRot*(t / 1000.0);
	if (o->xRot != 0 || o->xRoti != 0) {
		if (o->xRot > o->xRoti) {
			o->xRoti += angMov; 
		}
		if (o->xRot < o->xRoti) { 
			o->xRoti -= angMov; 
		}
		if (fabs(o->xRot - o->xRoti) < angMov) {
			actualizaRotOct((Octaedro)o, o->xRot, 1, 0, 0);
			o->xRot = o->xRoti = 0.0;
		}
	}
}

void display(void) 
{
	int i;
	float x, y, z;

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glShadeModel(GL_FLAT);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();


	x = distCamara * sin(alfaLongitude*3.14 / 180) * cos(alfaLatitude*3.14 / 180);
	z = distCamara * cos(alfaLongitude*3.14 / 180) * cos(alfaLatitude*3.14 / 180);
	y = distCamara * sin(alfaLatitude*3.14 / 180);
	gluLookAt(x, y, z, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);


	desenhaTabuleiro();

	for (int i = 0; i < nOctaedros; i++) {
		desenhaOctaedro(aOct[i]);
	}

	for (int i = 0; i < 2; i++)
	{
		desenhaCubo(cuboMov[i]);
	}

	for (i = 0; i < nCubos; i++)
		desenhaCubo(aCubos[i]);

	glFlush();

	glutSwapBuffers();
}

bool colisao(Cubo a, Cubo b)
{
	if ((a->x >= b->x - 0.4 && a->x <= b->x + 0.4) && (a->z >= b->z - 0.4 && a->z <= b->z + 0.4))
	{
		return true;
	}
	else return false;
}

void animaCubos(Cubo x, Cubo y)
{

	static int dir = 1;

	x->z += 0.1*CELL_SIZE*dir;
	y->z -= 0.1*CELL_SIZE*dir;

	for (int i = 0; i < nCubos; i++)
	{
		if (colisao(x, aCubos[i]))
		{
			aCubos[i]->z += 0.1*CELL_SIZE*dir;
		}

		if (colisao(y, aCubos[i]))
		{
			aCubos[i]->z -= 0.1*CELL_SIZE*dir;
		}
	}

	//distancia de movimentação entre os dois cubos
	if (x->z > 2.5*CELL_SIZE || x->z < -2.5*CELL_SIZE)
	{
		dir *= -1;
	}
}

void myReshape(int w, int h)
{
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w <= h)
		glOrtho(-2.0, 2.0, -2.0 * (GLfloat)h / (GLfloat)w,
			2.0 * (GLfloat)h / (GLfloat)w, -20.0, 20.0);
	else
		glOrtho(-2.0 * (GLfloat)w / (GLfloat)h,
			2.0 * (GLfloat)w / (GLfloat)h, -2.0, 2.0, -10.0, 10.0);
	glMatrixMode(GL_MODELVIEW);
}

void update(int v)
// timer callback
{
	int i;

	for (i = 0; i < nCubos; i++)
		updateCubo(aCubos[i], v);
		animaCubos(cuboMov[0], cuboMov[1]);

	for (int i = 0; i < nOctaedros; i++) {
		rodaOctaedro(aOct[i]);
		updateOctaedro(aOct[i],v);
	}

	glutPostRedisplay();
	glutTimerFunc(v, update, v);
}

void teclas(unsigned char key, int x, int y)
{
	switch (key) {
	case 'q':
	case 'Q':
		exit(0);
		break;
	case '+':
		distCamara -= dDistCamara; break;
	case '-':
		distCamara += dDistCamara; break;
	}

	//selectCubo(key - '0');

	glutPostRedisplay();
}

void teclasEspeciais(int key, int x, int y)
{
	switch (key) {
	case GLUT_KEY_UP:
	{
		for (int i = 0; i < nCubos; i++)
		{
			aCubos[i]->z -= 0.5*CELL_SIZE;
			for (int j = 0; j < 4; j++) {
				//verificação da colisão (dos cubos (objetos moveis) e dos octaedros (objetos fixos))
				if (colisao(aCubos[i], (Cubo)aOct[j]) || (aCubos[i]->z <= (float)((TAB_SIZE / 2) + 1)*0.5*CELL_SIZE*-1)) 
				{
					aCubos[i]->z += 0.5*CELL_SIZE;
					return;
				}
			}
			aCubos[i]->z += 0.5*CELL_SIZE;
		}

		for (int i = 0; i < nCubos; i++)
		{
			//if (aCubos[i]->z < 0.5*CELL_SIZE*((TAB_SIZE / CELL_SIZE) / 2)) {
			rodaCuboCima(aCubos[i]);
			//}
		}
		break;
	}
	case GLUT_KEY_DOWN:
	{
		for (int i = 0; i < nCubos; i++)
		{
				aCubos[i]->z += 0.5*CELL_SIZE;
				for (int j = 0; j < 4; j++) {
					//verificação da colisão (dos cubos moveis e dos octaedros)
					if (colisao(aCubos[i], (Cubo)aOct[j]) || (aCubos[i]->z >= (float)((TAB_SIZE / 2) + 1)*0.5*CELL_SIZE))
					{
						aCubos[i]->z -= 0.5*CELL_SIZE;
						return;
					}
				}
				aCubos[i]->z -= 0.5*CELL_SIZE;
			}

		for (int i = 0; i < nCubos; i++)
		{
			//if (aCubos[i]->z < 0.5*-CELL_SIZE*((TAB_SIZE / CELL_SIZE) / 2)) {
				rodaCuboBaixo(aCubos[i]);
			//}
		}
		break;
	}
	case GLUT_KEY_LEFT:
	{
		for (int i = nCubos - 1; i >= 0; i--)
		{
				aCubos[i]->x -= 0.5*CELL_SIZE;
				for (int j = 0; j < 4; j++) {
					//verificação da colisão (dos cubos moveis e dos octaedros)
					if (colisao(aCubos[i], (Cubo)aOct[j]) || (aCubos[i]->x <= (float)((TAB_SIZE / 2) + 1)*0.5*CELL_SIZE*-1))
					{
						aCubos[i]->x += 0.5*CELL_SIZE;
						return;
					}
				}
				aCubos[i]->x += 0.5*CELL_SIZE;
			}

		for (int i = 0; i < nCubos; i++)
		{
			//if (aCubos[i]->x > 0.5*CELL_SIZE*((TAB_SIZE / CELL_SIZE) / 2)) {
				rodaCuboEsq(aCubos[i]);
			//}
		}
		break;
	}
	case GLUT_KEY_RIGHT:
	{
		for (int i = nCubos - 1; i >= 0; i--)
		{
			aCubos[i]->x += 0.5*CELL_SIZE;
			for (int j = 0; j < 4; j++) {
				//verificação da colisão (dos cubos moveis e dos octaedros)
				if (colisao(aCubos[i], (Cubo)aOct[j]) || (aCubos[i]->x >= (float)((TAB_SIZE / 2) + 1)*0.5*CELL_SIZE))
				{
					aCubos[i]->x -= 0.5*CELL_SIZE;
					return;
				}
			}
			aCubos[i]->x -= 0.5*CELL_SIZE;
		}

		for (int i = 0; i < nCubos; i++)
		{
			//if (aCubos[i]->x > 0.5*-CELL_SIZE*((TAB_SIZE / CELL_SIZE) / 2)) {
				rodaCuboDir(aCubos[i]);
			//}
		}
		break;
	}
	}
	glutPostRedisplay();
}

void myInit()
{
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  //Correção de perspectiva. Vertices mais definidos

	int i;
	float entreCubos = CELL_SIZE;  //distancia entre os cubos
	float tamanhoCubo = 0.3;	   //tamanho da cada cubo
	float inicio = -(nCubos / 2) * entreCubos;

	if (nCubos % 2 == 0) // numero par de cubos
		inicio += (entreCubos / 2);


	aCubos = (Cubo*)malloc(sizeof(Cubo)*nCubos);
	for (i = 0; i < nCubos; i++) {
		aCubos[i] = criaCubo(inicio + (i*entreCubos), 0.0, 0.0, tamanhoCubo);
		aCubos[i]->r = ((float)rand()) / RAND_MAX;
		aCubos[i]->g = ((float)rand()) / RAND_MAX;
		aCubos[i]->b = ((float)rand()) / RAND_MAX;
	}

	aOct = (Octaedro*)malloc(sizeof(Octaedro)*nOctaedros);
	aOct[0] = criaOctaedro(-2.0*entreCubos, tamanhoCubo, -2.0*entreCubos, tamanhoCubo*2);
	aOct[0]->r = 0.0;
	aOct[0]->g = 0.0;
	aOct[0]->b = 1.0;


	aOct[1] = criaOctaedro(2.0*entreCubos, tamanhoCubo, 2.0*entreCubos, tamanhoCubo*2);
	aOct[1]->r = 0.0;
	aOct[1]->g = 0.0;
	aOct[1]->b = 1.0;

	aOct[2] = criaOctaedro(-2.0*entreCubos, tamanhoCubo, 2.0*entreCubos, tamanhoCubo*2);
	aOct[2]->r = 1.0;
	aOct[2]->g = 0.0;
	aOct[2]->b = 0.0;

	aOct[3] = criaOctaedro(2.0*entreCubos, tamanhoCubo, -2.0*entreCubos, tamanhoCubo*2);
	aOct[3]->r = 1.0;
	aOct[3]->g = 0.0;
	aOct[3]->b = 0.0;

	cuboMov = (Cubo*)malloc(sizeof(Cubo) * 2);
	cuboMov[0] = criaCubo(-2.5*entreCubos, 0.0, -2.5*entreCubos, tamanhoCubo);
	cuboMov[0]->r = 1;
	cuboMov[0]->g = 1;
	cuboMov[0]->b = 0;

	cuboMov[1] = criaCubo(2.5*entreCubos, 0.0, 2.5*entreCubos, tamanhoCubo);
	cuboMov[1]->r = 1;
	cuboMov[1]->g = 1;
	cuboMov[1]->b = 0;
}

void rato(GLint button, GLint state, GLint x, GLint y)
{
	xRato = x;
	yRato = y;

	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN)
	{
		camPanMode = true;
	}

	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
	{
		camPanMode = false;
	}

	if (button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN)
	{
		camZoomMode = true;
	}


	if (button == GLUT_RIGHT_BUTTON && state == GLUT_UP)
	{
		camZoomMode = false;
	}
}

void moveRatoPress(int x, int y)
{

	if (camPanMode == true)
	{
		alfaLongitude += (x - xRato) * angRato;

		if (alfaLatitude >= 9 && alfaLatitude < 51)
		{
			alfaLatitude -= (y - yRato) * angRato;
		}
		else if (alfaLatitude < 9) alfaLatitude = 9;
		else if (alfaLatitude > 50) alfaLatitude = 50;


		xRato = x;
		yRato = y;

		if (alfaLongitude >= 360) alfaLongitude -= 360;
		if (alfaLongitude < 0) alfaLongitude += 360;
		if (alfaLatitude >= 360) alfaLatitude -= 360;
		if (alfaLatitude < 0) alfaLatitude += 360;

		printf("Longitude,Latitude)=(%6.2f,%6.2f)\n", alfaLongitude, alfaLatitude);

		glutPostRedisplay();
	}
}

void main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(500,500);
	glutCreateWindow("Projeto Cubo");
	myInit();
	glutReshapeFunc(myReshape);
	glutDisplayFunc(display);
	glutKeyboardFunc(teclas);
	glutSpecialFunc(teclasEspeciais);
	glutMouseFunc(rato);
	glutMotionFunc(moveRatoPress);
	glEnable(GL_DEPTH_TEST); 
	glutTimerFunc(timeUpdate, update, timeUpdate);
	glutMainLoop();
}

