/* *********************************************************************
 *
 * User level program 
 *
 * Program Name:        Program to test first part of the TASK3 
 * Target:              Intel machines
 * Architecture:		x86
 * Compiler:            gcc
 * File version:        v1.0.0
 * Author:              Brahmesh S D Jain
 * Email Id:            Brahmesh.Jain@asu.edu
 **********************************************************************/

/* *************** INCLUDE DIRECTIVES FOR STANDARD HEADERS ************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include "mysiglib.h"
//#define DEBUG
pthread_t ThreadId[6] = {0};
/*
 * Mutex to protect the above global variable
 */
pthread_mutex_t ThreadIdMutex = PTHREAD_MUTEX_INITIALIZER;

/* *********************************************************************
 * NAME:             SignalHandlerAllThreads
 * CALLED BY:        kernel
 * DESCRIPTION:      called upon receiving SIGIO from any thread
 * INPUT PARAMETERS: sig_num not used
 * RETURN VALUES:    None
 ***********************************************************************/
void SignalHandlerAllThreads(int sig_num)
{
	pthread_mutex_lock(&ThreadIdMutex);
	if (pthread_equal(pthread_self(),ThreadId[0]))
	{
		printf("\n Thread0 signal handler was called by the SIGIO event");
	}
	else if (pthread_equal(pthread_self(),ThreadId[1]))
	{
		printf("\n Thread1 signal handler was called by the SIGIO event");
	}
	else if (pthread_equal(pthread_self(),ThreadId[2]))
	{
		printf("\n Thread2 signal handler was called by the SIGIO event");
	}
	else if (pthread_equal(pthread_self(),ThreadId[3]))
	{
		printf("\n Thread3 signal handler was called by the SIGIO event");
	}
	else if (pthread_equal(pthread_self(),ThreadId[4]))
	{
		printf("\n Thread4 signal handler was called by the SIGIO event");
	}
	else if (pthread_equal(pthread_self(),ThreadId[5]))
	{
		printf("\n Thread5 signal handler was called by the SIGIO event");
	}
	else
	{
#ifdef DEBUG
		printf("\n Error signal handling");
#endif
	}
	pthread_mutex_unlock(&ThreadIdMutex);
#ifdef DEBUG
	printf("\n SignalHandlerAllThreads - Pthread id = %lu",pthread_self());
	printf("\n SignalHandlerAllThreads - PPID id = %d",getpid());
#endif
}
/* *********************************************************************
 * NAME:             TestThread
 * CALLED BY:        kernel
 * DESCRIPTION:      Thread that tests the mysiglib
 * INPUT PARAMETERS: dummy not used
 * RETURN VALUES:    None
 ***********************************************************************/
void* TestThread(void* dummy)
{
	struct sigaction SignalAction;
	sigset_t block_mask;
	int ret,flags;
#ifdef DEBUG
	printf("\nTestThread PID = %d",getpid());
	printf("\nTestThread PPID = %d",getppid());
	printf("\nTestThread Phtread_ID = %lu",pthread_self());
#endif
    /* unblock signal SIGIO for this thread */
	memset(&SignalAction, 0, sizeof(SignalAction));
	sigfillset(&block_mask);
	SignalAction.sa_mask = block_mask;
	SignalAction.sa_handler = &SignalHandlerAllThreads;
	sigaction(SIGUSR1, &SignalAction, NULL);
//    pthread_sigmask(SIG_UNBLOCK, &block_mask, NULL);
	/* run till cntrl+c is pressed */
	while(1);
	return NULL;
}
/* *********************************************************************
 * NAME:             main
 * CALLED BY:        kernel
 * DESCRIPTION:      main that tests the part 3
 * INPUT PARAMETERS: none
 * RETURN VALUES:    None
 ***********************************************************************/
int main()
{
	struct sigaction SignalAction;
	sigset_t block_mask;
	int MouseEventFd,ret,flags,i;
    struct timespec sleeptime = {45,0},rem;
#ifdef DEBUG
	printf("\nMainthread PID = %d",getpid());
	printf("\nMainthread PPID = %d",getppid());
	printf("\nMainthread Phtread_ID = %lu",pthread_self());
#endif
	pthread_mutex_lock(&ThreadIdMutex);
	/* get the pthread Ids of the three testing threads */
	pthread_create(&ThreadId[0],NULL,&TestThread,NULL);
	pthread_create(&ThreadId[1],NULL,&TestThread,NULL);
	pthread_create(&ThreadId[2],NULL,&TestThread,NULL);
	pthread_create(&ThreadId[3],NULL,&TestThread,NULL);
	pthread_create(&ThreadId[4],NULL,&TestThread,NULL);
	pthread_create(&ThreadId[5],NULL,&TestThread,NULL);

	/* initialize mysiglib library */
	mysiglib_init();
	/* Register each thread for the SIGIO event */
	for(i = 0; i < 6; i++)
	{
		if (mysiglib_reg_handler(ThreadId[i]) < 0)
		{
			printf("\n unable to register the SIGIO for Thread%d with mysiglib",i);
		}
	}
	pthread_mutex_unlock(&ThreadIdMutex);
	/* Print message to move the mouse */
	printf("\n Enjoy playing with mouse for 45 sec");
	printf("\n Create mouse event by moving the mouse or clicking the button");
	/* run the test prograam for 45 sec */
	while(0 != nanosleep(&sleeptime,&rem))
	{
		memcpy(&sleeptime,&rem,sizeof(struct timespec));
	}
	/* Unregister all the threads from the library */
	pthread_mutex_lock(&ThreadIdMutex);
	for(i = 0; i < 6; i++)
	{
		if (mysiglib_unreg_handler(ThreadId[i]) < 0)
		{
			printf("\n unable to unregister the SIGIO for Thread%d with mysiglib",i);
		}
	}
	pthread_mutex_unlock(&ThreadIdMutex);
	/* Deintialize the mysiglib */
	mysiglib_deinit();
	return 0;
}
