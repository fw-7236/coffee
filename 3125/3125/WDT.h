/*--------------------------------------------------------------------------
Header file for WDTR.
Modify on 2018/01/09
--------------------------------------------------------------------------*/

#ifndef __WDTR_H__
#define __WDTR_H__

// Example Code
/*
void main(void)                 //Main Function Start
{
    if ((RSTS&0x08))            //Decision WDTR Occur (WDTRF=1)
    {
        RSTS = RSTS&0xF7;       //Clear WDTRF (WDT Timer Reset Flag)
        WDTR_CountClear();      //Clear WDTR Count Subroutine
        WDTR_Disable();
        while(1);
    }
    WDTR_Init();                //Call WDTR Initial Subroutine
    while(1)
    {
        WDTR_CountClear();      //Clear WDTR Count Subroutine
    }
}
*/

void WDTR_Init(void);
void WDTR_CountClear(void);
void WDTR_Disable(void);

#endif
