#include "unexploredFinder.h"


unexploredFinder::unexploredFinder(deque< deque<char> > map, int posX, int posY):
map(map),
posX(posX),
posY(posY){

}


unexploredFinder::~unexploredFinder(){
}


bool unexploredFinder::nodeInLists(vec2 node){

	for (int i = 0; i < openNodes.size(); i++){
		if (openNodes[i].x == node.x && openNodes[i].y == node.y)
			return true;
	}

	for (int i = 0; i < closedNodes.size(); i++){
		if (closedNodes[i].x == node.x && closedNodes[i].y == node.y)
			return true;
	}

	return false;
}

vec2 unexploredFinder::findUnexplored(){

	openNodes.clear();
	closedNodes.clear();

	openNodes.push_back(vec2(posX, posY));

	while (true){
		//cout << "haha";
		vec2 current = openNodes[0];
		openNodes.pop_front();

		int newPosX;
		int newPosY;
		targetNodes.clear();

		for (int i = 0; i < 4; i++){
			
			if (i == 0){
				//Nord
				newPosX = current.x;
				newPosY = current.y - 1;
				//cout << "N|";
			}

			if (i == 1){
				//Øst
				newPosX = current.x + 1;
				newPosY = current.y;
				//cout << "E|";
			}

			if (i == 2){
				//Sør
				newPosX = current.x;
				newPosY = current.y + 1;
				//cout << "S|";
			}

			if (i == 3){
				//Vest
				newPosX = current.x - 1;
				newPosY = current.y;
				//cout << "W|";
			}

			//cout << "lete(" << openNodes.size() << ");";
			//cout << "LOL: " << newPosX << " " << newPosY << endl;


			if (insideInternalmap(newPosX, newPosY)){
				//cout << "runde\n";
				if (map[newPosX][newPosY] != mapWallChar){

					vec2 newNode(newPosX, newPosY);

					if (map[newPosX][newPosY] == mapUnexploredChar)
						targetNodes.push_back(newNode);

					if (!nodeInLists(newNode))
						openNodes.push_back(newNode);

				}
			}
		}

		//find best and return:
		if (targetNodes.size() > 0)
			return findBestTargetNode();

		//no nodes left to explore
		if (openNodes.size() == 0){
			//cout << "TOM";
			return vec2(-1, -1);
			cin.get();
		}

		closedNodes.push_back(current);
	}
}

/*
takes all the closest nodes (same ditance) and selects the one with the fewest neighbors.
In order to prevent lonely node from being left behind
*/
vec2 unexploredFinder::findBestTargetNode(){

	if (targetNodes.size() == 1)
		return targetNodes[0];

	int numNeighbors[4] = { 0 };

	for (int nodeIndex = 0; nodeIndex < targetNodes.size(); nodeIndex++){

		int newPosX;
		int newPosY;
		vec2 current = targetNodes[nodeIndex];

		for (int i = 0; i < 4; i++){

			if (i == 0){
				//Nord
				newPosX = current.x;
				newPosY = current.y - 1;
			}

			if (i == 1){
				//Øst
				newPosX = current.x + 1;
				newPosY = current.y;
			}

			if (i == 2){
				//Sør
				newPosX = current.x;
				newPosY = current.y + 1;
			}

			if (i == 3){
				//Vest
				newPosX = current.x - 1;
				newPosY = current.y;
			}

			if (insideInternalmap(newPosX, newPosY))
				if (map[newPosX][newPosY] == mapUnexploredChar)
					numNeighbors[nodeIndex] ++;

		}

	}

	//cout << "Nodes:" << targetNodes.size();
	//for (int i = 0; i < 4; i++)
	//	cout << "NR: " << numNeighbors[i] << endl;
	//cin.get();

	int returnIndex = 0;
	int minValue = 5;
	for (int i = 0; i < targetNodes.size(); i++){
		if (numNeighbors[i] < minValue){
			minValue = numNeighbors[i];
			returnIndex = i;
		}
	}

	
	return targetNodes[returnIndex];

}

bool unexploredFinder::insideInternalmap(int x, int y){

	if (x < 0 || y < 0 || x >= map.size() || y >= map[0].size()) {
		return false;
	}
	return true;

}