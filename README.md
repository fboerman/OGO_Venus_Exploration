OGO_Venus_Exploration
=====================

This is the source code for the OGO Venus Exploration on the TUE (www.tue.nl). Its an implemenation of the mapping module for this project. For more information on the project see: http://www.es.ele.tue.nl/education/oo2/assignment.php
Please note that due to timeconstraint not everything was completed. Most notably the plan to dynamicly expand the matrix and the full implementation of the 4 matrix idea to allow negative coördinates.

A quick rundown of the project:

* v1.0 till v1.3:
	Simulator for on pc, simulates the algorithm and takes it inputs from a txt file.

* v1.5 and v1.6:
	arduino library for the mapping module, visual studio will give some errors if you open this, but thats because its for arduino
  
* v1.7:
	pathfinding added to the library allong with some small bug fixes.
	
How to use the arduino library:
* create an object with the class
* call object.mapmodule(arguments) in the top of the sourcefile there is an explanation what you need to put in this function
* call object.getdirection() to get the relative direction to the current direction

the Simulator is pretty self explainatory.

DISCLAIMER: This code is released for educational purpose only. Using this code is on your own risk and I do not give any support whatsoever. Its stricly forbidden to copy paste this code to use in any project, especially the original tue project that I did. If you do so, its on your own resposiblity. 