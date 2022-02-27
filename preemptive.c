#include <8051.h>

#include "preemptive.h"


#define SAVESTATE  \
        {__asm     \
         PUSH ACC  \
		 PUSH B    \
		 PUSH DPL  \
		 PUSH DPH  \
		 PUSH PSW  \
         __endasm; \
		 savedSP[thread.currentID]=SP;\
		 SP = savedSP[0];\
		 PSW = 0;\
        }

#define RESTORESTATE \
         { \
			savedSP[0] = SP;\
			SP = savedSP[thread.currentID];\
			__asm   \
           POP PSW  \
		   POP DPH  \
		   POP DPL  \
		   POP B    \
		   POP ACC  \
          __endasm; \
         }


 /* 
  * we declare main() as an extern so we can reference its symbol
  * when creating a thread for it.
  */

extern void main(void);



void Bootstrap(void) {

		TMOD = 0;  	// timer 0 mode 0
		IE = 0x82;  // enable timer 0 interrupt; keep consumer polling
					// EA  -  ET2  ES  ET1  EX1  ET0  EX0
		TR0 = 1; 	// set bit TR0 to start running timer 0

		SP = 0x3f;
		thread.bitmap = 0;
		thread.count = 0;
		threadDelay[0]=0;
		threadDelay[1]=0;
		threadDelay[2]=0;
		elapsed_time = 0;
		readyQueue = 0;
		readyQueueTail = 0;
		even_time = 0;
		SemaphoreCreate(&sema_thread,3);
		sleep = 0;
		
		
		thread.currentID=ThreadCreate(main);
		

		
		
		RESTORESTATE;
		__asm	\
		RET		\
		__endasm;\
}

ThreadID ThreadCreate(FunctionPtr fp) {
		
		EA = 0;
		tempPSW = PSW;
		PSW = 0;

		 EA=1;
		 __asm 
		PUSH DPL
		PUSH DPH
		__endasm;	
		 SemaphoreWait(&sema_thread);
		__asm 
		POP DPH
		POP DPL
		__endasm;	
		 EA=0;

		
		for(char temp = 1; temp < 1+MAXTHREADS; temp++){
			if(((thread.bitmap >> temp)&0x01)==0) {
				thread.bitmap |=  (1<<temp);
				newThreadID = temp;
				thread.count++;
				break;
			}
		}
		
		tempSP = SP;
		SP = (((newThreadID+4)<<4)-1);
		
		__asm 
		PUSH DPL\
		PUSH DPH\
		__endasm;
		
		tempACC = ACC;
		ACC = 0;
		__asm 
		PUSH ACC\
		PUSH ACC\
		PUSH ACC\
		PUSH ACC\
		__endasm;
		
		switch(newThreadID){
			case 0:
				ACC = 0b00000000;
				break;
			case 1:
				ACC = 0b00001000;
				break;
			case 2:
				ACC = 0b00010000;
				break;
			case 3:
				ACC = 0b00011000;
		}
		__asm 
		PUSH ACC\
		__endasm;
		ACC = tempACC;
		
		 savedSP[newThreadID] = SP;
		 SP = tempSP;
		 PSW = tempPSW;
		 EA = 1;
		 return newThreadID;
}



void ThreadYield(void) {
	EA = 0;
       SAVESTATE;
	   if(readyQueueTail == 0){
		   
		   do {
				if(thread.currentID == MAXTHREADS) thread.currentID = 0;
				thread.currentID++;
			} while (	!((thread.bitmap>>thread.currentID)&1) ||
						((sleep>>thread.currentID)&1) );
	   }
	   else{
		   
		   thread.currentID = readyQueue & 3;
			//P = s.waitQueue.deque()
			readyQueue >>= 2;
			readyQueueTail -= 2;
	   }
		RESTORESTATE;
		EA = 1;
		__asm RET __endasm;
}


