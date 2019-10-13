#ifndef POINT_H
#define POINT_H


struct Point2D{
	float x, y;
	Point2D(){
		this->x = this->y = 0;
	}
	Point2D(float x, float y){
		this-> x = x;
		this->y = y;
	}
};

#endif
