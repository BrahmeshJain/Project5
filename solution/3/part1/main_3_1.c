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
//#define DEBUG
pthread_t ThreadId[3] = {0};
/* *********************************************************************
 * NAME:             SignalHandlerAllThreads
 * CALLED BY:        kernel
 * DESCRIPTION:      called upon receiving SIGIO from any thread
 * INPUT PARAMETERS: sig_num not used
 * RETURN VALUES:    None
 ***********************************************************************/
void SignalHandlerAllThreads(int sig_num)
{
	if (pthread_equal(pthread_self(),ThreadId[0]))
	{
		printf("\n Main Thread signal handler was called by the SIGIO event");
	}
	else if (pthread_equal(pthread_self(),ThreadId[1]))
	{
		printf("\n Thread1 signal handler was called by the SIGIO event");
	}
	else if (pthread_equal(pthread_self(),ThreadId[2]))
	{
		printf("\n Thread2 signal handler was called by the SIGIO event");
	}
	else
	{
		printf("\n Error signal handling");
	}
#ifdef DEBUG
	printf("\n Pthread id = %lu",pthread_self());
	printf("\n PPID id = %d",getpid());
#endif
	return;
}
/* *********************************************************************
 * NAME:             TestThread1
 * CALLED BY:        kernel
 * DESCRIPTION:      Thread1 that tests the part 3
 * INPUT PARAMETERS: dummy not used
 * RETURN VALUES:    None
 ***********************************************************************/
void* TestThread1(void* dummy)
{
	struct sigaction SignalAction;
	sigset_t block_mask;
	int MouseEventFd,ret,flags;
	if((MouseEventFd = open("/dev/input/mice", O_RDONLY)) < 0)
	{
		perror("opening device");
		return 0;
	}
#ifdef DEBUG
	printf("\nTestThread1 PID = %d",getpid());
	printf("\nTestThread1 PPID = %d",getppid());
	printf("\nTestThread1 Phtread_ID = %lu",pthread_self());
#endif
    /* unblock signal SIGIO for this thread */
	memset(&SignalAction, 0, sizeof(SignalAction));
	sigfillset(&block_mask);
	SignalAction.sa_mask = block_mask;
	SignalAction.sa_handler = &SignalHandlerAllThreads;
	sigaction(SIGIO, &SignalAction, NULL);
    pthread_sigmask(SIG_UNBLOCK, &block_mask, NULL);
    /* Enable asynchronous notification from mouse*/
	ret = fcntl(MouseEventFd, F_SETOWN, getpid());
	if (ret < 0)
	{
		perror("fcntl1");
	}
	flags = fcntl(MouseEventFd, F_GETFL);
	fcntl(MouseEventFd, F_SETFL, flags | FASYNC);
	/* run till cntrl+c is pressed */
	while(1);
	return NULL;
}
/* *********************************************************************
 * NAME:             TestThread2
 * CALLED BY:        kernel
 * DESCRIPTION:      Thread1 that tests the part 3
 * INPUT PARAMETERS: dummy not used
 * RETURN VALUES:    None
 ***********************************************************************/
void* TestThread2(void* dummy)
{
	struct sigaction SignalAction;
	sigset_t block_mask;
	int MouseEventFd,ret,flags;
	if((MouseEventFd = open("/dev/input/mice", O_RDONLY)) < 0)
	{
		perror("opening device");
		return 0;
	}
#ifdef DEBUG
	printf("\nTestThread2 PID = %d",getpid());
	printf("\nTestThread2 PPID = %d",getppid());
	printf("\nTestThread2 Phtread_ID = %lu",pthread_self());
#endif
    /* unblock signal SIGIO for this thread */
	memset(&SignalAction, 0, sizeof(SignalAction));
	sigfillset(&block_mask);
	SignalAction.sa_mask = block_mask;
	SignalAction.sa_handler = &SignalHandlerAllThreads;
	sigaction(SIGIO, &SignalAction, NULL);
    pthread_sigmask(SIG_UNBLOCK, &block_mask, NULL);
	/* Enable asynchronous notification from mouse */
	ret = fcntl(MouseEventFd, F_SETOWN, getpid());
	if (ret < 0)
	{
		perror("fcntl1");
	}
	flags = fcntl(MouseEventFd, F_GETFL);
	fcntl(MouseEventFd, F_SETFL, flags | FASYNC);
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
	int MouseEventFd,ret,flags;
	
	/* get the pthread Ids of the three testing threads */
	ThreadId[0] = pthread_self();
	pthread_create(&ThreadId[1],NULL,&TestThread1,NULL);
	pthread_create(&ThreadId[2],NULL,&TestThread2,NULL);
	
	/* open mice device to capture mouse events */
	if((MouseEventFd = open("/dev/input/mice", O_RDONLY)) < 0)
	{
		perror("opening device");
		return 0;
	}
#ifdef DEBUG
	printf("\nMainthread PID = %d",getpid());
	printf("\nMainthread PPID = %d",getppid());
	printf("\nMainthread Phtread_ID = %lu",pthread_self());
#endif
    /* Unblock signal for SIGIO */
	memset(&SignalAction, 0, sizeof(SignalAction));
	sigfillset(&block_mask);
	SignalAction.sa_mask = block_mask;
	SignalAction.sa_handler = &SignalHandlerAllThreads;
	sigaction(SIGIO, &SignalAction, NULL);
	/* Enable asynchronous signal notification for mice */
	ret = fcntl(MouseEventFd, F_SETOWN, getpid());
	if (ret < 0)
	{
		perror("fcntl1");
	}
	flags = fcntl(MouseEventFd, F_GETFL);
	fcntl(MouseEventFd, F_SETFL, flags | FASYNC);
	/* run till cntrl+c is pressed */
	while(1);
	return 0;
}
