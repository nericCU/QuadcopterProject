#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <pthread.h>


#define MODEM "/dev/ttyUSB1"
#define BAUDRATE B4800    

int flag;

void *initializeSensors(void*){
    struct termios options;
    int tty_fd, flags;
    unsigned char c='D';

    fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK); // make the reads non-blocking
    memset(&options,0,sizeof(options)); //zero out struct termios options
    options.c_iflag=0;
    options.c_oflag=0;
    options.c_cflag=CS8|CREAD|CLOCAL; //8n1, see termios.h for more informaoptionsn
    options.c_lflag=0;
    options.c_cc[VMIN]=1;
    options.c_cc[VTIME]=5;
    if((tty_fd = open(MODEM , O_RDONLY | O_NONBLOCK)) == -1){ //open port as read only
        printf("Error while opening\n");
        exit(-1);
    }
    cfsetispeed(&options,BAUDRATE); // set baudrate
    tcsetattr(tty_fd,TCSANOW,&options); 
    while (flag != -1){
        if (read(tty_fd,&c,1)>0){ //ccheck to see if read was successfull
            write(STDOUT_FILENO,&c,1); //if new data is available on the serial port, print it out
            if(c == 'L')
              flag = 1;
            else if(c == 'F')     
              flag = 2;
            else if(c == 'R')
              flag = 3 ;
            else
              flag = 0;
        }
        usleep(50000);
    }
    close(tty_fd);
}

int main(){   

  pthread_t thread1;
  int iret1;

  iret1 = pthread_create( &thread1, NULL, initializeSensors, NULL);
  if(iret1)
  {
    fprintf(stderr,"Error - pthread_create() return code: %d\n",iret1);
    exit(EXIT_FAILURE);
  }

  int count1 = 0, count2 = 0, count3 = 0;
  while(1){
    if(flag == 1){
      printf(" %d\n", ++count1);
      flag = 0;
    }
    if(flag == 2){
      printf(" %d\n", ++count2);
      flag = 0;
    }
    if(flag == 3){
      printf(" %d\n", ++count3);
      flag = 0;
    }
    usleep(50000);
  }
  flag = -1;
  pthread_join( thread1, NULL);
  printf("exiting\n");
  return 0;
}