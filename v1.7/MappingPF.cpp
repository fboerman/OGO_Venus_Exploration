//copyright Williewonka 2013
//version 1.5(arduino implementation)

#include "Arduino.h"
#include "MappingPF.h"

//map legenda
//IMPASSEBLE 1;
//STEP 2;
//EMPTY 3;
//BOUNDARY 4;
//
//INPUT FRONT 
//line 1
//mountain 2
//empty 3
//rock 4
//BELOW ARE NOT REAL
//boundary 5
//step 6
//
//Direction legenda:
//f = forward
//l = turn left and then forward
//r = turn right and then forward
//b = do 180 turn
//
//states
//NOSTATE 0;
//TURNAROUND 1;
//PATHFINDING 2;
//DRIVEAROUND1 3; drive down
//DRIVEAROUND2 4; drive under the mountain
//DRIVEAROUND3 5; drive reverse down
//DRIVEAROUND4 6; drive reverse
//DRIVEAROUND 7; drive up


//input side
//mountain 1
//no mountain 0
//measurement after driving!

MappingPF::MappingPF(int a)
{
	x = 1;
	y = 1;
	y1 = 0;
	y2 = 19;
	x1 = 0;
	x2 = 19;
	Directhor = 'r';
	flag_override_steps = 0;
	flag_cinematic = 'n';
	state = 0;
	sidelook = 'n';
	flag_bottom_line = 0;
	Directhor_old = 'r';
	Direction = 'r';
	Directhornew = NULL;
}

void MappingPF::init()
{
	int i,j;
	int n = 20;
	for(i = 0;i<n;i++)
	{
		if(i == 0 || i == (n-1))
		{
			for(j=0;j<n;j++)
			{
				quadrant[i][j] = 4;
			}
		}else
		{
			for(j = 0;j<n;j++)
			{
				if(j == 0 || j == (n-1))
				{
					quadrant[i][j] = 4;
				}else
				{
					quadrant[i][j] = 3;
				}
			}
		}
	}
}

int MappingPF::WriteStep()//moves the robot one step in the set Directhors, will return false if move is not possible
{
	int x_old = x;
	int y_old = y;

	switch(Directhor)//adjust x en y coordinates aan de hand van de Directhor
	{
	case 'r':
		x++;
		break;
	case 'd':
		y++;
		break;
	case 'l':
		x--;
		break;
	case 'u':
		y--;
		break;
	}
	if(quadrant[y][x] == 3 || ( quadrant [y][x] == 2 && flag_override_steps))//checks if the movement is possible, if so then return false
	{
		quadrant[y][x] = 2;
		
		return 0;
	}
	else//if not restore the old coordinates and return true
	{
		x = x_old;
		y = y_old;
		return 1;
	}
	
}

int MappingPF::TurnAround()//lets the robot turn 180 degrees while shifting row
{
	sidelook = 'n';
	if(state == 0)
	{
		Directhor_old2 = Directhor;
	}
	switch(Directhor_old2)
	{
	case 'r':
		switch(state)
		{
		case 1://phase 2
			Directhor = 'l';
			if(WriteStep())
			{
				if(LookAround())
				{
					flag_override_steps = 1;
					return 0;
				}else
				{
					WriteStep();
				}
			}
			state = 0;
			return 99;
			break;
		case 0://no state
			Directhor = 'd';
			if(WriteStep()){//if the move failes twice it means that the turn failed and the only way back is the way we came. so activate the override flag
				Directhor = 'l';
				if(WriteStep()){
					flag_override_steps = 1;
					return 0;
				}
			}else
			{
				state = 1;
				return 99;
			}
			break;
		}
		break;
	case 'd'://2 possibilities but orientation is to turn to the right so Directhor is left
		switch(state)
		{
		case 1://phase 2
			Directhor = 'u';
			if(WriteStep())
			{
				if(LookAround())
				{
					flag_override_steps = 1;
					return 0;
				}else
				{
					WriteStep();
				}
			}
			state = 0;
			return 99;
			break;
		case 0://no state
			Directhor = 'l';
			if(WriteStep()){//if the move failes twice it means that the turn failed and the only way back is the way we came. so activate the override flag
				Directhor = 'u';
				if(WriteStep()){
					flag_override_steps = 1;
					return 0;
				}
			}else
			{
				state = 1;
				return 99;
			}
			break;
		}
		break;
	case 'l':
		switch(state)
		{
		case 1://phase 2
			Directhor = 'r';
			if(WriteStep())
			{
				if(LookAround())
				{
					flag_override_steps = 1;
					return 0;
				}else
				{
					WriteStep();
				}
			}
			state = 0;
			return 99;
			break;
		case 0://no state
			Directhor = 'd';
			if(WriteStep()){//if the move failes twice it means that the turn failed and the only way back is the way we came. so activate the override flag
				Directhor = 'r';
				if(WriteStep()){
					flag_override_steps = 1;
					return 0;
				}
			}else
			{
				state = 1;
				return 99;
			}
			break;
		}
		break;
	case 'u':

		if(quadrant[y][x-1] == 3)
		{
			Directhor = 'l';
		}else
		{
			if(quadrant[y][x+1] == 3)
			{
				Directhor = 'r';
			}
		}
		if(WriteStep()){
			flag_override_steps = 1;
			Directhor = 'd';
			WriteStep();
			return 99;
		}else
		{
			return 99;
		}
		break;
	}
}

