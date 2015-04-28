
#include "readSerial.h"
#include <iostream>

int main(){

	pid_t  pid;
	readSerial arduino;

	pid = fork();

	if (pid == 0) {
		while(1){
/*			if (arduino.sensorStatus() == 'L'){
				std::cout << "hello\n";

			}*/
/*				if (arduino.sensorStatus() != ' '){
									std::cout << arduino.sensorStatus();

				}*/
		}	
	}
	else {
		arduino.initialize();
	}
}