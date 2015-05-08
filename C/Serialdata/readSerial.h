#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>

#define BAUDRATE B4800    
#define MODEM "/dev/ttyUSB0"

#ifndef _readSerial
#define _readSerial


class readSerial{
private:
    struct termios tio;
    struct termios stdio;
    struct termios old_stdio;
    int tty_fd, flags, bytes_read;
    volatile char data;
    char buf[10];

public:
    //readSerial();

    void initialize(){
        tcgetattr(STDOUT_FILENO,&old_stdio);
        printf("Please start with /dev/ttyUSB0 (for example)\n");
        memset(&stdio,0,sizeof(stdio));
        stdio.c_iflag=0;
        stdio.c_oflag=0;
        stdio.c_cflag=0;
        stdio.c_lflag=0;
        stdio.c_cc[VMIN]=1;
        stdio.c_cc[VTIME]=0;
        tcsetattr(STDOUT_FILENO,TCSANOW,&stdio);
        tcsetattr(STDOUT_FILENO,TCSAFLUSH,&stdio);
        fcntl(STDIN_FILENO, F_SETFL, O_NONBLOCK);       // make the reads non-blocking
        memset(&tio,0,sizeof(tio));
        tio.c_iflag=0;
        tio.c_oflag=0;
        tio.c_cflag=CS8|CREAD|CLOCAL;           // 8n1, see termios.h for more information
        tio.c_lflag=0;
        tio.c_cc[VMIN]=1;
        tio.c_cc[VTIME]=5;
        if((tty_fd = open(MODEM , O_RDWR | O_NONBLOCK)) == -1){
            printf("Error while opening\n"); // Just if you want user interface error control
            exit(-1);
        }
        cfsetospeed(&tio,BAUDRATE);    
        cfsetispeed(&tio,BAUDRATE);            // baudrate is declarated above
        tcsetattr(tty_fd,TCSANOW,&tio);
        while (1){
            bytes_read = read(tty_fd,buf,9);
            if (bytes_read > 0 ){
                write(STDOUT_FILENO, buf, 9); // if new data is available on the serial port, print it out
                 //std::cout << buf;// << std::endl;
                 //data  = &c;
            }

        }
        close(tty_fd);
        tcsetattr(STDOUT_FILENO,TCSANOW,&old_stdio);
    }

    char sensorStatus(){
        return data;
    }
};
#endif // _readSerial