#pragma once

#include <Windows.h>
#include <stdlib.h> 
#include <iostream>
#include <conio.h>
#include <SDL.h>
#include <SDL_image.h>

#include "divClasses.h"
#include "defines.h"

#include <fstream>

#include <chrono>
#include <random>


#include "Vacuum.h"
using namespace std;


char getMapCoordinateData(int x, int y);
void initializeRoom(Vacuum *therobot);
//void printRoom2(Vacuum *therobot, vector< vector<char> > theRoom2);
void printRoom(Vacuum *therobot, deque< deque<char> > printRoom, bool printRight, int robotX, int RobotY);
void roomUpdate();
bool removePoop(int x, int y);
void displayCmlStatus(Vacuum * robot, bool toFile);
void handleKeys(unsigned char key, bool &running, bool &pause);
void loopSomePoop();

int mapSizeX = 0;
int mapSizeY = 0;

int numCycles = 0;
int poopThisCycle = 0;
int poopamnt = 0;
deque< deque<char> > roomMap;


const int	windowWidth = 1200;
const int	windowHeight = 600;
const int	SCREEN_BPP = 32;
int blockSize = 25;

SDL_Texture * background = nullptr;
SDL_Texture * normalSquareTex = nullptr;
SDL_Texture * WallSquareTex = nullptr;
SDL_Texture * unreachableSquareTex = nullptr;
SDL_Texture * poopSquareTex = nullptr;
SDL_Texture * unexploredSquareTex = nullptr;
SDL_Texture * robotText = nullptr;
SDL_Texture * powerStationText = nullptr;


SDL_Window*	window = nullptr;
SDL_Renderer * render = nullptr;

//function for loading image and returning as texture
SDL_Texture * loadImage(string path, SDL_Renderer *ren){

	SDL_Surface * image = nullptr;
	image = IMG_Load(path.c_str());
	if (image == nullptr){
		std::cout << SDL_GetError() << std::endl;
		cin.get();
		return nullptr;
	}

	//convert from surface/BMO -> Texture
	SDL_Texture * texture = nullptr;
	texture = SDL_CreateTextureFromSurface(ren, image);
	SDL_FreeSurface(image);

	return texture;

}

//just a function for creating a SDL_Rect
SDL_Rect * createRect(int x, int y, int width, int heiht){

	SDL_Rect * rec = new SDL_Rect();
	rec->x = x;
	rec->y = y;
	rec->w = width;
	rec->h = heiht;

	return rec;

}

//function for adding texture to the screen, the last four parameters are there if you need to cut out a part of the original image, as for characters
void addTexture(int x, int y, int width, int heiht, SDL_Texture * texture, SDL_Renderer *render, int orgX = 0, int orgY = 0, int orgWidth = NULL, int orgHeiht = NULL){

	SDL_Rect * rec = createRect(x, y, width, heiht);
	SDL_Rect * orgRec = createRect(orgX, orgY, orgWidth, orgHeiht);

	if (width == NULL && heiht == NULL){ //gets texture data (width/height)
		SDL_QueryTexture(texture, NULL, NULL, &rec->w, &rec->h);

	}

	//if no rezizing of org image:
	if (orgWidth == NULL && orgHeiht == NULL){ //gets texture data (width/height)
		SDL_RenderCopy(render, texture, NULL, rec);
		return;
	}

	SDL_RenderCopy(render, texture, orgRec, rec);

	delete rec;
	delete orgRec;

}

bool initSDL(){

	if (SDL_Init(SDL_INIT_EVERYTHING) < 0) {
		cout << "fuck";
		cin.get();
		return false;
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

	//set anti-alias:
	SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, 16);

	//Create Window
	window = SDL_CreateWindow("Assignment 1 AI", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, windowWidth, windowHeight, SDL_WINDOW_SHOWN);

	render = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
	if (render == nullptr){
		std::cout << SDL_GetError() << std::endl;

		//prints supported renderdrivers if render fails
		for (int i = 0; i < SDL_GetNumRenderDrivers(); i++){

			SDL_RendererInfo drinfo;
			SDL_GetRenderDriverInfo(i, &drinfo);

			printf("Driver name: %s \n", drinfo.name);

		}

		cin.get();
		return 1;
	}


	//clear everything!
	SDL_RenderClear(render);

	//load the textures:
	background = loadImage("bg.jpg", render);

	normalSquareTex = loadImage("IMGS2/Tiles/liquidWater.png", render);
	WallSquareTex = loadImage("IMGS2/Tiles/castleCenter.png", render);
	poopSquareTex = loadImage("IMGS2/Tiles/sandCenter.png", render);
	unreachableSquareTex = loadImage("IMGS2/Tiles/lock_green.png", render);
	unexploredSquareTex = loadImage("IMGS2/Tiles/boxItemAlt_disabled.png", render);
	robotText = loadImage("IMGS2/Player/p1_front.png", render);
	powerStationText = loadImage("IMGS2/Tiles/tochLit.png", render);

	//and display 
	addTexture(0, 0, windowWidth, windowHeight, background, render);

	SDL_RenderPresent(render);

	return true;

}


