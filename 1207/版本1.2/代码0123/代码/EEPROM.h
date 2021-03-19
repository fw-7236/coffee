/*--------------------------------------------------------------------------
Header file for EEPROM.
Modify on 2017/03/08.
--------------------------------------------------------------------------*/

#ifndef __EEPROM_H__
#define __EEPROM_H__

// Example Code
/*

void main()
{
    Write_EEPROM(0x05,0x33);
    if(Read_EEPROM(0x05)==0x33)
    {
        ROM_2ByteProgram(0x1004,0x3322);
        if(ROM_ByteRead(0x1004)==0x22)
        {
            ROM_1ByteProgram(0x1014,0x22);
        }
    }
    while(1);
}
*/

void ISP_Enable(void);
void ISP_Disable(void);
void ROM_1ByteProgram(unsigned int Addr, unsigned char Data);
void ROM_2ByteProgram(unsigned int Addr, unsigned int Data);
void Write_EEPROM(unsigned char Addr, unsigned char Data);
unsigned char Read_EEPROM(unsigned char Addr);
unsigned char ROM_ByteRead(unsigned int Addr);

#endif
