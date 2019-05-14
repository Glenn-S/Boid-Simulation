//---------------------------------------------------------------------
// Filename: README.txt
//
// CPSC 587, Assignment 4
// Glenn Skelton
//---------------------------------------------------------------------


**INSTALL**

Load up in QT Creator and setup in release mode. Other modes will casue
the program to lag. This program was developed on a Linux environoment
using the graphics lab computer.


**USER INTERFACE**

All calls originally part of the boilerplate for GIVR

In addition, the following key calls have been added

P - open the graph editor panel
CTRL-S - save the state of the program (ie. graphs)
F - Maximize / Original Size of screen
SPACE - pause/unpause the simulation
1 - engage/disengage obstacle mode

Modifications

**RESOURCES**

I used the notes provided from class and tutorial along with reading the Reynolds
article specified. In addition, I also used stackoverflow to help me remember the
syntax for creating a random number generator from:
https://stackoverflow.com/questions/686353/random-float-number-generation.
I also obtained my mesh model for my bee from:
https://free3d.com/3d-model/bee-89226.html


**NOTES**

The build files need to be put in the build folder otherwise the paths for the
filenames used to read and write the config file will not work correctly.

I did not implement any bonuses for this assignment because I ran out of time to
do so.




**USER MANUAL**

To operate, there is a configuration file that the user may modify to specify certain
start state parameters. These parameters include: the number of boids, the mass of the
boid, the avoidance, cohesion and gather range values, the associated forces for each
range, the arena radius, a general force multiplier for the arena and obstacle, a
minimum and maximum velocity for the boid, and specifications for the force graph
values.

To manipulate the program before starting, simply open the file named config.txt and
modify the values as seen appropriate. A special note to make is that the values for
the avoidance, cohesion, and gather(max range) values are specified as distances since
the ratio for the graph is set at 1/3 for each sub range of the boids ranges. As such it
is recommended to keep any graph manipulations within these ranges for each boid range.
The force mutliplier is only used for specifying the magnitude of the force applied by
the arena bounds and the obstacle.

Once the user has specified a configuration file with the name "config.txt" or modified
the current copy, start up the application. Once the application is open, the user may
open the graph editor by pressing P. Using the graph editor, the user may adjust the graph
values to control the forces applied in each boid range. When the user is satisfied with
their changes, pressing CTRL-S will save the current parameters in the config.txt file.

If the user wishes to pause the simulation, simply press the space bar to stop and resume.
Also, pressing F will enable fullscreen mode and vice-versa. By default the obstacle mode
is disabled. To enable, press the 1 key and to turn off again just press 1 once more. To
exit the program, press ESC.
