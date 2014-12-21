/***********************************************************************************************
************************** Steps for running the program****************************************
***********************************************************************************************/
1. TASK1 :
   Compile the program by running the command 
     "gcc -o task1.o main_1.c -lpthread"
   Run the program with root permission
     "sudo ./task1.o"
   Double click right button to stop the computation and report the elapsed time
2. TASK2 :
   Compile the program by running the command 
     "gcc -o task2.o main_2.c -lpthread"
   Run the program with root permission
     "sudo ./task12.o"
   wait for the all the tests to get completed.
3. TASK3 :
   First Part:
   Compile the program by running the commad
    "gcc -o main_3_1.o main_3_1.c -lpthread"
    Run the program by running the command 
    "sudo ./main_3_1.o"
    Make the mouse movements to see the SIGIO delivered to random thread
    press cntrl+c to terminate
    Second Part:
    Compile the program by running the command
     "gcc -o task3.o mysiglib.c mysiglib_test.c -lpthread"
     run the program by ruuning the command
     "sudo ./task3.o"
     play with the mouse for next 45sec or terminate the program with control + C
/***********************************************************************************************
************************** Approach in TASK1 ***************************************************
***********************************************************************************************/
1. Mouse device was at "/dev/input/event4" in my system. If you want to change, please modify the 
macro accordinglt #define MOUSE_DEVICE_FILE   "/dev/input/event4"

2. In task1, the mouse thread is created to handle double click on the mouse right button. The 
resonse time for double click has been set to 500ms. This can be change to any value by modifying
the macro, #define MILISEC_MOUSE_DELAY 500

3. When the double click is detected, the mouse thread sends SIGUSR1 signal to the main task.

4. Main task will be set the jump at the if branch statement and continues to keep counting the 
number of seconds elapsed from the start of the program. When signal is received at its signal 
handler function it will longjmp to the start of the if branch and terminates after printing the
appropriate message

/***********************************************************************************************
************************** Approach in TASK3 ***************************************************
***********************************************************************************************/
First part of the task is implemented in the main_3_1.c. It can be compiled and run by running
the command " gcc -o main_3_1.o main_3_1.c -lpthread". This program generates SIGIO signal by 
monitoring the mice events. Any event will trigger the SIGIO event. This will inturn trigger the
processes waiting for the these events. They receive event randomly. We can observe that in the
output.

Second part of the task involves writing the library for making the SIGIO signal available to
all the thread that have registered with the library. This how the library is implemented.
1) Module completely handles the SIGIO signal from the mouse. Hence it acts as a single point 
of contact for mouse SIGIO event. All other threads has to register with this library to receive
this signal
2) Module provides four functions for interfacing:- mysiglib_init,mysiglib_deinit, mysiglib_reg_handler,
mysiglib_unreg_handler. Respective functions are described and externed in the mysiglib.h
3) Any thread that wants to receive the SIGIO should register with mysiglib with its pthread_id.
when the SIGIO is received from the mice, will result in the pthread_kill(SIGUSR1,pthread_id) for
all the registered threads, Hence the anticipating thread should register the signal handler that
waits for SIGUSR1 signal from the library.
4) This has been tested in the mysiglib_test.c program.
