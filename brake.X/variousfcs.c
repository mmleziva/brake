#include "variousfcs.h"

controlbits controls;
uint8_t swec;
bint swfil;

inline void inivarifcs(void) //init switch or button 
{
 swfil.i=0xffff;   
 controls.SWITCH= true;
 controls.SWLAST= true;
}

//button or switch filter samples dig. input for first 2 of 8ms (otherwise leaves pin as output),
//then it gradually shifts the filter register 8 times, i.e. a total of 64ms
inline void buttonfilt(void)
{
    swec ++;
   swec &= 0x07;
   switch(swec)
   {
       case 0:
           SWEN= true;  //turn output to input
           break;
       case 1:              
           swfil.L<<=1; //shift of filtering byte
           swfil.b0= SW;   //setting of its first bit according to button state
           if(swfil.L == 0xff)
                controls.SWITCH= true;  //filtering state of button
           else if(swfil.L == 0x00)
                controls.SWITCH= false;
           if(!controls.SWITCH && controls.SWLAST)
                controls.SWREAL= true;      //set rising edge of button
           else if(controls.SWITCH && !controls.SWLAST)
               controls.SWUNRE= true;      //set falling edge of button
                controls.SWLAST= controls.SWITCH;
           break;
       case 2:
           SWEN= false;  //change input to output
           break;
       default:
           break;     
   }
}

//set ratio of pwm output; max. ratio is 1000 of 1024  
 void pwmvolt(int permilleratio)
{
     uint32_t lotr;
     if(permilleratio < 0)
     {
      permilleratio = -permilleratio;
      ec.Required_Direction= CCW;
     }
     else
      ec.Required_Direction= CW;
     if(permilleratio > 1000)permilleratio = 1000;
     lotr= ((uint32_t)(permilleratio + 0x400)) * PTPER;//t
     MDC= lotr >> 11; 
}

void Filt(adcfilter *pADCfilter)//adc  filter
{
  unil  filu;
  if(!pADCfilter->BEGIN) // start filtering 
  {
      pADCfilter->BEGIN=true;
      pADCfilter->filter = pADCfilter->adc;
  }
  else      //continuous filtering
  {
       filu.i= pADCfilter->filter;
       filu.j= 0;
       filu.il += (pADCfilter->kfil) * (uint32_t)(pADCfilter->adc - pADCfilter->filter);
      pADCfilter->filter= filu.i;
  }
}

#define MEMPAUSE 50
#define PARMBMEMS 6
#define MAXMBWORDS 128
//#define MAXPARSETS (MAXMBWORD/PARMBMEMS)
uint16_t mem,timem;

//write input values into modbus memory 
inline void inmbmem(void)
{
   if(mbs.readysend)
   {
     mbs.readysend= false;
     if(mbs.funCode==0x4)
     {
      timem=0;
      mem= PARMBMEMS;
     }
   }
   if(timem==0)//every MEMPAUSE ms
   {    
     for(int m=0;m<PARMBMEMS;m++)
     {
        if(mem < MAXMBWORDS)
        {
                    INPREGS[mem].W = INPREGS[m].W;
                    mem++;
        } 
        else
            break;//full input modbus registers
     }
   }
   timem++;
   timem %=MEMPAUSE;
}