int MappingPF::LookAround() //check around the current position to look if there is a square wich it hasnt been yet, return 0 if there is and 1 otherwise
{
	if(quadrant[y][x-1] == 3)
	{
		Directhor = 'l';
		return 0;
	}
	if(quadrant[y][x+1] == 3)
	{
		Directhor = 'r';
		return 0;
	}
	if(quadrant[y-1][x] == 3)
	{
		Directhor = 'u';
		return 0;
	}
	if(quadrant[y+1][x] == 3)
	{
		Directhor = 'd';
		return 0;
	}
	return 1;
}

int MappingPF::DriveAroundHills(int front, int side)//lets the robot drive around an hill
{
	switch(state)
	{
	case 0://nostate
		switch(Directhor)
		{
		case 'l':
			sidelook = 'r';
			break;
		case 'r':
			sidelook = 'l';
			break;
		}
		Directhor = 'd';
		state = 3;
		y_old = y;
		WriteStep();
		return 99;
		break;
	case 3://phase 1
		if(!side)
		{
			switch(sidelook)
			{
			case 'r':
				Directhor = 'l';
				break;
			case 'l':
				Directhor = 'r';
			}
			state = 4;
		}
		WriteStep();
		return 99;
		break;
	case 4://phase 2
		if(!side)
		{
			Directhor = 'u';
			state = 0;
		}
		WriteStep();
		return 99;
		break;
	}
}

void MappingPF::WriteMap(int front)
{
	int x_old = x;
	int y_old = y;
	switch(Directhor)
	{
	case 'r':
		x++;
		break;
	case 'd':
		y++;
		break;
	case 'l':
		x--;
		break;
	case 'u':
		y--;
		break;
	}
	if(front == 1 || front == 2)
	{
		quadrant[y][x] = 1;
	}

	y = y_old;
	x = x_old;

	return;
}

int MappingPF::CheckBoundary()
{
	flag_bottom_line = 0;
	switch (Directhor)
	{
	case 'u':
		if((y - 1) == y1)
		{
			return 1;
		}
		break;
	case 'd':
		if((y + 1) == y2)
		{
			return 1;
		}
		break;
	case 'r':
		if((x + 1) == x2)
		{
			if((y + 1) == y2)
			{
				flag_bottom_line = 1;
			}
			return 1;
		}
		break;
	case 'l':
		if((x - 1) == x1)
		{
			if((y + 1) == y2)
			{
				flag_bottom_line = 1;
			}
			return 1;	
		}
		break;
	}
	return 0;
}

int MappingPF::Explorer(int front, int side) //drives the robot when there is no special state
{
		if(quadrant[y-1][x] == 3)//check if going up is possible
		{
			Directhor = 'u';
			flag_override_steps = 0;
			WriteStep();
			return 99;
		}
		if(Directhor == 'd')//if going down then see if left or right is possibility
		{
			if(quadrant[y][x+1] == 3)
			{
				Directhor = 'r';
			}else
			{
				if(quadrant[y][x-1] == 3)
				{
					Directhor = 'l';
				}
			}
		}
		if(flag_override_steps){//check if the override step is still necesary
			if(quadrant[y][x+1] == 3){
				flag_override_steps = 0;
				Directhor = 'r';
				return 99;
			}else{
				if(quadrant[y][x-1] == 3){
					flag_override_steps = 0;
					Directhor = 'l';
					return 99;
				}
			}
		}

		if(flag_override_steps)
		{
			if(quadrant[y+1][x] != 1)
			{
				Directhor = 'd';
					if(!WriteStep())
					{
						return 99;
					}
			}
			else
			{
				switch(front)
				{
				case 5:
				case 2:
				case 1:
					if(flag_bottom_line)
					{
						return 0;
					}else
					{
						TurnAround();
					}

					break;
				default:
					if(!WriteStep())
					{
						return 99;
					}
					break;
				}
			}
		}else
		{
			switch(front)//using this info the actual algorithm will choose what to do
			{
			case 3:
				//free space at front so move forward
				if(!WriteStep())
				{
					return 99;
				}
				break;
			case 2:
				//mountain in front of robot so initiate driving around
				switch(Directhor)
				{
				case 'u':
					Directhor = 'r';
					state = 4;
					WriteStep();
					return 99;
					break;
				case 'd':
					TurnAround();
					return 99;
					break;
				}
				DriveAroundHills(front,side);
				return 99;
				break;
			case 5:
				//end of quadrant so turn around
				if(quadrant[y+1][x] == 4) //check if quadrant line is beneath, then quadrant has finished
				{
					return 0;
				}else
				{
					TurnAround();
					return 99;
				}		
				break;
			case 1:
				//a line in front of it, turn around
				if(quadrant[y+1][x] == 5)
				{
					return 0;
				}else
				{
					TurnAround();
					return 99;
				}
				
				break;
			case 6:
				//previous step
					TurnAround();
					return 99;
				break;
			}
		}

}

