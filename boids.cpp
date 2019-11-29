#define GLUT_DISABLE_ATEXIT_HACK
#include <bits/stdc++.h>
#include <iostream>
#include <math.h>
#include <vector>
#include <cstdlib>
#include <time.h>
#include <GL/glut.h>
#include "boids.h"

using namespace std;

RandNumbers temp;

std::vector<Boid*> boids;
std::vector<Objective*> objectives = {};
std::vector<Predator*> predators = {};
std::vector<Rectangle*> obstacles = {};

Boid::Boid(){
	grid_x = -1;
	grid_y = -1;
}

void Boid::move_up(){
	if (pt->y >= GRID_SIZE) pt->y -= boid_speed;
	else pt->y += boid_speed;
}
	
void Boid::move_left(){
	if (pt->x <= -GRID_SIZE) pt->x += boid_speed;
	else pt->x -= boid_speed;
}	
	
void Boid::move_down(){
	if (pt->y <= -GRID_SIZE) pt->y += boid_speed;
	else pt->y -= boid_speed;
}
	
void Boid::move_right(){
	if (pt->x >= GRID_SIZE) pt->x -= boid_speed;
	else pt->x += boid_speed;
}
	
void Boid::move_towards_center(){
	center_x = 0;
	center_y = 0;
	int count = 0;
	for(int i = 0; i < boids.size(); i++){
		if (boids[i]->get_pt()->x == pt->x 
			&& boids[i]->get_pt()->y == pt->y) continue;
		if (boids[i]->get_pt()->distance(this->pt) <= this->view_distance){
			/*center_x += (boids[i]->get_pt()->x - this->pt->x);
			center_y += (boids[i]->get_pt()->y - this->pt->y);*/
			center_x += (boids[i]->get_pt()->x);
			center_y += (boids[i]->get_pt()->y);
			count++;
		}
	}
		
	center_x /= count;
	center_y /= count;
}
	
void Boid::avoid_boids(){
	distance_x = 0;
	distance_y = 0;
	int count = 0;
	for(int i = 0; i < boids.size(); i++){
		if (boids[i]->get_pt()->x == pt->x 
			&& boids[i]->get_pt()->y == pt->y) continue;
		float dist = boids[i]->get_pt()->distance(this->pt);
		if (dist <= 20.0f){
			distance_x = distance_x - (boids[i]->get_pt()->x - this->pt->x);
			distance_y = distance_y - (boids[i]->get_pt()->y - this->pt->y);
			count++;
			//random_move();
		}
	}
	/*if (count > 0){
		distance_x /= count;
		distance_y /= count;
	}*/
}

void Boid::move_towards_objective(){
	objective_x = 0; objective_y = 0;
	go_to_obj_only = false;
	float min_dist = 9999.0f;
	if (objectives.empty()) return;
	Objective* obj = objectives[0];
	
	/*if (objective != nullptr){
		float dist = objective->pt->distance(this->pt);
		objective_x = (objective->pt->x - this->pt->x);
		objective_y = (objective->pt->y - this->pt->y);
		return;
	}*/
	
	for (int i = 0; i < objectives.size(); i++){
		obj = objectives[i];
		float dist = obj->pt->distance(this->pt);
		if (dist <= this->view_distance){
			//objective_x += (obj->pt->x - this->pt->x);
			//objective_y += (obj->pt->y - this->pt->y);
			/*if (dist <= 20.0f){
				go_to_obj_only = false;
				this->boid_speed = RUNNING_SPEED;*/
				if (dist < min_dist){
					objective_x = (obj->pt->x - this->pt->x);
					objective_y = (obj->pt->y - this->pt->y);
					min_dist = dist;
					objective = obj;
				}
			//}
		}
	}
}

