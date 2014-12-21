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
#include <stdbool.h>
//#define DEBUG

struct mysiglib_struct
{
    pthread_t kill_threadId;
    struct mysiglib_struct *next;
};
struct mysiglib_struct *head = NULL, *current = NULL;
volatile bool deinit_flag = false;

/* *********************************************************************
 * NAME:             mysiglib_reg_handler
 * CALLED BY:        user program
 * DESCRIPTION:      user program to register a signal handler for SIGIO
 * INPUT PARAMETERS: threadid which is to be registered
 * RETURN VALUES:    0 if success
 ***********************************************************************/
int mysiglib_reg_handler(pthread_t threadid)
{
	struct mysiglib_struct *ptr;
	if (NULL == head)
	{
		ptr = (struct mysiglib_struct*)malloc(sizeof(struct mysiglib_struct));
		if (NULL == ptr)
		{
			printf("\n linked list Node creation failed \n");
			return -1;
		}
		else
		{
			ptr->kill_threadId = threadid;
			ptr->next = NULL;
			head = ptr;
			current = ptr;
		}
	}
	else
	{
		ptr = (struct mysiglib_struct*)malloc(sizeof(struct mysiglib_struct));
		if (NULL == ptr)
		{
			printf("\n linked list Node creation failed \n");
			return -1;
		}
		else
		{
			ptr->kill_threadId = threadid;
			ptr->next = NULL;
			current->next = ptr;
			current = ptr;
		}
	}
#ifdef DEBUG
	printf("\n mysiglib registered thread %lu",threadid);
#endif
	return 0;
}
/* *********************************************************************
 * NAME:             search_in_list
 * CALLED BY:        mysiglib_unreg_handler
 * DESCRIPTION:      searches the linked list and return the pointer
 * INPUT PARAMETERS: val : threadid tobe searched 
 * 					 prev : pointer to pointer of the prev entry
 * RETURN VALUES:    0 if success
 * REFERENCE:		 http://www.thegeekstuff.com/2012/08/c-linked-list-example/
 ***********************************************************************/
static struct mysiglib_struct* search_in_list(pthread_t val, struct mysiglib_struct **prev)
{
    struct mysiglib_struct *ptr = head;
    struct mysiglib_struct *tmp = NULL;
    bool found = false;

    while(ptr != NULL)
    {
        if(ptr->kill_threadId == val)
        {
            found = true;
            break;
        }
        else
        {
            tmp = ptr;
            ptr = ptr->next;
        }
    }

    if(true == found)
    {
        if(prev)
            *prev = tmp;
        return ptr;
    }
    else
    {
        return NULL;
    }
}
/* *********************************************************************
 * NAME:             mysiglib_unreg_handler
 * CALLED BY:        user program
 * DESCRIPTION:      user program to unregister a signal handler for SIGIO
 * INPUT PARAMETERS: threadid which is to be unregistered
 * RETURN VALUES:    0 if success
 * REFERENCE:		 http://www.thegeekstuff.com/2012/08/c-linked-list-example/
 ***********************************************************************/
int mysiglib_unreg_handler(pthread_t threadid)
{
    struct mysiglib_struct *prev = NULL;
    struct mysiglib_struct *del = NULL;

    del = search_in_list(threadid,&prev);
    if(del == NULL)
    {
        return -1;
    }
    else
    {
        if(prev != NULL)
            prev->next = del->next;

        if(del == current)
        {
            current = prev;
        }
        else if(del == head)
        {
            head = del->next;
        }
    }
    free(del);
    del = NULL;
#ifdef DEBUG
	printf("\n mysiglib unregistered thread %lu",threadid);
#endif
    return 0;
}
/* *********************************************************************
 * NAME:             SignalHandlerAllThreads
 * CALLED BY:        kernel
 * DESCRIPTION:      called upon receiving SIGIO from any thread
 * INPUT PARAMETERS: sig_num not used
 * RETURN VALUES:    None
 ***********************************************************************/
void SignalHandlerLibrary(int sig_num)
{
	struct mysiglib_struct *ptr = head;
#ifdef DEBUG
	printf("\nSIGIO event recieved by mysiglib");
#endif
	/* send the SIGIO signal to all the registered threads*/
    while (ptr != NULL)
    {
#ifdef DEBUG
		printf("\n Calling kill of %lu",ptr->kill_threadId);
#endif
		/* to make sure that each mouse event triggers all thread handlers */
		printf("\n");
		pthread_kill(ptr->kill_threadId,SIGUSR1);
        ptr = ptr->next;
    }
    return;
}

/* *********************************************************************
 * NAME:             mysiglib_main
 * CALLED BY:        kernel
 * DESCRIPTION:      Library mainfunction that receives the SIGIO signal
 * INPUT PARAMETERS: none
 * RETURN VALUES:    None
 ***********************************************************************/
void* mysiglib_main(void* dummy)
{
	struct sigaction SignalAction;
	sigset_t block_mask;
	int MouseEventFd,ret,flags;
	
	/* open mice device to capture mouse events */
	if((MouseEventFd = open("/dev/input/mice", O_RDONLY)) < 0)
	{
		perror("opening device");
		return 0;
	}
#ifdef DEBUG
	printf("\nmysiglib_main PID = %d",getpid());
	printf("\nmysiglib_main PPID = %d",getppid());
	printf("\nmysiglib_main Phtread_ID = %lu\n",pthread_self());
#endif
    /* Unblock signal for SIGIO */
	memset(&SignalAction, 0, sizeof(SignalAction));
	sigfillset(&block_mask);
	SignalAction.sa_mask = block_mask;
	SignalAction.sa_handler = &SignalHandlerLibrary;
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
	while(deinit_flag == false)
	{
		;
	}
	return NULL;
}
/* *********************************************************************
 * NAME:             mysiglib_init
 * CALLED BY:        module using this library
 * DESCRIPTION:      intializes library
 * INPUT PARAMETERS: none
 * RETURN VALUES:    None
 ***********************************************************************/
void mysiglib_init(void)
{
	pthread_t mysiglib_main_threadid;
	pthread_create(&mysiglib_main_threadid,NULL,&mysiglib_main,NULL);
#ifdef DEBUG
	printf("\nInitialized mysiglib");
#endif
}
/* *********************************************************************
 * NAME:             mysiglib_deinit
 * CALLED BY:        module using this library
 * DESCRIPTION:      deintializes library
 * INPUT PARAMETERS: none
 * RETURN VALUES:    None
 ***********************************************************************/
void mysiglib_deinit(void)
{
	deinit_flag = true;
#ifdef DEBUG
	printf("\nDeInitialized mysiglib\n");
#endif
}
