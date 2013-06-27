//copyright Williewonka 2013
//version 1.5(arduino implementation)

#include "Arduino.h"
#include "Mapping.h"

//map legenda
//IMPASSEBLE 1;
//STEP 2;
//EMPTY 3;
//BOUNDARY 4;
//
//input front
//line 1
//mountain 2
//empty 3
//rock 4
//boundary 5
//
//states
//NOSTATE 0;
//TURNAROUND 1;
//PATHFINDING 2;
//DRIVEAROUND1 3; drive down
//DRIVEAROUND2 4; drive under the mountain
//DRIVEAROUND3 5; drive reverse down
//DRIVEAROUND4 6; drive reverse


//input side
//mountain 1
//no mountain 0
//measurement after driving!

Mapping::Mapping()
{
	x = 1;
	y = 1;
	directhor = 'r';
	flag_override_steps = 0;
	flag_cinematic = 'n';
	state = 0;
	sidelook = 'n';
	flag_bottom_line = 0;
}
int Mapping::WriteStep()//moves the robot one step in the set directhors, will return false if move is not possible
{
	int x_old = x;
	int y_old = y;

	switch(directhor)//adjust x en y coordinates aan de hand van de directhor
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
	}else//if not restore the old coordinates and return true
	{
		x = x_old;
		y = y_old;
		return 1;
	}
	
}

int Mapping::TurnAround()//lets the robot turn 180 degrees while shifting row
{
	if(state == 0)
	{
		directhor_old = directhor;
	}
	switch(directhor_old)
	{
	case 'r':
		switch(state)
		{
		case 1://phase 2
			directhor = 'l';
			WriteStep();
			state = 0;
			return 99;
			break;
		case 0://no state
			directhor = 'd';
			if(WriteStep()){//if the move failes twice it means that the turn failed and the only way back is the way we came. so activate the override flag
				directhor = 'l';
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
	case 'd'://2 possibilities but orientation is to turn to the right so directhor is left
		switch(state)
		{
		case 1://phase 2
			directhor = 'u';
			WriteStep();
			state = 0;
			return 99;
			break;
		case 0://no state
			directhor = 'l';
			if(WriteStep()){//if the move failes twice it means that the turn failed and the only way back is the way we came. so activate the override flag
				directhor = 'u';
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
			directhor = 'r';
			WriteStep();
			state = 0;
			return 99;
			break;
		case 0://no state
			directhor = 'd';
			if(WriteStep()){//if the move failes twice it means that the turn failed and the only way back is the way we came. so activate the override flag
				directhor = 'r';
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
			directhor = 'l';
		}else
		{
			if(quadrant[y][x+1] == 3)
			{
				directhor = 'r';
			}
		}
		if(WriteStep()){
			flag_override_steps = 1;
			directhor = 'd';
			WriteStep();
			return 99;
		}else
		{
			return 99;
		}
		break;
	}
}

int Mapping::CheckAround() //check around the current position to look if there is a square wich it hasnt been yet, return 0 if there is and 1 otherwise
{
	if(quadrant[y][x-1] == 3)
	{
		directhor = 'l';
		return 0;
	}else
	{
		if(quadrant[y][x+1] == 3)
		{
			directhor = 'r';
			return 0;
		}else
		{
			if(quadrant[y-1][x] == 3)
			{
				directhor = 'u';
				return 0;
			}
			else
			{
				if(quadrant[y+1][x] == 3)
				{
					TurnAround();
					return 0;
				}else
				{
					return 1;
				}
			}
		}
	}
}

int Mapping::DriveAroundHills(int front, int side)//lets the robot drive around an hill
{
	switch(state)
	{
	case 0://nostate
		switch(directhor)
		{
		case 'l':
			sidelook = 'r';
			break;
		case 'r':
			sidelook = 'l';
			break;
		}
		directhor = 'd';
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
				directhor = 'l';
				break;
			case 'l':
				directhor = 'r';
			}
			state = 4;
		}
		WriteStep();
		return 99;
		break;
	case 4://phase 2
		if(!side)
		{
			directhor = 'u';
			sidelook = 'n';
			state = 0;
		}
		WriteStep();
		return 99;
		break;
	case 5://reverse phase 1
		if(!CheckAround())
		{
			WriteStep();
			state = 0;
			return 99;
		}else
		{
			directhor = 'u';
			return 99;
		}
		break;
	case 6://reverse phase 2
		if(!CheckAround())
		{
			WriteStep();
			state = 0;
			return 99;
		}else
		{
			if(front == 2 || front == 1)
			{
				state = 5;
				directhor = 'u';
				return 99;
			}else
			{
				switch(sidelook)
				{
				case 'r':
					directhor = 'l';
					break;
				case 'l':
					directhor = 'r';
					break;
				}
				return 99;
			}
		}

	}
}

void Mapping::WriteMap(int front)
{
	int x_old = x;
	int y_old = y;
	switch(directhor)
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
	switch(front)
	{
	case 2:
	case 3:
		quadrant[y][x] = 1;
		break;

	case 1:
		quadrant[y][x] = 3;
		break;
	}
	y = y_old;
	x = x_old;

	return;
}

int Mapping::CheckBoundary()
{
	flag_bottom_line = 0;
	switch (directhor)
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

int Mapping::Explorer(int front, int side) //drives the robot when there is no special state
{
		if(quadrant[y-1][x] == 3)
		{
			directhor = 'u';
			flag_override_steps = 0;
			return 99;
		}
		if(flag_override_steps){//check if the override step is still necesary
			if(quadrant[y][x+1] == 3){
				flag_override_steps = 0;
				directhor = 'r';
				return 99;
			}else{
				if(quadrant[y][x-1] == 3){
					flag_override_steps = 0;
					directhor = 'l';
					return 99;
				}
			}
		}

		if(flag_override_steps)
		{
			if(quadrant[y+1][x] != 1)
			{
				directhor = 'd';
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
			default:
				//none of the above so a previous step (if the quadrant is correct) wich means turning around
					TurnAround();
					return 99;
				break;
			}
		}

}

int Mapping::MapModule(int front, char side, int error) //sets the directhor in global variable and updates the map,  returns steps if succesful, 0 if quadrant has ended
{

	if(CheckBoundary())
	{
		front = 5;
	}
	
	if(error)
	{
		//what to do with map if there is an error
	}else
	{
		if(front != 5)
		{
			WriteMap(front);
		}
		if(sidelook != 'n' && side == 1)//write the mountain to map if the robot is looking at it and sees a mountain
		{
			switch(directhor)
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
	}
	int returning;
	switch(state)
	{
	case 2:
		{
			//pathfinding
		}
		break;
	case 3:
	case 4:
		{
			if(front == 5 || error == 1)//if the robot hits border of quadrant or another problem then stop the driving riding around procedure and rerun the algorithm to decide what needs to happen next
			{
				state += 2;
				return DriveAroundHills(front,side);
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
				return MapModule(front,side,error);
			}else
			{
				return returning;
			}
		}
		break;
	default://no state
		{
			return Explorer(front,side);//for no state the robot will just go exploring, this also applies for when the override flag is active
		}
		break;
	}
}