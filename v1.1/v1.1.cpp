//copyright Williewonka 2013
//version 1.1

#include <iostream>
#include <fstream>
#include <string>
using namespace std;

// Global Variables
int width;
int height;
char** matrix;//format matrix[y][x]
int step;
int x = 1;
int y = 1;
char direction = 'r';//up(u), down(d), left(l), right(r)
char front;
int flag_override_steps = 0;//if this flag is active the robot is returning over a previous taken path. this overrides a couple of security rules

//agenda for map:
//@=border of quadrant
//x=hill
//o=explorable terrain
void ReadMatrix(){//read matrix from txt file
	string filename = "";
	cout << "Insert filename for matrix, dont forget extension!" << endl;
	getline(cin,filename);//activate this line to get custom input for txt file
	//filename = "map.txt";//disable this line and the next one for custom input for txt file
	//cout << filename << endl;
	cout << "specify height: ";
	cin >> height;
	cout << "specify width: ";
	cin >> width;
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
	for (int i = 0; i < height; i++)
	{
		for (int j = 0; j < width; j++)
		{
			if(matrix[i][j] == 'o' || matrix[i][j] == 'x' || matrix[i][j] == '@')
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
	cin.ignore();
}

int MoveForward()//moves the robot one step in the set directions, will block and return false if move is not possible
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
	if(matrix[y][x] == 'o' || (matrix[y][x] != 'x' && matrix[y][x] != '@' && flag_override_steps))//checks if the movement is possible
	{
		step++;
		if(step == 64)
		{
			matrix[y][x] = 63;
		}else
		{
			matrix[y][x] = step;
		}
		
		PrintMatrix();
		return 0;
	}else//if not restore the old coordinates and return false
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
		direction = 'r';
		if(returning=MoveForward()){
			flag_override_steps = 1;
			direction = 'd';
		}
		break;
	}
}

void DriveAround()//lets the robot drive around an hill
{
	int depth = 1;
	char direction_old = direction;
	int y_old = y;
	direction = 'd';

	switch(direction_old)
	{
	case 'r':
		while(matrix[y][x+depth] == 'x')//simulates the ultrasound scanner to see if there is still a hill next to the robot, as long as there is, the robot drives down
		{
			if(!MoveForward()){
				break;
			}
		}
		direction = 'r';//robot drives under the hill
		MoveForward();
		while(matrix[y-1][x] == 'x')
		{
			if(!MoveForward()){
				break;
			}
		}
		direction = 'u';//the robot drives up alongside the other side of the hill until it is at the same height as beginning the routine
		while(y != y_old)
		{
			if(!MoveForward()){
				break;
			}
		}		
		break;
	case 'l'://same thing but for the case that the robot approaches the hill from diferent direction
		while(matrix[y][x-depth] == 'x')
		{
			if(MoveForward()){
				break;
			}
		}
		direction = 'l';
		MoveForward();
		while(matrix[y-1][x] == 'x')
		{
			if(MoveForward()){
				break;
			}
		}
		direction = 'u';
		while(y != y_old)
		{
			if(MoveForward()){
				break;
			}
		}
		direction = 'l';
		break;
	}
}

void main()
{
	ReadMatrix();//read and print the matrix
	PrintMatrix();
	step = 0;//initialize number of steps
	while(!(x == 11 && y == 11))//while the quadrant isnt finished yet keep exploring
	{
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
		switch(direction)//simulate what the sensors will see at the front of the robot
		{
		case 'r':
			front = matrix[y][x+1];
			break;
		case 'd':
			front = matrix[y+1][x];
			break;
		case 'l':
			front = matrix[y][x-1];
			break;
		case 'u':
			front = matrix[y-1][x];
			break;
		}
		switch(front)//using this info the actual algorithm will choose what to do
		{
		case 'o':
			//free space at front so move forward
			MoveForward();
			break;
		case 'x':
			//mountain in front of robot so initiate driving around
			DriveAround();
			break;
		case '@':
			//end of quadrant so turn around
			TurnAround();
			break;
		default:
			//none of the above so a previous step (if the matrix is correct) wich means turning around, if the flag for overiding is active then dont turn but override previous step because direction is set previous in this case
			if(flag_override_steps)
			{
				MoveForward();
			}else
			{
				TurnAround();
			}
			
			break;
		}

	}
	PrintMatrix();//print the matrix one last time ^^ 
}