void Boid::adapt_velocity(){
	vel_x = 0; vel_y = 0;
	int count = 0;
	float dist;
	for(int i = 0; i < boids.size(); i++){
		if (boids[i]->get_pt()->x == pt->x 
			&& boids[i]->get_pt()->y == pt->y) continue;
		dist = this->pt->distance(boids[i]->get_pt());
		if (dist <= this->view_distance){
			vel_x += boids[i]->boid_speed;
			vel_y += boids[i]->boid_speed;
			count++;
		}
	}
	
	vel_x /= count;
	vel_y /= count;
	
	vel_x = vel_x - this->boid_speed;
	vel_y = vel_y - this->boid_speed;
}

void Boid::avoid_predator(){
	predator_x = 0; predator_y = 0;
	if (predators.empty()) return;
	
	Predator* pred;
	
	//this->boid_speed_x = REGULAR_SPEED;
	//this->boid_speed_y = REGULAR_SPEED;
	
	for(int i = 0; i < predators.size(); i++){
		pred = predators[i];
		if (pred->get_pt()->distance(this->pt) <= this->view_distance){
			//this->boid_speed = RUNNING_SPEED;
			this->boid_speed *= 2;
			/*this->boid_speed_x *= 2;
			this->boid_speed_y *= 2;*/
			predator_x += (this->pt->x - pred->get_pt()->x);
			predator_y += (this->pt->y - pred->get_pt()->y);
		}
	}
	
}
	
void Boid::random_move(){
		
	if (cur_moves >= max_moves){
		moved = false;
	}
	
	if (!moved){
		srand(temp.rdtsc());
		dir = rand() % 4;
		cur_moves = 0;
		moved = true;
	} else {
		cur_moves++;
	}
	
	if (dir == 0){ //move up
		move_up();
	} else if (dir == 1){ //move left
		move_left();
	} else if (dir == 2){ //move down
		move_down();
	} else { //move right
		move_right();
	}
}

void Boid::avoid_obstacle(){
	float min_distance = 9999.0f;
	obstacle_x = 0.0f;
	obstacle_y = 0.0f;
	
	float ahead_x = pt->x + last_move_x * 15;
	float ahead_y = pt->y + last_move_y * 15;
	Point2D* ahead = new Point2D(ahead_x, ahead_y);
	Point2D* ahead2 = new Point2D(ahead_x/2.0f, ahead_y/2.0f);
	
	for(int i = 0; i < obstacles.size(); i++){
		Point2D* center = obstacles[i]->get_center();
		float dist = ahead->distance(center);
		float dist2 = ahead2->distance(center);
		if (dist <= obstacles[i]->radius || dist2 <= obstacles[i]->radius ){
			//std::cout << "MIN_DIST\n";
			if (obstacles[i]->distance(pt) < min_distance){
				obstacle_x = (ahead->x - center->x);
				obstacle_y = (ahead->y - center->y);
				min_distance = obstacles[i]->distance(pt);
			}
		}
	}
	
	obstacle_x *= 5.0f;
	obstacle_y *= 5.0f;
}
	
void Boid::set_pt(Point2D* pt) { this->pt = pt; }
	
Point2D* Boid::get_pt() { return this->pt; }
	
