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

/*
                         Main application
 */



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
    
      Control();
      modrec();  //modbus receiving test   
    }
    return 1; 
}
/**
 End of File
*/

