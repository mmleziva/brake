/* 
 * File:   variousfcs.h
 * Author: marek
 *
 * Created on 7. února 2023, 18:26
 */

#ifndef VARIOUSFCS_H
#define	VARIOUSFCS_H
#include <xc.h> // include processor files - each processor file is guarded. 
#include <stdbool.h>
#include "ecmotor.h"
#include "modbus.h"
#define SW _RA2
#define SWEN  _TRISA2       //set output/input 
#define MEMPAUSE 50
#define PARMBMEMS 6
#define MAXMBWORDS 128
//#define MAXPARSETS (MAXMBWORD/PARMBMEMS)

typedef struct 
{
    bool BEGIN;
    uint16_t kfil;
    int16_t  adc;
    int16_t filter;	
}adcfilter;

typedef struct 
    {
     unsigned int SWITCH: 1;   
     unsigned int SWLAST: 1;  
     unsigned int SWREAL: 1;  	 
     unsigned int SWUNRE: 1;  	 
    }controlbits;
void inmbmem(void);    
void inivarifcs(void);    
void buttonfilt(void);  
extern uint16_t INP[PARMBMEMS];
bool writeflash(uint32_t flash_addr,uint16_t *data);
bool writeflashDW(uint32_t flash_addr,uint16_t Data0, uint16_t Data1);
void Filt(adcfilter *pADCfilter);//adc  filter
extern controlbits controls;





#endif	/* VARIOUSFCS_H */