int MappingPF::CheckMap()//returns what is on the map of the next square
{
	switch(Directhor)
	{
	case 'd':
		return quadrant[y+1][x];
		break;
	case 'u':
		return quadrant[y-1][x];
		break;
	case 'l':
		return quadrant[y][x-1];
		break;
	case 'r':
		return quadrant[y][x+1];
		break;
	}
	return 0;
}

void MappingPF::WriteMapMountain()
{
	switch(Directhor)
	{
	case 'r':
	case 'l':
		quadrant[y-1][x] = 1;
		break;

	case 'u':
		switch(sidelook)
		{
		case 'r':
			quadrant[y][x+1] = 1;
			break;
		case 'l':
			quadrant[y][x-1] = 1;
			break;
		}
		break;

	case 'd':
		switch(sidelook)
		{
			case 'r':
				quadrant[y][x-1] = 1;
			break;
			case 'l':
				quadrant[y][x+1] = 1;
			break;
		}
		break;
	}
}

char MappingPF::GetDirection()//returns the direction relative to the current setting of the robot
{
	char result;

	switch(Directhor_old)
	{
	case 'r':
		switch(Directhor)
		{
			case 'r':
				result = 'f';
				break;

			case 'l':
				result = 'b';
				break;

			case 'u':
				result = 'l';
				break;

			case 'd':
				result = 'r';
				break;
		}

		break;
	case 'l':
		switch(Directhor)
		{
			case 'r':
				result = 'b';
				break;

			case 'l':
				result = 'f';
				break;

			case 'u':
				result = 'r';
				break;

			case 'd':
				result = 'l';
				break;
		}

		break;
	case 'u':
		switch(Directhor)
		{
			case 'r':
				result = 'r';
				break;

			case 'l':
				result = 'l';
				break;

			case 'u':
				result = 'f';
				break;

			case 'd':
				result = 'b';
				break;
		}

		break;
	case 'd':
		switch(Directhor)
		{
			case 'r':
				result = 'l';
				break;

			case 'l':
				result = 'r';
				break;

			case 'u':
				result = 'b';
				break;

			case 'd':
				result = 'f';
				break;
		}

		break;
	}
	return result;
}

int MappingPF::MapModule(int front, int side, int error, int outputcounter) //sets the Directhor in global variable and updates the map,  returns steps if succesful, 0 if quadrant has ended
{
	int returning;

	error = !error;

	if(CheckBoundary())
	{
		front = 5;
	}
	else
	{
		if(CheckMap() != 3)
		{
			front = CheckMap();
		}
	}
	
	if(error)
	{
		//what to do with map if there is an error
		switch(Directhor)
		{
		case 'r':
			x--;
			break;

		case 'l':
			x++;
			break;

		case 'd':
			y--;
			break;

		case 'u':
			y++;
			break;
		}
		Directhor = Directhor_old;
	}else
	{
		Directhor_old = Directhor;
	}

	if(front != 5 && front != 4)
	{
		WriteMap(front);
	}

	if(sidelook != 'n' && side == 1)//write the mountain to map if the robot is looking at it and sees a mountain
	{
		WriteMapMountain();
	}
	
	switch(state)
	{
	case 2:
		{
			//pathfinding
			if(Directhornew!=NULL) {
				Directhor=Directhornew;
				Directhornew=NULL;
				return 99;
			}

			char next_turn = printcoords();
			if(next_turn == NULL)
			{
				switch(state)
				{
				case 2:
					pathfinding(0,0,xtemp,ytemp);
					return MapModule(3,0,1,0);
					break;
				case 5:
					state = 0;
					return MapModule(3,0,1,0);
					break;
				}
				state = 5;

			}

			switch(next_turn)
			{
			case 'l':
			case 'r':
			case 'u':
			case 'd':
				Directhor = next_turn;
				return 99;
			case 'c':
				if(quadrant[y][x+1]==1) {
					Directhor='d';
					Directhornew='r';
				}
				else {
					Directhor='r';
					Directhornew='d';
				}
				break;
			case 'z':
				if(quadrant[y+1][x]==1) {
					Directhor='l';
					Directhornew='d';
				}

				else {
					Directhor='d';
					Directhornew='l';
				}
				break;
			case 'q': 
				if(quadrant[y][x-1]==1) {
					Directhor='u';
					Directhornew='l';
				}
				else {
					Directhor='l';
					Directhornew='u';
				}
				break;
			case 'e':
				if(quadrant[y][x+1]==1) {
					Directhor='u';
					Directhornew='r';
				}

				else {
					Directhor='r';
					Directhornew='u';
				}
				break;

			}
		}
		break;
	case 3:
	case 4:
		{
			if(error)
			{
				state = 0;
				TurnAround();
			}

			DriveAroundHills(front,side);
			return 99;
		}
		break;
	case 1:
		{
			returning = TurnAround();
			if(!returning)//check if turnaround is possible without overriding steps, if not then the function has activated the flag and the whole procedure needs to be rerun.
			{
				return MapModule(front,side,error,outputcounter);
			}else
			{
				return returning;
			}
		}
		break;
	case 0://no state
		{
			return Explorer(front,side);//for no state the robot will just go exploring, this also applies for when the override flag is active
		}
		break;
	}
}

