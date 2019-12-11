#include "predator.h"

std::vector<Boid*> all_boids;
std::vector<Circle*> all_obstacles;

float getNorm_(float x, float y){
	return sqrt(pow(x, 2) + pow(y, 2));
}

Predator::Predator(){
	acc_x = 0.0f; acc_y = 0.0f;
	speed_x = 1.0f; speed_y = -1.0f;
}

int rdtsc(){
	__asm__ __volatile__("rdtsc");
}

void Predator::move_up(){
	if (pt->y >= 400) pt->y -= speed;
	else pt->y += speed;
}

void Predator::move_left(){
	if (pt->x <= -400) pt->x += speed;
	else pt->x -= speed;
}	

void Predator::move_down(){
	if (pt->y <= -400) pt->y += speed;
	else pt->y -= speed;
}

void Predator::move_right(){
	if (pt->x >= 400) pt->x -= speed;
	else pt->x += speed;
}

void Predator::random_move(){
	if (cur_moves >= max_moves){
		moved = false;
	}
	
	if (!moved){
		srand(rdtsc());
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

void Predator::move_towards_boids(){
	objective_x = 0; objective_y = 0;
	if (boids.empty()) return;
	Boid* obj;
	float cur_dist, min_dist;
	min_dist = 99999.0f;
	
	if (!killed_obj) {
		objective_x = (cur_obj->get_pt()->x - this->pt->x);
		objective_y = (cur_obj->get_pt()->y - this->pt->y);
		return;
	}
	
	for (int i = 0; i < all_boids.size(); i++){
		obj = boids[i];
		cur_dist = obj->get_pt()->distance(this->pt);
		if (cur_dist < min_dist){
			cur_obj = obj; 
			//objective_x = objective_x - (obj->get_pt()->x - this->pt->x);
			//objective_y = objective_y - (obj->get_pt()->y - this->pt->y);
			min_dist = cur_dist;
		}
	}
	
	objective_x = objective_x - (cur_obj->get_pt()->x - this->pt->x);
	objective_y = objective_y - (cur_obj->get_pt()->y - this->pt->y);
	
	killed_obj = false;
}

void Predator::avoid_obstacle(){
	obstacle_x = 0.0f;
	obstacle_y = 0.0f;
	avoiding = false;
	
	float normal = getNorm_(this->speed_x, this->speed_y);
	float temp_spd_x = this->speed_x / normal, temp_spd_y = this->speed_y / normal;
	
	int count = 0;
	Point2D* mostThreatening = 0;
	float ahead_x = pt->x + this->view_distance;
	float ahead_y = pt->y + this->view_distance;
	
	for(int i = 0; i < all_obstacles.size(); i++){
		float dist = obstacles[i]->center->distance(this->pt);
		float radius = obstacles[i]->radius;
		//if (dist < radius){
		if (dist < this->view_distance){
			if (dist < radius + 15.0f)
				avoiding = true;
			
			if (mostThreatening == 0 || pt->distance(obstacles[i]->center) < pt->distance(mostThreatening)){
				mostThreatening = obstacles[i]->center;
			}
			
			obstacle_x = obstacle_x - (obstacles[i]->center->x - pt->x);
			//obstacle_x = obstacle_x - (obstacles[i]->center->x - ahead_x);
			obstacle_y = obstacle_y - (obstacles[i]->center->y - pt->y);
			//obstacle_y = obstacle_y - (obstacles[i]->center->y - ahead_y);
			count++;
		}
	}
	
	/*if (mostThreatening != 0){
	obstacle_x = (pt->x - mostThreatening->x);
	obstacle_y = (pt->y - mostThreatening->y);
	}*/
	
	
	if (count > 0){
		obstacle_x = obstacle_x / count;
		obstacle_y = obstacle_y / count;
	}
	
}


void Predator::move(){
	//random_move();
	move_towards_boids();
	avoid_obstacle();
	
	move_x = 0.0f; move_y = 0.0f;
	float normal;
	
	if (avoiding){
		move_x = obstacle_x; move_y = obstacle_y;
		acc_x = obstacle_x; acc_y = obstacle_y;
	} else {
		move_x += (objective_x + obstacle_x);
		move_y += (objective_y + obstacle_y);
	}
	
	normal = sqrt(pow(move_x, 2) + pow(move_y, 2));
	if (move_x != 0) move_x /= normal;
	if (move_y != 0) move_y /= normal;
	
	//cout << "move_x " << move_x << ' ' << move_y << endl;
	
	acc_x += move_x; acc_y += move_y;
	
	pt->x += speed_x;
	pt->y += speed_y;
	
	speed_x += acc_x;
	speed_y += acc_y;
	
	if (getNorm_(speed_x, speed_y) >= max_speed){
		float norm = getNorm_(speed_x, speed_y);
		speed_x = speed_x / norm * max_speed;
		speed_y = speed_y / norm * max_speed;
	}
	
	if (pt->y >= 400) pt->y = -400;
	else if (pt->y <= -400) pt->y = 400;
	if (pt->x >= 400) pt->x = -400;
	else if (pt->x <= -400) pt->x = 400;
	
	if (all_boids.empty()) return;
	for (int i = 0; i < all_boids.size(); i++){
		if (all_boids[i]->get_pt()->distance(this->pt) <= 10.0f){
			all_boids.erase(all_boids.begin() + i);
			killed_obj = true;
			i--;
		}
	}
	
}

void Predator::draw(){
	/*glPointSize(6);
	glBegin(GL_POINTS);
	glColor3d(255, 0, 0);
	glVertex2f(pt->x, pt->y);
	glEnd();*/
	
	float tam = 35.0f;
	
	//cout << "Texture " << texture << endl;
	
	glPushMatrix();
	glColor3d(255,255,255);
	glBindTexture(GL_TEXTURE_2D, texture);
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
	glPopMatrix();
	
}

void Predator::draw_line(){
	glLineWidth(2);
	glBegin(GL_LINES);
	glColor3d(0,255,0);
	glVertex2f(pt->x, pt->y);
	glVertex2f(pt->x + 15*move_x, pt->y + 15*move_y);
	glEnd();
}
