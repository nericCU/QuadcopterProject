Built and tested in an Unix (Ubuntu 14.10) Environment 
This program uses the Unix version of the ARDroneSDk3
gcc version 4.8.2 (Ubuntu 4.8.2-19ubuntu1) 

This program must be placed inside the same directory as the sdk folder for the headers/libraries to be linked correctly.

compile: make
run: make run

This program is built off the Unix sample provided by Parrot.

This program adds several new commands that you send to the drone. If you press 1, it starts a simple object avoidance routine that runs on a different thread so you can still send commands through the keyboard (most notably the "emergency command").

If you press 2, it starts a routine that executes a simple route with simple object avoidance.

If you press 3, it starts a routine that executes an advanced route.


'P' to stop the drone
'm' for emergency
