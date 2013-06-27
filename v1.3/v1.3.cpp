//copyright Williewonka 2013
//version 1.3

#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
using namespace std;

// Global Variables
int width = 13;
int height = 11;
char** matrix;//format matrix[y][x]
int step;
int x = 1;
int y = 1;
char direction = 'r';//up(u), down(d), left(l), right(r)
int flag_override_steps = 0;//if this flag is active the robot is returning over a previous taken path. this overrides a couple of security rules
char flag_cinematic = 'n';
int state = 0;//state for the driving around hills


//agenda for map:
//@=border of quadrant
//x=hill
//o=explorable terrain
//+=whiteline

char SeeFront()
{
			switch(direction)//simulate what the sensors will see at the front of the robot
			{
			case 'r':
				return matrix[y][x+1];
				break;
			case 'd':
				return matrix[y+1][x];
				break;
			case 'l':
				return matrix[y][x-1];
				break;
			case 'u':
				return matrix[y-1][x];
				break;
			}
}

void ReadMatrix(){//read matrix from txt file
	string filename = "";
	cout << "Insert filename for matrix, dont forget extension!" << endl;
	//getline(cin,filename);//activate this line to get custom input for txt file
	filename = "map2.txt";//disable this line and the next one for custom input for txt file
	cout << filename << endl;
	//activate the next 4 lines for user input
	cout << "specify height: ";
	//cin >> height;
	cout << height << endl;
	cout << "specify width: ";
	//cin >> width;
	cout << width << endl;

	//create matrix:
	matrix = new char*[width];
	for(int i = 0; i < height; ++i)
	{
		matrix[i] = new char[height];
	}
	fstream input (filename);
	if(!input) {
		cout << endl << "Failed to open file " << filename;
		return;
	}

	while (!input.eof())
	{
		for (int i = 0; i < height; i++)
		{
			for (int j = 0; j < width; j++)
			{
				input >> matrix[i][j];
			}
		}
	}
	input.close();
}

void PrintMatrix(){//prints matrix from array
	if(flag_cinematic == 'y')
	{
		system("cls");

	}
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if(matrix[i][j] == 'o' || matrix[i][j] == 'x' || matrix[i][j] == '@' || matrix[i][j] == '+')
			{
				cout << matrix[i][j] << "   ";
			}else
			{
				int getal = (int)matrix[i][j];
				if(getal < 10)
				{
					cout << (int)matrix[i][j] << "   ";
				}else
				{
					if(getal >= 10 && getal < 100){
						cout << (int)matrix[i][j] << "  ";
					}else
					{
						cout << (int)matrix[i][j] << " ";
					}
				}
				
			}
		
		}
		cout << endl;
	}
	if(flag_cinematic == 'y')//if cinematic mod is enabled, dont wait for enter but wait a short time before calculating the next step
	{
		Sleep(50);
	}else
	{
		if(flag_cinematic == 'n')
		{
			cin.ignore();
		}
	}
	
}

int MoveForward()//moves the robot one step in the set directions, will block and return 1 if move is not possible
{
	int x_old = x;
	int y_old = y;

	switch(direction)//adjust x en y coordinates aan de hand van de direction
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
	if(matrix[y][x] == 'o' || ( matrix [y][x]  != '+' && matrix[y][x] != 'x' && matrix[y][x] != '@' && flag_override_steps))//checks if the movement is possible, if so then return false
	{
		step++;
		if(step == 64)//very bad fix for char integer conversion bug ^^
		{
			matrix[y][x] = 63;
		}else
		{
			if(step == 120)
			{
				matrix[y][x] = 119;
			}else
			{
				if(step == 43)
				{
					matrix[y][x] = 42;
				}else
				{
					matrix[y][x] = step;
				}
				
			}
			
		}
		
		PrintMatrix();
		return 0;
	}else//if not restore the old coordinates and return true
	{
		x = x_old;
		y = y_old;
		return 1;
	}
	
}

void TurnAround()//lets the robot turn 180 degrees while shifting row
{
	char direction_old = direction;
	int returning;

	switch(direction_old)
	{
	case 'r':
		direction = 'd';
		if(returning=MoveForward()){//if the move failes twice it means that the turn failed and the only way back is the way we came. so activate the override flag
			direction = 'l';
			if(MoveForward()){
				flag_override_steps = 1;
			}
		}else{
			direction = 'l';
		}
		break;
	case 'd'://2 possibilities but orientation is to turn to the right so direction is left
		direction = 'l';
		if(returning=MoveForward()){
			direction = 'u';
			if(MoveForward()){
				flag_override_steps = 1;
			}
		}else{
			direction = 'u';
		}
		break;
	case 'l':
		direction = 'd';
		if(returning=MoveForward()){
			direction = 'r';
			if(MoveForward()){
				flag_override_steps = 1;
			}
		}else{
			direction = 'r';
		}
		break;
	case 'u':
		if(matrix[y][x-1] == 'o')
		{
			direction = 'l';
		}else
		{
			if(matrix[y][x+1] == 'o')
			{
				direction = 'r';
			}
		}
		if(returning=MoveForward()){
			flag_override_steps = 1;
			direction = 'd';
		}
		break;
	}
}

