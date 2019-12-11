#ifndef PREDATOR_H
#define PREDATOR_H

#include <bits/stdc++.h>
#include <iostream>
#include <math.h>
#include <vector>
#include <cstdlib>
#include <time.h>
#include <GL/glut.h>
#include "point.h"
#include "boids.h"
#include "circle.h"

class Boid;

class Predator{
private:
	float speed = 0.8f;
	
	float speed_x, speed_y;
	float acc_x, acc_y;
	float max_speed = 2.5f;
	
	float obstacle_x, obstacle_y;
	
	Boid* cur_obj;
	bool killed_obj = true;
	
	float view_angle = 135.0f;
	float view_distance = 100.0f;
	
	float objective_x, objective_y;
	float move_x, move_y;
	
	bool moved = false;
	bool avoiding;
	int dir, max_moves = 20, cur_moves;
	
	Point2D* pt;
	void move_up();
	void move_left();
	void move_down();
	void move_right();
	void random_move();
	void move_towards_boids(); //move towards closest boid
	void avoid_obstacle();
	
public:
	GLuint texture;
	Predator();
	void set_pt(Point2D* pt) { this->pt = pt; };
	Point2D* get_pt() { return this->pt; };
	void move();
	void draw();
	void draw_line();
};

extern std::vector<Boid*> all_boids;
extern std::vector<Circle*> all_obstacles;

#endif
