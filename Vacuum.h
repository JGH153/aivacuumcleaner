#pragma once

#include <iostream>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <deque>
#include <vector>
#include <string>
#include "windows.h" 

using namespace std;

#include "divClasses.h"
#include "defines.h"
#include "aStar.h"
#include "unexploredFinder.h"




char getMapCoordinateData(int x,int y);
bool insideMap(int xpos, int ypos);
bool removePoop(int x, int y);
void roomUpdate();





class Vacuum
{
public:
	Vacuum(int mapSizeX, int mapSizeY, int startX, int startY);
	~Vacuum();
	void senseSquare(int worldX, int worldY, int localX, int localY);
	void senseEnvironment();
	void printInternalMap();
	void performCycle(deque< deque<char> > theRoom);
	bool suckPoop(deque< deque<char> >  theRoom, int x, int y);
	

	bool findNextUnexplored();
	bool moveToCell(int newCellX, int newCellY);
	void resetInternalMap();
	bool unexploredLeft();
	
	int xpos;
	int ypos;
	int targetX;
	int targetY;
	int mapSizeX;
	int mapSizeY;
	bool searchDirection;
	bool placed;
	int completedRuns;
	char robotIcon;
	string status;
	deque<pos> aStarPath;
	deque<vec2> unexploredCells;
	//vector< vector<char> > internalmap;

	void expandInternalmap(char direction);
	bool insideInternalmap(int x, int y);

	deque< deque<char> > internalmap;
	int worldPosX; //only var to outside coords
	int worldPosY; //only var to outside coords
	int worldToLocalDiffX;
	int worldToLocalDiffY;
	int numMoves;
	int numUnknownSquaresExplored;

	bool wallsChanged;
	bool mapExplored;  //afther first run
	int powerLVL;
	bool gotoPowerStation;
	int powerStationPosX;
	int powerStationPosY;

	bool handlePower();
	
};

void printRoom(Vacuum *therobot, deque< deque<char> > printRoom, bool printRight, int robotX, int RobotY);
void displayCmlStatus(Vacuum * robot, bool toFile);