#ifndef CIRCLE_H
#define CIRCLE_H

#include "point.h"
#include <algorithm>
#include <GL/glut.h>

struct Circle{
	Point2D* center;
	float radius;
	GLuint texture;
	
	Circle(Point2D* c, float r){
		this->center = c;
		this->radius = r;
	}
	
	float distance(Point2D* query_pt){
		//return abs(sqrt(pow(query_pt->x - center->x, 2) + pow(query_pt->y - center->y, 2)) - radius);
		return sqrt(pow(query_pt->x - center->x,2) + pow(query_pt->y - center->y, 2));
	}
	
	void print(){
		std::cout << "Center x " << center->x << ", y: " << center->y << std::endl;
		std::cout << "radius " << radius << std::endl;
	}
	
	void draw(){
		
		glPushMatrix();
		
		glBindTexture(GL_TEXTURE_2D, texture);
		
		glBegin(GL_POLYGON);
		//glColor3d(0,255,0);
		float x_val, y_val;
		for(int i = 1; i <= 360; i++){
			
			float cosine = cos(i * M_PI/180);
			float sine = sin(i * M_PI/180);
			
			x_val = cosine * radius; //pythagoras
			y_val = sine * radius; //theorem
			
			float tx = cosine * 0.5 + 0.5, ty = sine * 0.5 + 0.5;
			
			glTexCoord2f(tx, ty);
			
			glVertex2f(center->x + x_val, center->y + y_val);
		}
		
		glEnd();
		glPopMatrix();
		
		glBegin(GL_POINTS);
		glVertex2f(center->x, center->y);
		glEnd();
	}
};

#endif
