#pragma once

#include "Vacuum.h"

Vacuum::Vacuum(int mapSizeX, int mapSizeY, int startX, int startY) {
	placed = false;
	searchDirection = true;
	robotIcon = 'x';
	int completedRuns = 0;
	this->mapSizeX = 1;
	this->mapSizeY = 1;

	this->mapSizeX = mapSizeX;
	this->mapSizeY = mapSizeY;

	xpos = 1;
	ypos = 1;

	numMoves = 0;
	numUnknownSquaresExplored = 0;

	wallsChanged = false;
	mapExplored = false;

	worldPosX = startX;
	worldPosY = startY;

	worldToLocalDiffX = worldPosX - xpos;
	worldToLocalDiffY = worldPosY - ypos;

	deque< deque<char> > internalmapTemp(3, deque<char>(3));
	internalmap = internalmapTemp;

	for (int y = 0; y < 3; y++){
		for (int x = 0; x < 3; x++) {
			internalmap[x][y] = '?';
		}
	}

	internalmap[xpos][ypos] = mapPowerStationChar;
	powerStationPosX = xpos;
	powerStationPosY = ypos;
	powerLVL = robotMaxPower;

}

void Vacuum::expandInternalmap(char direction){
	//U = Up, R = Right, D = Down, L = Left

	if (direction == 'U'){
		for (int x = 0; x < internalmap.size(); x++) {
			internalmap[x].push_front('?');
		}
		ypos++;
		powerStationPosY ++;
	}

	if (direction == 'R'){
		internalmap.push_back(deque<char>(internalmap[0].size(), '?'));
	}

	if (direction == 'D'){
		for (int x = 0; x < internalmap.size(); x++) {
			internalmap[x].push_back('?');
		}
	}

	if (direction == 'L'){
		internalmap.push_front(deque<char>(internalmap[0].size(), '?'));
		xpos++;
		powerStationPosX++;
	}

	aStarPath.clear();
	unexploredCells.clear();

}

Vacuum::~Vacuum() {

}


void Vacuum::senseSquare(int worldX, int worldY, int localX, int localY) {
	if (localX < 0 || localX >= internalmap.size() || localY < 0 || localY >= internalmap[0].size()) {
		return;
	}
	if (internalmap[localX][localY] == mapUnexploredChar)
		numUnknownSquaresExplored++;
	internalmap[localX][localY] = getMapCoordinateData(worldX, worldY);

}

void Vacuum::senseEnvironment() {

	senseSquare(worldPosX, worldPosY, xpos, ypos);

}

void Vacuum::printInternalMap() {

	printRoom(this, internalmap, true, xpos, ypos);

}

bool Vacuum::unexploredLeft(){

	for (int y = 0; y < internalmap[0].size(); y++){
		for (int x = 0; x < internalmap.size(); x++) {

			if (internalmap[x][y] == '?'){
				targetX = x;
				targetY = y;
				return true;
			}

		}
	}


	return false;

}

//true for en ukjent finnes, false for alt utforsket
bool Vacuum::findNextUnexplored(){

	if (!unexploredLeft())
		return false;

	unexploredFinder finder(internalmap, xpos, ypos);
	vec2 nextUnexplored = finder.findUnexplored();

	if (nextUnexplored.x == -1 && nextUnexplored.x == -1)
		return false;

	targetX = nextUnexplored.x;
	targetY = nextUnexplored.y;

	return true;


}

bool Vacuum::moveToCell(int newCellX, int newCellY){

	

	int newPosX = xpos;
	int newPosY = ypos;
	int newPosWorldX = worldPosX;
	int newPosWorldY = worldPosY;

	if (newCellX < xpos){
		newPosX--;
		newPosWorldX--;
	}
	else if (newCellX > xpos){
		newPosX++;
		newPosWorldX++;
	}

	else if (newCellY < ypos){
		newPosY--;
		newPosWorldY--;
	}
	else if (newCellY > ypos){
		newPosY++;
		newPosWorldY++;
	}

	if (getMapCoordinateData(newPosWorldX, newPosWorldY) == (char)219){

		internalmap[newPosX][newPosY] = (char)219;


		if (mapExplored){
			status = "UNEXPECTED WALL AFTER ALL EXPLORED, EXPLORING";
			wallsChanged = true;
		}else
			status = "CRASHED WITH WALL";

		return false;

	}


	xpos = newPosX;
	ypos = newPosY;

	worldPosX = newPosWorldX;
	worldPosY = newPosWorldY;



	if (xpos <= 0)
		expandInternalmap('L');
	else if (ypos <= 0)
		expandInternalmap('U');
	else if (xpos >= internalmap.size() -1)
		expandInternalmap('R');
	else if (ypos >= internalmap[0].size() - 1){
		//cout << "EXP: " << ypos  << "#" << internalmap[0].size() - 1 << endl;
		expandInternalmap('D');
	}

	numMoves++;

	return true;

}

