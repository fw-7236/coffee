#include "OB38R08T1.h"
#include <absacc.h>    /* Include Macro Definitions */
#include "EEPROM.h"

/*ISPFC.ISPF[2:0]*/
#define d_1Byte_Program_ROM     0
#define d_2Byte_Program_ROM     1
#define d_Write_EEPROM          3
#define d_Read_EEPROM           4

//====================================================================


void ISP_Enable(void)
{
    EA     = 0;
    TAKEY  = 0x55;
    TAKEY  = 0xAA;
    TAKEY  = 0x5A;
    IFCON |= 0x01;      // ISPE=1, Enable ISP function
    EA     = 1;
}

void ISP_Disable(void)
{
    EA     = 0;
    TAKEY  = 0x55;
    TAKEY  = 0xAA;
    TAKEY  = 0x5A;
    IFCON &= 0xFE;      // ISPE=0, Disable  ISP function
    EA     = 1;
}

//void ROM_1ByteProgram(unsigned int Addr, unsigned char Data)
//{
//    ISP_Enable();
//    ISPFAH = Addr>>8;
//    ISPFAL = Addr;
//    ISPFDL = Data;
//    ISPFC  = d_1Byte_Program_ROM;
//    ISP_Disable();
//}

//void ROM_2ByteProgram(unsigned int Addr, unsigned int Data)
//{
//    ISP_Enable();
//    ISPFAH = Addr>>8;
//    ISPFAL = Addr;
//    ISPFDL = Data;
//    ISPFDH = Data>>8;
//    ISPFC  = d_2Byte_Program_ROM;
//    ISP_Disable();
//}

void Write_EEPROM(unsigned char Addr, unsigned char Data)
{
    ISP_Enable();
    ISPFAH = 0x00;
    ISPFAL = Addr;
    ISPFDL = Data;
    ISPFC  = d_Write_EEPROM;
    ISP_Disable();
}

unsigned char Read_EEPROM(unsigned char Addr)
{
    unsigned char Data;
    ISP_Enable();
    ISPFAH = 0x00;
    ISPFAL = Addr;
    ISPFC  = d_Read_EEPROM;
    Data   = ISPFDL;
    ISP_Disable();
    return Data;
}


//unsigned char ROM_ByteRead(unsigned int Addr)
//{
//    return CBYTE[Addr];         //The range of valid index values for this macro is 0-255
//}