//Om SDL er includa MÅ MAIN HA DE TO PARAMTERENE
int main(int numArgs, char* argv[]) {


	initSDL();
	srand(time(NULL));

	bool running = true;
	bool pause = false;
	SDL_Event sdlEvent;
	
	mapSizeX = mapMinSizeX + rand() % mapMaxRandX;
	mapSizeY = mapMinSizeY + rand() % mapMaxRandY;

	//mapSizeX = 6;
	//mapSizeY = 6;

	blockSize = ((windowWidth - 10) / 2) / (mapSizeX+2);
	if (windowHeight / (mapSizeY + 2) < blockSize)
		blockSize = windowHeight / (mapSizeY+2);


	deque< deque<char> > roomTemp(mapSizeX, deque<char>(mapSizeY));
	roomMap = roomTemp;

	//cout << roomMap.size() << " " << roomMap[0].size();

	Vacuum *robot = new Vacuum(mapSizeX, mapSizeY, rand() % mapSizeX, rand() % mapSizeY);
	initializeRoom(robot);

	//roomMap[1][1] = (char)219;
	//roomMap[0][1] = (char)219;
	//roomMap[1][0] = (char)219;


	//clear logfile:
	
	std::ofstream outfile;

	outfile.open("log.txt", std::ios_base::trunc);
	outfile << "Log  for run on map of sizze: " << mapSizeX << "x" << mapSizeY << endl << endl;


	while (running) {
		SDL_RenderClear(render);
		addTexture(0, 0, windowWidth, windowHeight, background, render);

		while (SDL_PollEvent(&sdlEvent)) {
			if (sdlEvent.type == SDL_QUIT) {
				return sdlEvent.type;
				running = false;
			}

			else if (sdlEvent.type == SDL_KEYDOWN) {
				//If keyboard button was pressed
				handleKeys((unsigned char)sdlEvent.key.keysym.sym, running, pause);
			}

			else if (sdlEvent.type == SDL_MOUSEBUTTONDOWN) {
				int x = 0, y = 0;
				SDL_GetMouseState(&x, &y);
				int whichX = x / blockSize;
				int whichY = y / blockSize;

				if (sdlEvent.button.button == SDL_BUTTON_LEFT) {
					if (insideMap(whichX, whichY)) {
						if (roomMap[whichX][whichY] == mapWallChar){
							roomMap[whichX][whichY] = mapEmptyChar;
								robot->wallsChanged = true;
						}
						else{
							roomMap[whichX][whichY] = mapWallChar;
						}
						
					}
				}else if (sdlEvent.button.button == SDL_BUTTON_RIGHT) {
					if (insideMap(whichX, whichY)) {
						roomMap[whichX][whichY] = (roomMap[whichX][whichY] == mapPoopChar) ? mapEmptyChar : mapPoopChar;

					}
				}
			}
		}
		
		if (!pause) {
			system("CLS");
			robot->performCycle(roomMap);
			numCycles++;
			displayCmlStatus(robot, false);

			loopSomePoop();

			//if (robot->status.size() > 0)
			//	Sleep(1000); //in milliseconds
			//else
			//	Sleep(200);

			//if (robot->status.size() > 0)
			//	Sleep(500); //in milliseconds
			//else
			//	Sleep(50);

			robot->status = "";
		}

		printRoom(robot, roomMap, false, robot->worldPosX, robot->worldPosY);
		robot->printInternalMap();
		SDL_RenderPresent(render);
	}

	return 0;
}

void displayCmlStatus(Vacuum * robot, bool toFile){

	cout << "Poops on map: " << poopamnt << endl;
	cout << "robot->status: " << robot->status << endl;
	cout << "Completed runs: " << robot->completedRuns << endl;
	cout << "Num Moves:" << robot->numMoves << endl;
	cout << "Num Cycles: " << numCycles << endl;
	cout << "Num Unknown Squares Explored: " << robot->numUnknownSquaresExplored << endl;
	cout << "Power LVL: " << robot->powerLVL << endl;
	cout << "Efficiency: " << (float)((float)robot->numUnknownSquaresExplored) / (float)robot->numMoves * 100 << "%" <<  endl;

	if (toFile){
		std::ofstream outfile;

		outfile.open("log.txt", std::ios_base::app);
		outfile << "Poops on map: " << poopamnt << endl;
		outfile << "robot->status: " << robot->status << endl;
		outfile << "Completed runs: " << robot->completedRuns << endl;
		outfile << "Num Moves:" << robot->numMoves << endl;
		outfile << "Num Cycles: " << numCycles << endl;
		outfile << "Num Unknown Cells Explored: " << robot->numUnknownSquaresExplored << endl;
		outfile << "Power LVL: " << robot->powerLVL << endl;
		outfile << "Efficiency: " << (float)((float)robot->numUnknownSquaresExplored) / (float)robot->numMoves * 100 << "%" << endl;
		outfile << endl;

	}
	

}

