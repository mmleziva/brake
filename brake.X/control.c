#include "control.h"
//#include "mcc_generated_files/dma.h"

#define T PTPER
#define NSTART 10
// Allocate and reserve a page of flash for this test to use.  The compiler/linker will reserve this for data and not place any code here.

 __prog__  uint16_t flashPage[(FLASH_ERASE_PAGE_SIZE_IN_PC_UNITS/2)] __attribute__((space(prog),aligned(FLASH_ERASE_PAGE_SIZE_IN_PC_UNITS)))\
         ={MAXCYC,FORCERATIO,0,HOLDTIME,RETIME,MAXCLOSE, GREASECYCLES,GREASEREVS,CURBRAKE};//declaration of constants to program flash
uint32_t flash_address;
bool RESULT;
uint16_t *ptforat, *ptsecforce, *ptgrease, *ptsecnofo;
uint16_t start,clamp,released, curax, curcomp,nforces,milisec;
int16_t *ptspace, *ptclose,*ptgredist, *ptcureal;
float voltage, voltmotor,curmotor, delta;
const float resmotor= 0.3;
unil dt,dto;
adcfilter curr,trim,volt;
uint16_t ocp,timgled,shinegled=2,darkgled=600,j;
int16_t current,curmes, startcyco, startmem,starcomp;
uint32_t ms, greasetime;
sets *ptsets;




inline void interf()// after reading modbus instruction
{    
    if(mbs.funCode== 0x10)
    {
                FLASH_Unlock(FLASH_UNLOCK_KEY);
    
    // ------------------------------------------
    // Fill a page of memory with data.  
    // ------------------------------------------
    
    // Erase the page of flash at this address
            RESULT = FLASH_ErasePage(flash_address);
   
    // Program flash with a data pattern.  For the data pattern we will use the index 
    // into the flash as the data.
  
            RESULT = FLASH_WriteRow16(flash_address, (uint16_t *)(&(HOLDREGSR[0].W)));

    // Clear Key for NVM Commands so accidental call to flash routines will not corrupt flash
    FLASH_Lock();
    
  // CVR2CONbits.CVR = *ptcurrmax + 0x40;//t     //current comparator limit
  }
    
}

void writrev(unil trev)  //write revolving time into modbus register
{
      if(ec.Current_Direction== CCW)
          trev.il= -trev.il;
      INPREGS[2].W = trev.l;// write it to Modbus input registers
      INPREGS[3].W = trev.h;
}


void __attribute__ ((weak)) TMR1_CallBack(void)//1ms tick
{
    if(start>0)     //delay after starting the program
        start--;
    if(ocp>0)       //monitoring the maximum time without changing the Hall sensors of the motor position
        ocp--;
            // green LED flashing times 
    timgled++;     //time of green LED shine/dark 
    if(GRE== DARK)
    {
        if(timgled >= darkgled)
        {
          timgled=0;
          GRE= SHINE;
        }
    }
    else
    {
        if(timgled >= shinegled)
        {
          timgled=0;
          GRE= DARK;            
        }
    }
    buttonfilt();
            //switch button filtering
   
   milisec  ++;
   milisec &= 0x3ff;
   if(milisec==0)//1s
   {
     if(state==HOLD)
     {
       clamp++;
     }
     else if(state==RELEASE)
     {
        released++; 
     }
     if(greasetime != 0)greasetime--;
   }
   //curax= trim.filter >> 2 ;//t
   //curax += (curax>>1);//max.1,875 of 2,5V(18,75 of 25A)
   if(ec.ComutPh1)
   {
    ec.ComutPh1= false;
   }
   if(ec.ComutPh2)
   {
    ec.ComutPh2= false;
   }
   if(ec.ComutPh3)
   {
    ec.ComutPh3= false;
    IEC2bits.IC3IE = false;// disable interrupt from Hall sensors  
    if(ec.Orient == false) //motor is 4-pole, on the opposite side of rotor ec.Orient=true
    {          
             dto.hl=dt.hl;
             dt.hl= t2.hl-t1.hl;//time between Hall sensor detections 
             writrev(dt);
             IEC2bits.IC3IE = true; //enable interrupt from Hall sensors            
             //if(state == INTO)
             if(ec.Run)
             {
                 //slow down of motor revolution
                   if(((int32_t)(dt.hl-dto.hl) > SLOWMAX)&&(abs(startcycles-starcomp) >= 24))//t
                   {
                       shinegled=200;
                       darkgled=200;
                       ec.SlowToBrake= true;
                   } 
                 //  else
                    
             }
    }
    IEC2bits.IC3IE = true;  //enable interrupt from Hall sensors 
   }
   if(startcyco== startcycles)//no motor revolution
   {       
     if(ms == 100) 
     {
      ms++;
      dto.hl=dt.hl;
      dt.hl= 0x7fffffff;//if 100ms no Hall sensor edge, motor is used not to move 
      writrev(dt);//write revolving time to Modbus input registers
     }
     else if(ms < 100)
         ms++;
   }
   else
   {
       ms=0;
       INPREGS[1].W = startcycles;//write distance to Modbus input register
   }
   startcyco= startcycles;
   inmbmem();
}

