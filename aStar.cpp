#include "aStar.h"


aStar::aStar(deque< deque<char> > map, int mapSizeX, int mapSizeY) :
map(map),
mapSizeX(mapSizeX),
mapSizeY(mapSizeY)
{

}


aStar::~aStar()
{
}

bool aStar::nodeInLists(node * currentNode, deque<node*> * openNodes, deque<node*> * closedNodes){

	for (int i = 0; i < openNodes->size(); i++){
		if (openNodes->at(i) == currentNode)
			return true;
	}

	for (int i = 0; i < closedNodes->size(); i++){
		if (closedNodes->at(i) == currentNode)
			return true;
	}

	return false;

}

bool aStar::nodeAlreadyExplored(int xPos, int yPos, deque<node*> * openNodes, deque<node*> * closedNodes){

	for (int i = 0; i < openNodes->size(); i++){
		if (openNodes->at(i)->x == xPos && openNodes->at(i)->y == yPos)
			return true;
	}

	for (int i = 0; i < closedNodes->size(); i++){
		if (closedNodes->at(i)->x == xPos && closedNodes->at(i)->y == yPos)
			return true;
	}

	return false;

}

bool aStar::outsideMap(int x, int y){

	if ((x < 0)
		|| (y < 0)
		|| (x >= mapSizeX)
		|| (y >= mapSizeY)){
		return true;
	}
	return false;


}

void aStar::addNewAdjacentNodes(int nextIndex, deque<node*> * openNodes, deque<node*> * closedNodes, startEndStruct startAndEnd){

	//henter en ut, fjerner den fra listen og setter den i utforskede noder:
	node * currentNode = openNodes->at(nextIndex);
	openNodes->erase(openNodes->begin() + nextIndex);
	closedNodes->push_back(currentNode);

	//en for hver retning, Nord, øst, sør, vest, ^>v<
	for (int i = 0; i < 4; i++){

		int xPos;
		int yPos;

		if (i == 0){
			//Nord
			xPos = currentNode->x;
			yPos = currentNode->y - 1;
			//cout << "N|";
		}

		if (i == 1){
			//Øst
			xPos = currentNode->x + 1;
			yPos = currentNode->y;
			//cout << "E|";
		}

		if (i == 2){
			//Sør
			xPos = currentNode->x;
			yPos = currentNode->y + 1;
			//cout << "S|";
		}

		if (i == 3){
			//Øst
			xPos = currentNode->x - 1;
			yPos = currentNode->y;
			//cout << "W|";
		}

		if (!outsideMap(xPos, yPos)){

			if (!nodeAlreadyExplored(xPos, yPos, openNodes, closedNodes) 
				&& map[xPos][yPos] != mapWallChar
				&& map[xPos][yPos] != mapUnreachableChar){

				float disFromStart = abs(xPos - startAndEnd.startX) + abs(yPos - startAndEnd.startY);
				float disFromTarget = abs(xPos - startAndEnd.endX) + abs(yPos - startAndEnd.endY);

				//cout << "Hjem: " << disFromStart << " Borte: " << disFromTarget << " xP: " << xPos << " yP: " << yPos << endl;

				node * newNode = new node(xPos, yPos, disFromStart, disFromTarget, currentNode);
				openNodes->push_back(newNode);

			}

		}

	}


}

int aStar::getNextByLowestSum(deque<node*> * openNodes){

	int lowestIndex = -1;
	int lowestSum = 10E4; //noe stort

	for (int i = 0; i < openNodes->size(); i++){

		if (openNodes->at(i)->sum < lowestSum){
			lowestIndex = i;
			lowestSum = openNodes->at(i)->sum;
		}

	}


	return lowestIndex;
}

bool aStar::isAtEnd(deque<node*> * closedNodes, startEndStruct startAndEnd){

	for (int i = 0; i < closedNodes->size(); i++){
		if (closedNodes->at(i)->x == startAndEnd.endX && closedNodes->at(i)->y == startAndEnd.endY){
			//cout << closedNodes->at(i)->x << closedNodes->at(i)->y << "TROLOL" <<  endl;
			return true;
		}
	}

	return false;

}





