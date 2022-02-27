/* 
 * file: testcoop.c 
 */
#include <8051.h>
#include "preemptive.h"
#define BUFSIZE 3

__data __at (0x35) unsigned char Car;
__data __at (0x36) unsigned char spots[2];
__data __at (0x38) Semaphore spotsSync;

__data __at (0x3A) unsigned char car_delay1;
__data __at (0x3B) unsigned char car_delay2;
__data __at (0x3C) unsigned char car_delay3;
__data __at (0x3D) unsigned char car_delay4;
__data __at (0x3E) unsigned char car_delay5;

void MakeParkingLot(){
	spots[0] = 0;
	spots[1] = 0;
	SemaphoreCreate(&spotsSync, 1);
}
#define Park_Body(s)\
void Park ##s(){\
		 SemaphoreWait(&spotsSync);\
	EA=0;\
	SBUF='C';while(!TI);TI=0;\
	SBUF='a';while(!TI);TI=0;\
	SBUF='r';while(!TI);TI=0;\
	SBUF=' ';while(!TI);TI=0;\
	SBUF='0'+s;while(!TI);TI=0;\
	SBUF=' ';while(!TI);TI=0;\
	SBUF='g';while(!TI);TI=0;\
	SBUF='o';while(!TI);TI=0;\
	SBUF='t';while(!TI);TI=0;\
	SBUF=' ';while(!TI);TI=0;\
	SBUF='s';while(!TI);TI=0;\
	SBUF='p';while(!TI);TI=0;\
	SBUF='o';while(!TI);TI=0;\
	SBUF='t';while(!TI);TI=0;\
	SBUF=' ';while(!TI);TI=0;\
	if(spots[0] == 0) {spots[0] = s;SBUF='0';}\
	else {spots[1] = s;SBUF='1';}\
	while(!TI);TI=0;\
	SBUF='\t';while(!TI);TI=0;\
	SBUF='\t';while(!TI);TI=0;\
	SBUF=':';while(!TI);TI=0;\
	SBUF='[';while(!TI);TI=0;\
	if(spots[0]==0){\
	SBUF='n';while(!TI);TI=0;}\
	else{\
	SBUF='0'+spots[0];while(!TI);TI=0;}\
	SBUF=',';while(!TI);TI=0;\
	if(spots[1]==0){\
	SBUF='n';while(!TI);TI=0;}\
	else{\
	SBUF='0'+spots[1];while(!TI);TI=0;}\
	SBUF=']';while(!TI);TI=0;\
	SBUF='\n';while(!TI);TI=0;\
	EA=1;\
		SemaphoreSignal(&spotsSync);\
	delay(car_delay ##s);\
		 SemaphoreWait(&spotsSync);\
	EA=0;\
	SBUF = 'C';while(!TI);TI=0;\
	SBUF = 'a';while(!TI);TI=0;\
	SBUF = 'r';while(!TI);TI=0;\
	SBUF = ' ';while(!TI);TI=0;\
	SBUF='0'+s;while(!TI);TI=0;\
	SBUF=' ';while(!TI);TI=0;\
	SBUF='a';while(!TI);TI=0;\
	SBUF='t';while(!TI);TI=0;\
	SBUF=' ';while(!TI);TI=0;\
	if(spots[0] == s) {spots[0] = 0;SBUF='0';}\
	else {spots[1] = 0;SBUF='1';}\
	while(!TI);TI=0;\
	SBUF=' ';while(!TI);TI=0;\
	SBUF='l';while(!TI);TI=0;\
	SBUF='e';while(!TI);TI=0;\
	SBUF='f';while(!TI);TI=0;\
	SBUF='t';while(!TI);TI=0;\
	SBUF=' ';while(!TI);TI=0;\
	SBUF='a';while(!TI);TI=0;\
	SBUF='f';while(!TI);TI=0;\
	SBUF='t';while(!TI);TI=0;\
	SBUF='e';while(!TI);TI=0;\
	SBUF='r';while(!TI);TI=0;\
	SBUF=' ';while(!TI);TI=0;\
	if(car_delay ##s/10!=0){\
	SBUF='0'+car_delay ##s/10;while(!TI);TI=0;}\
	SBUF='0'+car_delay ##s-(car_delay ##s/10)*10;while(!TI);TI=0;\
	SBUF='c';while(!TI);TI=0;\
	SBUF='\t';while(!TI);TI=0;\
	SBUF=':';while(!TI);TI=0;\
	SBUF='[';while(!TI);TI=0;\
	if(spots[0]==0){\
	SBUF='n';while(!TI);TI=0;}\
	else{\
	SBUF='0'+spots[0];while(!TI);TI=0;}\
	SBUF=',';while(!TI);TI=0;\
	if(spots[1]==0){\
	SBUF='n';while(!TI);TI=0;}\
	else{\
	SBUF='0'+spots[1];while(!TI);TI=0;}\
	SBUF=']';while(!TI);TI=0;\
	SBUF='\n';while(!TI);TI=0;\
	EA=1;\
		SemaphoreSignal(&spotsSync);\
	ThreadExit();\
}
#define Park(s) Park_Body(s)
#define zeroFunctorBody(s) void zeroFunctor(){return;}
#define zeroFunctor(s) zeroFunctorBody(s)
zeroFunctor(__COUNTER__)
Park(__COUNTER__)
Park(__COUNTER__)
Park(__COUNTER__)
Park(__COUNTER__)
Park(__COUNTER__)
void MakeCars(){
	// SemaphoreWait(&sem_car);
	// ThreadCreate(Park1);
	// SemaphoreSignal(&sem_car);
	
	// SemaphoreWait(&sem_car);
	// ThreadCreate(Park2);
	// SemaphoreSignal(&sem_car);
	
	// SemaphoreWait(&sem_car);
	// ThreadCreate(Park3);
	// SemaphoreSignal(&sem_car);
	
	// SemaphoreWait(&sem_car);
	// ThreadCreate(Park4);
	// SemaphoreSignal(&sem_car);	
	
	// SemaphoreWait(&sem_car);
	// ThreadCreate(Park5);
	// SemaphoreSignal(&sem_car);
	
	
	
	
	ThreadCreate(Park1);
	ThreadCreate(Park2);
	ThreadCreate(Park3);
	ThreadCreate(Park4);
	ThreadCreate(Park5);
	
	ThreadExit();
}
void main(void) {
		//init uart
		TMOD |= 0x20;
		TH1= -6;
		SCON=0x50;
		TR1=1;
		EA=1;
		ES=1;
		
		//init problem
		SemaphoreCreate(&sem_car,2);
	   car_delay1 = 40;
	   car_delay2 = 20;
	   car_delay3 = 14;
	   car_delay4 = 10;
	   car_delay5 = 30;
	   MakeParkingLot();
	   MakeCars();
}

void _sdcc_gsinit_startup(void) {
        __asm
                ljmp  _Bootstrap
        __endasm;
}

void _mcs51_genRAMCLEAR(void) {}
void _mcs51_genXINIT(void) {}
void _mcs51_genXRAMCLEAR(void) {}
void timer0_ISR(void) __interrupt(1) {
        __asm
                ljmp _myTimer0Handler
        __endasm;
}


