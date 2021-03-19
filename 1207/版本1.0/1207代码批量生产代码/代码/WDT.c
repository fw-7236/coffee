#include "OB38R08A1.h"
#include "WDT.h"

#define WDT_VECTOR  17          //WDT Interrupt Vevtor
#define d_WDTE      1           //WDT Enable Bit
#define d_WDTM      0x07        //WDTM[3:0] WDT Clock Source Divider Bit (0~F)
#define d_CWDTR     0        //if set 0: watchdog reset, set 1: watchdog interrupt
#define d_IEWDT     0        //WDT interrupt enable if set 0: Disable WDT interrupt, set 1: Enable WDT interrupt


void WDT_initialize(void)    //Initialize WDT
{
    EA    = 0;                      //for WDT Interrupt 
    TAKEY = 0x55;                   //**************************************//
    TAKEY = 0xAA;                   //Set To Enable The WDTC Write Attribute//
    TAKEY = 0x5A;                   //**************************************//
    WDTC  = (d_WDTM)|(d_WDTE<<5)|(d_CWDTR<<6);    //Set WDT Reset Time and Enable WDT and select RST/Interrupt
    IEN2  = (d_IEWDT<<1);           //for WDT Interrupt 
    EA    = 1;                      //for WDT Interrupt 
}

void WDT_CountClear(void)
{
    WDTK = 0x55;                //WDT Timer Clear To Zero
}

void WDT_Disable(void)
{
    TAKEY = 0x55;               //**************************************//
    TAKEY = 0xAA;               //Set To Enable The WDTC Write Attribute//
    TAKEY = 0x5A;               //**************************************//
    WDTC  = 0x00;               //Disable WDT Function
}

void WDT_ISR(void) interrupt WDT_VECTOR //WDT Interrupt Subroutine
{
    P0 = 0;                     //When Into interrupt subroutine WDTIF(IRCON2&0x02) will automatic clear 
}