void MappingPF::deletelist() {
  node *temp;                  
  temp = (node*)malloc(sizeof(node));

  while(headnode!=NULL) {
    temp = headnode;
    headnode = temp->nextnode;
    free(temp);
  }
}

char MappingPF::inversedir(char PFdirecthor) {
  switch(PFdirecthor) {
  case 'l':
    PFdirecthor='r';
    break;
  case 'r':
    PFdirecthor='l';
    break;
  case 'u':
    PFdirecthor='d';
    break;
  case 'd':
    PFdirecthor='u';
    break;
  case 'e':
    PFdirecthor='z';
    break;
  case 'z':
    PFdirecthor='e';
    break;
  case 'q':
    PFdirecthor='c';
    break;
  case 'c':
    PFdirecthor='q';
    break;
  }
  return PFdirecthor;
}

void MappingPF::addtolist(int PFvalue,int8_t PFx,int8_t PFy,int8_t PFxpar,int8_t PFypar, char PFdirecthor) { //Add the nodes to the list, save the appropriate F,G and H value, coordinates and coordinates of the "parent"
  node *temp;
  int PFdx;
  int PFdy;
  int PFHvalue;
  temp=(node *)malloc(sizeof(node));
  if (headnode== NULL) {
    headnode=temp;
    headnode->nextnode=NULL;
  }
  else {
    temp->nextnode=headnode;
    headnode=temp;
  }

  temp->Gvalue=PFvalue;
  PFdx=abs(PFx-PFxgoal)*10;
  PFdy=abs(PFy-PFygoal)*10;
  PFHvalue=(PFdx+PFdy);
  temp->Fvalue=temp->Gvalue+PFHvalue;
  temp->xcoord=PFx;
  temp->ycoord=PFy;
  temp->xparent=PFxpar;
  temp->yparent=PFypar;
  temp->dirtoparent=inversedir(PFdirecthor);
  temp->closed=0; //All nodes added to the list should be evaluated further, closed=1 if the it has been evaluated

}

int MappingPF::checkadjacent(int PFxnew,int PFynew,int PFx,int PFy,int PFG,int PFvalue,char PFdirecthor) { //returns 2 if the node that should be added is already in the list, else a 1.
	node *temp1;
	temp1=headnode;
	int PFdx;int PFdy;int PFHvalue;
	while(temp1!=NULL) {
			if(temp1->xcoord==PFxnew &&temp1->ycoord==PFynew) {
				//printf("Already exists\n");
				if((PFG+PFvalue)<temp1->Gvalue) {
					//printf("G is smaller than before\n"); 
					temp1->xparent=PFx;
					temp1->yparent=PFy;
					temp1->Gvalue=PFG+PFvalue;
					PFdx=abs(PFxnew-PFxgoal)*10;
					PFdy=abs(PFynew-PFygoal)*10;
					PFHvalue=(PFdx+PFdy);
					temp1->Fvalue=temp1->Gvalue+PFHvalue;
					temp1->closed=0;
					return 1;
				}
				return 2;
			}
			
			temp1 = temp1->nextnode;
		}
	addtolist(PFG+PFvalue,PFxnew,PFynew,PFx,PFy,PFdirecthor);
	return 1;
}

void MappingPF::evaluateadjacent(int8_t PFx,int8_t PFy,byte PFG) {

  int8_t PFxnew=PFx;
  int8_t PFynew=PFy-1; //upper square
  if(quadrant[PFynew][PFxnew]==2) 
  {
    checkadjacent(PFxnew,PFynew,PFx,PFy,PFG,10,'u');	
  }
  PFxnew=PFxnew+1; //upper right square
  if(quadrant[PFynew][PFxnew]==2) {
    checkadjacent(PFxnew,PFynew,PFx,PFy,PFG,14,'e');	
  }
  PFynew=PFynew+1; //right square
  if(quadrant[PFynew][PFxnew]==2) {
    checkadjacent(PFxnew,PFynew,PFx,PFy,PFG,10,'r');	
  }
  PFynew=PFynew+1; //lower right square
  if(quadrant[PFynew][PFxnew]==2) {
    checkadjacent(PFxnew,PFynew,PFx,PFy,PFG,14,'c');
  }
  PFxnew=PFxnew-1; //lower square
  if(quadrant[PFynew][PFxnew]==2) {
    checkadjacent(PFxnew,PFynew,PFx,PFy,PFG,10,'d');	
  }
  PFxnew=PFxnew-1; //lower left square
  if(quadrant[PFynew][PFxnew]==2) {
    checkadjacent(PFxnew,PFynew,PFx,PFy,PFG,14,'z');
  }
  PFynew=PFynew-1; //left square
  if(quadrant[PFynew][PFxnew]==2) {
    checkadjacent(PFxnew,PFynew,PFx,PFy,PFG,10,'l');	
  }
  PFynew=PFynew-1; //upper left square
  if(quadrant[PFynew][PFxnew]==2) {
    checkadjacent(PFxnew,PFynew,PFx,PFy,PFG,14,'q');
  }
}

