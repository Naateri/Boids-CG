#define GLUT_DISABLE_ATEXIT_HACK
#include <bits/stdc++.h>
#include <iostream>
#include <math.h>
#include <vector>
#include <cstdlib>
#include <time.h>
#include <GL/glut.h>
#include "boids.h"
#include "objective.h"
#include "predator.h"

#define KEY_ESC 27
#define BOIDS 50
using namespace std;

RandNumbers temp1;

float distance(Point2D* a, Point2D* b){
	return sqrt(pow(a->x - b->x, 2) + pow(a->y - b->y, 2));
}

//vector<Objective*> objectives;
//std::vector<Boid*> boids;

//dibuja un simple gizmo
void displayGizmo(){
	glBegin(GL_LINES);
	//glEnable(GL_PROGRAM_POINT_SIZE);
	glEnd();
}
bool r = false;

void draw_point(int x,int y);


void generate_points(int num_points){
	srand(temp1.rdtsc());
	Point2D* pt;
	Boid* boid;
	int x, y;
	for(int i = 0; i < num_points; i++){
		x = rand()%600;
		y = rand()%600;
		pt = new Point2D(x - 300, 300 - y);
		boid = new Boid;
		//points.push_back(pt);
		boid->set_pt(pt);
		boids.push_back(boid);
	}
}

void OnMouseClick(int button, int state, int x, int y){
	Point2D* pt;
	//Boid* boid;
	Objective* obj;
	Boid* boid;
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
		//convertir x,y
		pt = new Point2D(x - 300, 300 - y);
		cout << x << ' ' << y << endl;
		obj = new Objective;
		obj->pt = pt;
		objectives.push_back(obj);
		
	}else if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){
		r = true;
		pt = new Point2D(x-300, 300-y);
		boid = new Boid;
		boid->set_pt(pt);
		boids.push_back(boid);
	}else if(button == GLUT_RIGHT_BUTTON && state == GLUT_UP){
		r = false;
	}
}

void OnMouseMotion(int x, int y){
	Point2D* pt;
	if(r){
		;
	}
	//opcional
}

void idle(){ // AGREGAR ESTA FUNCION
	glutPostRedisplay();
}

//funcion llamada a cada imagen
void glPaint(void) {
	
	//cout << "Objectives: " << objectives.size() << endl;
	all_boids = boids;
	
	//El fondo de la escena al color initial
	glClear(GL_COLOR_BUFFER_BIT); //CAMBIO
	glLoadIdentity();
	glOrtho(-300.0f,  300.0f,-300.0f, 300.0f, -1.0f, 1.0f);
	//glPointSize(6);
	//glBegin(GL_POINTS);
	
	//drawing objectives
	for (int i = 0; i < objectives.size(); i++){
		objectives[i]->draw();
	}
	
	//moving boids
	for (int i = 0; i < boids.size(); i++){
		boids[i]->move();
		boids[i]->draw();
		boids[i]->draw_line();
	}
	
	//predator
	for (int i = 0; i < predators.size(); i++){
		predators[i]->move();
		predators[i]->draw();
		predators[i]->draw_line();
	}
	
	boids = all_boids;
	
	//dibuja el gizmo
	displayGizmo();
	//doble buffer, mantener esta instruccion al fin de la funcion
	glutSwapBuffers();
}

void draw_point(int x, int y){
	glBegin(GL_POINTS);
	glVertex2f(x-300,300-y);
	glEnd();
	displayGizmo();
	
	glutSwapBuffers();
}

//
//inicializacion de OpenGL
//
void init_GL(void) {
	//Color del fondo de la escena
	glClearColor(0, 0, 0, 0.0f); //(R, G, B, transparencia) en este caso un fondo negro
	
	//modo projeccion
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
}

//en el caso que la ventana cambie de tamaño
GLvoid window_redraw(GLsizei width, GLsizei height) {
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();	
}

GLvoid window_key(unsigned char key, int x, int y) {
	switch (key) {
	case KEY_ESC:
		exit(0);
		break;
	case '1':{
		Predator* p;
		Point2D* pt = new Point2D(-300.0f, 300.0f);
		p = new Predator;
		p->set_pt(pt);
		predators.push_back(p);
		break;
	}
	default:
		break;
	}
	
}
//
//el programa principal
//
int main(int argc, char** argv) {
	
	generate_points(BOIDS);
	
	//Inicializacion de la GLUT
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(600, 600); //tamaño de la ventana
	glutInitWindowPosition(100, 100); //posicion de la ventana
	glutCreateWindow("Grafito"); //titulo de la ventana
	
	init_GL(); //funcion de inicializacion de OpenGL
	
	glutDisplayFunc(glPaint);
	glutReshapeFunc(&window_redraw);
	// Callback del teclado
	glutKeyboardFunc(&window_key);
	glutMouseFunc(&OnMouseClick);
	glutMotionFunc(&OnMouseMotion);
	glutIdleFunc(&idle);
	
	glutMainLoop(); //bucle de rendering
	//no escribir nada abajo de mainloop
	return 0;
}
