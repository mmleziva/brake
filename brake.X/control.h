
#ifndef CONTROL_H
#define	CONTROL_H
#define FLASH_WRITE_ROW_SIZE_IN_INSTRUCTIONS  64U
#define FLASH_ERASE_PAGE_SIZE_IN_INSTRUCTIONS 512U

#define FLASH_ERASE_PAGE_SIZE_IN_PC_UNITS  (FLASH_ERASE_PAGE_SIZE_IN_INSTRUCTIONS*2U)
#define FLASH_WRITE_ROW_SIZE_IN_PC_UNITS (FLASH_WRITE_ROW_SIZE_IN_INSTRUCTIONS*2U)
#define FLASH_HAS_ECC  1

#define FLASH_UNLOCK_KEY 0x00AA0055

#define FLASH_ERASE_PAGE_MASK (~((FLASH_ERASE_PAGE_SIZE_IN_INSTRUCTIONS*2UL) - 1U)) 
//#define SLOWMAX  10000   //250ns*10000=2.5ms
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
#define HOLDTIME 2      //braking time
#define RETIME 10000      //braking pause
#define MAXCLOSE 1000
#define GRESEC 10000 //s 
#define GREASEREVS 1800
#define CURRMAX 0x38    //22A peak current/ comparator limit Im=(CURRMAx)/2.56
#define GREASECYCLES 100

//#define CURBRAKE 320    //current for ratio drop
#define CURBRAKE 240    //current for ratio drop

#include <xc.h> // include processor files - each processor file is guarded.
//#include "modbus.h"
#include <math.h>
#include "variousfcs.h"

extern  __prog__  uint16_t flashPage[] __attribute__((space(prog),aligned(FLASH_ERASE_PAGE_SIZE_IN_PC_UNITS)));
    
 typedef struct 
{
 uint16_t   GREASENA;
} sets;


//extern controlbits controls;
extern uint16_t start;
void iniControl(void);
void compcurrent(void);
extern uint16_t ocp,shinegled,darkgled,clamp,released,nforces;
extern int16_t current,startmem, *ptspace, *ptclose,*ptgredist,starcomp;
extern uint16_t *ptsecforce,*ptsecnofo,*ptgrease;
extern uint32_t ms, greasetime;
extern unil dt;
extern sets *ptsets;
#endif	/* CONTROL_H */

