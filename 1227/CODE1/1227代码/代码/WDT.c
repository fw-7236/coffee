#include "OB38R08T1.h"
#include "WDT.h"

#define d_WDTRE  1                    //WDT Enable Bit
#define d_WDTRM  0x07                 //WDTM[3:0] WDT Clock Source Divider Bit (0~F)

void WDTR_Init(void)                  //Initialize WDT
{
    TAKEY = 0x55;                     //**************************************//
    TAKEY = 0xAA;                     //Set To Enable The WDTC Write Attribute//
    TAKEY = 0x5A;                     //**************************************//
    WDTRC = (d_WDTRM) | (d_WDTRE<<5); //Set WDT Reset Time and Enable WDT and select RST/Interrupt
}

void WDTR_CountClear(void)
{
    WDTK = 0x55;                //WDT Timer Clear To Zero
}

void WDTR_Disable(void)
{
    TAKEY = 0x55;               //**************************************//
    TAKEY = 0xAA;               //Set To Enable The WDTC Write Attribute//
    TAKEY = 0x5A;               //**************************************//
    WDTRC = 0x00;               //Disable WDT Function
}

