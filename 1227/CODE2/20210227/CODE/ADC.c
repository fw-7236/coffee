#include "OB38R08T1.h"
#include "ADC.h"

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
unsigned int ADC_Val[10];//储储AD转换
void ADC(unsigned char channel)
{
    if(channel==0)
    {
        if (d_ADCnEN0 & 0x01)
        {
            ADC_Start(d_ADC_CH0_IN);
            while(!ADCfinish);
            ADCfinish = 0;
        }
    }
    else if(channel==3)
    {
        if (d_ADCnEN0 & 0x08)
        {
            ADC_Start(d_ADC_CH3_IN);
            while(!ADCfinish);
            ADCfinish = 0;
        }
    }
    else if(channel==8)
    {
        if (d_ADCnEN1 & 0x40)
        {
            ADC_Start(d_ADC_CH8_IN);
            while(!ADCfinish);
            ADCfinish = 0;
        }
    }
}
unsigned int GetADCCalVal(unsigned char channel)
{
    unsigned int xdata AdPerVal=0;
    unsigned char xdata i=0;
    AdPerVal=0;
    for(i=0; i<10; i++)
    {
        ADC(channel);
        ADC_Val[i]=n_data;
        AdPerVal=AdPerVal+ADC_Val[i];
    }
    return AdPerVal/10;
}
#define SHUI_REF  2000
#define MIN_REF   50
unsigned char CheckShuiState(void)
{
    unsigned int xdata ADValc=0;
    ADValc=GetADCCalVal(d_ADC_CH3_IN);
    if(ADValc<SHUI_REF)
    {
        if(ADValc>MIN_REF)
        {
            return 0;
        }
        else
        {
            //return 2;
            return 0;
        }
    }
    else
    {
        return 1;
    }
}
unsigned char CheckShuiStateB(void)
{
    unsigned int xdata ADValc=0;
    ADValc=GetADCCalVal(d_ADC_CH0_IN);
    if(ADValc<SHUI_REF)
    {
        if(ADValc>MIN_REF)
        {
            return 0;
        }
        else
        {
            //return 2;
            return 0;
        }
    }
    else
    {
        return 1;
    }
}

unsigned char CheckShuiXiang(void)//检查水箱是否OK
{
    unsigned int xdata ADValc=0;
    ADValc=GetADCCalVal(d_ADC_CH8_IN);
    if(ADValc<SHUI_REF)
    {
        if(ADValc>MIN_REF)
        {
            return 0;
        }
        else
        {
            //return 2;
            return 0;
        }
    }
    else
    {
        return 1;//表示没有水箱
    }
}