void MappingPF::addtocoords(int8_t PFx,int8_t PFy,char PFdirecthor) {
	coords *temp;
	coords *temp1;
		temp=headcoords;
		while(temp!=NULL) {
			if(temp->xmove==PFx &&temp->ymove==PFy) {
				return;
			}
			temp = temp->nextcoords;
		}
    temp1=(coords *)malloc(sizeof(coords));
	if (headcoords== NULL) {
		headcoords=temp1;
		headcoords->nextcoords=NULL;
    }
    else {
		temp1->nextcoords=headcoords;
		headcoords=temp1;
    }

		temp1->dirmove=inversedir(PFdirecthor);
		temp1->xmove=PFx;
		temp1->ymove=PFy;

}

void MappingPF::createroute() {
  node *tempnode;
  char PFdirtopar;
  int8_t PFx=PFxgoal;
  int8_t PFy=PFygoal;

	while(!(PFx==PFxstart&&PFy==PFystart)) {
		tempnode=headnode;
		while(tempnode!=NULL) {
		  if(tempnode->xcoord==PFx&&tempnode->ycoord==PFy) {
			PFdirtopar=tempnode->dirtoparent;
			addtocoords(PFx,PFy,PFdirtopar);
			break;
		  }
		tempnode = tempnode->nextnode;
	}

    while(!(PFx==tempnode->xparent&&PFy==tempnode->yparent)) {
      addtocoords(PFx,PFy,PFdirtopar);
      quadrant[PFy][PFx]=5;
      switch(PFdirtopar) {
      case 'r':
        PFx=PFx+1;
        break;
      case 'u':
        PFy=PFy-1;
        break;
      case 'd':
        PFy=PFy+1;
        break;
      case 'l':
        PFx=PFx-1;
        break;
      case 'q':
        PFx=PFx-1;
        PFy=PFy-1;
        break;
      case 'e':
        PFx=PFx+1;
        PFy=PFy-1;
        break;
      case 'c':
        PFx=PFx+1;
        PFy=PFy+1;
        break;
      case 'z':
        PFx=PFx-1;
        PFy=PFy+1;
        break;
      }

    }
  }
}

char MappingPF::printcoords() {
	coords *temp=headcoords;
char PFnextdir;
if(headcoords==NULL) {
  return NULL;
}

headcoords=temp->nextcoords;
PFnextdir=temp->dirmove;
free(temp);
return PFnextdir;

}

int MappingPF::checkdiag(int PFx,int PFy,int PFxpar, int PFypar,char PFdirecthor,int PFfunctioncount,int PFGvalue) {
  switch(PFdirecthor) {
  case 'q':
    if(quadrant[PFy-1][PFx-1]!=3 || (quadrant[PFy][PFx-1]!=3 && quadrant[PFy-1][PFx]!=3))
      return 2;
    else
      return jumpadjacent((PFx-1),(y-1),PFxpar,PFypar,'q',PFfunctioncount,PFGvalue);
  case 'e':
    if(quadrant[PFy-1][PFx+1]!=3 || (quadrant[PFy-1][PFx]!=3 && quadrant[PFy][PFx+1]!=3))
      return 2;	
    else
      return jumpadjacent(PFx+1,(PFy-1),PFxpar,PFypar,'e',PFfunctioncount,PFGvalue);

  case 'c':
    if(quadrant[PFy+1][PFx+1]!=3 || (quadrant[PFy+1][PFx]!=3 && quadrant[PFy][PFx+1]!=3))
      return 2;
    else
      return jumpadjacent(PFx+1,(PFy+1),PFxpar,PFypar,'c',PFfunctioncount,PFGvalue);

  case 'z':
    if(quadrant[PFy+1][PFx-1]!=3 || (quadrant[PFy][PFx-1]!=3 && quadrant[PFy+1][PFx]!=3))
      return 2;
    else
      return jumpadjacent((PFx-1),(PFy+1),PFxpar,PFypar,'z',PFfunctioncount,PFGvalue);
  }


}

