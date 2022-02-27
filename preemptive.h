/*
 * file: cooperative.h
 *
 * this is the include file for the cooperative multithreading
 * package.  It is to be compiled by SDCC and targets the EdSim51 as
 * the target architecture.
 *
 * CS 3423 Fall 2018
 */

#ifndef __PREEMPTIVE_H__
#define __PREEMPTIVE_H__

#define MAXTHREADS 3  /* not including the scheduler */
/* the scheduler does not take up a thread of its own */


typedef char ThreadID;
typedef void (*FunctionPtr)(void);

ThreadID ThreadCreate(FunctionPtr);
void ThreadYield(void);
void ThreadExit(void);
void myTimer0Handler(void);
void delay(unsigned char);
unsigned char now(void);

typedef struct{
			signed char value;
			unsigned char waitQueue;
		} Semaphore;

 __data __at (0x20) char savedSP[4];
 __data __at (0x24) union {
	struct{
		unsigned char bitmap : 4;
		unsigned char currentID : 2;
		unsigned char count : 2;
	};
 }thread;
 __data __at (0x25) char tempSP;
 __data __at (0x26) char tempACC;
 __data __at (0x27) char tempPSW;
 __data __at (0x28) char newThreadID;
 __data __at (0x29) unsigned char threadDelay[3];
 __data __at (0x2C) unsigned char elapsed_time;
 __data __at (0x2D) unsigned char even_time;
 __data __at (0x2E) Semaphore sem_car;
 
 __data __at (0x30) char sleep;
 
 __data __at (0x31) unsigned char readyQueue;
 __data __at (0x32) unsigned char readyQueueTail;
 __data __at (0x33) Semaphore sema_thread;



#define SAVELOCAL\
	__asm\
		push ar7\
		mov ar7, r7\
	__endasm;
#define RESTORELOCAL\
	__asm\
		mov r7, ar7\
		pop ar7\
	__endasm;

// // semaphore busy waiting
// // create a counting semaphore s that is initialized to n
// #define SemaphoreCreate(s, n) s = n;
		
// // do (busy-) wait() on semaphore s	
// #define SemaphoreWaitBody(s,label)\
	// {__asm \
		// PUSH ACC\
		// label ##$:\
		// MOV ACC, CNAME(s) \
		// JZ label ##$\
		// JB  ACC.7, label ##$\
		// DEC CNAME(s)\
		// POP ACC\
	// __endasm;} 
	    // //PSW = tempPSW;}
// #define SemaphoreWaitDress(s,label) SemaphoreWaitBody(s,label)
// #define SemaphoreWait(s) SemaphoreWaitDress(s,__COUNTER__)

// // signal () semaphore s

// #define CNAME(s) _ ## s
// #define SemaphoreSignal(s)\
	// { __asm;         \
		// INC CNAME(s) \
	  // __endasm;}   


//Semaphore non busy waiting

#define SemaphoreCreate(s, n) {(s)->value = n;(s)->waitQueue = 0;}
void SemaphoreWait(Semaphore *s);
void SemaphoreSignal(Semaphore *s);




#endif // __PREEMPTIVE_H__
