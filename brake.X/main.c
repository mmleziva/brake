/**
  Generated main.c file from MPLAB Code Configurator

  @Company
    Microchip Technology Inc.

  @File Name
    main.c

  @Summary
    This is the generated main.c using PIC24 / dsPIC33 / PIC32MM MCUs.

  @Description
    This source file provides main entry point for system initialization and application code development.
    Generation Information :
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.171.1
        Device            :  dsPIC33EV32GM002
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.70
        MPLAB 	          :  MPLAB X v5.50
*/


/**
  Section: Included Files
*/
//#include "mcc_generated_files/system.h"

#include "control.h"
#define OCPSET 150 //ms   

/*
                         Main application
 */


void setReady(void)
{
 // YELLOW= DARK;
  shinegled=2;  
  darkgled=998;
  state= READY;
  ec.Required_Direction=CW;
  ec.Run= false;
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
    ec.Required_Direction=CCW;
    ocp= OCPSET;
    ec.Run= true;
    ForceCommutation();
   // RED=SHINE;
}

void setInto(void)
{
                    shinegled=200;  
                    darkgled=800;
                    state= INTO;
                   // ocp= OCPSET;
                    ec.SlowToBrake= false;
                    ec.Required_Direction=CW;       
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

int main(void)
{
    // initialize the device
    SYSTEM_Initialize();
   // BOOT_DEMO_Initialize();
    iniControl();
    while (1)
    {
        // Add your application code
      ClrWdt();
     // BOOT_DEMO_Tasks();
      if(state== VOLTON)
      {
        if(start==0)//delay of ready state
            setReady();
      }
      else if(controls.SWREAL)  //edge of switch
      {
        controls.SWREAL= false;
        {
            switch(state)
            {                
                case READY:
                    setInto();
                 break;
                 
                 case AWAY:
                  //  setReady();
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
      else if(controls.SWUNRE)  //edge of switch
      {
          controls.SWUNRE= false;
          switch(state)
            {              
               case INTO:
                 setAway();
                 break;
               case RELEASE:
                 nforces++;
                 setAway();
                 break;
               case HOLD:
                 nforces++;
                 setAway();          
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
              ec.SlowToBrake= false;
              state= HOLD; 
               clamp=0;
            }
            else if((startcycles-startmem) > *ptclose)
            {
                setReady();  
            }
          break;
          case HOLD:
            if(clamp> *ptsecforce)   //s after HOLD time switch to RELEASE
            {
                clamp=0;
                shinegled=990;  
                darkgled=10;
                ec.SlowToBrake= false;//t
                state=RELEASE;
                ec.Run= false;
                ForceCommutation();
            }
          break;
          case RELEASE:
            if(released > *ptsecnofo)
            {
              released=0;
              state= HOLD; 
              clamp=0;
              shinegled=100;
              darkgled=100;
              ec.Run= true;
               ForceCommutation();
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
               greasetime= GRESEC;  
               nforces=0;
               startmem= startcycles;
               starcomp= startcycles; 
               ec.Required_Direction=CCW;
               ocp= OCPSET;
                state= GREASE;
               ec.Run= true;
               ForceCommutation();
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
                    ec.SlowToBrake= false;
                    state= RETGREA;
                    starcomp= startcycles; 
                    ec.Required_Direction=CW;
                    ocp= OCPSET;
                    ec.Run= true;
                    ForceCommutation();
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
      modrec();     
    }
    return 1; 
}
/**
 End of File
*/