void Boid::move(){
	
	//random_move();
	
	move_towards_center();
	
	avoid_boids();
	
	adapt_velocity();
	
	move_towards_objective();
	
	avoid_predator();
	
	avoid_obstacle();
	
	move_x = 0.0f; move_y = 0.0f;
	float normal;
	
	/*objective_x += (center_x - this->pt->x);
	objective_y += (center_y - this->pt->y);*/
	
	if (go_to_obj_only){
		move_x = objective_x;
		move_y = objective_y;
	} else {
		move_x = ((center_x/100) + distance_x + (vel_x/8) + objective_x/2 + boids.size()*predator_x
				   + obstacle_x);
		move_y = ((center_y/100) + distance_y + (vel_y/8) + objective_y/2 + boids.size()*predator_y
				   + obstacle_y);
	}
	
	if (objective_x == 0 && objective_y == 0){
		gen_objective();
	}
	
	/*if (move_x == 0 && move_y == 0) {
		random_move();
		return;
	}*/
	
	normal = sqrt(pow(move_x, 2) + pow(move_y, 2));
	if (move_x != 0) move_x /= normal;
	if (move_y != 0) move_y /= normal;
	
	//cout << "move_x " << move_x << ' ' << move_y << endl;
	
	/*boid_speed_x *= move_x;
	boid_speed_y *= move_y;*/
	
	pt->x += (boid_speed * move_x);
	pt->y += (boid_speed * move_y);
	
	/*pt->x += boid_speed_x;
	pt->y += boid_speed_y;*/
	
	last_move_x = move_x;
	last_move_y = move_y;
	
	if (pt->y >= GRID_SIZE) pt->y--;
	else if (pt->y <= -GRID_SIZE) pt->y++;
	if (pt->x >= GRID_SIZE) pt->x--;
	else if (pt->x <= -GRID_SIZE) pt->x++;
	
//	pt->x += objective_x;
//	pt->y += objective_y;
	
	if (!objectives.empty()){
		for (int i = 0; i < objectives.size(); i++){
			if (objectives[i]->pt->distance(this->pt) <= 7.5f){
				objectives[i] = nullptr;
				objectives.erase(objectives.begin() + i);
				i--;
			}
		}
	}
	
	//random_move();
}

void Boid::gen_objective(){
	
	RandNumbers rand_num;
	
	bool finished = false;
	
	srand(rand_num.rdtsc());
	int temp_x, temp_y, sign;
	
	Point2D* pt = new Point2D;
	Objective* temp = new Objective;
	
	do{
	
		temp_x = rand() % ((int)this->view_distance);
		temp_y = rand() % ((int)this->view_distance);
		
		sign = rand() % 2;
		
		temp->pt = pt;
		if (sign == 0){
			temp->pt->x = temp_x + this->pt->x;
			temp->pt->y = temp_y + this->pt->y;
		} else {
			temp->pt->x = this->pt->x - temp_x;
			temp->pt->y = this->pt->y - temp_y;
		}
		
		if (temp->pt->y >= GRID_SIZE || temp->pt->y <= -GRID_SIZE) finished = false;
		else if (temp->pt->x >= GRID_SIZE || temp->pt->x <= -GRID_SIZE) finished = false;
		else finished = true;
		
	} while (!finished);
	
	objectives.push_back(temp);
}

float Boid::get_dist(){
	return this->view_distance;
}

void Boid::find_grid_pos(int size){
	grid_x = 0; grid_y = 0;
	for(int i = 0; i < (size-1); i++){
		int start = i * view_distance, end = (i+1) * view_distance;
		int start_x, end_x, start_y, end_y;
		start_x = start - GRID_SIZE;
		end_x = end - GRID_SIZE;
		if (start_x <= pt->x && pt->x <= end_x){
			grid_x = i;
		}
		
		start_y = 400 - start;
		end_y = 400 - end;
		
		if (start_x <= pt->y && pt->y <= end_x){
			grid_y = i;
		}
	}
	
	//cout << "Position: " << pt->x << ", " << pt->y << endl;
	//cout << "Grid: [" << grid_x << "][" << grid_y << "]\n";
}

void Boid::draw(){
	glPointSize(6);
	glBegin(GL_POINTS);
	glColor3d(0, 0, 255);
	glVertex2f(pt->x, pt->y);
	glEnd();
}

void Boid::draw_line(){
	glLineWidth(2);
	glBegin(GL_LINES);
	glColor3d(0,0,255);
	glVertex2f(pt->x, pt->y);
	//glVertex2f(pt->x + this->view_distance*move_x, pt->y + this->view_distance*move_y);
	glVertex2f(pt->x + 15*move_x, pt->y + 15*move_y);
	glEnd();
}