int CheckAround() //check around the current position to look if there is a square wich it hasnt been yet, return 0 if there is and 1 otherwise
{
	if(matrix[y][x-1] == 'o')
	{
		direction = 'l';
		return 0;
	}else
	{
		if(matrix[y][x+1] == 'o')
		{
			direction = 'r';
			return 0;
		}else
		{
			if(matrix[y-1][x] == 'o')
			{
				direction = 'u';
				return 0;
			}
			else
			{
				if(matrix[y+1][x] == 'o')
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

int DriveAroundHills(int reverse)//lets the robot drive around an hill
{
	int depth = 1;
	char direction_old = direction;
	int y_old = y;
	if(reverse)
	{
			flag_override_steps = 1;
			while(1)
			{
				if(!CheckAround())
				{
					return 0;
				}else
				{
					switch(state)
					{
					case 1:
						direction = 'u';
						MoveForward();
						break;
					case 2:
						switch(direction_old)
						{
						case 'l':
							direction = 'r';
							MoveForward();
							break;
						case 'r':
							direction = 'l';
							MoveForward();
							break;
						}
						if(SeeFront() == '+' || SeeFront() == 'x' || SeeFront() == '@')
						{
							state = 1;
						}
						break;
					}
				}
			}
	}else
	{
		direction = 'd';
		state = 1;
		switch(direction_old)
		{
		case 'r':
			while(matrix[y][x+depth] == 'x')//simulates the ultrasound scanner to see if there is still a hill next to the robot, as long as there is, the robot drives down
			{
				if(MoveForward()){
					return 1;
				}
			}
			direction = 'r';//robot drives under the hill
			state = 2;
			MoveForward();
			while(matrix[y-1][x] == 'x')
			{
				if(MoveForward()){
					return 1;
				}
			}

			break;

		case 'l'://same thing but for the case that the robot approaches the hill from diferent direction
			while(matrix[y][x-depth] == 'x')
			{
				if(MoveForward()){
					return 1;
				}
			}
			direction = 'l';
			state = 2;
			MoveForward();
			while(matrix[y-1][x] == 'x')
			{
				if(MoveForward()){
					return 1;
				}
			}
			break;
		}
		
	}
	return 0;
}

void main()
{
	char front;
	ReadMatrix();//read and print the matrix
	cout << "cinematic mode?(y/n) ";
	cin >> flag_cinematic;
	PrintMatrix();
	step = 0;

	while(1)
	{
		front = SeeFront();

		if(matrix[y-1][x] == 'o')
		{
			direction = 'u';
		}
		if(flag_override_steps){//check if the override step is still necesary
			if(matrix[y][x+1] == 'o'){
				flag_override_steps = 0;
				direction = 'r';
			}else{
				if(matrix[y][x-1] == 'o'){
					flag_override_steps = 0;
					direction = 'l';
				}
			}
		}
		//if(step == 39)
		//{
		//	int check =1;
		//}

		if(flag_override_steps)
		{
				if(matrix[y+1][x] != '@' && matrix[y+1][x] != '+' && matrix[y+1][x] != 'x')
				{
					direction = 'd';
					MoveForward();
				}
				else
				{
					switch(front)
					{
					case '@':
					case '+':
					case 'x':
						if(matrix[y+1][x] == '@')
						{
							cin.ignore();
							return;
						}else
						{
							TurnAround();
						}

						break;
					default:
						MoveForward();
						break;
					}
				}
		}
		else
		{
			switch(front)//using this info the actual algorithm will choose what to do
			{
			case 'o':
				//free space at front so move forward
				MoveForward();
				break;
			case 'x':
				//mountain in front of robot so initiate driving around
				state = 0;
				if(DriveAroundHills(0))
				{
					if(CheckAround())
					{
						DriveAroundHills(1);		
					}
				}
				break;
			case '@':
				//end of quadrant so turn around
				if(matrix[y+1][x] == '@' && direction == 'r')
				{
					cin.ignore();
					return;
				}else
				{
					TurnAround();
				}		
				break;
			case '+':
				//a line in front of it, so follow it
				if(matrix[y+1][x] == '@' && direction == 'r')
				{
					cin.ignore();
					return;
				}else
				{
					TurnAround();
				}
				
				break;
			default:
				//none of the above so a previous step (if the matrix is correct) wich means turning around
					TurnAround();
				break;
			}
		}
		
	}
	PrintMatrix();//print the matrix one last time ^^ 
}