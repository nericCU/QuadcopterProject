/*
    Copyright (C) 2014 Parrot SA

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions
    are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the 
      distribution.
    * Neither the name of Parrot nor the names
      of its contributors may be used to endorse or promote products
      derived from this software without specific prior written
      permission.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
    "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
    LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
    FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
    COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
    INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
    BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
    OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED 
    AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
    OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
    SUCH DAMAGE.
*/
/**
 * @file ihm.c
 * @brief This file contains sources about ncurses IHM used by arsdk example "BebopDroneDecodeStream"
 * @date 15/01/2015
 */

/*****************************************
 *
 *             include file :
 *
 *****************************************/

#include <stdlib.h>
#include <curses.h>
#include <string.h>

#include <libARSAL/ARSAL.h>

#include "ihm.h"

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <pthread.h>




/*****************************************
 *
 *             define :
 *
 *****************************************/

#define HEADER_X 0
#define HEADER_Y 0

#define INSTRUCTION_X 0
#define INSTRUCTION_Y 2

#define BATTERY_X 0
#define BATTERY_Y 10

#define INFO_X 0
#define INFO_Y 12

#define MODEM "/dev/ttyUSB0"
#define BAUDRATE B4800    

 int flag;


void *initializeSensors(void* s){
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

void *avoidObstacles(void *data){
    
    IHM_t *ihm = (IHM_t *) data;

    ihm->onInputEventCallback (IHM_INPUT_EVENT_TAKEOFF_LANDING, ihm->customData);
    sleep(5);

     while(1){
        if(flag == 1){
            ihm->onInputEventCallback (IHM_INPUT_EVENT_RIGHT, ihm->customData);
            flag = 0;
        }else if(flag == 2){
            ihm->onInputEventCallback (IHM_INPUT_EVENT_BACK, ihm->customData);
            flag = 0;
        }else if(flag == 3){
            ihm->onInputEventCallback (IHM_INPUT_EVENT_LEFT, ihm->customData);
            flag = 0;
        }
        usleep(100000);
        ihm->onInputEventCallback (IHM_INPUT_EVENT_NONE, ihm->customData);
    }
}

void *straight_route_with_object_avoidance(void* data){
    sleep(10);
    IHM_t *ihm = (IHM_t *) data;
    
    ihm->onInputEventCallback (IHM_INPUT_EVENT_FORWARD, ihm->customData);
    usleep(1000000);
    ihm->onInputEventCallback (IHM_INPUT_EVENT_NONE, ihm->customData);
    do {
        usleep(1000000);
    } while(flag != 0);

    ihm->onInputEventCallback (IHM_INPUT_EVENT_FORWARD, ihm->customData);
    usleep(1000000);
    ihm->onInputEventCallback (IHM_INPUT_EVENT_NONE, ihm->customData);
    do {
        usleep(1000000);
    } while(flag != 0);

    ihm->onInputEventCallback (IHM_INPUT_EVENT_FORWARD, ihm->customData);
    usleep(1000000);
    ihm->onInputEventCallback (IHM_INPUT_EVENT_NONE, ihm->customData);
    do {
        usleep(1000000);
    } while(flag != 0);
}

/*****************************************
 *
 *             private header:
 *
 ****************************************/
void *IHM_InputProcessing(void *data);

/*****************************************
 *
 *             implementation :
 *
 *****************************************/

IHM_t *IHM_New (IHM_onInputEvent_t onInputEventCallback)
{
    int failed = 0;
    IHM_t *newIHM = NULL;
    
    // check parameters
    if (onInputEventCallback == NULL)
    {
        failed = 1;
    }
    
    if (!failed)
    {
        //  Initialize IHM
        newIHM = malloc(sizeof(IHM_t));
        if (newIHM != NULL)
        {
            //  Initialize ncurses
            newIHM->mainWindow = initscr();
            newIHM->inputThread = NULL;
            newIHM->run = 1;
            newIHM->onInputEventCallback = onInputEventCallback;
            newIHM->customData = NULL;
        }
        else
        {
            failed = 1;
        }
    }
    
    if (!failed)
    {
        //raw();                  // Line buffering disabled
        cbreak();
        keypad(stdscr, TRUE);
        noecho();               // Don't echo() while we do getch
        timeout(100);
        
        refresh();
    }
    
    if (!failed)
    {
        //start input thread
        if(ARSAL_Thread_Create(&(newIHM->inputThread), IHM_InputProcessing, newIHM) != 0)
        {
            failed = 1;
        }
    }
    
    if (failed)
    {
        IHM_Delete (&newIHM);
    }

    return  newIHM;
}

void IHM_Delete (IHM_t **ihm)
{
    //  Clean up

    if (ihm != NULL)
    {
        if ((*ihm) != NULL)
        {
            (*ihm)->run = 0;
            
            if ((*ihm)->inputThread != NULL)
            {
                ARSAL_Thread_Join((*ihm)->inputThread, NULL);
                ARSAL_Thread_Destroy(&((*ihm)->inputThread));
                (*ihm)->inputThread = NULL;
            }
            
            delwin((*ihm)->mainWindow);
            (*ihm)->mainWindow = NULL;
            endwin();
            refresh();
            
            free (*ihm);
            (*ihm) = NULL;
        }
    }
}

void IHM_setCustomData(IHM_t *ihm, void *customData)
{
    if (ihm != NULL)
    {
        ihm->customData = customData;
    }
}

void *IHM_InputProcessing(void *data)
{
    IHM_t *ihm = (IHM_t *) data;
    int key = 0;
    
    if (ihm != NULL)
    {
        while (ihm->run)
        {
            key = getch();
            
            if (key == 27) // escape character
            {
                if(ihm->onInputEventCallback != NULL)
                {
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_EXIT, ihm->customData);
                }
            }
            else if(key == KEY_UP)
            {
                if(ihm->onInputEventCallback != NULL)
                {
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_FORWARD, ihm->customData);
                }
            }
            else if(key == KEY_DOWN)
            {
                if(ihm->onInputEventCallback != NULL)
                {
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_BACK, ihm->customData);
                }
            }
            else if(key == KEY_LEFT)
            {
                if(ihm->onInputEventCallback != NULL)
                {
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_LEFT, ihm->customData);
                }
            }
            else if(key == KEY_RIGHT)
            {
                if(ihm->onInputEventCallback != NULL)
                {
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_RIGHT, ihm->customData);
                }
            }
            else if(key == 'm')
            {
                if(ihm->onInputEventCallback != NULL)
                {
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_EMERGENCY, ihm->customData);
                }
            }
            else if(key == 'z')
            {
                if(ihm->onInputEventCallback != NULL)
                {
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_UP, ihm->customData);
                }
            }
            else if(key == 's')
            {
                if(ihm->onInputEventCallback != NULL)
                {
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_DOWN, ihm->customData);
                }
            }
            else if(key == 'q')
            {
                if(ihm->onInputEventCallback != NULL)
                {
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_YAW_LEFT, ihm->customData);
                }
            }
            else if(key == 'd')
            {
                if(ihm->onInputEventCallback != NULL)
                {
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_YAW_RIGHT, ihm->customData);
                }
            }
            else if(key == 'i')
            {
                if(ihm->onInputEventCallback != NULL)
                {
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_CAM_UP, ihm->customData);
                }
            }
            else if(key == 'k')
            {
                if(ihm->onInputEventCallback != NULL)
                {
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_CAM_DOWN, ihm->customData);
                }
            }
            else if(key == 'j')
            {
                if(ihm->onInputEventCallback != NULL)
                {
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_CAM_LEFT, ihm->customData);
                }
            }
            else if(key == 'l')
            {
                if(ihm->onInputEventCallback != NULL)
                {
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_CAM_RIGHT, ihm->customData);
                }
            }
            else if(key == ' ')
            {
                if(ihm->onInputEventCallback != NULL)
                {
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_TAKEOFF_LANDING, ihm->customData);
                }
            }
            else if(key == 'b')
            {
                if(ihm->onInputEventCallback != NULL)
                {

                    pthread_t thread1, thread2;
                    int iret1, iret2;

                    iret1 = pthread_create( &thread1, NULL, initializeSensors, NULL);
                    if(iret1)
                    {
                    fprintf(stderr,"Error - pthread_create() return code: %d\n",iret1);
                    exit(EXIT_FAILURE);
                    }

                    sleep(2);
                    iret2 = pthread_create( &thread2, NULL, avoidObstacles, data);
                    if(iret1)
                    {
                    fprintf(stderr,"Error - pthread_create() return code: %d\n",iret1);
                    exit(EXIT_FAILURE);
                    }



                    // ihm->onInputEventCallback (IHM_INPUT_EVENT_TAKEOFF_LANDING, ihm->customData);
                    // sleep(5);
                    //   int count1 = 0, count2 = 0, count3 = 0;

                    // while(1){
                    //     if(flag == 1){
                    //         ihm->onInputEventCallback (IHM_INPUT_EVENT_RIGHT, ihm->customData);
                    //         ++count1;
                    //           flag = 0;
                    //     }else if(flag == 2){
                    //         ihm->onInputEventCallback (IHM_INPUT_EVENT_BACK, ihm->customData);
                    //         ++count2;
                    //         flag = 0;
                    //     }else if(flag == 3){
                    //         ihm->onInputEventCallback (IHM_INPUT_EVENT_LEFT, ihm->customData);
                    //         flag = 0;
                    //         ++count3;

                    //     }
                    //     usleep(100000);
                    //     ihm->onInputEventCallback (IHM_INPUT_EVENT_NONE, ihm->customData);

                    // }
                    // flag = -1;
                    
                }
            } else if(key == 'n'){
                if(ihm->onInputEventCallback != NULL)
                {

                    //simple square route
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_TAKEOFF_LANDING, ihm->customData);
                    usleep(10000000);

                    ihm->onInputEventCallback (IHM_INPUT_EVENT_BACK, ihm->customData);
                    usleep(1000000);
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_NONE, ihm->customData);
                    usleep(1000000);
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_LEFT, ihm->customData);
                    usleep(1000000);
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_NONE, ihm->customData);
                    usleep(1000000);
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_FORWARD, ihm->customData);
                    usleep(1000000);
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_NONE, ihm->customData);
                    usleep(1000000);
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_RIGHT, ihm->customData);
                    usleep(1000000);
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_NONE, ihm->customData);
                    usleep(1000000);

                    ihm->onInputEventCallback (IHM_INPUT_EVENT_BACK, ihm->customData);
                    usleep(1000000);
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_NONE, ihm->customData);
                    usleep(1000000);
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_LEFT, ihm->customData);
                    usleep(1000000);
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_NONE, ihm->customData);
                    usleep(1000000);
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_FORWARD, ihm->customData);
                    usleep(1000000);
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_NONE, ihm->customData);
                    usleep(1000000);
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_RIGHT, ihm->customData);
                    usleep(1000000);
                    ihm->onInputEventCallback (IHM_INPUT_EVENT_NONE, ihm->customData);
                    usleep(1000000);

                    ihm->onInputEventCallback (IHM_INPUT_EVENT_TAKEOFF_LANDING, ihm->customData);
                }
            } else if(key == '1'){
                if(ihm->onInputEventCallback != NULL){

                    pthread_t thread1, thread2, thread3;
                    int iret1, iret2, iret3;

                    iret1 = pthread_create( &thread1, NULL, initializeSensors, NULL);
                    if(iret1)
                    {
                    fprintf(stderr,"Error - pthread_create() return code: %d\n",iret1);
                    exit(EXIT_FAILURE);
                    }

                    sleep(2);
                    iret2 = pthread_create( &thread2, NULL, avoidObstacles, data);
                    if(iret2)
                    {
                    fprintf(stderr,"Error - pthread_create() return code: %d\n",iret2);
                    exit(EXIT_FAILURE);
                    }
                    iret3 = pthread_create( &thread3, NULL, straight_route_with_object_avoidance, data);
                    if(iret3)
                    {
                    fprintf(stderr,"Error - pthread_create() return code: %d\n",iret3);
                    exit(EXIT_FAILURE);
                    }
                }

            }
            else
            {
                if(ihm->onInputEventCallback != NULL)
                {
                    //ihm->onInputEventCallback (IHM_INPUT_EVENT_NONE, ihm->customData);
                }
            }
            
            usleep(10);
        }
    }
    
    return NULL;
}

void IHM_PrintHeader(IHM_t *ihm, char *headerStr)
{
    if (ihm != NULL)
    {
        move(HEADER_Y, 0);   // move to begining of line
        clrtoeol();          // clear line
        mvprintw(HEADER_Y, HEADER_X, headerStr);
    }
}

void IHM_PrintInstruction(IHM_t *ihm, char *instructionStr)
{
    if (ihm != NULL)
    {
        move(INSTRUCTION_Y, 0);   // move to begining of line
        clrtoeol();          // clear line
        mvprintw(INSTRUCTION_Y, INSTRUCTION_X, instructionStr);
    }
}

void IHM_PrintInfo(IHM_t *ihm, char *infoStr)
{
    if (ihm != NULL)
    {
        move(INFO_Y, 0);    // move to begining of line
        clrtoeol();         // clear line
        mvprintw(INFO_Y, INFO_X, infoStr);
    }
}

void IHM_PrintBattery(IHM_t *ihm, uint8_t percent)
{
    if (ihm != NULL)
    {
        move(BATTERY_Y, 0);     // move to begining of line
        clrtoeol();             // clear line
        mvprintw(BATTERY_Y, BATTERY_X, "Battery: %d", percent);
    }
}



