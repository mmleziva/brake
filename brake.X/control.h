
#ifndef CONTROL_H
#define	CONTROL_H
#define SLOWMAX  40000   //250ns*40000=10ms
#define GREEN _RA2
#define GRE _LATA2
#define YELLOW _RA3
#define RED  _RB4
#define CURED  _RB1//t
#define DARK  1 //LED off
#define SHINE  0 //LED on
    //stages of the program
#define VOLTON 0
#define READY 1
#define INTO 2
#define HOLD 3
#define RELEASE 4
#define AWAY 5
#define GREASE 6
#define RETGREA 7
#define MAXCYC 600    
#define FORCERATIO 190   //holding PWM ratio  12kN
#define HOLDTIME 3      //braking time
#define RETIME 10000      //braking pause
#define MAXCLOSE 1000
#define GRESEC 30000 //s 
#define GREASEREVS 900
#define CURRMAX 0x38    //22A peak current/ comparator limit Im=(CURRMAx)/2.56
#define GREASECYCLES 20000
#define CYCLES 0
#define CURBRAKE 240    //current for ratio drop
//#define CURBRAKE 40    //current for ratio drop

#include <xc.h> // include processor files - each processor file is guarded.
#include "modbus.h"
#include <math.h>
#include "variousfcs.h"

    
 typedef struct 
{
 uint16_t   GREASENA:1; //grease enable
 uint16_t   AUTOTEST:1; //brake autotest enable
} sets;


void iniControl(void);
void Control(void);
void compcurrent(void);
#endif	/* CONTROL_H */