void ThreadExit(void) {
		EA=0;
		thread.count -=1;
		thread.bitmap &= ~(1<<thread.currentID);
		if(thread.count == 0) {while(1);}
		
		
		
		do {
				if(thread.currentID == MAXTHREADS) thread.currentID = 0;
				thread.currentID++;
		} while (!((thread.bitmap>>thread.currentID)&1));
		SemaphoreSignal(&sema_thread);
		RESTORESTATE;
		EA=1;
		__asm RET __endasm;
}
void myTimer0Handler(void){
	EA = 0;
       SAVESTATE;
	   //use every timer interrupt as a time unit
	   //even_time++;
	   	 // __asm 
		// PUSH DPL
		// PUSH DPH
		// __endasm;
	   if(even_time){ 
			even_time = 0;
		   elapsed_time++;
		   if(threadDelay[0]>0)
					threadDelay[0]--;
			if(threadDelay[1]>0)
					threadDelay[1]--;
			if(threadDelay[2]>0)
					threadDelay[2]--;
	   }else even_time=1;
		// __asm 
		// POP DPH
		// POP DPL
		// __endasm;	   
	   
	   if(readyQueueTail == 0){
		   do {
				if(thread.currentID == MAXTHREADS) thread.currentID = 0;
				thread.currentID++;
			} while ( !((thread.bitmap>>thread.currentID)&0x01) ||
					((sleep>>thread.currentID)&1));
	   }
	   else{
			thread.currentID = readyQueue & 3;
			readyQueue >>= 2;
			readyQueueTail -= 2;
	   }

		RESTORESTATE;
		EA = 1;
		__asm RETI __endasm;
}

void delay(unsigned char n){
	SAVELOCAL
	threadDelay[thread.currentID-1] = n;
	RESTORELOCAL
	while(threadDelay[thread.currentID-1] > 0){}
}
//除了有critical section，基本上不會拖時間
unsigned char now(void){
	if(elapsed_time != 0xff) return elapsed_time;
	return 0;
	}


void DeReadyQueue(){


		EA=0;

		SAVESTATE;
		__asm 
		PUSH DPL
		PUSH DPH
		__endasm;
		
		thread.currentID = readyQueue & 3;
		readyQueue >>= 2;
		readyQueueTail -= 2;
		
		__asm 
		POP DPH
		POP DPL
		__endasm;
		RESTORESTATE
		EA = 1;
		__asm RET __endasm;
	
}
void NormalYeild(){
	EA=0;
	// if(((thread.bitmap>>2)&1) && !((sleep>>2)&1) ) thread.currentID=2;
	// else if( ((thread.bitmap>>3)&1) && !((sleep>>3)&1) )  thread.currentID=3;
	// else thread.currentID=1;
	SAVESTATE;
			// __asm 
		// PUSH DPL
		// PUSH DPH
		// __endasm;
	do {
			if(thread.currentID == MAXTHREADS) thread.currentID = 0;
			thread.currentID++;
		} while (	!((thread.bitmap>>thread.currentID)&1) ||
					((sleep>>thread.currentID)&1) );
					
				// __asm 
		// POP DPH
		// POP DPL
		// __endasm;
	RESTORESTATE
	EA = 1;
	//__asm RET __endasm;
}

//Semaphore non busy waiting
void SemaphoreWait(Semaphore *s){	
	 __asm 
		PUSH DPL
		PUSH DPH
		__endasm;
	//while(readyQueueTail != 0)DeReadyQueue();
	
	s->value -= 1;
	if(s->value < 0){
		s->waitQueue = (thread.currentID << ((-s->value-1)*2)) | s->waitQueue;
		// v2  = (-s->value-1)*2;
		// v2 = thread.currentID << v2;
		// v2 = v2 | s->waitQueue;
		// s->waitQueue = v2;
		//sleep
		sleep |= (1<<thread.currentID);
		//if(readyQueue) DeReadyQueue();
		//NormalYeild();
		//else//DeReadyQueue();
		while((sleep >>thread.currentID)&1 );

	}
		    __asm 
		POP DPH
		POP DPL
		__endasm;
}
void SemaphoreSignal(Semaphore *s){
	 __asm 
		PUSH DPL
		PUSH DPH
		__endasm;	
	s->value += 1;
	if(s->value <= 0){
		
		
		//wakeup(P)
		
		unsigned char p = (s->waitQueue) & 3;
		s->waitQueue >>= 2;
		readyQueue |= p << readyQueueTail;
		sleep &= (~(1<< p ));
		readyQueueTail += 2;
		//P = s.waitQueue.deque()
		
	}
	    __asm 
		POP DPH
		POP DPL
		__endasm;	
}