void Vacuum::resetInternalMap(){
	completedRuns++;
	//rehide map
	for (int y = 0; y < internalmap[0].size(); y++){
		for (int x = 0; x < internalmap.size(); x++) {
			//Flush memory to check if walls have changed every n'th time
			if (wallsChanged){

				//power station may not move/change
				if (internalmap[x][y] != mapPowerStationChar)
					internalmap[x][y] = '?';

			}else if (internalmap[x][y] != mapPowerStationChar) {

				if (internalmap[x][y] == '?') {
					internalmap[x][y] = mapUnreachableChar;
				}
					

				if (internalmap[x][y] != mapWallChar && internalmap[x][y] != mapUnreachableChar){
					internalmap[x][y] = '?';
				}

			}
		}
	}

	wallsChanged = false;
	displayCmlStatus(this, true);
	Sleep(1000);
	numMoves = 0;
	numUnknownSquaresExplored = 0;
	//more poop
	roomUpdate();


}


bool Vacuum::insideInternalmap(int x, int y){

	if (x < 0 || y < 0 || x >= internalmap.size() || y >= internalmap[0].size()) {
		return false;
	}
	return true;

}

bool Vacuum::handlePower(){

	if (internalmap[xpos][ypos] == mapPowerStationChar && gotoPowerStation){

		powerLVL += powerChargedEachCycle;
		status = "Charging...";

		if (powerLVL >= robotMaxPower){
			powerLVL = robotMaxPower;
			gotoPowerStation = false;
		}

		return false;
	}


	if (powerLVL < robotCriticalPower && !gotoPowerStation){
		gotoPowerStation = true;
		aStarPath.clear();
	}


	if (gotoPowerStation){
		status = "Going to power station!";

		//MOVE
		if (aStarPath.size() > 0) {
			//Go to new pos

			//if blocked, delete path and recalculate on next tick
			if (!moveToCell(aStarPath[aStarPath.size() - 1].x, aStarPath[aStarPath.size() - 1].y))
				aStarPath.clear();

			//remove last visited cell
			if (aStarPath.size() != 0)
				aStarPath.pop_back();

		}
		else{
			//calculate path to power station

			startEndStruct startAndEnd(xpos, ypos, powerStationPosX, powerStationPosY);
			aStar aStar(internalmap, internalmap.size(), internalmap[0].size());
			aStarPath = aStar.findPath(startAndEnd);

			//no path
			if (aStarPath.size() == 0){
				status = "Rout to power station blocked!!";
				return false;
			}
			else //a path!
				aStarPath.pop_back();

		}



		return false;
	}

	return true;

}

//returning on successful action because only one action is allowed each cycle
void Vacuum::performCycle(deque< deque<char> > theRoom) {

	
	//if (!insideMap(xpos, ypos)) {
	//	cout << "LOL---------------------" << endl;
	//	return;
	//}

	if (powerLVL <= 0){
		status = "Out of power...";
		return;
	}

	powerLVL--;

	


	if (mapExplored && wallsChanged){
		status = "ENVIRONMENT HAVE CHANGED, RE-EXPLORING";
		mapExplored = false;
		resetInternalMap();
		return;
	}


	senseEnvironment();



	if (internalmap[xpos][ypos] == 'P') {
		if (removePoop(worldPosX, worldPosY)) {
			status = "POOP AWAY!";
			internalmap[xpos][ypos] = '.';
			return;
		}
	}

	//skip rest of cycle if we need power
	if (!handlePower())
		return;


	
	/*
	Rekkefølge:
	Finn korteste vei til uutforsket rute lengst øverst til vestre.
	Deretter hver mot høyre til alle på den, så neste linje
	PÅ neste linjer starter vi igjen på ruten lengst til vestre
	Kort forklart: >v>v>v>v>...

	*/
	//finn hvilken rute på interalmap som ikke er utforsket utifra regler ovenfor:
	//vil neste move resultere i vegg legges den til på kartet og rekalkulerer ruten
	//om vi har ny spawning av poop kan han utforske alt, så sette alt untatt vegger til ukjent og starte på nytt.


	if (aStarPath.size() > 0) {
		//Go to new pos

		//if blocked, delete path and recalculate on next tick
		if (!moveToCell(aStarPath[aStarPath.size() - 1].x, aStarPath[aStarPath.size() - 1].y)){
			aStarPath.clear();
			unexploredCells.clear();
		}
			

		//remove last visited cell
		if (aStarPath.size() != 0)
			aStarPath.pop_back();

	} else {
		//Search algorifsnhjvhw

		if (!findNextUnexplored()){
			status = "ALL EXPLORED";
			mapExplored = true;
			resetInternalMap();
			return;
		}

		//gen path
		startEndStruct startAndEnd(xpos, ypos, targetX, targetY);
		aStar aStar(internalmap, internalmap.size(), internalmap[0].size());
		aStarPath = aStar.findPath(startAndEnd);

		//no path
		if (aStarPath.size() == 0){
			internalmap[targetX][targetY] = mapUnreachableChar;
		}else //a path!
			aStarPath.pop_back();


	}
}
