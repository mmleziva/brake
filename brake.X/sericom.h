

#ifndef _SERICOM_H    /* Guard against multiple inclusion */
#define _SERICOM_H
//#define Fosc 60802500
#define Fosc  121605000
#define FCY  (Fosc/2)
#define BAUDRATE 115200
#define BRGVAL (((FCY/BAUDRATE)/4) - 1)//used by modbus

//#define SMAX 138
//#define SAMPSTER 100
//#define SAMPS 128
//#define BUFMAX 64
#define BUFOUTMAX 138
#define BUFINMAX 64
//#include <stdio.h>
#include <xc.h> // include processor files - each processor file is guarded. 
#include <stdbool.h>
#include "mcc_generated_files/mcc.h"
//#include "mcc_generated_files/system.h"
//#include "mcc_generated_files/tmr4.h"
//#include "mcc_generated_files/dma.h"

typedef struct 
{
 unsigned Pre  :1; 
 unsigned Rec  :1; 
 unsigned ErrPar: 1; 
 unsigned Master: 1; 
 unsigned Uart1: 1; 
 unsigned Uart2: 1; 
 unsigned Fred:  1;
 unsigned WRITBUSY: 1;
}UARTFLAGS;


extern bool FURT;
void startU1tr(int n);
extern UARTFLAGS uarf1;
extern char rxData,rstring[BUFINMAX+1];
extern short int irx,nstr;
extern unsigned char  *pbufo;
void serini();

//#include "config/basic/definitions.h"
#endif /* _SERICOM_H */