deque<pos> aStar::findPath(startEndStruct startAndEnd){

	int currentX = startAndEnd.startX;
	int currentY = startAndEnd.startY;

	float disFromStart = abs(currentX - startAndEnd.startX) + abs(currentY - startAndEnd.startY);
	float disFromTarget = abs(currentX - startAndEnd.endX) + abs(currentY - startAndEnd.endY);

	deque<node*> * openNodes = new deque<node*>;
	deque<node*> * closedNodes = new deque<node*>;

	//legger til og utforsker den første
	node * startNode = new node(startAndEnd.startX, startAndEnd.startY, disFromStart, disFromTarget, NULL);
	openNodes->push_back(startNode);
	addNewAdjacentNodes(0, openNodes, closedNodes, startAndEnd);

	while (true){

		int nextIndex = getNextByLowestSum(openNodes);

		if (nextIndex < 0){
			//cout << "Det finnes ingen vei!\n";
			//returnerer tom vei:
			deque<pos> veien;
			return veien;
		}

		//Vi er fremme når slutten ligger i closedNodes:
		if (isAtEnd(closedNodes, startAndEnd)){

			//cout << "Veien er funnet, A* er i mal!" << endl;
			delete openNodes;
			return getPathHome(closedNodes, startAndEnd);

		}
		else{

			//cout << "neste som utforskes er index: " << nextIndex << " med x: " << openNodes->at(nextIndex)->x << " og y: " << openNodes->at(nextIndex)->y << endl;
			addNewAdjacentNodes(nextIndex, openNodes, closedNodes, startAndEnd);

		}

	}

}











void aStar::displayMap(deque< deque<char> > displayMap){

	for (int y = 0; y < mapSizeY; y++){
		for (int x = 0; x < mapSizeX; x++){

			cout << (char)(int)(displayMap[x][y] + 0) << " ";

		}
		cout << endl << endl;
	}

}

deque<pos> aStar::getPathHome(deque<node*> * closedNodes, startEndStruct startAndEnd){

	//cout << "SUFF: " << startAndEnd.startX << " " << startAndEnd.startY << " " << startAndEnd.endX << " " << startAndEnd.endY << "\n";

	node * currentNode = nullptr;
	bool atStart = false;

	for (int i = 0; i < closedNodes->size(); i++){
		if (closedNodes->at(i)->x == startAndEnd.endX && closedNodes->at(i)->y == startAndEnd.endY)
			currentNode = closedNodes->at(i);
	}

	//leser over til lokalt kart
	//char moveMap[MapSizeX][MapSizeY];
	deque< deque<char> > moveMap(mapSizeX, deque<char>(mapSizeY));

	/*cout << "haha";
	for (int y = 0; y < mapSizeY; y++){
		for (int x = 0; x < mapSizeX; x++){
			moveMap[x][y] = map[x][y];
		}
	}
	cout << "haha2";*/

	deque<string> roadHome;
	deque<pos> roadHomePos;



	while (!atStart){

		string currentPos;
		/*currentPos += "X: " + to_string(currentNode->x);
		currentPos += " Y: " + to_string(currentNode->y);*/
		roadHome.push_back(currentPos);

		//setter opp kart
		if (currentNode->x == startAndEnd.endX && currentNode->y == startAndEnd.endY){
			moveMap[currentNode->x][currentNode->y] = 'E'; //start?
		}
		else if (currentNode->x == startAndEnd.startX && currentNode->y == startAndEnd.startY){
			moveMap[currentNode->x][currentNode->y] = 'B'; // end?
		}
		else{
			moveMap[currentNode->x][currentNode->y] = '!';
		}

		roadHomePos.push_back(pos(currentNode->x, currentNode->y));

		if (currentNode->parent == NULL)
			atStart = true;
		else
			currentNode = currentNode->parent;

	}

	/*cout << "Streg for steg hjem pa (" << roadHome.size() << ")steg blir:-------------" << endl;
	for (int i = roadHome.size() - 1; i >= 0; i--){
		cout << roadHome[i] << endl;
	}
	cout << endl;

	cout << "viser veien med map (0 er tom, 1 er veien, 4 er start, 6 er slutt, 9 er vegg)" << endl;
	cout << endl;
	displayMap(moveMap);
	cout << endl;*/

	delete currentNode;
	delete closedNodes;

	return roadHomePos;

}
