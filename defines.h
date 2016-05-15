#pragma once


const int mapMinSizeX = 10;
const int mapMinSizeY = 10;

const int mapMaxRandX = 12;
const int mapMaxRandY = 10;

const int mapWallChance = 15;
const int mapPoopChance = 15;
const int mapRePoopChance = 5; //0 -> 1000%

const int robotMaxPower = 500; 
const int robotCriticalPower = 50;
const int powerChargedEachCycle = 8; 

const char mapWallChar = (char)219;
const char mapUnreachableChar = (char)178;
const char mapEmptyChar = '.';
const char mapPoopChar = 'P';
const char mapUnexploredChar = '?';
const char mapPowerStationChar = (char)206;