int MappingPF::jumpadjacent(int8_t PFx,int8_t PFy,int8_t PFxpar, int8_t PFypar,char PFdirecthor,int PFfunctioncount,int PFGvalue) { //return 1 if JP found, 2 if not (dead end), 3 if goal is reached, 4 if node can be moved to the closed list (hor,ver and diag don't give a result)
  byte resulthor,resultver,resulthor2, resultver2, resultcheck,checkdiag1,checkdiag2, checkdiag3;
  if(PFx==PFxgoal&&PFy==PFygoal) {
    checkadjacent(PFx,PFy,PFxpar,PFypar,PFGvalue,PFfunctioncount*10,PFdirecthor);
    return 3;
  }

  if(quadrant[PFy][PFx]==1)
    return 2;

  PFfunctioncount++;

  switch(PFdirecthor) {
  case 'r':
    if(quadrant[PFy][PFx+1]==1)
      return 2;
    resulthor=jumpadjacent((PFx+1),PFy,PFxpar,PFypar,'r',PFfunctioncount,PFGvalue); //check current directhor

    if(PFfunctioncount==0) { //If PFfunctioncount=0, the current point is saved in the linked list alreadPFy, meaning the diagonals should be checked as well.
      checkdiag1=checkdiag(PFx,PFy,PFxpar,PFypar,'e',PFfunctioncount,PFGvalue);
      checkdiag2=checkdiag(PFx,PFy,PFxpar,PFypar,'c',PFfunctioncount,PFGvalue);

      if(checkdiag1==1 || checkdiag2==1) //If one of these diagonals returns a one, a jumppoint is found, meaning a 1 should be returned
        return 1;
    }
    if((quadrant[PFy-1][PFx]==1&&quadrant[PFy-1][PFx+1]==0) || (quadrant[PFy+1][PFx]==1 && quadrant[PFy+1][PFx+1]==0)) {
      resultcheck= checkadjacent(PFx,PFy,PFxpar,PFypar,PFGvalue,PFfunctioncount*10,'r');
      if(resultcheck==2&&PFfunctioncount==0)
        return 4;
      if(resultcheck==2&&PFfunctioncount!=0)
        return 2;
      else return 1;
    }

    return resulthor;

  case 'l':
    if(quadrant[PFy][PFx-1]==1)
      return 2;
    resulthor=jumpadjacent((PFx-1),PFy,PFxpar,PFypar,'l',PFfunctioncount,PFGvalue);

    if(PFfunctioncount==0) {
      checkdiag1=checkdiag(PFx,PFy,PFxpar,PFypar,'q',PFfunctioncount,PFGvalue);
      checkdiag2=checkdiag(PFx,PFy,PFxpar,PFypar,'z',PFfunctioncount,PFGvalue);
      if(checkdiag1==1 || checkdiag2==1)
        return 1;
    }
    if((quadrant[PFy-1][PFx]==1&&quadrant[PFy-1][PFx-1]==0) || (quadrant[PFy+1][PFx]==1 && quadrant[PFy+1][PFx-1]==0)) {
      resultcheck= checkadjacent(PFx,PFy,PFxpar,PFypar,PFGvalue,PFfunctioncount*10,'l');
      if(resultcheck==2&&PFfunctioncount==0)
        return 4;
      if(resultcheck==2&&PFfunctioncount!=0)
        return 2;
      else return 1;
    }
    return resulthor;
  case 'u':
    if(quadrant[PFy-1][PFx]==1)
      return 2;
    resultver=jumpadjacent(PFx,(PFy-1),PFxpar,PFypar,'u',PFfunctioncount,PFGvalue);

    if(PFfunctioncount==0) {
      checkdiag1=checkdiag(PFx,PFy,PFxpar,PFypar,'q',PFfunctioncount,PFGvalue);
      checkdiag2=checkdiag(PFx,PFy,PFxpar,PFypar,'e',PFfunctioncount,PFGvalue);
      if(checkdiag1==1 || checkdiag2==1)
        return 1;
    }
    if((quadrant[PFy][PFx-1]==1&&quadrant[PFy-1][PFx-1]==0) || (quadrant[PFy][PFx+1]==1&&quadrant[PFy-1][PFx+1]==0)) {
      resultcheck= checkadjacent(PFx,PFy,PFxpar,PFypar,PFGvalue,PFfunctioncount*10,'u');
      if(resultcheck==2&&PFfunctioncount==0)
        return 4;
      if(resultcheck==2&&PFfunctioncount!=0)
        return 2;
      else return 1;
    }
    return resultver;
  case 'd': 
    if(quadrant[PFy+1][PFx]==1)
      return 2;
    resultver=jumpadjacent(PFx,(PFy+1),PFxpar,PFypar,'d',PFfunctioncount,PFGvalue);

    if(PFfunctioncount==0) {
      checkdiag1=checkdiag(PFx,PFy,PFxpar,PFypar,'c',PFfunctioncount,PFGvalue);
      checkdiag2=checkdiag(PFx,PFy,PFxpar,PFypar,'z',PFfunctioncount,PFGvalue);
      if(checkdiag1==1 || checkdiag2==1)
        return 1;
    }
    if((quadrant[PFy][PFx-1]==1&&quadrant[PFy+1][PFx-1]==0) || (quadrant[PFy][PFx+1]==1&&quadrant[PFy+1][PFx+1]==0)) {
      resultcheck= checkadjacent(PFx,PFy,PFxpar,PFypar,PFGvalue,PFfunctioncount*10,'d');
      if(resultcheck==2&&PFfunctioncount==0)
        return 4;
      if(resultcheck==2&&PFfunctioncount!=0)
        return 2;
      else return 1;
    }
    return resultver;
  case 'q':
    if((quadrant[PFy][PFx+1]==1 && quadrant[PFy-1][PFx+1]==0) || (quadrant[PFy+1][PFx]==1 && quadrant[PFy+1][PFx-1]==0))
      checkadjacent(PFx,PFy,PFxpar,PFypar,PFGvalue,PFfunctioncount*14,'q');

    resulthor=jumpadjacent((PFx-1),PFy,PFx,PFy,'l',PFfunctioncount,PFGvalue);
    resultver=jumpadjacent(PFx,(PFy-1),PFx,PFy,'u',PFfunctioncount,PFGvalue);

    checkdiag1=checkdiag(PFx,PFy,PFxpar,PFypar,'q',PFfunctioncount,PFGvalue);

    if(PFfunctioncount==0) {
      resulthor=jumpadjacent((PFx+1),PFy,PFx,PFy,'r',PFfunctioncount,PFGvalue);
      resultver=jumpadjacent(PFx,(PFy+1),PFx,PFy,'d',PFfunctioncount,PFGvalue);

      checkdiag2=checkdiag(PFx,PFy,PFxpar,PFypar,'e',PFfunctioncount,PFGvalue);

      checkdiag3=(PFx,PFy,PFxpar,PFypar,'z',PFfunctioncount,PFGvalue);
    }

    if(resulthor==2&&resultver==2) { //If both adjacent squares result in a deadend continue searching in diagonal directhor. Else one of these squares will result in a jump point, returning a 1 as well (JP found).
      if(quadrant[PFy-1][PFx-1]==1) //if this is the case, this route is a dead end, return 2 and search from the nePFxt node in the linked list.
        return 4;
      return checkdiag1;
    }
    else {
      checkadjacent(PFx,PFy,PFxpar,PFypar,PFGvalue,PFfunctioncount*14,'q');
      return 1;
    }
  case 'e':
    if((quadrant[PFy][PFx-1]==1 && quadrant[PFy-1][PFx-1]==0) || (quadrant[PFy+1][PFx]==1 && quadrant[PFy+1][PFx+1]==0))
      checkadjacent(PFx,PFy,PFxpar,PFypar,PFGvalue,PFfunctioncount*14,'e');

    resulthor=jumpadjacent((PFx+1),PFy,PFx,PFy,'r',PFfunctioncount,PFGvalue);
    resultver=jumpadjacent(PFx,(PFy-1),PFx,PFy,'u',PFfunctioncount,PFGvalue);

    checkdiag1=checkdiag(PFx,PFy,PFxpar,PFypar,'e',PFfunctioncount,PFGvalue);

    if(PFfunctioncount==0) {
      resulthor2=jumpadjacent((PFx-1),PFy,PFx,PFy,'l',PFfunctioncount,PFGvalue);
      resultver2=jumpadjacent(PFx,(PFy+1),PFx,PFy,'d',PFfunctioncount,PFGvalue);
      checkdiag2=checkdiag(PFx,PFy,PFxpar,PFypar,'c',PFfunctioncount,PFGvalue);
      checkdiag3=checkdiag(PFx,PFy,PFxpar,PFypar,'q',PFfunctioncount,PFGvalue);
    }

    if(resulthor==2&&resultver==2) { //If both adjacent squares result in a deadend continue searching in diagonal directhor. Else one of these squares will result in a jump point, returning a 1 as well (JP found).
      if(quadrant[PFy-1][PFx+1]==1) //if this is the case, this route is a dead end, return 2 and search from the nePFxt node in the linked list.
        return 4;
      return checkdiag1;
    }
    else {
      checkadjacent(PFx,PFy,PFxpar,PFypar,PFGvalue,PFfunctioncount*14,'e');
      return 1;
    }
  case 'c':
    if((quadrant[PFy][PFx-1]==1 && quadrant[PFy+1][PFx-1]==0) || (quadrant[PFy-1][PFx]==1 && quadrant[PFy-1][PFx+1]==0))
      checkadjacent(PFx,PFy,PFxpar,PFypar,PFGvalue,PFfunctioncount*14,'c');

    resulthor=jumpadjacent((PFx+1),PFy,PFx,PFy,'r',PFfunctioncount,PFGvalue);
    resultver=jumpadjacent(PFx,(PFy+1),PFx,PFy,'d',PFfunctioncount,PFGvalue);

    checkdiag1=checkdiag(PFx,PFy,PFxpar,PFypar,'c',PFfunctioncount,PFGvalue);
    
    if(checkdiag1==3) {
    checkadjacent(PFx,PFy,PFxpar,PFypar,PFGvalue,PFfunctioncount*10,PFdirecthor);
    return 3;
  }

    if(PFfunctioncount==0) {
      resulthor2=jumpadjacent((PFx-1),PFy,PFx,PFy,'l',PFfunctioncount,PFGvalue);
      resultver2=jumpadjacent(PFx,(PFy-1),PFx,PFy,'u',PFfunctioncount,PFGvalue);

      checkdiag2=checkdiag(PFx,PFy,PFxpar,PFypar,'z',PFfunctioncount,PFGvalue);

      checkdiag3=checkdiag(PFx,PFy,PFxpar,PFypar,'e',PFfunctioncount,PFGvalue);
    }

    if(resulthor==2&&resultver==2) { //If both adjacent squares result in a deadend continue searching in diagonal directhor. Else one of these squares will result in a jump point, returning a 1 as well (JP found).
      if(quadrant[PFy+1][PFx+1]==1) //if this is the case, this route is a dead end, return 2 and search from the nePFxt node in the linked list.
        return 4;
      return checkdiag1;
    }
    else {
      checkadjacent(PFx,PFy,PFxpar,PFypar,PFGvalue,PFfunctioncount*14,'c');
      return 1;
    }
  case 'z':
    if((quadrant[PFy][PFx+1]==1 && quadrant[PFy+1][PFx+1]==0) || (quadrant[PFy-1][PFx]==1 && quadrant[PFy-1][PFx-1]==0))
      checkadjacent(PFx,PFy,PFxpar,PFypar,PFGvalue,PFfunctioncount*14,'z');

    resulthor=jumpadjacent((PFx-1),PFy,PFx,PFy,'l',PFfunctioncount,PFGvalue);
    resultver=jumpadjacent(PFx,(PFy+1),PFx,PFy,'d',PFfunctioncount,PFGvalue);

    checkdiag1=checkdiag(PFx,PFy,PFxpar,PFypar,'z',PFfunctioncount,PFGvalue);

    if(PFfunctioncount==0) {
      resulthor2=jumpadjacent((PFx+1),PFy,PFx,PFy,'r',PFfunctioncount,PFGvalue);
      resultver2=jumpadjacent(PFx,(PFy-1),PFx,PFy,'u',PFfunctioncount,PFGvalue);
      checkdiag2=checkdiag(PFx,PFy,PFxpar,PFypar,'c',PFfunctioncount,PFGvalue);
      checkdiag3=checkdiag(PFx,PFy,PFxpar,PFypar,'e',PFfunctioncount,PFGvalue);
    }

    if(resulthor==2&&resultver==2) { //If both adjacent squares result in a deadend continue searching in diagonal directhor. Else one of these squares will result in a jump point, returning a 1 as well (JP found).
      if(quadrant[PFy+1][PFx-1]==1) //if this is the case, this route is a dead end, return 2 and search from the nePFxt node in the linked list.
        return 4;
      return checkdiag1;
    }
    else {
      checkadjacent(PFx,PFy,PFxpar,PFypar,PFGvalue,PFfunctioncount*14,'z');
      return 1;
    }
  }
}

