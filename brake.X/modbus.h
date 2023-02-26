#ifndef MODBUS_H
#define	MODBUS_H
#define MBUART1
//#define MBUART2
#include "sericom.h"
#include "crc.h"
#define NREGS 12
#define INREGS 128

typedef  union 
{
 unsigned char UB[2];
 unsigned short int  UI;
 unsigned char B[2];
 unsigned short int  I;
}SHORT ;

typedef  union 
{
 unsigned short int W;
 struct
 {
     unsigned char L;
     unsigned char H;
 };
}WORD;


typedef  union 
{
 uint32_t u;
 float f;
 unsigned char UB[4];
 unsigned long int UL;
 unsigned short int  UI[2];
 unsigned char B[4];
 long int L;
 short int  I[2];
 float F;
}FLUI;

extern void cfgModbus(void);
extern void modrec(void);
extern WORD HOLDREGSR[NREGS];
extern WORD INPREGS[INREGS];
extern  uint16_t ADR0COILS;    
extern  uint16_t ADR0DISCIN; 
extern  uint16_t ADR0INPREG; 
extern  uint16_t ADR0HOLDREG; 

typedef struct 
{
 unsigned char unitId;
 unsigned char funCode;
 WORD startAdr;
 WORD regCount;
 char byteCount;
 WORD *pdat;    
 unsigned int CRC;
 unsigned char errcode; 
 unsigned char received: 1;
 unsigned char readysend: 1;
 //unsigned char master: 1; 
}MODBUSTRUCT;
extern MODBUSTRUCT mbs;  //modbus structures
void interf(void);
//void mwend (void);

#endif  //MODBUS_H