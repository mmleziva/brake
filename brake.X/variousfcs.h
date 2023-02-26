/* 
 * File:   variousfcs.h
 * Author: marek
 *
 * Created on 7. �nora 2023, 18:26
 */

#ifndef VARIOUSFCS_H
#define	VARIOUSFCS_H
#include <xc.h> // include processor files - each processor file is guarded. 
#include <stdbool.h>
#include "ecmotor.h"
#include "modbus.h"
#define SW _RA2
#define SWEN  _TRISA2       //set output/input 
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
void pwmvolt(int permilleratio);
void Filt(adcfilter *pADCfilter);//adc  filter
extern controlbits controls;





#endif	/* VARIOUSFCS_H */

