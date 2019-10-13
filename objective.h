#ifndef OBJECTIVE_H
#define OBJECTIVE_H

#include <bits/stdc++.h>
#include <iostream>
#include <math.h>
#include <vector>
#include <cstdlib>
#include <time.h>
#include <GL/glut.h>
#include "point.h"

struct Objective{
	Point2D* pt;
	void draw(){
		glPointSize(6);
		glBegin(GL_POINTS);
		glColor3d(255, 0, 0);
		glVertex2f(pt->x, pt->y);
		glEnd();
	}
};

#endif
