/* *********************************************************************
 *
 * Set Jump and Long Jump
 *
 * Program Name:        Program to demonstrate setjump and longjump
 * Target:              Intel machines
 * Architecture:		x86
 * Compiler:            gcc
 * File version:        v1.0.0
 * Author:              Brahmesh S D Jain
 * Email Id:            Brahmesh.Jain@asu.edu
 **********************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>
#include <time.h>
#include <setjmp.h>
#include <linux/input.h>
#define MOUSE_DEVICE_FILE   "/dev/input/event4"
#define MILISEC_MOUSE_DELAY 500
#define CPU_FREQ_MHZ 2400
jmp_buf jumpoint;

/*
 * Assembly level read Time stamp counter
 */

static __inline__ unsigned long long rdtsc(void)
{
    unsigned long long int x;
    __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
    return x;
}
/* *********************************************************************
 * NAME:             MouseHandlerThread
 * CALLED BY:        Started by main() thread
 * DESCRIPTION:      Handles the button click from the mouse
 * INPUT PARAMETERS: dummy not used
 * RETURN VALUES:    None
 * REFERENCE: http://stackoverflow.com/questions/588307/c-obtaining-
 *            milliseconds-time-on-linux-clock-doesnt-seem-to-work-properl
 ***********************************************************************/
void* MouseHandlerThread(void* dummy)
{
	int MouseEventFd;
	struct input_event MouseEvent;
	struct timeval time_first_click,time_second_click;
	unsigned char FirstClickDone = 0;
	long seconds,useconds,mtime;
	if((MouseEventFd = open(MOUSE_DEVICE_FILE, O_RDONLY)) < 0)
	{
		perror("opening device");
		return NULL;
	}
	while(read(MouseEventFd, &MouseEvent, sizeof(struct input_event)))
	{
		if((MouseEvent.type == EV_KEY) && (MouseEvent.value == 0) && (MouseEvent.code == BTN_RIGHT))
		{
			if (0 == FirstClickDone)
			{
				/* this is the first click */
				gettimeofday(&time_first_click,NULL);
				FirstClickDone = 1;
				printf("\n Right click was done\n");
			}
			else
			{
				/* second click*/
				gettimeofday(&time_second_click,NULL);
				seconds  = time_second_click.tv_sec  - time_first_click.tv_sec;
				useconds = time_second_click.tv_usec - time_first_click.tv_usec;
			    mtime = ((seconds) * 1000 + useconds/1000.0) + 0.5;
			    if (mtime < MILISEC_MOUSE_DELAY)
			    {
					/* double click capture */
					/* detected double click */
					/* call long jump signal handler */
					printf("\n Right double click was done \n");
					kill(0,SIGUSR1);
				}
				else
				{
					/* make this click as the first click */
					time_first_click = time_second_click;
					printf("\n Right click was done\n");
				}
			}
		}
	}
	printf("\n Exiting the MouseHandlerThread");
}

void SignalHandler(int sig_num)
{
	longjmp(jumpoint,-1);
	return;
}
int main()
{
	struct sigaction SignalAction;
	sigset_t block_mask;
	pthread_t MouseHandlerThreadId;
	int result = 0;
	/* create mouse thread to handle mouse event */
	pthread_create(&MouseHandlerThreadId,NULL,&MouseHandlerThread,NULL);
	
    /* unblock signal SIGUSR1 for this thread */
	memset(&SignalAction, 0, sizeof(SignalAction));
	sigfillset(&block_mask);
	SignalAction.sa_mask = block_mask;
	SignalAction.sa_handler = &SignalHandler;
	sigaction(SIGUSR1, &SignalAction, NULL);
    printf("\n\n Right double click to stop computation ");
	if (setjmp(jumpoint) == 0)
	{
		while(1)
		{
			result = result + 1;
			sleep(1);
		}
	}
	else
	{
		printf("Result of computation/time elapsed since program start %d sec\n\n",result);
	}
	return 0;
}

