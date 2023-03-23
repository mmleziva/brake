#include "control.h"
#define OCPSET 150 //ms  
#define OCPBACK 450 //ms   

#define T PTPER
#define NSTART 10

// Allocate and reserve a page of flash for this test to use.  The compiler/linker will reserve this for data and not place any code here.
 __prog__  uint16_t flashPage[(FLASH_ERASE_PAGE_SIZE_IN_PC_UNITS/2)] __attribute__((space(prog),aligned(FLASH_ERASE_PAGE_SIZE_IN_PC_UNITS)))\
         ={MAXCYC,FORCERATIO,0,HOLDTIME,RETIME,MAXCLOSE, GREASECYCLES,GREASEREVS,CURBRAKE,CYCLES};//declaration of constants to program flash
 __prog__  uint16_t flashPage2[(FLASH_ERASE_PAGE_SIZE_IN_PC_UNITS/2)] __attribute__((space(prog),aligned(FLASH_ERASE_PAGE_SIZE_IN_PC_UNITS)));
uint16_t start,clamp,released, curax, curcomp,nforces,milisec;
float voltage, voltmotor,curmotor, delta;
const float resmotor= 0.3;
unil dt,dto;
adcfilter curr,trim,volt;
uint16_t ocp,timgled,shinegled=2,darkgled=600,j;
int16_t current,curmes, startcyco, startmem,starcomp;
uint32_t ms, greasetime;
uint32_t flash_addr, flash_addr2;
bool FINCYC;
 // sets pointers to  holding registers of Modbus
 int16_t *const ptspace= (int16_t *)&HOLDREGSR[0].W;
 uint16_t *const ptforat =(uint16_t *)&HOLDREGSR[1].W;
 sets *const ptsets=     (sets *)&HOLDREGSR[2].W;
 uint16_t *const ptsecforce=(uint16_t *)&HOLDREGSR[3].W;
 uint16_t * const ptsecnofo=(uint16_t *)&HOLDREGSR[4].W;
 int16_t * const ptclose= (int16_t *)&HOLDREGSR[5].W;
 uint16_t *const  ptgrease= (uint16_t *)&HOLDREGSR[6].W;
 int16_t * const ptgredist=(int16_t *)&HOLDREGSR[7].W;
 int16_t * const ptcureal=(int16_t *)&HOLDREGSR[8].W;
 int16_t * const ptcycles=(int16_t *)&HOLDREGSR[9].W;
 uint16_t * const ptcycgrease=(uint16_t *)&HOLDREGSR[10].W;

void setReady(void)
{
 // YELLOW= DARK;
  shinegled=2;  
  darkgled=998;
  state= READY;
  ec.Run= false;
   pwmvolt(0);//maximum velocity MDC=0.94*PTPER 
  //IOCON1= 0xC7A0;
  //IOCON2= 0xC7A0;
  //IOCON3= 0xC7A0;  
}

void setAway(void)
{
    shinegled=990;  
    darkgled=10;
    state= AWAY;
    startcycles=0;
    startmem= startcycles;
    starcomp= startcycles;
  //  ec.Required_Direction=CCW;
     pwmvolt(-1000);//maximum velocity MDC=0.94*PTPER 
    ec.SlowToBrake= false;
    ocp= OCPBACK;
    ec.Run= true;
    ForceCommutation();
}

void setInto(void)
{
                    shinegled=200;  
                    darkgled=800;
                    state= INTO;
                    ec.SlowToBrake= false;
                    pwmvolt(1000);//maximum velocity MDC=0.94*PTPER 
             //       ec.Required_Direction=CW;       
                    IC4CON1bits.ICM=0;//set capture zero
                    IC3CON1bits.ICM=0;
                    TMR3HLD=0;
                    TMR2=0;
                    IC4CON1bits.ICM=1;
                    IC3CON1bits.ICM=1;
                    startmem= startcycles;
                    starcomp= startcycles; 
                    ocp= OCPSET;
                    ec.Run= true;
                    ForceCommutation();
                    ec.Orient= false;
}

