#pragma once
class node{
public:

	int x;
	int y;

	float disFromStart;
	float disFromTarget;
	float sum;
	node * parent;

	node(int x, int y, float disFromStart, float disFromTarget, node * parent) :
		x(x),
		y(y),
		disFromStart(disFromStart),
		disFromTarget(disFromTarget),
		parent(parent){

		sum = disFromStart + disFromTarget;

	}



};

struct startEndStruct{
public:
	int startX;
	int startY;
	int endX;
	int endY;
	startEndStruct(int startX, int startY, int endX, int endY) :
		startX(startX), startY(startY), endX(endX), endY(endY){    }

};

struct pos{
	int x;
	int y;
	pos(int x, int y) : x(x), y(y){}
};


struct vec2 {

	int x;
	int y;

	vec2(){ x = 0; y = 0; }
	vec2(int x, int y) : x(x), y(y){}

};