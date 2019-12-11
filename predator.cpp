#include "predator.h"

std::vector<Boid*> all_boids;

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
	
	for (int i = 0; i < boids.size(); i++){
		obj = boids[i];
		cur_dist = obj->get_pt()->distance(this->pt);
		if (cur_dist < min_dist){
			cur_obj = obj;
			objective_x = (obj->get_pt()->x - this->pt->x);
			objective_y = (obj->get_pt()->y - this->pt->y);
			min_dist = cur_dist;
		}
	}
	killed_obj = false;
}

void Predator::move(){
	//random_move();
	move_towards_boids();
	
	move_x = 0.0f; move_y = 0.0f;
	float normal;
	move_x += (objective_x);
	move_y += (objective_y);
	
	normal = sqrt(pow(move_x, 2) + pow(move_y, 2));
	if (move_x != 0) move_x /= normal;
	if (move_y != 0) move_y /= normal;
	
	//cout << "move_x " << move_x << ' ' << move_y << endl;
	
	acc_x += move_x; acc_y += move_y;
	
	pt->x += speed_x;
	pt->y += speed_y;
	
	speed_x += acc_x;
	speed_y += acc_y;
	
	if (getNorm_(speed_x, speed_y) >= abs(max_speed)){
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
		if (all_boids[i]->get_pt()->distance(this->pt) <= 5.0f){
			all_boids.erase(all_boids.begin() + i);
			killed_obj = true;
			i--;
		}
	}
	
}

void Predator::draw(){
	glPointSize(6);
	glBegin(GL_POINTS);
	glColor3d(255, 0, 0);
	glVertex2f(pt->x, pt->y);
	glEnd();
}

void Predator::draw_line(){
	glLineWidth(2);
	glBegin(GL_LINES);
	glColor3d(0,255,0);
	glVertex2f(pt->x, pt->y);
	glVertex2f(pt->x + 15*move_x, pt->y + 15*move_y);
	glEnd();
}
