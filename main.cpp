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
#include "rectangle.h"
#include "circle.h"

#define KEY_ESC 27
#define BOIDS 1000
#define OBJECTIVES 40
#define OBSTACLES 7
#define RADIUS 40.0f

#define SIZE 800
#define SIZE_F 400.0f

using namespace std;

bool show_grid = false;

RandNumbers temp1;
GLuint fish_texture = 0;
GLuint predator_texture = 0;
GLuint obstacle_texture = 0;

float distance(Point2D* a, Point2D* b){
	return sqrt(pow(a->x - b->x, 2) + pow(a->y - b->y, 2));
}

//vector<Objective*> objectives;
//std::vector<Boid*> boids;

//vector<Boid*>** GRID;
int size;

void clear_grid(){
	for (int i = 0; i < size; i++){
		for (int j = 0; j < size; j++){
			GRID[i][j].clear();
		}
	}
}

int click = 0;
bool draw_rectangle = false;

//dibuja un simple gizmo
void displayGizmo(){
	glBegin(GL_LINES);
	//glEnable(GL_PROGRAM_POINT_SIZE);
	glEnd();
}
bool r = false;

void draw_point(int x,int y);

void gen_obstacles(float radius){
	srand(temp1.rdtsc());
	//Rectangle* rect;
	Circle* circ;
	Point2D* pt, *pt2;
	
	int x, y;
	for(int i = 0; i < OBSTACLES; i++){
		x = rand()%(SIZE);
		y = rand()%(SIZE);
		
		pt = new Point2D(x - SIZE_F, SIZE_F - y);
		
		/*x = rand()%(SIZE << 1);
		y = rand()%(SIZE << 1);
		
		pt2 = new Point2D(x - SIZE, SIZE - y);
		
		rect = new Rectangle(pt, pt2);*/
		
		circ = new Circle(pt, radius);
		
		circ->texture = obstacle_texture;
		
		circ->print();
		
		obstacles.push_back(circ);
	}
}


void generate_points(int num_points){
	srand(temp1.rdtsc());
	Point2D* pt;
	Boid* boid;
	int x, y;
	for(int i = 0; i < num_points; i++){
		/*x = rand()%(SIZE << 1);
		y = rand()%(SIZE << 1);*/
		x = rand() % SIZE; y = rand() % SIZE;
		pt = new Point2D(x - SIZE_F, SIZE_F - y);
		boid = new Boid;
		boid->set_pt(pt);
		boid->texture = fish_texture;
		boids.push_back(boid);
		
	}
	
	cout << "Boids created\n";
	
	float view_distance = boid->get_dist();
	cout << view_distance << endl;
	size = (SIZE << 1)/view_distance + 1;
	//size = SIZE / view_distance + 1;
	
	//create matrix
	
	GRID = new vector<Boid*>*[size];
	for (int i = 0; i < size; i++){
		GRID[i] = new vector<Boid*>[size];
	}
	
	vector<Boid*> temp;
	temp.clear();
	
	for (int i = 0; i < size; i++){
		for (int j = 0; j < size; j++){
			GRID[i][j] = temp;
		}
	}
}

vector<Point2D*> all_points;

