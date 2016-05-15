#pragma once
#include "Vacuum.h"
#include "divClasses.h"
#include <deque>

class unexploredFinder
{
public:
	unexploredFinder(deque< deque<char> > map, int posX, int posY);
	~unexploredFinder();

	int posX;
	int posY;
	deque< deque<char> > map;

	deque <vec2> openNodes;
	deque <vec2> closedNodes;
	deque <vec2> targetNodes;

	bool nodeInLists(vec2 node);
	bool insideInternalmap(int x, int y);
	vec2 findUnexplored();
	vec2 findBestTargetNode();


};