void setGrease(void)
{
 greasetime= GRESEC;  
               nforces=0;
               startmem= startcycles;
               starcomp= startcycles; 
               ec.Required_Direction=CCW;
               ocp= OCPBACK;
                state= GREASE;
                 pwmvolt(-1000);//maximum velocity MDC=0.94*PTPER 
               ec.Run= true;
               ForceCommutation();
}

void setRetgrea(void)
{
      ec.SlowToBrake= false;
      state= RETGREA;
      pwmvolt(1000);//maximum velocity MDC=0.94*PTPER 
      starcomp= startcycles; 
      ec.Required_Direction=CW;
      ocp= OCPSET;
      ec.Run= true;
      ForceCommutation();
}

void setHold(void)
{
  ec.SlowToBrake= false;
  pwmvolt(*ptforat);         //increase of current after impact
  state= HOLD; 
  clamp=0;
}

void setRelease(void)
{
    clamp=0;
    shinegled=990;  
    darkgled=10;
    ec.SlowToBrake= false;//t
    state=RELEASE;
    pwmvolt(0);//maximum velocity MDC=0.94*PTPER 
    ec.Run= false;
    ForceCommutation();
}

inline void interf()//program link between reading and writing the modbus protocol
{    
    if(mbs.funCode== 0x10)
    {
         if(*ptcycles != (int16_t) FLASH_ReadWord16(flash_addr2))
           writeflashDW(flash_addr2,*ptcycles,*ptcycgrease);
        else
           writeflash(flash_addr,(uint16_t *)(&(HOLDREGSR[0].W)));
  }  
}

void writrev(unil trev)  //write revolving time into modbus registers
{
      if(ec.Current_Direction== CCW)
          trev.il= -trev.il;
      INP[2] = trev.l;// write it to Modbus input registers
      INP[3] = trev.h;
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
     {  if(ptsets->AUTOTEST && (released > *ptsecforce))
        {
         released=0;
         setAway();
        }
        else
         released++; 
     }
     else if(state==READY)
     {
         if(ptsets->AUTOTEST) 
         {
             if(released > (*ptsecforce*2))
             {
                 released=0;
                 setInto();
             }
             else 
                 released++;             
         }
         else
             released=0;
     }
     if(greasetime != 0)greasetime--;
   }
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
       INP[1] = startcycles;//write distance to Modbus input register
   }
   startcyco= startcycles;
   INPREGS[0].W= *ptcycles;
   inmbmem();
}

