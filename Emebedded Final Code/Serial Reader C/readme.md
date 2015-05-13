Built and tested in an Unix (Ubuntu 14.10) Environment

compile: g++ -o main main.cpp
run: ./main

This program reads data from the serial port and displays them onto the screen.
This program assumes only 1 byte of data is sent at a time.
This program ignores all incoming data except for the control characters L, F, and R.
When these characters are read, it sets a flag that is read from main to output the # of times that character has been read.

Baud rate and Port set is set to 4800 and /dev/ttyUSB0 and can be changed.

This program was designed to be used with the Unix version of the SDK to run the 3.0 drone.
