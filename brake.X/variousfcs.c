#include "variousfcs.h"

controlbits controls;
uint8_t swec;
bint swfil;
bool RES;

uint16_t INP[PARMBMEMS];


bool writeflashDW(uint32_t flash_addr,uint16_t Data0, uint16_t Data1)
{
               FLASH_Unlock(FLASH_UNLOCK_KEY);
    
    // ------------------------------------------
    // Fill a page of memory with data.  
    // ------------------------------------------
    
    // Erase the page of flash at this address
            RES = FLASH_ErasePage(flash_addr);
   
    // Program flash with a data pattern.  For the data pattern we will use the index 
    // into the flash as the data.
  
    RES &=  FLASH_WriteDoubleWord16(flash_addr,Data0,Data1);

    // Clear Key for NVM Commands so accidental call to flash routines will not corrupt flash
    FLASH_Lock();
    return RES;
}


bool writeflash(uint32_t flash_addr,uint16_t *ptdata)
{
               FLASH_Unlock(FLASH_UNLOCK_KEY);
    
    // ------------------------------------------
    // Fill a page of memory with data.  
    // ------------------------------------------
    
    // Erase the page of flash at this address
            RES = FLASH_ErasePage(flash_addr);
   
    // Program flash with a data pattern.  For the data pattern we will use the index 
    // into the flash as the data.
  
            RES &= FLASH_WriteRow16(flash_addr, ptdata);

    // Clear Key for NVM Commands so accidental call to flash routines will not corrupt flash
    FLASH_Lock();
    return RES;
}


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
                    INPREGS[mem].W = INP[m];
                    mem++;
        } 
        else
            break;//full input modbus registers
     }
   }
   timem++;
   timem %=MEMPAUSE;
}