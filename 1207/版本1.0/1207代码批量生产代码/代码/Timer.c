#include "OB38R08A1.h"
#include "Timer.h"

#define TIMER0_VECTOR   1       //TIMER0 Interrupt Vevtor
#define d_T0MOD         0x00    //TMOD[3:0] TIMER0 Mode setting 
#define d_T1MOD         0x00    //TMOD[7:4] TIMER1 Mode setting
#define ET2             0x00    //TIMER2 overflow Interrupt Enable (Default 0 Disable)
#define ET1             0x00    //TIMER1 overflow Interrupt Enable (Default 0 Disable)
#define ET0             0x01    //TIMER0 overflow Interrupt Enable (Default 0 Disable)

#define d_MODEVALUE_T0      8192    //MODE0 13bit 8192
#define d_RELOADVALUE_T0    3000     //User Define Reload Value 
void TimCal(void);
void TIMER0_initialize(void)  //Initialize TIMER0
{
    IEN0 |= (ET2<<5)|(ET1<<3)|(ET0<<1);    //IE=IEN0
    TMOD  = (d_T1MOD<<4)|(d_T0MOD);
    TH0   = (d_MODEVALUE_T0-d_RELOADVALUE_T0)/32;    //MODE0 13bit 8192
    TL0   = (d_MODEVALUE_T0-d_RELOADVALUE_T0)%32;
    TR0   = 1;   //Timer0 Start bit
}

void TIMER0_ISR(void) interrupt TIMER0_VECTOR
{
    TH0 = (d_MODEVALUE_T0-d_RELOADVALUE_T0)/32;    //MODE0 13bit 8192
    TL0 = (d_MODEVALUE_T0-d_RELOADVALUE_T0)%32;
    
    // To do something by using timer interrupt.
    TimCal();
}
