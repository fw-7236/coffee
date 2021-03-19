#include "OB38R08T1.h"
#include "Timer.h"

#define TIMER1_VECTOR 1    //TIMER1 Interrupt Vevtor
#define d_T0MOD       0x00 //TMOD[3:0] TIMER0 Mode setting 
#define d_T1MOD       0x05 //TMOD[7:4] TIMER1 Mode setting
#define ET2           0x00 //TIMER2 overflow Interrupt Enable (Default 0 Disable)
#define ET1           0x01 //TIMER1 overflow Interrupt Enable (Default 0 Disable)
#define ET0           0x00 //TIMER0 overflow Interrupt Enable (Default 0 Disable)

#define d_MODEVALUE_T1      65536    //MODE1 16bit 65536
#define d_RELOADVALUE_T1    0      //User Define Reload Value 

void TIMER1_initialize(void)  //Initialize TIMER1
{
    IEN0 |= (ET2<<5)|(ET1<<3)|(ET0<<1); //IE=IEN0
    TMOD  = (d_T1MOD<<4)|(d_T0MOD);
    TH1   = (d_MODEVALUE_T1-d_RELOADVALUE_T1)/256;
    TL1   = (d_MODEVALUE_T1-d_RELOADVALUE_T1)%256;
    TR1   = 1; //Timer1 Start bit
}

void TIMER1_ISR(void) interrupt TIMER1_VECTOR
{
    unsigned char xdata testH=0,testL=0;
    testH=TH1;
    testL=TL1;
}
