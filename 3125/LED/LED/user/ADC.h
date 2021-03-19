/*--------------------------------------------------------------------------
Header file for ADC.
Modify on 2017/12/26.
--------------------------------------------------------------------------*/

#ifndef __ADC_H__
#define __ADC_H__

// Example Code
/*
void main(void)
{
    ADC_Init();
    ADC();
    while(1);
}
*/

void         ADC_Start(unsigned char n_ADC_CH);
void         ADC_Init(void);
unsigned int ADC_Read12();
void         ADC(void);
unsigned char GetADKey(void);

//===============================
//DEFINITIONs
//===============================
#define d_ADCnEN0           0x10
#define d_ADCnEN1           0x00
#define d_ADC_CLK_Sel       d_ADC_CLK_DIV192

#define d_ADC_CLK_DIV6      0x00
#define d_ADC_CLK_DIV12     0x01
#define d_ADC_CLK_DIV18     0x02
#define d_ADC_CLK_DIV24     0x03
#define d_ADC_CLK_DIV30     0x04
#define d_ADC_CLK_DIV36     0x05
#define d_ADC_CLK_DIV42     0x06
#define d_ADC_CLK_DIV48     0x07
#define d_ADC_CLK_DIV54     0x08
#define d_ADC_CLK_DIV60     0x09
#define d_ADC_CLK_DIV66     0x0A
#define d_ADC_CLK_DIV72     0x0B
#define d_ADC_CLK_DIV78     0x0C
#define d_ADC_CLK_DIV84     0x0D
#define d_ADC_CLK_DIV90     0x0E
#define d_ADC_CLK_DIV96     0x0F
#define d_ADC_CLK_DIV102    0x10
#define d_ADC_CLK_DIV108    0x11
#define d_ADC_CLK_DIV114    0x12
#define d_ADC_CLK_DIV120    0x13
#define d_ADC_CLK_DIV126    0x14
#define d_ADC_CLK_DIV132    0x15
#define d_ADC_CLK_DIV138    0x16
#define d_ADC_CLK_DIV144    0x17
#define d_ADC_CLK_DIV150    0x18
#define d_ADC_CLK_DIV156    0x19
#define d_ADC_CLK_DIV162    0x1A
#define d_ADC_CLK_DIV168    0x1B
#define d_ADC_CLK_DIV174    0x1C
#define d_ADC_CLK_DIV180    0x1D
#define d_ADC_CLK_DIV186    0x1E
#define d_ADC_CLK_DIV192    0x1F


#define d_ADC_CH0_IN        0
#define d_ADC_CH1_IN        1
#define d_ADC_CH2_IN        2
#define d_ADC_CH3_IN        3
#define d_ADC_CH4_IN        4
#define d_ADC_CH5_IN        5
#define d_ADC_CH6_IN        6
#define d_ADC_CH7_IN        7
#define d_ADC_CH8_IN        8
#define d_ADC_BG_IN         9

#define d_ADCR00            0
#define d_ADCR01            0x10
#define d_ADCR10            0x20

#define AD_SIG_KEY          0x01
#define AD_DU_KEY           0x02
#define AD_TMP1_KEY         0x03
#define AD_TMP2_KEY         0x04
#define AD_CLEAR_KEY        0x05
#define AD_SET_KEY          0x06
#endif
