/* *********************************************************************
 *
 * mysiglib header file
 *
 * Program Name:        library header file that needs to be included by
						the module which is using mysigio
 * Target:              Intel machines
 * Architecture:		x86
 * Compiler:            gcc
 * File version:        v1.0.0
 * Author:              Brahmesh S D Jain
 * Email Id:            Brahmesh.Jain@asu.edu
 **********************************************************************/
 
/* *********************************************************************
 * NAME:             mysiglib_init
 * CALLED BY:        module using this library
 * DESCRIPTION:      intializes library
 * INPUT PARAMETERS: none
 * RETURN VALUES:    None
 ***********************************************************************/
extern void mysiglib_init(void);

/* *********************************************************************
 * NAME:             mysiglib_deinit
 * CALLED BY:        module using this library
 * DESCRIPTION:      deintializes library
 * INPUT PARAMETERS: none
 * RETURN VALUES:    None
 ***********************************************************************/
extern void mysiglib_deinit(void);

/* *********************************************************************
 * NAME:             mysiglib_reg_handler
 * CALLED BY:        user program
 * DESCRIPTION:      user program to register a signal handler for SIGIO
 * INPUT PARAMETERS: threadid which is to be registered
 * RETURN VALUES:    0 if success
 ***********************************************************************/ 
extern int mysiglib_reg_handler(pthread_t threadid);

/* *********************************************************************
 * NAME:             mysiglib_unreg_handler
 * CALLED BY:        user program
 * DESCRIPTION:      user program to unregister a signal handler for SIGIO
 * INPUT PARAMETERS: threadid which is to be unregistered
 * RETURN VALUES:    0 if success
 ***********************************************************************/
extern int mysiglib_unreg_handler(pthread_t threadid);