void MappingPF::pathfinding(int8_t PFxs,int8_t PFys,int8_t PFxg,int8_t PFyg) {
  PFxgoal=PFxg;
  PFygoal=PFyg;
  PFystart=PFys;
  PFxstart=PFxs;
  node *temp1;
  node *lowest;
  headnode=NULL;
  int8_t PFxprev=NULL;
  int8_t PFyprev=NULL;
  char PFdirecthor;
  int8_t PFx,PFy;
  int PFmovecount=0;
  int8_t PFxpar,PFypar;
  byte PFjumpresult=0;
  char PFnextmove;
  evaluateadjacent(PFxstart,PFystart,0);

  temp1=headnode;
  lowest=headnode;

  while(!(lowest->xcoord==PFxgoal&&lowest->ycoord==PFygoal)) { //Do the following as long as the goal is not reached
    lowest=headnode;
    temp1=headnode;
    PFdirecthor='n';
    while(temp1!=NULL) { //Finds the square with the lowest F value
      if(lowest->closed==1) {
        lowest=temp1;
      }
      if(temp1->Fvalue<lowest->Fvalue&&temp1->closed==0)
      {
        lowest=temp1;
      }
      temp1 = temp1->nextnode;
    }

    PFx=lowest->xcoord;
    PFy=lowest->ycoord;

    if(PFx==PFxprev&&PFy==PFyprev) 
      lowest->closed=1;
    if(PFx==PFxgoal&&PFy==PFygoal)
      break;

    quadrant[PFy][PFx]=2;

    PFdirecthor=inversedir(lowest->dirtoparent);
    PFxpar=PFx;
    PFypar=PFy;

    PFjumpresult=jumpadjacent(PFx,PFy,PFxpar,PFypar,PFdirecthor,-1,lowest->Gvalue);
    if(PFjumpresult==1)
      lowest->closed=0;

    if(PFjumpresult=4)
      lowest->closed=1;

    if(PFjumpresult==3)
      break;

    PFxprev=lowest->xcoord;
    PFyprev=lowest->ycoord;
  }
  //Serial.println(freeRam());
  createroute(); //prints the route, coordinates backwards
  //Serial.println(freeRam());
  deletelist();
  //Serial.println(freeRam());

  //The following should be in the mapping module (prints can be removed) So call printcoords which returns the following step (or a NULL if the goal is reached).
  PFnextmove=printcoords();
   while(PFnextmove!=NULL) {
    Serial.print("dir=");
    Serial.println(PFnextmove);
    PFnextmove=printcoords();
  }
}