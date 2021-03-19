/*--------------------------------------------------------------------------
Header file for WDT.
Modify on 2018/01/09
--------------------------------------------------------------------------*/

#ifndef __WDT_H__
#define __WDT_H__

// Example Code
/*
void main(void)                 //Main Function Start
{
    if ((RSTS&0x08))            //Decision WDT Occur (WDTF=1)
    {
        RSTS = RSTS&0xF7;       //Clear WDTF (WDT Timer Reset Flag)
        WDT_CountClear();       //Clear WDT Count Subroutine
        WDT_Disable();
        while(1);
    }
    WDT_initialize();           //Call WDT Initial Subroutine
    while(1)
    {
        WDT_CountClear();           //Clear WDT Count Subroutine
    }
}
*/

void WDT_initialize(void);
void WDT_CountClear(void);
void WDT_Disable(void);

#endif