inline void compcurrent(void)//compare current
{
  if((current > *ptcureal)&&(ocp == 0))    
    {
 //     if(ec.Required_Direction == CW)//t
      {
                        shinegled=100;
                        darkgled=100;
                        ec.SlowToBrake= true;
                        if(state == INTO)
                            setHold();                           
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
    switch(state)
    {
        case  READY:   
         pwmvolt(0);
         curcomp=curr.filter;// current compensation
         if(FINCYC)//flag of finish of cycle
         {
             FINCYC=false;
             (*ptcycles)++;
              writeflashDW(flash_addr2,*ptcycles,*ptcycgrease);
         }
         
         break;
        case  RELEASE:   
         pwmvolt(0);
         curcomp=curr.filter;// current compensation
         break;
        case  HOLD:   
          break;
        case  INTO:   
          break;
        case  RETGREA:   
          break;
        case  AWAY:   
          break;
        case  GREASE:   
         break;
        default:
         pwmvolt(0);//stop
         break;
    }
    current= curr.filter-curcomp;
    compcurrent();
    INP[0] = current;// writes parameters to  input registers
    INP[4] = volt.filter;
}

inline void iniControl(void)// initialization at program start
{
 inivarifcs();
 start=NSTART;
  cfgModbus();
  flash_addr = FLASH_GetErasePageAddress((uint32_t)flashPage);
  flash_addr2 = FLASH_GetErasePageAddress((uint32_t)flashPage2);
  FLASH_Unlock(FLASH_UNLOCK_KEY);
  for(int jn=0;jn<NREGS;jn++)
  {
      HOLDREGSR[jn].W = (int16_t) FLASH_ReadWord16(flash_addr+ jn*2U);
  }
  *ptcycles= (int16_t) FLASH_ReadWord16(flash_addr2);
  *ptcycgrease= (int16_t) FLASH_ReadWord16(flash_addr2+2U);
  curr.kfil=0x1000;//0.0625
  trim.kfil=0x800;//0.03125
  volt.kfil=0x800;//0.03125
  CVR2CONbits.CVR = CURRMAX + 0x40;//t     //current comparator limit
 //  ptsets->GREASENA= true;//t
  greasetime= GRESEC;
}

inline void Control(void)
{
 if(state== VOLTON)
      {
        if(start==0)//delay of ready state
            setReady();
      }
      else if(controls.SWREAL)  //rise edge of switch
      {
        controls.SWREAL= false;
        {
           if(!ptsets->AUTOTEST)
            switch(state)
            {                
                case READY:
                    setInto();
                 break;               
                 case AWAY:
                    setInto();
                 break;
                 case GREASE:
                    setInto();                    
                  break;
                  case RETGREA:
                    setInto();                    
                  break;
                  default:
                    break;
            }
            
        }
      }
      else if(controls.SWUNRE)  //falling edge of switch
      {
        controls.SWUNRE= false;
        if(!ptsets->AUTOTEST)
          switch(state)
            {              
               case INTO:
                 setAway();
                 break;
               case RELEASE:
                 nforces++;
                 setAway();
                 FINCYC=true;
                 break;
               case HOLD:
                 nforces++;
                 setAway();  
                 FINCYC=true;
                 break;
               default:
                  break;
             } 
      }         
      switch(state)
      {   
          case INTO:          
            if(ec.SlowToBrake)
            {
                setHold();
              
            }
            else if((startcycles-startmem) > *ptclose)
            {
                setReady();  
            }
          break;
          case HOLD:
            if(clamp> *ptsecforce)   //s after HOLD time switch to RELEASE
            {
                setRelease();               
            }
          break;
          case RELEASE:
            if(released > *ptsecnofo)
            {
              released=0;
              setHold();
              /*
              state= HOLD; 
              clamp=0;
              shinegled=100;
              darkgled=100;
              ec.Run= true;
               ForceCommutation();
               */ 
            }
            break;
          case AWAY:
            if(ec.SlowToBrake)
            {
              ec.SlowToBrake= false;
              setReady();  
            }
            else  if((startcycles-startmem) < -*ptspace)
            {
                if(ptsets->AUTOTEST)
                    FINCYC=true;
                setReady();  
            }
          break;
          case READY:
            if(ec.SlowToBrake)
            {
              ec.SlowToBrake= false;
            }
            //else  //t 
            if((ptsets->GREASENA) && ((greasetime==0)||(nforces >= *ptgrease)) && (dt.hl > 0xfffff))
            {
                setGrease();
              
            }
          break;
          case GREASE:                          
            if(((startcycles-startmem) < -*ptgredist)||ec.SlowToBrake)
            {
                   if(ec.Run)
                   {
                       ec.Run= false;
                       if(ec.SlowToBrake)
                           ForceCommutation();                           
                   }
                   else if(dt.hl > 0xfffff)
                   {
                       setRetgrea();
                   }
            }
          break;
          case RETGREA:    
               if(ec.SlowToBrake)
               {
                   ec.SlowToBrake= false;
                   setAway();
               }
               else if((startcycles-startmem) >= 0)
               {
                setReady();  
               }
          break;     
          default:
              ec.SlowToBrake= false;
          break;
      } 
}

uint16_t calcduty()
{
    uint16_t t;
    delta= curmotor* resmotor/ voltage;
    t= (uint16_t)((1 - 2*delta)/(1 + 2*delta)* T);
    return t;
}