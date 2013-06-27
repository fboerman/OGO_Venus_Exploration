#ifndef Mapping_h
#define Mapping_h
#include "Arduino.h"

class Mapping
{ 
public :
Mapping(int  a);
// Global Variables
int quadrant[20][20];//format quadrant[y][x]
int x;
int y;
char Directhor;
char Directhor_old;
char Directhor_old2;
int flag_override_steps;//if this flag is active the robot is returning over a previous taken path. this overrides a couple of security rules
char flag_cinematic;
int state;
char sidelook;
int y_old;
char Direction_old;
char Direction;
int check;
int x1, x2, y1, y2;//borders of the quadrant with x2>x1 && y2>y1
int flag_bottom_line;//is set to 1 if robot is at bottomline of quadrant
//the functions
int WriteStep();
int TurnAround();
int LookAround();
int DriveAroundHills(int front, int side);
void WriteMap(int front);
int CheckBoundary();
int Explorer(int front, int side);
int MapModule(int front, int side, int error, int outputcounter);
void init();
int Quadrant(int x,int y);
int CheckMap();
void WriteMapMountain();
char GetDirection();
};


#endif