inline void compcurrent(void)

{
  if((current > *ptcureal)&&(ocp == 0))    
    {
      if(ec.Required_Direction == CW)
      {
                        shinegled=100;
                        darkgled=100;
                        ec.SlowToBrake= true;
                        if(state == INTO)
                            state= HOLD;
      }
    } 
}

void __attribute__ ((weak)) ADC1_CallBack(void)
{
   
    // RED= !RED;           //!test
    volt.adc =  ADC1BUF1;
    trim.adc =  ADC1BUF0;
    curr.adc =  ADC1BUF3;
    Filt(&curr);
    Filt(&trim);
    Filt(&volt);
    //increase of current after impact
    
    if((state== READY)||(state== RELEASE))
    {
        pwmvolt(0);
        curcomp=curr.filter;// current compensation
    }
    else if(state == HOLD)//the pulse ratio of the motor is reduced when forced
    {  
        pwmvolt(*ptforat);
       
     }   
    else if((state == INTO)||(state == RETGREA))//the motor speed is almost at maximum
    {      
       //MDC= PTPER-(PTPER >> 5);//maximum velocity MDC=0.94*PTPER 
        pwmvolt(1000);
    } 
    else if((state == AWAY)||(state == GREASE))//the motor speed is almost at maximum
    {      
       //MDC= PTPER-(PTPER >> 5);//maximum velocity MDC=0.94*PTPER 
        pwmvolt(-1000);
    } 
    else pwmvolt(0);
    
    current= curr.filter-curcomp;
    compcurrent();
    INPREGS[0].W = current;// writes parameters to Modbus input registers
    INPREGS[4].W = volt.filter;
}

inline void iniControl(void)// initialization at program start
{
 inivarifcs();
 start=NSTART;
  cfgModbus();
  // Get flash page aligned address of flash 
  flash_address = FLASH_GetErasePageAddress((uint32_t)&flashPage[0]);
  FLASH_Unlock(FLASH_UNLOCK_KEY);
  for(j=0;j<12;j++)
  {
      HOLDREGSR[j].W = (int16_t) FLASH_ReadWord16(flash_address+ j*2U);
  }
  curr.kfil=0x1000;//0.0625
  trim.kfil=0x800;//0.03125
  volt.kfil=0x800;//0.03125
  // sets pointers to  holding registers of Modbus
  ptspace= (int16_t *)&HOLDREGSR[0].W;
  ptforat =(uint16_t *)&HOLDREGSR[1].W;
  ptsets=     (sets *)&HOLDREGSR[2].W;
  ptsecforce=(uint16_t *)&HOLDREGSR[3].W;
  ptsecnofo=(uint16_t *)&HOLDREGSR[4].W;
  ptclose= (int16_t *)&HOLDREGSR[5].W;
  ptgrease= (uint16_t *)&HOLDREGSR[6].W;
  ptgredist=(int16_t *)&HOLDREGSR[7].W;
  ptcureal=(int16_t *)&HOLDREGSR[8].W;
  CVR2CONbits.CVR = CURRMAX + 0x40;//t     //current comparator limit
  //   ptsets->GREASENA= true;//t
}

uint16_t calcduty()
{
    uint16_t t;
    delta= curmotor* resmotor/ voltage;
    t= (uint16_t)((1 - 2*delta)/(1 + 2*delta)* T);
    return t;
}