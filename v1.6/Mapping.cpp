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

Mapping::Mapping(int a)
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
}

int Quadrant(int x, int y)
{
	int matrix1[5][5];
	int matrix2[5][5];
	int matrix3[5][5];
	int matrix4[5][5];

	if(x > 0 && y > 0)
	{
		return matrix1[y][x];
	}else
	{
		if(x > 0 && y < 0)
		{
			return matrix2[abs(y)][x];
		}else
		{
			if(x < 0 && y < 0)
			{
				return matrix3[abs(y)][abs(x)];
			}
			else
			{
				if(x < 0 && y > 0)
				{
					return matrix4[y][abs(x)];
				}
			}
		}
	}
}

void Mapping::init()
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

int Mapping::WriteStep()//moves the robot one step in the set Directhors, will return false if move is not possible
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

int Mapping::TurnAround()//lets the robot turn 180 degrees while shifting row
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

int Mapping::LookAround() //check around the current position to look if there is a square wich it hasnt been yet, return 0 if there is and 1 otherwise
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

int Mapping::DriveAroundHills(int front, int side)//lets the robot drive around an hill
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
			//sidelook = 'n';
			state = 0;
		}
		WriteStep();
		return 99;
		break;
	//case 5://reverse phase 1
	//	if(!LookAround())
	//	{
	//		WriteStep();
	//		state = 0;
	//		return 99;
	//	}else
	//	{
	//		Directhor = 'u';
	//		return 99;
	//	}
	//	break;
	//case 6://reverse phase 2
	//	if(!LookAround())
	//	{
	//		WriteStep();
	//		state = 0;
	//		return 99;
	//	}else
	//	{
	//		if(front == 2 || front == 1)
	//		{
	//			state = 5;
	//			Directhor = 'u';
	//			return 99;
	//		}else
	//		{
	//			switch(sidelook)
	//			{
	//			case 'r':
	//				Directhor = 'l';
	//				break;
	//			case 'l':
	//				Directhor = 'r';
	//				break;
	//			}
	//			return 99;
	//		}
	//	}
	//	break;
	}
}

void Mapping::WriteMap(int front)
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

int Mapping::CheckBoundary()
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

int Mapping::Explorer(int front, int side) //drives the robot when there is no special state
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

int Mapping::CheckMap()//returns what is on the map of the next square
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

void Mapping::WriteMapMountain()
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

char Mapping::GetDirection()//returns the direction relative to the current setting of the robot
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

int Mapping::MapModule(int front, int side, int error, int outputcounter) //sets the Directhor in global variable and updates the map,  returns steps if succesful, 0 if quadrant has ended
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
			//if(front != 3)//if the robot hits border of quadrant or another problem then stop the driving riding around procedure and rerun the algorithm to decide what needs to happen next
			//{
			//	state += 2;
			//	return DriveAroundHills(front,side);
			//}
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