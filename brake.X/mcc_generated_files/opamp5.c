
/**
  OPAMP5 Generated Driver File 

  @Company
    Microchip Technology Inc.

  @File Name
    opamp5.c

  @Summary
    This is the generated driver implementation file for the OPAMP5 driver using PIC24 / dsPIC33 / PIC32MM MCUs

  @Description
    This header file provides implementations for driver APIs for OPAMP5. 
    Generation Information : 
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.171.1
        Device            :  dsPIC33EV32GM002
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.70
        MPLAB             :  MPLAB X v5.50
*/

/*
    (c) 2020 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

/**
  Section: Included Files
*/

#include "opamp5.h"

/**
  Section: Driver Interface
*/

void OPAMP5_Initialize(void)
{   
    
    // PSIDL disabled; 
    CMSTAT = 0x00;
    // CON enabled; CPOL Not Inverted; OPAEN OP-Amp; EVPOL Disabled; COE disabled; CCH C5IN1-; CREF C5IN1+; CEVT disabled; 
    CM5CON = 0x8400 & ~(0x8000);  //disabling Comparator CON bit
    // SELSRCC PWM1L; SELSRCB PWM1L; SELSRCA PWM1L; 
    CM5MSKSRC = 0x00;
    // AANEN disabled; ABNEN disabled; ACNEN disabled; HLMS disabled; OANEN disabled; ABEN disabled; ACEN disabled; AAEN disabled; PAGS disabled; OBEN disabled; OCEN disabled; NAGS disabled; OCNEN disabled; OBNEN disabled; OAEN disabled; 
    CM5MSKCON = 0x00;
    // CFSEL FOSC/2; CFLTREN disabled; CFDIV 1:1; 
    CM5FLTR = 0x00;
    
    CM5CONbits.CON = 1; //enabling Comparator CON bit
}
bool OPAMP5_OutputStatusGet(void)
{
    return (CM5CONbits.COUT);
}
bool OPAMP5_EventStatusGet(void)
{   
    return (CM5CONbits.CEVT);      
}
void OPAMP5_EventStatusReset(void)
{
    CM5CONbits.CEVT = 0;
}
bool OPAMP5_HLMSStatusGet(void)
{
    return (CM5MSKCONbits.HLMS);
}

void OPAMP5_ComparatorDisable(void)
{
    CM5CONbits.CON = 0;
}

void OPAMP5_ComparatorEnable(void)
{
    CM5CONbits.CON = 1;
}

void __attribute__ ((weak)) OPAMP5_CallBack(void)
{
    // Add your custom callback code here
}

void OPAMP5_Tasks ( void )
{
	if(IFS1bits.CMIF)
	{
		// OPAMP5 callback function 
		OPAMP5_CallBack();
		
		// Clear the CEVT bit to enable further interrupts
		OPAMP5_EventStatusReset();
		
		// clear the OPAMP5 interrupt flag
		IFS1bits.CMIF = 0;
	}
}

/**
  End of File
*/
