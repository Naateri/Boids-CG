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
	float distance(Point2D* b){
		return sqrt(pow(this->x - b->x, 2) + pow(this->y - b->y, 2));
	}
};

#endif
