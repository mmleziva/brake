#include "modbus.h"

 uint16_t ADR0COILS;    
 uint16_t ADR0DISCIN; 
 uint16_t ADR0INPREG=30000; 
 uint16_t ADR0HOLDREG=40000; 

UARTFLAGS mbu;//*puf;
short int N;
short int numrec;
SHORT aux;
short int shaddr;
unsigned char *pbi, *pbo;
short  int Regs;

WORD HOLDREGSR[NREGS];
WORD INPREGS[INREGS];


MODBUSTRUCT mbs;  //modbus structures
 

void __attribute__ ((weak)) TMR4_CallBack(void)
{
  #ifdef MBUART1 
    if(irx >3)
    {
     TMR4_Stop ();
     mbu.ErrPar= uarf1.ErrPar; 
     mbu.Rec= 1;
     mbu.Uart1=1;
     pbi= (unsigned char *) rstring;
     numrec= irx;
    }
    else
    {
      //Autos();        
    }
    irx=0;
  #endif

 #ifdef MBUART2
 {
    if(irx >3)
    {
     TMR4_Stop ();
     mbu.ErrPar= uarf2.ErrPar; 
     mbu.Rec= 1;
     mbu.Uart2=1;
     pbi= (unsigned char *) &rstring[0];
     numrec= irx;
    }
    else
    {
    //  Autos();
    } 
    irx=0;
 }
    #endif
     
}
 
 void cfgModbus(void)
{
   // initT4();
        #ifdef MBUART2
    cfgUart2();  
        #endif
  // TMR4_Start(); 
    PR4= ((FCY/BAUDRATE) - 1)*120;
    serini();
}


unsigned char * mbrcopy(unsigned char *p, WORD *q, int n )//read n words from char array pointered by p to word array pointered by q 
{
    int i=n;
    while(i> 0)
    {
     q->H = *p;
     p++;
     q->L = *p;
     p++;
     q++;
     i--;
    }
    return p;
}

unsigned char * mbwcopy(WORD *q, unsigned char *p,  int n )//write n words from word array pointered by q to char array pointered by p 
{
    int i=n;
    while(i> 0)
    {
     *p= q->H;
     p++;
     *p= q->L;
     p++;
     q++;
     i--;
    }
    N += n;
    N += n;
    return p;
}

void mbsr() //modbus slave read
{
  mbs.errcode=0;
  //mbu.ErrPar =0;
  mbs.CRC= CRC16( pbi, numrec );
 // if((mbs.CRC !=0)|| mbu.ErrPar)
  if((mbs.CRC ==0))//t
 //     mbs.errcode= -1;   //ILLEGAL DATA
  //else
  {
   mbs.unitId= *pbi;
   pbi++;
   mbs.funCode= *pbi;
   pbi++; 
   switch(mbs.funCode) 
   {
      case 0x03:                                 
         mbs.startAdr.H= *pbi; 
         pbi++;
         mbs.startAdr.L= *pbi;
         pbi++;
         mbs.regCount.H= *pbi;
         pbi++;
         mbs.regCount.L= *pbi;     
         pbi++;
         mbs.received= true;
      break;
      case 0x04:                                 
         mbs.startAdr.H= *pbi; 
         pbi++;
         mbs.startAdr.L= *pbi;
         pbi++;
         mbs.regCount.H= *pbi;
         pbi++;
         mbs.regCount.L= *pbi;     
         pbi++;
         mbs.received= true;
      break;
      case 0x10:
         mbs.startAdr.H= *pbi; 
         pbi++;
         mbs.startAdr.L= *pbi;
         pbi++;
         mbs.regCount.H= *pbi;
         pbi++;
         mbs.regCount.L= *pbi;  
         pbi++;
         mbs.byteCount= *pbi;
         pbi++;
         shaddr= (mbs.startAdr.W - ADR0HOLDREG);
         if((shaddr < 0)||((shaddr + mbs.regCount.W)> NREGS))
                 mbs.errcode= 2;
         else
         mbrcopy(pbi, &HOLDREGSR[shaddr],mbs.regCount.W);  
         mbs.received= true;
      break;
      default:
          mbs.errcode=1;
   }       
  }  
}

//modbus slave write
void mbsw() //modbus slave write
{
  pbo=pbufo;
  N=0;
  *pbo=mbs.unitId;
  pbo++;N++;
   if(mbs.errcode != 0)
   {
       *pbo=mbs.funCode +0x80;
       pbo++; N++;
       *pbo=mbs.errcode; 
       pbo++; N++;
   }     
   else
   {
    *pbo=mbs.funCode; 
    pbo++; N++;
    switch(mbs.funCode) 
    {
      case 0x03:                     
        mbs.byteCount= mbs.regCount.W << 1;      
        *pbo=mbs.byteCount; 
        pbo++; N++;
        shaddr= (mbs.startAdr.W - ADR0HOLDREG);
         int Regs= mbs.regCount.W;
         pbo= mbwcopy(&HOLDREGSR[shaddr],pbo,Regs);      
      break;
      case 0x04:                     
        mbs.byteCount= mbs.regCount.W << 1;      
        *pbo=mbs.byteCount; 
        pbo++; N++;
        shaddr= (mbs.startAdr.W - ADR0INPREG);
        Regs= mbs.regCount.W;
         pbo= mbwcopy(&INPREGS[shaddr],pbo,Regs);      
      break;
      case 0x10:              
         *pbo=mbs.startAdr.H; 
         pbo++; N++;
         *pbo=mbs.startAdr.L;
         pbo++; N++;
         *pbo=mbs.regCount.H;
         pbo++; N++;
         *pbo=mbs.regCount.L;
         pbo++; N++;        
      break;
      default:
          mbs.errcode=1;
    }
  }
  aux.UI= CRC16(pbufo, N);
  *pbo=aux.UB[0];
  pbo++; N++;
  *pbo= aux.UB[1];
  mbs.readysend = true;
 #ifdef MBUART1
  if(mbu.Uart1)   
  {
    startU1tr(N);//start transmit of this card data to UART1->USB 
    /*
    IFS1bits.U1TXIF= false;
    FURT=DMAC_ChannelTransfer(DMAC_CHANNEL_0, (const void *)pbufo, N*2  , (const void *)&U1TXREG, 1, 1); //Harmony pherif.func.        
    DCH0ECONbits.CFORCE = true;//transfer init manually 
    LED4_Set();
    WDTCONbits.WDTCLRKEY=0x5743;
    */
  }
#endif
  #ifdef MBUART2
  if(mbu.Uart2)
      //startU2tr(N);
#endif
}


void modrec(void)
{
    if(mbu.Rec)
    {
        mbu.Rec=0;
        {
            mbu.Master=0;
            mbsr(); 
            if(mbs.CRC==0)
            {
             interf();
             mbsw();
//             mwend();
            }
        }
    
    }
}

