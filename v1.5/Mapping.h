#ifndef Mapping_h
#define Mapping_h
#include "Arduino.h"

class Mapping
{ 
public :
Mapping();
// Global Variables
int quadrant[30][30];//format quadrant[y][x]
int x;
int y;
char directhor;
int flag_override_steps;//if this flag is active the robot is returning over a previous taken path. this overrides a couple of security rules
char flag_cinematic;
int state;
char sidelook;
int y_old;
char direction_old;
int x1, x2, y1, y2;//borders of the quadrant with x2>x1 && y2>y1
int flag_bottom_line;//is set to 1 if robot is at bottomline of quadrant
//the functions
int WriteStep();
int TurnAround();
int CheckAround();
int DriveAroundHills(int front, int side);
void WriteMap(int front);
int CheckBoundary();
int Explorer(int front, int side);
int MapModule(int front, char side, int error);

};


#endif