char getMapCoordinateData(int x, int y) {
	if (x < 0 || y < 0 || x >= mapSizeX || y >= mapSizeY) {
		return mapWallChar;
	}
	return roomMap[x][y];
}

void initializeRoom(Vacuum *therobot) {

	poopThisCycle = 0;

	int wallamnt = 0;
	for (int y = 0; y < mapSizeY; y++) {
		for (int x = 0; x < mapSizeX; x++) {

			roomMap[x][y] = '.';

			if (!(x == therobot->worldPosX && y == therobot->worldPosY)){

				if ((rand() % 101) >= (100 - mapWallChance)) {
					roomMap[x][y] = mapWallChar;
					wallamnt++;
				}
				else if ((rand() % 101) >= (100 - mapPoopChance)) {
					roomMap[x][y] = mapPoopChar;
					poopThisCycle++;
					poopamnt++;
				}
			}else
				roomMap[x][y] = mapPowerStationChar; //add charging station
		}
	}

}

//add some random poop while running
void loopSomePoop(){

	std::default_random_engine generator(time(NULL));

	//random number between 0 and 1
	std::uniform_real_distribution<float> distribution(0.0, 1.0);

	if ((rand() % 1001) >= (1000 - mapRePoopChance) && poopamnt < poopThisCycle){

		bool poopPlaced = false;
		while (!poopPlaced){

			int x = mapSizeX * distribution(generator);
			int y = mapSizeY * distribution(generator);
			if (roomMap[x][y] == mapEmptyChar){
				roomMap[x][y] = mapPoopChar;
				poopPlaced = true;
				poopamnt++;
			}
			cout << "LETER";

		}


	}


}

void roomUpdate() {

	poopThisCycle = 0;

	for (int y = 0; y < mapSizeY; y++) {
		for (int x = 0; x < mapSizeX; x++) {
			if ((rand() % 101) >= (100 - mapPoopChance) && roomMap[x][y] == '.') {
				roomMap[x][y] = 'P';
				poopThisCycle++;
				poopamnt++;
			}
		}
	}
	numCycles = 0;
}


void printRoom(Vacuum *therobot, deque< deque<char> > printRoom, bool printRight, int robotX, int RobotY) {

	int offsetX = 0;
	if (printRight)
		offsetX = windowWidth / 2;


	for (int y = 0; y < printRoom[0].size(); y++){
		for (int x = 0; x < printRoom.size(); x++) {

			
			
			if (printRoom[x][y] == mapEmptyChar)
				addTexture(blockSize * x + offsetX, blockSize * y, blockSize, blockSize, normalSquareTex, render);
			if (printRoom[x][y] == mapWallChar)
				addTexture(blockSize * x + offsetX, blockSize * y, blockSize, blockSize, WallSquareTex, render);
			if (printRoom[x][y] == mapPoopChar)
				addTexture(blockSize * x + offsetX, blockSize * y, blockSize, blockSize, poopSquareTex, render);
			
			if (printRoom[x][y] == mapUnreachableChar)
				addTexture(blockSize * x + offsetX, blockSize * y, blockSize, blockSize, unreachableSquareTex, render);

			if (printRoom[x][y] == mapUnexploredChar)
				addTexture(blockSize * x + offsetX, blockSize * y, blockSize, blockSize, unexploredSquareTex, render);

			if (printRoom[x][y] == mapPowerStationChar){
				addTexture(blockSize * x + offsetX, blockSize * y, blockSize, blockSize, normalSquareTex, render);
				addTexture(blockSize * x + offsetX, blockSize * y, blockSize, blockSize, powerStationText, render);
			}

			

			if (x == robotX && y == RobotY) {
				//cout << therobot->robotIcon << ' ';
				addTexture(blockSize * x + offsetX, blockSize * y, blockSize, blockSize, robotText, render);

			}
			else {
				//cout << printRoom[x][y] << ' ';
			}

		}
		//cout << endl;
	}
}

bool insideMap(int xpos, int ypos) {
	if (xpos < 0 || ypos < 0 || xpos >= mapSizeX || ypos >= mapSizeY) {
		return false;
	}
	return true;
}

bool removePoop(int x, int y){

	if (!insideMap(x, y))
		return false;

	if (roomMap[x][y] == 'P'){
		roomMap[x][y] = '.';
		poopamnt--;
		return true;
	}
	return false;
}

void handleKeys(unsigned char key, bool &running, bool &pause)
{	//If escape is pressed
	if (key == 27) {
		running = false;
	}
	if (key == ' ') {
		pause = !pause;
	}
}
