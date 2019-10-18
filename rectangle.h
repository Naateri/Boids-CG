#ifndef RECTANGLE_H
#define RECTANGLE_H
	
#include "point.h"
#include <algorithm>
#include <GL/glut.h>

struct Rectangle{
	Point2D* bottom_right, *top_left;
	float radius;
	
	Rectangle(Point2D* br, Point2D* tl){
		this->bottom_right = br;
		this->top_left = tl;
		float a = bottom_right->x - top_left->x;
		float b = top_left->y - bottom_right->y;
		radius = sqrt(a*a + b*b)/2;
	}
	
	Point2D* get_center(){
		Point2D* temp = new Point2D((bottom_right->x + top_left->x)/2.0f,
			(top_left->y + bottom_right->y)/2.0f);
		return temp;
	}
	
	float distance(Point2D* query_pt){
		float dx, dy;
		dx = std::max(top_left->x - query_pt->x, std::max(0.0f, query_pt->x - bottom_right->x));
		dy = std::max(bottom_right->y - query_pt->y, std::max(0.0f, query_pt->y - top_left->y));
		return sqrt(dx*dx + dy*dy);
	}
	
	Point2D* get_botleft(){
		Point2D* temp = new Point2D(top_left->x, bottom_right->y);
		return temp;
	}
	
	Point2D* get_topright(){
		Point2D* temp = new Point2D(bottom_right->x, top_left->y);
		return temp;
	}
	
	Point2D* get_botright(){
		return this->bottom_right;
	}
	
	Point2D* get_topleft(){
		return this->top_left;
	}
	
	void print(){
		std::cout << "Bottom right: " << bottom_right->x << ' ' << bottom_right->y << std::endl;
		std::cout << "Top left: " << top_left->x << ' ' << top_left->y << std::endl;
	}
	
	void draw(){
		//glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		/*glColor3d(255,0,0);
		glRectf( this->top_left->x, this->top_left->y,
				this->bottom_right->x, this->bottom_right->y);*/
		
		Point2D* center = this->get_center();
		glBegin(GL_LINE_STRIP);
			glColor3d(0,255,0);
		
			float x_val, y_val;
			for(int i = 1; i <= 360; i++){
				x_val = cos(i * M_PI/180) * radius; //pythagoras
				y_val = sin(i * M_PI/180) * radius; //theorem
				glVertex2f(center->x + x_val, center->y + y_val);
			}
		
		glEnd();
	}
};

#endif
