#include "OB38R08T1.h"
#include "ADC.h"
#include "WDT.h"

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

//                                -1  0   1   2   3   4   5   6   7   8   9  10   11  12  13  14  15 16  17  18  19
code unsigned int TemTable[161]= {3981,3975,3969,3962,3955,3948,3941,3933,3925,3917,3908,3899,3890,3880,3870,3860,3849,3838,3826,3815,3802,
//                                20   21   22    23  24   25   26   27   28   29   30   31   32   33   34    35  36   37  38   39
                                  3789,3776,3762,3748,3734,3718,3703,3687,3670,3653,3636,3618,3599,3581,3561,3541,3521,3500,3478,3456,
//                                40   41  42    43   44   45   46   47   48   49  50    51   52   53   54   55   56    57   58  59
                                  3433,3410,3387,3363,3338,3313,3288,3262,3235,3208,3181,3153,3125,3096,3067,3038,3008,2978,2947,2916,
//                                60  61     62   63  64   65   66  67    68   69    70   71  72   73   74   75   76   77   78   79
                                  2885,2853,2821,2789,2757,2724,2691,2658,2625,2592,2558,2525,2491,2457,2423,2389,2355,2321,2287,2253,
//                               80   81   82    83   84   85   86    87  88   89   90   91   92   93   94   95   96   97  98    99
                                  2220,2186,2152,2119,2085,2052,2019,1986,1953,1920,1888,1856,1824,1793,1761,1730,1700,1669,1639,1609,
//                               100  101  102   103  104  105  106  107  108
                                  1580,1551,1522,1494,1466,1438,1411,1384,1357,1331,1305,1280,1255,1230,1206,1182,1159,1135,1113,1091,
//                               120
                                  1069,1047,1026,1005,985,965,945,926,907,888,870,852,835,818,801,784,768,753,737,722,
//                               140 141 142 143 144 145 146 147 148 149 150 151 152 153 154 155 156 157 158 159
                                  707,692,678,664,650,637,624,611,599,586,574,563,551,540,529,518,507,487,487,477
                                 };


#define AD_TMP_MAX   1020
#define AD_TMP_MIN   100

unsigned int ADC_Val[10];//储储AD转换
void ADC(unsigned char pos,unsigned char channel)
{
    ADCfinish = 0;
    if (channel==d_ADC_CH0_IN)
    {
        ADC_Start(d_ADC_CH0_IN);
        while(!ADCfinish);
        ADC_Val[pos]=n_data;
    }
    else if (channel==d_ADC_CH1_IN)
    {
        ADC_Start(d_ADC_CH1_IN);
        while(!ADCfinish);
        ADC_Val[pos]=n_data;
    }
    else if (channel==d_ADC_CH2_IN)
    {
        ADC_Start(d_ADC_CH2_IN);
        while(!ADCfinish);
        ADC_Val[pos]=n_data;
    }
    else if (channel==d_ADC_CH3_IN)
    {
        ADC_Start(d_ADC_CH3_IN);
        while(!ADCfinish);
        ADC_Val[pos]=n_data;
    }
    else if (channel==d_ADC_CH4_IN)
    {
        ADC_Start(d_ADC_CH4_IN);
        while(!ADCfinish);
        ADC_Val[pos]=n_data;
    }
    else if (channel==d_ADC_CH5_IN)
    {
        ADC_Start(d_ADC_CH5_IN);
        while(!ADCfinish);
        ADC_Val[pos]=n_data;
    }
    else if (channel==d_ADC_CH6_IN)
    {
        ADC_Start(d_ADC_CH6_IN);
        while(!ADCfinish);
        ADC_Val[pos]=n_data;
    }
    else if (channel==d_ADC_CH7_IN)
    {
        ADC_Start(d_ADC_CH7_IN);
        while(!ADCfinish);
        ADC_Val[pos]=n_data;
    }
}


unsigned int GetADCCalVal(unsigned char channel)
{
    unsigned char i;
    unsigned int ADVal=0;
    ADVal=0;
    for(i=0; i<10; i++)
    {
        ADC(i,channel);
        ADVal=ADC_Val[i]+ADVal;
    }
    ADVal=ADVal/10;
    return ADVal;
}

unsigned char GetTempVal(unsigned char channl)
{
    unsigned int xdata ADValc=0;
    unsigned char xdata i,CTemp;
    unsigned char xdata chann=0;
    chann=channl;
    ADValc=GetADCCalVal(chann);//
    for(i=0; i<160; i++)
    {
        if((ADValc<(TemTable[i-1]))&&(ADValc>(TemTable[i+1])))
        {
            CTemp=i;
            return CTemp;
        }
    }
    return 0;
}

unsigned char GetCafeTmp(void)
{
    unsigned char xdata Ctmp=0;
    WDTR_Disable();
    Ctmp=GetTempVal(d_ADC_CH0_IN);
    WDTR_Init();
    return  Ctmp;
}

unsigned char GetZhenQiTmp(void)
{
    unsigned char xdata Ztmp=0;
    WDTR_Disable();
    Ztmp=GetTempVal(d_ADC_CH1_IN);
    WDTR_Init();
    return  Ztmp;
}

#define SHUI_NO   600
unsigned char GetShuiLevel(void)
{
    unsigned int xdata ADValc=0;
    WDTR_Disable();
    ADValc=GetADCCalVal(d_ADC_CH3_IN);
    WDTR_Init();
    if(ADValc<SHUI_NO)//表示有水
    {
        return 1;
    }
    else
    {
        return 0;//表示无水
    }
}



