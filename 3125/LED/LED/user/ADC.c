#include "OB38R08T1.h"
#include "ADC.h"
extern unsigned char CtrBuz(unsigned char ctr);
unsigned int n_data    = 0x0000;
bit          ADCfinish = 0;


void ADC_Start(unsigned char n_ADC_CH)
{
    ADCC2  = n_ADC_CH; //Set a new channel
    ADCC2 |= 0x80;      //ADC start conversion
}

void ADC_Init(void)
{
    ADCC1 = d_ADCnEN0;                 //Set ADC channel
    ADCCS = d_ADCnEN1 | d_ADC_CLK_Sel; //Set ADC channel & Select ADC clock
    IEADC = 1;                         //Enable ADC interrupt.
    EA    = 1;
}

unsigned int ADC_Read12() //12-bit ADC
{
    unsigned int n_ADCD12 = 0x0000;
    n_ADCD12 = (ADCDH<<4) | ADCDL;     //12-bit ADC
    return n_ADCD12;
}

void ADCInterrupt(void) interrupt d_ADC_Vector // ADC Interrupt Vector Address 0053h => interrupt 10
{
    n_data = ADC_Read12(); //Read ADC
    ADCfinish = 1;
}
//void Delayus(unsigned int tim)
//{
//    unsigned int  i,j;
//    for (i=0; i<tim; i++)
//        for (j=0; j<100; j++);
//}

void ADC(void)
{
//    if (d_ADCnEN0 & 0x01)
//    {
//        ADC_Start(d_ADC_CH0_IN);
//        while(!ADCfinish);
//        ADCfinish = 0;
//    }
//    if (d_ADCnEN0 & 0x02)
//    {
//        ADC_Start(d_ADC_CH1_IN);
//        while(!ADCfinish);
//        ADCfinish = 0;
//    }
//    if (d_ADCnEN0 & 0x04)
//    {
//        ADC_Start(d_ADC_CH2_IN);
//        while(!ADCfinish);
//        ADCfinish = 0;
//    }
//    if (d_ADCnEN0 & 0x08)
//    {
//        ADC_Start(d_ADC_CH3_IN);
//        while(!ADCfinish);
//        ADCfinish = 0;
//    }
    if (d_ADCnEN0 & 0x10)
    {
        ADC_Start(d_ADC_CH4_IN);
        while(!ADCfinish);
        ADCfinish = 0;
    }
//    if (d_ADCnEN0 & 0x20)
//    {
//        ADC_Start(d_ADC_CH5_IN);
//        while(!ADCfinish);
//        ADCfinish = 0;
//    }
//    if (d_ADCnEN0 & 0x40)
//    {
//        ADC_Start(d_ADC_CH6_IN);
//        while(!ADCfinish);
//        ADCfinish = 0;
//    }
//    if (d_ADCnEN0 & 0x80)
//    {
//        ADC_Start(d_ADC_CH7_IN);
//        while(!ADCfinish);
//        ADCfinish = 0;
//    }
//    if (d_ADCnEN1 & 0x40)
//    {
//        ADC_Start(d_ADC_CH8_IN);
//        while(!ADCfinish);
//        ADCfinish = 0;
//    }
//    if (d_ADCnEN1 & 0x80)
//    {
//        ADC_Start(d_ADC_BG_IN);
//        while(!ADCfinish);
//        ADCfinish = 0;
//    }
}

unsigned int GetADCCalVal()
{
    unsigned char xdata i=0;
    unsigned int xdata OKVal=0;
    unsigned int xdata total=0;
    OKVal=0;
    for(i=0; i<10; i++)
    {
        ADC();
        if((n_data>220)&&(n_data<3870))//除掉无效值
        {
            total+=n_data;
            OKVal++;
        }
    }
    if(OKVal>6)
    {
        return total/OKVal;//返回平均值
    }
    return 0;
}


#define  RL1       10//下拉电阻10K  20
#define  RDUB      24//双杯电阻
#define  RSIG      62//单杯电阻
#define  RTMP1     12//按键电阻 手动
#define  RTMP2     7.5//温度1电阻
#define  CLAER     4.3//温度2电阻

#define  SING_AD      568
#define  DBU_AD       1204
#define  TMP1_AD      1861
#define  TMP2_AD      2340
#define  CLEAR_AD     2864
#define  SET_AD       1500
#define  KEY_OFFSET   150

unsigned char GetADKey(void)
{
    unsigned int xdata ADValc=0;
    ADValc=GetADCCalVal();
    if((ADValc>(SING_AD-KEY_OFFSET))&&(ADValc<(SING_AD+KEY_OFFSET)))
    {
        return AD_SIG_KEY;
    }
    else if((ADValc>(DBU_AD-KEY_OFFSET))&&(ADValc<(DBU_AD+KEY_OFFSET)))
    {
        return AD_DU_KEY;
    }
    else if((ADValc>(CLEAR_AD-KEY_OFFSET))&&(ADValc<(CLEAR_AD+KEY_OFFSET)))
    {
        return AD_CLEAR_KEY;
    }
    else if((ADValc>(TMP1_AD-KEY_OFFSET))&&(ADValc<(TMP1_AD+KEY_OFFSET)))
    {
        return AD_TMP1_KEY;
    }
    else if((ADValc>(TMP2_AD-KEY_OFFSET))&&(ADValc<(TMP2_AD+KEY_OFFSET)))
    {
        return AD_TMP2_KEY;
    }
	else if((ADValc>(SET_AD-KEY_OFFSET))&&(ADValc<(SET_AD+KEY_OFFSET)))
    {
        return AD_SET_KEY;
    }
    return 0;
}
