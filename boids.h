#ifndef BOIDS_H
#define BOIDS_H

#include "point.h"
#include <bits/stdc++.h>
#include <iostream>
#include <math.h>
#include <vector>
#include <cstdlib>
#include <time.h>
#include <GL/glut.h>
#include "objective.h"
#include "predator.h"
#include "rectangle.h"

#define REGULAR_SPEED 0.4f
#define RUNNING_SPEED 0.8f

class Predator;
struct Rectangle;

struct RandNumbers{
	int rdtsc(){
		__asm__ __volatile__("rdtsc");
	}
	
};

class Boid{
private:
	
	float boid_speed = 0.5f;
	
	float view_angle = 135.0f;
	float view_distance = 75.0f;
	
	float center_x, center_y;
	float objective_x, objective_y;
	float distance_x, distance_y;
	float predator_x, predator_y;
	float obstacle_x, obstacle_y;
	
	float move_x, move_y;
	float last_move_x, last_move_y;
	bool moved = false;
	bool go_to_obj_only = false;
	int dir, max_moves = 20, cur_moves;
	
	Point2D* pt;
	void move_up();
	void move_left();
	void move_down();
	void move_right();
	void move_towards_center();
	void avoid_boids();
	void random_move();
	void move_towards_objective();
	void avoid_predator();
	void avoid_obstacle();
	
public:
	void set_pt(Point2D* pt);
	Point2D* get_pt();
	void move();
	void draw();
	void draw_line();
};

extern std::vector<Boid*> boids;
extern std::vector<Objective*> objectives;
extern std::vector<Predator*> predators;
extern std::vector<Rectangle*> obstacles;

#endif
