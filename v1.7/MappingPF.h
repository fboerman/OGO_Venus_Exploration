#ifndef MappingPF_h
#define MappingPF_h
#include "Arduino.h"

typedef signed char int8_t;

typedef struct node {
  int Fvalue;
  int Gvalue;
  int8_t xcoord;
  int8_t ycoord;
  int8_t xparent;
  int8_t yparent;
  byte closed;
  char dirtoparent;
  node *nextnode;
};

typedef struct coords {
  int8_t xmove;
  int8_t ymove;
  char dirmove;
  coords *nextcoords;
};

class MappingPF
{ 
public :
MappingPF(int  a);
// Global Variables
int quadrant[20][20];//format quadrant[y][x]
int x;
int y;
char Directhor;
char Directhornew;
char Directhor_old;
char Directhor_old2;
int flag_override_steps;//if this flag is active the robot is returning over a previous taken path. this overrides a couple of security rules
char flag_cinematic;
int state;
char sidelook;
int y_old;
char Direction_old;
char Direction;
int x1, x2, y1, y2;//borders of the quadrant with x2>x1 && y2>y1
int flag_bottom_line;//is set to 1 if robot is at bottomline of quadrant
int xtemp;
int ytemp;

//From pathfinding
node *headnode;
coords *headcoords;
int8_t PFxgoal;
int8_t PFygoal;
int8_t PFxstart;
int8_t PFystart;

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
int CheckMap();
void WriteMapMountain();
char GetDirection();

//From PF
void deletelist();
char inversedir(char PFdirecthor);
void addtolist(int PFvalue,int8_t PFx,int8_t PFy,int8_t PFxpar,int8_t PFypar, char PFdirecthor);
int checkadjacent(int PFxnew,int PFynew,int PFx,int PFy,int PFG,int PFvalue,char PFdirecthor);
void evaluateadjacent(int8_t PFx,int8_t PFy,byte PFG);
void addtocoords(int8_t PFx,int8_t PFy,char PFdirecthor);
void createroute();
char printcoords();
int checkdiag(int PFx,int PFy,int PFxpar, int PFypar,char PFdirecthor,int PFfunctioncount,int PFGvalue);
int jumpadjacent(int8_t PFx,int8_t PFy,int8_t PFxpar, int8_t PFypar,char PFdirecthor,int PFfunctioncount,int PFGvalue);
void pathfinding(int8_t PFxs,int8_t PFys,int8_t PFxg,int8_t PFyg);
};


#endif