void OnMouseClick(int button, int state, int x, int y){
	Point2D* pt, *prev_pt;
	//Boid* boid;
	Objective* obj;
	Boid* boid;
	Rectangle* rect;
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN){
		pt = new Point2D(x - SIZE, SIZE - y);
		cout << x << ' ' << y << endl;
		obj = new Objective;
		obj->pt = pt;
		objectives.push_back(obj);
	}else if(button == GLUT_RIGHT_BUTTON && state == GLUT_DOWN){
		r = true;
		pt = new Point2D(x-SIZE, SIZE-y);
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

void draw_grid(){
	glColor3d(0,255,0);
	glLineWidth(2);
	glBegin(GL_LINES);
	float view_distance = boids[0]->get_dist();
	for(int i = 0; i < size; i++){
		//glBegin(GL_LINES);
		for (int j = 0; j < size; j++){
			glVertex2d(i * view_distance -  SIZE, j * view_distance - SIZE);
			glVertex2d(SIZE - i * view_distance, j * view_distance - SIZE);
			
			glVertex2d(j * view_distance - SIZE, i * view_distance -  SIZE);
			glVertex2d(j * view_distance - SIZE, SIZE - i * view_distance);
		}
		//glEnd();
	}
	glEnd();
}

//funcion llamada a cada imagen
void glPaint(void) {
	
	//cout << "Objectives: " << objectives.size() << endl;
	all_boids = boids;
	
	all_obstacles = obstacles;
	
	//El fondo de la escena al color initial
	glClear(GL_COLOR_BUFFER_BIT); //CAMBIO
	glLoadIdentity();
	glOrtho(-SIZE_F,  SIZE_F,-SIZE_F, SIZE_F, -1.0f, 1.0f);
	
	/*gluPerspective(45.0, 1.0, 1.0, 1200.0);
	glTranslatef(0, 0, -1200.0);*/
		
	//moving boids
	
	clear_grid();
	
	glBindTexture(GL_TEXTURE_2D, 0);
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	for (int i = 0; i < boids.size(); i++){
		boids[i]->move();
		boids[i]->find_grid_pos(size);
		
		GRID[boids[i]->grid_x][boids[i]->grid_y].push_back(boids[i]);
		
		boids[i]->draw();
		//boids[i]->draw_line();
		
		//boids[i]->edges();
	}
	//glDisable(GL_BLEND);
	
	//predator
	
	glBindTexture(GL_TEXTURE_2D, predator_texture);
	
	for (int i = 0; i < predators.size(); i++){
		predators[i]->move();
		predators[i]->draw();
		//predators[i]->draw_line();
	}
	
	glBindTexture(GL_TEXTURE_2D, 0);
	//glDisable(GL_TEXTURE_2D);
	
	//obstacles
	for (int i = 0; i < obstacles.size(); i++){
		obstacles[i]->draw();
	}
	
	if (show_grid) draw_grid();
	
	boids = all_boids;
	
	//dibuja el gizmo
	//displayGizmo();
	//doble buffer, mantener esta instruccion al fin de la funcion
	glutSwapBuffers();
}

void draw_point(int x, int y){
	glBegin(GL_POINTS);
	glVertex2f(x-SIZE,SIZE-y);
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
	
	//glEnable(GL_TEXTURE_2D);
	
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
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
		Point2D* pt = new Point2D(-SIZE_F, SIZE_F);
		p = new Predator;
		p->set_pt(pt);
		p->texture = predator_texture;
		predators.push_back(p);
		break;
	}
	case '2':
		draw_rectangle = !draw_rectangle;
		click = 0;
		if (draw_rectangle) cout << "Clicking now draws rectangles.\n";
		else cout << "Clicking now draws objectives.\n";
		break;
	default:
		break;
	}
	
}
//
//el programa principal
//
int main(int argc, char** argv) {
	
	//fish_texture = TextureManager::Inst()->LoadTexture("Textures/fish.png", GL_BGRA_EXT, GL_RGBA);
	
	//Inicializacion de la GLUT
	
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(800, 800); //tamaño de la ventana
	glutInitWindowPosition(100, 100); //posicion de la ventana
	glutCreateWindow("My Boids project"); //titulo de la ventana
	
	init_GL(); //funcion de inicializacion de OpenGL
	
	cout << "initgl\n";
	
	/*string fish = "real_fish.png";
	const char* texture1 = fish.c_str();
	
	fish_texture = TextureManager::Inst()->LoadTexture(texture1, GL_RGBA, GL_RGBA);*/
	
	fish_texture = TextureManager::Inst()->LoadTexture("Textures/fish.png", GL_RGBA, GL_RGBA);
	predator_texture = TextureManager::Inst()->LoadTexture("Textures/sharpedo.png", GL_RGBA, GL_RGBA);
	//predator_texture = TextureManager::Inst()->LoadTexture("Textures/fish.png", GL_RGBA, GL_RGBA);
	
	obstacle_texture = TextureManager::Inst()->LoadTexture("Textures/coral.png", GL_RGBA, GL_RGBA);
	
	generate_points(BOIDS);
	//gen_objectives();
	gen_obstacles(RADIUS);
	
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
