/* *********************************************************************
 *
 * User level program 
 *
 * Program Name:        Signal handling in Linux
 * Target:              Intel Galileo Gen1
 * Architecture:		x86
 * Compiler:            i586-poky-linux-gcc
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
#define SLEEP_TIME 1
//#define DEBUG
volatile sig_atomic_t Terminate_thread = 0;
/*
 * Assembly level read Time stamp counter
 */

static __inline__ unsigned long long rdtsc(void)
{
    unsigned long long int x;
    __asm__ volatile (".byte 0x0f, 0x31" : "=A" (x));
    return x;
}
static sem_t Task2CSemaphore;
/* *********************************************************************
 * NAME:             SignalGenerateThread
 * CALLED BY:        Started by main() thread
 * DESCRIPTION:      Generates a signal after 5 seconds
 * INPUT PARAMETERS: dummy not used
 * RETURN VALUES:    None
 ***********************************************************************/
void* SignalGenerateThread(void* dummy)
{
	sleep(SLEEP_TIME);
#ifdef DEBUG
	printf ("\nSignalGenerateThread : Wakeup from sleep  My pid is %d and parent pid is %d",(int)getpid(),(int)getppid());
#endif
    /* Generate the signal now*/
	kill(0,SIGUSR1);
    return NULL;
}
/* *********************************************************************
 * NAME:             SignalHandler
 * CALLED BY:        Kernel when SignalGenerateThread generates a signal
 * DESCRIPTION:      For testing it just prints the message and raises
 *                   flag for the termination of Test1
 * INPUT PARAMETERS: sig_num not used
 * RETURN VALUES:    None
 ***********************************************************************/
void SignalHandler(int sig_num)
{
#ifdef DEBUG
	printf("\nSignal Handler was called now at %llu",rdtsc());
#endif
	Terminate_thread = 1;
	return;
}
int main()
{
	struct sigaction SignalAction;
	pthread_t SignalGenerateThreadId;
	sigset_t block_mask;
	unsigned char EndDoWhile = 0;
    pthread_t this_thread = pthread_self();
	// struct sched_param is used to store the scheduling priority
	struct sched_param OriginalThreadParams, ModifiedThreadParams;
    int OriginalPolicy, ModifiedPolicy,ret;
    struct timespec sleeptime = {10,0},rem;
    /* Initialize semaphore */
    sem_init(&Task2CSemaphore,0,0);
	memset(&SignalAction, 0, sizeof(SignalAction));
	sigfillset(&block_mask);
	SignalAction.sa_mask = block_mask;
	SignalAction.sa_handler = &SignalHandler;
	sigaction(SIGUSR1, &SignalAction, NULL);
    /* ***********************************************************************************************
    *********************************TASK 2A**********************************************************
    *************************************************************************************************/
    printf("\n*********************************************************************************************");
	printf("\nTest2A : Checking signal handling functionality when the thread is running");
    printf("\n*********************************************************************************************");
	printf("\nStarting signal generating thread, which generrates the signal after 3 second");
	pthread_create(&SignalGenerateThreadId,NULL,&SignalGenerateThread,NULL);
	do
	{
		if (0 != Terminate_thread)
		{
			EndDoWhile = 1;
			printf("\nSignal Handler requested to stop computation");
			Terminate_thread = 0;
		}
	}while(0 == EndDoWhile);
	EndDoWhile = 0;
	printf("\nTask2A completed \n\n");
    /* Wait for SignalGenerateThread to get over */
    pthread_join(SignalGenerateThreadId, NULL);  
    
    /* ***********************************************************************************************
    *********************************TASK 2B**********************************************************
    *************************************************************************************************/
    printf("\n*********************************************************************************************");
	printf("\nTest2B : Checking signal handling functionality when the thread is runnable(Realtime priority)");
    printf("\n*********************************************************************************************");
    ret = pthread_getschedparam(this_thread, &OriginalPolicy, &OriginalThreadParams);
    if(ret != 0)
    {
        printf("\nCouldn't retrieve scheduling paramers");
        return 0;
    }
    else
    {
		printf("\nPresent priority of the task is %d",OriginalThreadParams.sched_priority);
	}
	/* Change the policy and priority to a maximum value */
	ModifiedThreadParams.sched_priority = sched_get_priority_max(SCHED_RR);
	printf("\nTrying to set thread realtime prio = %d",ModifiedThreadParams.sched_priority);
	 
	/* Attempt to set thread real-time priority to the SCHED_RR policy */
	ret = pthread_setschedparam(this_thread, SCHED_RR, &ModifiedThreadParams);
	if (ret != 0)
	{
		printf("\nUnsuccessful in setting thread realtime prio");
		return;
	}
	else
	{
		printf("\nSuccessful in setting thread realtime prio");
	}
	printf("\nStarting signal generating thread, which generrates thae signal after 2 second");
	pthread_create(&SignalGenerateThreadId,NULL,&SignalGenerateThread,NULL);
	do
	{
		if (0 != Terminate_thread)
		{
			EndDoWhile = 1;
			printf("\nSignal Handler requested to stop computation");
			Terminate_thread = 0;
		}
	}while(0 == EndDoWhile);
	EndDoWhile = 0;
    /* Wait for SignalGenerateThread to get over */
    pthread_join(SignalGenerateThreadId, NULL);
    /* Change the task priorities to normal */
	ret = pthread_setschedparam(this_thread, SCHED_OTHER, &OriginalThreadParams);
	if (ret != 0)
	{
		printf("\nUnsuccessful in setting thread back to standard priority");
		return;
	}
	else
	{
		printf("\nSuccessful in setting thread to normal priority");
	}
	printf("\nTask2B completed\n\n");
    /* ***********************************************************************************************
    *********************************TASK 2C**********************************************************
    *************************************************************************************************/
    printf("\n*********************************************************************************************");
	printf("\nTest2C : Checking signal handling functionality when the thread is blocked by a semaphore");
    printf("\n*********************************************************************************************");
	pthread_create(&SignalGenerateThreadId,NULL,&SignalGenerateThread,NULL);
	/* Block the task */
	printf("\nwaiting for the semaphore");
    ret = sem_wait(&Task2CSemaphore);
    if(ret < 0)
    {
		perror("Semaphore");
	}
	else
	{
		printf("\nSemaphore released without any error");
		return 0;
	}
	printf("\nTask2C completed \n\n");
    /* Wait for SignalGenerateThread to get over */
    pthread_join(SignalGenerateThreadId, NULL); 
	Terminate_thread = 0;
    /* ***********************************************************************************************
    *********************************TASK 2D**********************************************************
    *************************************************************************************************/
    printf("\n*********************************************************************************************");
	printf("\nTest2D : Checking signal handling functionality when the thread is delayed");
    printf("\n*********************************************************************************************");
	pthread_create(&SignalGenerateThreadId,NULL,&SignalGenerateThread,NULL);
	/* Block the task */
	printf("\nMain task going to sleep for 10 secs");
    ret = nanosleep(&sleeptime,&rem);
    if (ret < 0)
    {
		perror("nanosleep");
		printf("\nRemaining time is %d",(int)rem.tv_sec);
	}
    printf("\nMain Task returned from sleep");
	printf("\nTask2D completed\n\n");
    /* Wait for SignalGenerateThread to get over */
    pthread_join(SignalGenerateThreadId, NULL);  
	return 0;
}
