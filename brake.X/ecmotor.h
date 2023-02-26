#ifndef ECMOTOR_H
#define	ECMOTOR_H

#include <xc.h> // include processor files - each processor file is guarded. 
#include <stdbool.h>
#include <libq.h>

#define INV

 #define CCW    	1		// Counter Clock Wise direction
 #define CW      0		// Clock Wise direction

typedef union       //16 single bits amd uint16_t union
{ 
    unsigned int i;
    struct 
    {
	 unsigned int b0: 1;   
	 unsigned int b1: 1;
     unsigned int b2: 1;
     unsigned int b3: 1;   
	 unsigned int b4: 1;
     unsigned int b5: 1;
     unsigned int b6: 1;   
	 unsigned int b7: 1;
     unsigned int b8: 1;
     unsigned int b9: 1;   
	 unsigned int b10: 1;
     unsigned int b11: 1;
     unsigned int b12: 1;
     unsigned int b13: 1;   
	 unsigned int b14: 1;
     unsigned int b15: 1;
    }; 
    struct
    {
     uint8_t L;  
     uint8_t H;
    };
} bint;

typedef union
{ 
    int32_t il;
    uint32_t hl;
    struct 
    {
	 uint16_t l;
     uint16_t h;	 
    }; 
    struct 
    {
	 int16_t j;
     int16_t i;	 
    }; 
    _Q16 q;
    float f;
} unil;

typedef struct 
{
   uint16_t Required_Direction:1;
   uint16_t Current_Direction:1;
   uint16_t Last_Direction:1;
   uint16_t Orient:1;
   uint16_t ComutPh1:1;// flag of hall sensor detection of communication
   uint16_t ComutPh2:1;
   uint16_t ComutPh3:1;
   uint16_t SlowToBrake:1;
   uint16_t Run:1;
} motorbits;


extern int state;
extern int16_t startcycles;
extern motorbits ec;
extern unil t1,t2;
void  IC1_CallBack(void);
void  IC2_CallBack(void);
void  IC3_CallBack(void);
void  ForceCommutation(void);
#endif	/* ECMOTOR */

