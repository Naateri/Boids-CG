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
std::vector<Circle*> obstacles = {};

float getNorm(float x, float y){
	return sqrt(pow(x, 2) + pow(y, 2));
}

Boid::Boid(){
	grid_x = -1;
	grid_y = -1;
	acc_x = 0.0f; acc_y = 0.0f;
	boid_speed_x = 1.0f; boid_speed_y = -1.0f;
	//texture = TextureManager::Inst()->LoadTexture("fish.png", GL_BGRA_EXT, GL_RGBA);
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
	
	if (count > 0){
		center_x /= count;
		center_y /= count;
	}
	
	center_x = center_x - this->pt->x;
	center_y = center_y - this->pt->y;
	
	center_x = center_x - this->boid_speed_x;
	center_y = center_y - this->boid_speed_y;
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
	if (count > 0){
		distance_x /= count;
		distance_y /= count;
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
			vel_x += boids[i]->boid_speed_x;
			vel_y += boids[i]->boid_speed_y;
			count++;
		}
	}
	
	if (count > 0){
		vel_x /= count;
		vel_y /= count;
	}
	
	vel_x = vel_x - this->boid_speed_x;
	vel_y = vel_y - this->boid_speed_y;
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

void Boid::avoid_obstacle(){
	obstacle_x = 0.0f;
	obstacle_y = 0.0f;
	avoiding = false;
	
	/*
	float temp_spd_x, temp_spd_y, normal, dynamic_length;
	normal = sqrt(pow(this->boid_speed_x,2) + pow(this->boid_speed_y,2));
	temp_spd_x = this->boid_speed_x / normal; temp_spd_y = this->boid_speed_y / normal;
	
	dynamic_length = normal / max_speed;
	
	//float ahead_x = pt->x + temp_spd_x * dynamic_length;
	//float ahead_y = pt->y + temp_spd_y * dynamic_length;
	
	float ahead_x = pt->x + temp_spd_x * this->view_distance;
	float ahead_y = pt->y + temp_spd_y * this->view_distance;
	
	Point2D* ahead = new Point2D(ahead_x, ahead_y);
	Point2D* ahead2 = new Point2D(ahead_x/2.0f, ahead_y/2.0f);
	
	Point2D* mostThreatening = 0;
	
	for(int i = 0; i < obstacles.size(); i++){
		Point2D* center = obstacles[i]->center;
		float dist = ahead->distance(center);
		float dist2 = ahead2->distance(center);
		float dist3 = pt->distance(center);
		
		float radius = obstacles[i]->radius + 50.0f;
		
		if (dist <= radius || dist2 <= radius || dist3 <= radius ){ //colission
			
			if (mostThreatening == 0 || pt->distance(center) < pt->distance(mostThreatening)){
				mostThreatening = center;
			}
		}
	}*/
	
	int count = 0;
	Point2D* mostThreatening = 0;
	float ahead_x = pt->x + this->view_distance;
	float ahead_y = pt->y + this->view_distance;
	
	for(int i = 0; i < obstacles.size(); i++){
		float dist = obstacles[i]->center->distance(this->pt);
		float radius = obstacles[i]->radius * 2.0;
		if (dist < radius){
			
			avoiding = true;
			
			if (mostThreatening == 0 || pt->distance(obstacles[i]->center) < pt->distance(mostThreatening)){
				mostThreatening = obstacles[i]->center;
			}
			
			obstacle_x = obstacle_x - (obstacles[i]->center->x - pt->x);
			obstacle_y = obstacle_y - (obstacles[i]->center->y - pt->y);
			count++;
		}
	}
	
	/*
	if (mostThreatening != 0){
		obstacle_x = (pt->x - mostThreatening->x);
		obstacle_y = (pt->y - mostThreatening->y);
	}
	*/
	
	/*
	if (count > 0){
		obstacle_x = obstacle_x / count;
		obstacle_y = obstacle_y / count;
	}*/
	
}
	
void Boid::set_pt(Point2D* pt) { this->pt = pt; }
	
Point2D* Boid::get_pt() { return this->pt; }
	
void Boid::move(){
	move_towards_center();
	
	avoid_boids();
	
	adapt_velocity();
	
	//move_towards_objective();
	
	//avoid_predator();
	
	avoid_obstacle();
	
	move_x = 0.0f; move_y = 0.0f;
	float normal;
	
	/*objective_x += (center_x - this->pt->x);
	objective_y += (center_y - this->pt->y);*/
	
	if (go_to_obj_only){
		move_x = objective_x;
		move_y = objective_y;
	} else {
		move_x = ((center_x/100) + distance_x + (vel_x/8) + predator_x
				   + 10*obstacle_x);
		move_y = ((center_y/100) + distance_y + (vel_y/8) + predator_y
				   + 10*obstacle_y);
	}
	
	/*if (objective_x == 0 && objective_y == 0){
		gen_objective();
	}*/
	
	/*if (move_x == 0 && move_y == 0) {
		random_move();
		return;
	}*/
	
	normal = sqrt(pow(move_x, 2) + pow(move_y, 2));
	if (move_x != 0) move_x /= normal;
	if (move_y != 0) move_y /= normal;
	
	acc_x += move_x; acc_y += move_y;
	
	/*pt->x += (boid_speed * move_x);
	pt->y += (boid_speed * move_y);*/
	
	pt->x += boid_speed_x;
	pt->y += boid_speed_y;
	
	boid_speed_x += acc_x;
	boid_speed_y += acc_y;
	
	if (getNorm(boid_speed_x, boid_speed_y) >= abs(max_speed)){
		float norm = getNorm(boid_speed_x, boid_speed_y);
		boid_speed_x = boid_speed_x / norm * max_speed;
		boid_speed_y = boid_speed_y / norm * max_speed;
	}
	
	last_move_x = move_x;
	last_move_y = move_y;
	
	/*if (pt->y >= GRID_SIZE) pt->y = pt->y--;
	else if (pt->y <= -GRID_SIZE) pt->y++;
	if (pt->x >= GRID_SIZE) pt->x--;
	else if (pt->x <= -GRID_SIZE) pt->x++;*/
}

void Boid::edges(){
	if (pt->y >= GRID_SIZE) pt->y = 0-GRID_SIZE;
	else if (pt->y <= -GRID_SIZE) pt->y = GRID_SIZE;
	if (pt->x >= GRID_SIZE) pt->x = 0-GRID_SIZE;
	else if (pt->x <= -GRID_SIZE) pt->y = GRID_SIZE;
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
	//glPointSize(6);
	//glBegin(GL_POINTS);
	
	/*if (!avoiding) glColor3d(0, 0, 255);
	else glColor3d(255,0,0);*/
	//glColor3d(255,255,255);
	//glVertex2f(pt->x, pt->y);
	
	float tam = 5.0f;
	
	//cout << "Texture " << texture << endl;
	
	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glColor3d(255,255,255);
	
	glBindTexture(GL_TEXTURE_2D, fish_texture);
	glBegin(GL_QUADS);
	
	glTexCoord2f(0.0f, 0.0f);
	glVertex3f(pt->x - tam, pt->y - tam, 0.0f);
	
	glTexCoord2f(0.0f, 1.0f);
	glVertex3f(pt->x - tam, pt->y + tam, 0.0f);
	
	glTexCoord2f(1.0f, 1.0f);
	glVertex3f(pt->x + tam, pt->y + tam, 0.0f);
	
	glTexCoord2f(1.0f, 0.0f);
	glVertex3f(pt->x + tam, pt->y - tam, 0.0f);
	
	glEnd();
	
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
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
