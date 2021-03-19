#include "OB38R08T1.h"
#include "EEPROM.h"
#include "Timer.h"
#include "wdt.h"
#define  ONES          445///ѭ��ʱ��Ϊ1�� 440
//*******************************************************************

typedef enum _WORKState
{
    OPT_STATE_INIT     =     0,//0x00 ��ʾ������ʼ��״̬ ȫ���������ʾ888 1��
    OPT_STATE_PREH     =     1,//0x01 ��ʾ����Ԥ�Ƚ׶� ���ˮλ ������Χֵ��ʾNG ��ʾ0x000 �������״̬ 0x09
    OPT_STATE_IDLE     =     2,//�ȴ��û�����״̬
    OPT_STATE_HEAD1    =     3,//��ʾ����״̬1
    OPT_STATE_HEAD2    =     4,//��ʾ����״̬2
    OPT_STATE_CLERA    =     5,//��ʾ���״̬
    OPT_STATE_CLERB    =     6,//��ʾ�������
    OPT_STATE_SHEAD1   =     7,//��ʾ�¶�����1
    OPT_STATE_SHEAD2   =     8,//��ʾ�¶�����2
    OPT_STATE_ERR1     =     9,//��ˮ״̬
    OPT_STATE_SLEEP    =     10,//�ػ�״̬
    OPT_STATE_ERR2     =    11,//��ˮ����2����
} WorkState;

WorkState  swWorkState=OPT_STATE_INIT;//����״̬��
WorkState  swWorkStateB=OPT_STATE_INIT;//����״̬��

bit  HeatFlag;//���ȱ��ֵ
bit  TempFlag;//�¶�״̬

bit  HeatFlagB;//���ȱ��ֵ
bit  TempFlagB;//�¶�״̬

unsigned char xdata SenTemp=0;
unsigned int xdata  ShuiTime=0;//��ˮʱ�����

unsigned char xdata SenTempB=0;
unsigned int xdata  ShuiTimeB=0;//��ˮʱ�����

unsigned int xdata MkCafeTime=0;
unsigned int xdata MkCafeTimeB=0;
unsigned int xdata ZhenTime=0;
unsigned int xdata ZhenTimeB=0;
unsigned char xdata PowerState=0;
unsigned char xdata PowerStateB=0;
unsigned char xdata LedOnOFF=0;
unsigned char xdata LedOnOFFB=0;
unsigned char  Ledflag=1;
unsigned char  LedBflag=1;

#define FA_RA   1000

#define TAR_CAFE    95
#define TAR_ZHEN    110

unsigned int AutCtrTime=0;
unsigned int AutCtrTimeB=0;
unsigned int TimeCc=0;
unsigned int TimeCcB=0;

unsigned char Lamp=0;
unsigned char LampB=0;
unsigned char Temp1=40;//��¼T1�¶�ֵ
unsigned char Temp2=60;//��¼T2�¶�ֵ

unsigned char Temp1B=40;//��¼T1�¶�ֵ
unsigned char Temp2B=60;//��¼T2�¶�ֵ

unsigned char HeadMaxTmp=0;
unsigned char HeadMaxTmpB=0;

unsigned char ShuiFlag=0;
unsigned char ShuiFlagB=0;
unsigned int KeyTime=0;
unsigned int KeyTimeB=0;

unsigned char KeyFlag=0;
unsigned char KeyFlagB=0;
unsigned char KeyBut=0;
unsigned char KeyButB=0;
unsigned char HeatFa=0;//
unsigned char HeatFaB=0;//
unsigned char ErrCode=0;
unsigned char ErrCodeB=0;
unsigned char StartMach=0xAA;
unsigned char StartMachB=0xAA;
unsigned int DelayTime=0;
unsigned int DelayTimeB=0;


static unsigned char xdata sda1=0;
static unsigned char xdata sda2=0;
static unsigned char xdata sda3=0;
static unsigned char xdata sda4=0;
static unsigned char xdata sdaB1=0;
static unsigned char xdata sdaB2=0;
static unsigned char xdata sdaB3=0;
static unsigned char xdata sdaB4=0;
unsigned char distime=0;


//�洢
#define DFE_CAFE    25
#define DFE_HT1     5
#define DFE_HT2     5

#define MAX_ZT      300  //����ʱ��
#define MAX_HT1     30
#define MAX_HT2     30
#define MAX_CFE     60
#define MAX_POWER   900  //15���ӹػ�


#define TMP1_VAL    0  //
#define TMP2_VAL     1  //

#define TMP1B_VAL    0  //
#define TMP2B_VAL     1  //

#define DAFETMP  60    //�ָ���Ĭ���¶�

#define KeyLong   20
#define KeyNor    0
#define STMPMAX   90
#define STMPMIN   40

#define TMELOW    35

#define SW_L  1
#define SW_LONG  280
#define SW_LONGA  50

unsigned char CheckShuiState(void);
unsigned char CheckShuiStateB(void);
unsigned char CheckShuiXiang(void);
void Display(unsigned char da1,unsigned char da2,unsigned char da3,unsigned char da4);
void ADC_Init(void);

#define  ADD_SW     0x06 //��ʾ��1����
#define  SUB_SW     0x01 //��ʾ��1����
#define  CLEAR_SW   0x02 //��ʾ��ఴ��
#define  TMP1_SW    0x03 //��ʾ�¶�1����
#define  TMP2_SW    0x04 //��ʾ�¶�1����
#define  START_SW   0x05 //��ʾ��������

#define  START_LED  0x10
//**********************************************************************
#define  CT1_S         P1_7   //ˮ�õ������1 
#define  CT1_Z         P1_6   //�������ȿ��ƻ��߼��ȿɿع����	 PWM0
#define  CT1_C         P3_1  //��ŷ����ؿ���

#define  LEVE1         P0_7 //ADC7  ˮλ��������
#define  SCR1          P0_6 //��̬�̵������� ����
#define  P_SW          P0_5 //ѹ������

#define  CS             P1_5 //MAX6675 IC A
#define  SCK            P0_0
#define  SDA            P0_1

#define  SPI_STB        P1_2
#define  SPI_CLK        P1_3
#define  SPI_DA         P1_4
//***************************************************************************
#define  CT2_S          P1_0   //ˮ�õ������   
#define  CT2_Z          P1_1   //�������ȿ��ƻ��߼��ȿɿع����	 
#define  CT2_C          P3_0  //��ŷ����ؿ���

#define  LEVE2          P0_4 //ADC0  ˮλ��������
#define  SCR2           P0_3 //��̬�̵������� ����
#define  P_SW2          P0_2 //ѹ������

#define  CS2             P2_3//MAX6675 TWO

#define  SPI_STB2        P3_5
#define  SPI_CLK2        P3_4
#define  SPI_DA2         P3_3

#define  PW1             P2_6 //��Դ����1
#define  PW2             P2_5 //��Դ����2
//**********************************************************************
//*******************************************************************
// 00 ��ʾ˫��IO
// 01 ��ʾ�������
// 10 ��ʾ��������
// 11 ��ʾ��©
void GPIO_Init()
{
    P0 = 0x00;
    P1 = 0x00;
    P2 = 0x30;
    P3 = 0x00;
//P0.0-->SCK  MAX6675 IIC
//P0.1-->SDA  MAX6675 IIC
//P0.2-->P_SW2  ѹ������2
//P0.3-->SCR2	����
//P0.4-->LEVE2  ˮ�����2
//P0.5-->P_SW   ѹ������
//P0.6-->SCR1   ��̬�̵���
//P0.7-->LEVE1  ˮλ�������
//      7  6  5  4  3  2  1  0
//P0M1  0  0  0	 0	0  0  0	 0
//P0M0  0  0  0	 0	0  0  1	 1
    P0M1 = 0x90;
    P0M0 = 0x01;
//      7  6  5  4  3  2  1  0
//P1M1  0  0  1	 1	0  1  1	 0
//P1M0  1  1  0	 0	1  0  0	 0
//P1.0-->CT2_S  ˮ�õ������
//P1.1-->CT2_Z  �������ȿ��ƻ��߼��ȿɿع����
//P1.2-->SPI_STB
//P1.3-->SPI_CLK
//P1.4-->SPI_DA
//P1.5-->CS       MAX6675Ƭѡ
//P1.6-->CT1_Z    ��̬�̵���
//P1.7-->CT1_S    //ˮ�õ������1
    P1M1 = 0x00;
    P1M0 = 0xEF;
//P2.3 ��� P2.5 ��� P2.6����
//      7  6  5  4  3  2  1  0
//P2M1  0  0  0	 0	0  0  0	 0
//P2M0  0  0  0	 0	1  0  0	 0

//      7  6  5  4  3  2  1  0
//P3M1  0  0  0	 0	0  0  0	 0
//P3M0  0  0  1	 1	1  0  0	 0
//******************************************************
//P2.3--->CS2
    P2M1 = 0x60;
    P2M0 = 0x08;
//***********************************************************
//P3.0--->CT2_C   ��ŷ����ؿ���2
//P3.1--->CT1_C   ��ŷ����ؿ���1
//P3.2--->AD1     ����
//P3.3--->DIO2    //SPI TWO
//P3.4--->CLK2    //SPI
//P3.5--->STB2
    P3M1 = 0x00;
    P3M0 = 0xF7;
//********************************************************
    CT1_S=0;
    CT1_Z=0; //PWM0
    CT1_C=0;
    P_SW=1;

    CT2_S=0;
    CT2_Z=0; //PWM0
    CT2_C=0;
    P_SW2=1;
//********************************************************
}
void delayNP(unsigned int tim)
{
    unsigned int  i,j;
    for (i=0; i<tim; i++)
    {
        for (j=0; j<10; j++);
    }
}
void delayns(unsigned int tim)
{
    unsigned int i;
    for(i=0; i<tim; i++)
    {
    }
}
/**************************************************************/
//���ܣ���TM1628����8λ����
/**************************************************************/
void send_8bit(unsigned char dat)
{
    unsigned char i;
    for(i=0; i<8; i++)
    {
        if(dat&0x01)
            SPI_DA=1;
        else SPI_DA=0;
        SPI_CLK=0;
        delayns(2);
        SPI_CLK=1;
        delayns(2);
        dat=dat>>1;
    }
}
unsigned char read_8bit(void)//�½������
{
    unsigned char xdata reData,i;
    SPI_DA=1;
    reData=0;
    for(i=0; i<8; i++)
    {
        SPI_CLK=0;
        delayns(2);
        SPI_CLK=1;
        delayns(2);
        reData=reData>>1;
        if(SPI_DA)
        {
            reData=reData|0x80;
        }
    }
    return reData;
}



//************************************************************************************************************************
unsigned int MAX6675_ReadReg(unsigned char channel)
{
    unsigned char i;
    unsigned int dat;
    i   = 0;
    dat = 0;
    SCK = 0;
    if(channel==1)
    {
        CS=0;
    }
    else
    {
        CS2=0;
    }
    delayNP(200);
    for(i=0; i<16; i++)		//get D15-D0 from 6675
    {
        SCK = 1;
        delayNP(40);
        dat = dat<<1;
        if( SDA==1 )
            dat = dat|0x01;
        SCK = 0;
        delayNP(40);
    }
    if(channel==1)
    {
        CS=1;
    }
    else
    {
        CS2=1;
    }
    return dat;
}
//*********************************************************************************************************************
//��������                                                                          0x0A  0x0b                0x0f 0x10
//��������
unsigned char const CODE[][2] =
//   0           1         2            3          4            5           6
{   {0x3F,0x00},{0x06,0x00},{0x5b,0x00},{0x4f,0x10},{0x66,0x00},{0x6d,0x00},{0x7D,0x00},
//   7           8     		 9	   E          L            o	  �� 0x0D	  0x0E
    {0x07,0x00},{0x7f,0x00},{0x6f,0x00},{0x79,0x00},{0x38,0x00},{0x5c,0x00},{0x00,0x00},{0xFF,0x00}
}; //4λ8��ģʽ�£����������0-9�ı���


/*****************����ָ��*************************************/
void command(unsigned char com)
{
    SPI_STB=1;
    delayns(200);
    SPI_STB=0;
    send_8bit(com);
}
//*******************************************************************************************************
/**************************************************************/
//���ܣ���1-4λ�������ʾ����0-3
/**************************************************************/
void Display(unsigned char da1,unsigned char da2,unsigned char da3,unsigned char da4)
{
    if(ErrCode==0x01)
    {
        da1=0x0D;
        da2=0x0A;
        da3=0x01;
        if(sda4==da4)
            return;
    }
    command(0x01); //������ʾģʽ��4λ13��ģʽ
    command(0x40); //������������,���õ�ַ�Զ���1ģʽ
    command(0xc0); //������ʾ��ַ����00H��ʼ
    send_8bit(CODE[da1][0]);
    send_8bit(CODE[da1][1]);
    send_8bit(CODE[da2][0]);
    send_8bit(CODE[da2][1]);
    send_8bit(CODE[da3][0]);
    send_8bit(CODE[da3][1]);
    send_8bit(0x00);
    send_8bit(0x00);
    send_8bit(da4);
    send_8bit(0x00);
    command(0x8E); //��ʾ�����������ʾ������Ϊ����
    SPI_STB=1;
    sda1=da1;
    sda2=da2;
    sda3=da3;
    sda4=da4;
}
unsigned char CheckKey(void)//��ȡ����ֵ
{
    unsigned char  Key1,Key2,Key3,Key4,Key5;
    unsigned char KeyVal=0;
    command(0x42);//���ö�ȡ����ָ��
    Key1=read_8bit(); //
    Key2=read_8bit();
    Key3=read_8bit();//SW2A
    Key4=read_8bit();//SW1A
    Key5=read_8bit();//SW1B  SW2B
    KeyVal=0x00;
    if((Key1&0x01)>0)//��ʾ����1
    {
        KeyVal=0x01;
    }
    else if((Key1&0x08)>0)//��ʾ����2
    {
        KeyVal=0x02;
    }
    else if((Key2&0x01)>0)//��ʾ����1
    {
        KeyVal=0x03;
    }
    else if((Key2&0x08)>0)//��ʾ����2
    {
        KeyVal=0x04;
    }
    else if((Key3&0x01)>0)//��ʾ����1
    {
        KeyVal=0x05;
    }
    else if((Key3&0x08)>0)//��ʾ����2
    {
        KeyVal=0x06;
    }
    return KeyVal;
}
/*****************����ָ��*************************************/
void send_8bitB(unsigned char dat)
{
    unsigned char i;
    for(i=0; i<8; i++)
    {
        if(dat&0x01)
            SPI_DA2=1;
        else SPI_DA2=0;
        SPI_CLK2=0;
        delayns(2);
        SPI_CLK2=1;
        delayns(2);
        dat=dat>>1;
    }
}
unsigned char read_8bitB(void)//�½������
{
    unsigned char xdata reData,i;
    SPI_DA2=1;
    reData=0;
    for(i=0; i<8; i++)
    {
        SPI_CLK2=0;
        delayns(2);
        SPI_CLK2=1;
        delayns(2);
        reData=reData>>1;
        if(SPI_DA2)
        {
            reData=reData|0x80;
        }
    }
    return reData;
}
void commandB(unsigned char com)
{
    SPI_STB2=1;
    delayns(200);
    SPI_STB2=0;
    send_8bitB(com);
}
/**************************************************************/
void DisplayB(unsigned char da1,unsigned char da2,unsigned char da3,unsigned char da4)
{

    if(ErrCodeB==0x01)
    {
        da1=0x0D;
        da2=0x0A;
        da3=0x01;
        if(sda4==da4)
            return;
    }
    commandB(0x01); //������ʾģʽ��4λ13��ģʽ
    commandB(0x40); //������������,���õ�ַ�Զ���1ģʽ
    commandB(0xc0); //������ʾ��ַ����00H��ʼ
    send_8bitB(CODE[da1][0]);
    send_8bitB(CODE[da1][1]);
    send_8bitB(CODE[da2][0]);
    send_8bitB(CODE[da2][1]);
    send_8bitB(CODE[da3][0]);
    send_8bitB(CODE[da3][1]);
    send_8bitB(0x00);
    send_8bitB(0x00);
    send_8bitB(da4);
    send_8bitB(0x00);
    commandB(0x8E); //��ʾ�����������ʾ������Ϊ����
    SPI_STB2=1;
    sdaB1=da1;
    sdaB2=da2;
    sdaB3=da3;
    sdaB4=da4;
}
//******************************************************************************************************
unsigned char CheckKeyB(void)//��ȡ����ֵ
{
    unsigned char  Key1,Key2,Key3,Key4,Key5;
    unsigned char KeyVal=0;
    commandB(0x42);//���ö�ȡ����ָ��
    Key1=read_8bitB(); //
    Key2=read_8bitB();
    Key3=read_8bitB();//SW2A
    Key4=read_8bitB();//SW1A
    Key5=read_8bitB();//SW1B  SW2B
    KeyVal=0x00;
    if((Key1&0x01)>0)//��ʾ����1
    {
        KeyVal=0x01;
    }
    else if((Key1&0x08)>0)//��ʾ����2
    {
        KeyVal=0x02;
    }
    else if((Key2&0x01)>0)//��ʾ����1
    {
        KeyVal=0x03;
    }
    else if((Key2&0x08)>0)//��ʾ����2
    {
        KeyVal=0x04;
    }
    else if((Key3&0x01)>0)//��ʾ����1
    {
        KeyVal=0x05;
    }
    else if((Key3&0x08)>0)//��ʾ����2
    {
        KeyVal=0x06;
    }
    return KeyVal;
}
//*******************************************************************
void  WriteTmp1Eeprom(void)
{
    Write_EEPROM(TMP1_VAL,Temp1);
}
void  ReadTmp1Eeprom(void)
{
    Temp1=Read_EEPROM(TMP1_VAL);
    if(Temp1>STMPMAX)//����ȡֵ���ڷ�Χֵ �ָ�Ĭ��ֵ
    {
        Temp1=DAFETMP;
        WriteTmp1Eeprom();
    }
}
void  WriteTmp2Eeprom(void)
{
    Write_EEPROM(TMP2_VAL,Temp2);
}
void  ReadTmp2Eeprom(void)
{
    Temp2=Read_EEPROM(TMP2_VAL);
    if(Temp2>STMPMAX)//����ȡֵ���ڷ�Χֵ �ָ�Ĭ��ֵ
    {
        Temp2=DAFETMP;
        WriteTmp2Eeprom();
    }
}
//**************************************************************
void  WriteTmp1BEeprom(void)
{
    Write_EEPROM(TMP1B_VAL,Temp1);
}
void  ReadTmp1BEeprom(void)
{
    Temp1B=Read_EEPROM(TMP1B_VAL);
    if(Temp1B>STMPMAX)//����ȡֵ���ڷ�Χֵ �ָ�Ĭ��ֵ
    {
        Temp1B=DAFETMP;
        WriteTmp1BEeprom();
    }
}
void  WriteTmp2BEeprom(void)
{
    Write_EEPROM(TMP2B_VAL,Temp2);
}
void  ReadTmp2BEeprom(void)
{
    Temp2B=Read_EEPROM(TMP2B_VAL);
    if(Temp2B>STMPMAX)//����ȡֵ���ڷ�Χֵ �ָ�Ĭ��ֵ
    {
        Temp2B=DAFETMP;
        WriteTmp2BEeprom();
    }
}
//*********************************************************************
void TimCal(void)
{
    TimeCc++;
    TimeCcB++;
    if(MkCafeTime<60000)
        MkCafeTime++;
    if(MkCafeTimeB<60000)
        MkCafeTimeB++;
    AutCtrTime++;
    AutCtrTimeB++;
    WDTR_CountClear();
}

//*******************************************************************************
//���ط����߲����� �����������׸���
#define TMP_OFFSET  0
void FlashTempVal(void)
{
    unsigned int t=0;
    static unsigned char  CurSenErr=0;
    unsigned char Flag_connect=0;
    t=MAX6675_ReadReg(1);
    Flag_connect=t&0x04;
    if(Flag_connect>0)
    {
        CurSenErr++;
        SenTemp=25;
    }
    else
    {
        CurSenErr=0;
        t=t>>3;
        SenTemp=t/4-TMP_OFFSET;
    }
    if(CurSenErr>5)
    {
        if(ErrCode!=0x01)
        {
            ErrCode=0x01;
            Display(0x0D,0x0A,0x01,Lamp);//��ʾ��������·E3
        }
        //swWorkState=OPT_STATE_ERR1;
    }
}

void FlashTempValB(void)
{
    unsigned int t=0;
    static unsigned char  CurSenErr=0;
    unsigned char Flag_connect=0;
    t=MAX6675_ReadReg(2);
    Flag_connect=t&0x04;
    if(Flag_connect>0)
    {
        CurSenErr++;
        SenTempB=25;
    }
    else
    {
        CurSenErr=0;
        t=t>>3;
        SenTempB=t/4-TMP_OFFSET;
    }
    if(CurSenErr>5)
    {
        if(ErrCodeB!=0x01)
        {
            ErrCodeB=0x01;
            DisplayB(0x0D,0x0A,0x01,Lamp);//��ʾ��������·E3
        }
        //swWorkState=OPT_STATE_ERR1;
    }
}




extern unsigned char CheckShuiState(void);
#define ZHEQITMP  130
#define CAFETMP   124
#define OFFSET_TMP  7

#define ZQVALL  120
#define BU_2MIN  234  //120��ʱ��
#define BU_FULL  20 //��ˮʱ�� ��⵽ˮ֮��10��
#define BEN_RA   200 //��ˮ�ø���
#define ZQVALH  125

#define SHUI_FULL  0
#define SHUI_EMPT  1
void TemControlAuto(void)
{
    unsigned char TemVal=0;
    unsigned char ShuiResult=0;
    unsigned int CalTemm=0;
//***************************************************************
    if(AutCtrTime>(0.5*ONES))
    {
        AutCtrTime=0;
        FlashTempVal();
        if(HeatFlag==1)//��ʾҪ���м��ȡ�����¶ȡ����ˮλ ѭ������
        {
            if(P_SW==1)//���ѹ������
            {
                CT1_Z=0;
                HeatFa=0x00;
                StartMach=0x00;
            }
            else //��ʾ�����¶ȵ���130�� ��ʼ����
            {
                CT1_Z=1;
                HeatFa=0xAA;
            }
        }
        else
        {
            CT1_Z=0;
            HeatFa=0x00;
        }
        ShuiResult=CheckShuiState();
        switch(ShuiResult)
        {
        case SHUI_FULL:	  //��ʾ�Ѿ���ˮ
            if(ShuiFlag==1)
            {
                ShuiTime=0;
                ShuiFlag=0x55;
            }
            if(ShuiFlag==0x55)
            {
                ShuiTime++;
                if(ShuiTime>BU_FULL)//��ˮ10��
                {
                    ShuiFlag=0;
                    ShuiTime=0;
                    CT1_S=0;
                }
            }
            break;
        case SHUI_EMPT://��Ҫ��ˮ
            if((ShuiFlag==0)&&((swWorkState==OPT_STATE_INIT)||(swWorkState==OPT_STATE_IDLE)))
            {
                CT1_S=1;//������ˮ
                delayNP(BEN_RA);//��ʱ �����źŸ���
                ShuiFlag=1;
                ShuiTime=0;
                Display(0x00,0x00,0x00,0x00);//
            }
            ShuiTime++;
            delayNP(1000);
            if(ShuiTime>BU_2MIN)//��ʾ��ˮ����2����
            {
                CT1_S=0;
                ShuiFlag=0;
                ShuiTime=0;
                Display(0x0D,0x0A,0x02,Lamp);//��ʾ��ˮʧ��
                swWorkState=OPT_STATE_ERR1;
            }
            break;
        default:
            break;
        }
    }
}

void TemControlAutoB(void)
{
    unsigned char TemVal=0;
    unsigned char ShuiResult=0;
    unsigned int CalTemm=0;
//***************************************************************
    if(AutCtrTimeB>(0.5*ONES))
    {
        AutCtrTimeB=0;
        FlashTempValB();
        if(HeatFlagB==1)//��ʾҪ���м��ȡ�����¶ȡ����ˮλ ѭ������
        {
            if(P_SW2==1)//���ѹ������
            {
                CT2_Z=0;
                HeatFaB=0x00;
                StartMachB=0x00;
            }
            else //��ʾ�����¶ȵ���130�� ��ʼ����
            {
                CT2_Z=1;
                HeatFaB=0xAA;
            }
        }
        else
        {
            CT2_Z=0;
            HeatFaB=0x00;
        }
        ShuiResult=CheckShuiStateB();
        switch(ShuiResult)
        {
        case SHUI_FULL:	  //��ʾ�Ѿ���ˮ
            if(ShuiFlagB==1)
            {
                ShuiTimeB=0;
                ShuiFlagB=0x55;
            }
            if(ShuiFlagB==0x55)
            {
                ShuiTimeB++;
                if(ShuiTimeB>BU_FULL)//��ˮ10��
                {
                    ShuiFlagB=0;
                    ShuiTimeB=0;
                    CT2_S=0;
                }
            }
            break;
        case SHUI_EMPT://��Ҫ��ˮ
            if((ShuiFlagB==0)&&((swWorkStateB==OPT_STATE_INIT)||(swWorkStateB==OPT_STATE_IDLE)))
            {
                CT2_S=1;//������ˮ
                delayNP(BEN_RA);//��ʱ �����źŸ���
                ShuiFlagB=1;
                ShuiTimeB=0;
                DisplayB(0x00,0x00,0x00,0x00);//
            }
            ShuiTime++;
            delayNP(1000);
            if(ShuiTime>BU_2MIN)//��ʾ��ˮ����2����
            {
                CT2_S=0;
                ShuiFlagB=0;
                ShuiTimeB=0;
                DisplayB(0x0D,0x0A,0x02,Lamp);//��ʾ��ˮʧ��
                swWorkStateB=OPT_STATE_ERR1;
            }
            break;
        default:
            break;
        }
    }
}

//void  ExidWorkJob(void)
//{

//}
//void CafeWorkDoJob(void)
//{

//}
void ClearWorkDoJob(unsigned char LED)
{
    unsigned char Lmp=0;
    if(TempFlag==0)
    {
        if(LED==0x01)
        {
            Lmp=0x02;
        }
        else
        {
            Lmp=0x16;
        }
        if(SenTemp<TMELOW)//��ʾ����С��35��
        {
            Display(0x0D,0x0B,0x0C,Lmp);//��ʾLO
        }
        else
        {
            if(SenTemp<100)
            {
                Display(0x0D,(SenTemp+TMP_OFFSET)/10,(SenTemp+TMP_OFFSET)%10,Lmp);//ˢ������
            }
            else
            {
                Display((SenTemp+TMP_OFFSET)/100,(SenTemp+TMP_OFFSET-100)/10,(SenTemp+TMP_OFFSET)%10,Lmp);//ˢ������
            }
        }
    }
    else
    {
        if(LED==0x01)
        {
            Lmp=0x02;
        }
        else
        {
            Lmp=0x1A;
        }
        if(SenTemp<TMELOW)//��ʾ����С��35��
        {
            Display(0x0D,0x0B,0x0C,Lmp);//��ʾLO
        }
        else
        {
            if(SenTemp<100)
            {
                Display(0x0D,(SenTemp+TMP_OFFSET)/10,(SenTemp+TMP_OFFSET)%10,Lmp);//ˢ������
            }
            else
            {
                Display((SenTemp+TMP_OFFSET)/100,(SenTemp+TMP_OFFSET-100)/10,(SenTemp+TMP_OFFSET)%10,Lmp);//ˢ������
            }
        }
    }
}

#define DIS_DLY 1700
void ButAdjPro(unsigned char Type)
{
    static unsigned char  AddBFlag=0;
    static unsigned char  SubBFlag=0;
    static unsigned char  AddDelay=0;
    static unsigned char  SubDelay=0;
    static unsigned char  AddLongKey=0;
    static unsigned char  SubLongKey=0;
    static unsigned char AddUp=0;
    if((CheckKey()==ADD_SW)&&(AddBFlag==0xAA))//��ʾ
    {
        AddDelay++;
        AddUp=0;
        if(AddDelay>SW_L)
        {
            AddDelay=0;
            AddBFlag=0x00;
            if(Type==0x01)
            {
                if(Temp1<STMPMAX)
                {
                    Temp1++;
                    Display(0x0D,Temp1/10,Temp1%10,0x25);//ˢ������
                }
            }
            else
            {
                if(Temp2<STMPMAX)
                {
                    Temp2++;
                    Display(0x0D,Temp2/10,Temp2%10,0x29);//ˢ������
                }
            }
        }
    }
    else if((CheckKey()==ADD_SW)&&(AddBFlag==0x00))
    {
        AddDelay++;
        AddUp=0;
        if(AddDelay>SW_LONGA)
        {
            AddDelay=SW_LONGA+5;
            AddLongKey=0xAA;
            delayNP(DIS_DLY);
            if(Type==0x01)
            {
                if(Temp1<STMPMAX)
                {
                    Temp1++;
                    Display(0x0D,Temp1/10,Temp1%10,0x25);//ˢ������
                }
            }
            else
            {
                if(Temp2<STMPMAX)
                {
                    Temp2++;
                    Display(0x0D,Temp2/10,Temp2%10,0x29);//ˢ������
                }
            }
        }
    }
    else if(CheckKey()!=ADD_SW)
    {
        AddBFlag=0xAA;
        AddDelay=0;
        AddLongKey=0;
    }
    if((CheckKey()==SUB_SW)&&(SubBFlag==0xAA))
    {
        SubDelay++;
        if(SubDelay>SW_L)
        {
            SubDelay=0;
            SubBFlag=0x00;
            if(Type==0x01)
            {
                if(Temp1>STMPMIN)
                {
                    Temp1--;
                    Display(0x0D,Temp1/10,Temp1%10,0x25);//ˢ������
                }
            }
            else
            {
                if(Temp2>STMPMIN)
                {
                    Temp2--;
                    Display(0x0D,Temp2/10,Temp2%10,0x29);//ˢ������
                }
            }
        }
    }
    else if((CheckKey()==SUB_SW)&&(SubBFlag==0x00))
    {
        SubDelay++;
        if(SubDelay>SW_LONGA)
        {
            SubDelay=SW_LONGA+5;
            SubLongKey=0xAA;
            delayNP(DIS_DLY);
            if(Type==0x01)
            {
                if(Temp1>STMPMIN)
                {
                    Temp1--;
                    Display(0x0D,Temp1/10,Temp1%10,0x25);//ˢ������
                }
            }
            else
            {
                if(Temp2>STMPMIN)
                {
                    Temp2--;
                    Display(0x0D,Temp2/10,Temp2%10,0x29);//ˢ������
                }
            }
        }
    }
    else if(CheckKey()!=SUB_SW)
    {
        SubBFlag=0xAA;
        SubDelay=0;
        SubLongKey=0;
    }
}
void ProWorkState(void)
{
    unsigned char RButKey=0;

    switch(swWorkState)
    {
    case OPT_STATE_INIT://��ʼ��
        if(ShuiFlag==0)
        {
            if(TimeCc>(3*ONES))
            {
                TimeCc=0;
                if(SenTemp<TMELOW)//��ʾ����С��35��
                {
                    Display(0x0D,0x0B,0x0C,0x16);//��ʾLO
                }
                else
                {
                    Display(0x0D,Temp1/10,Temp1%10,0x16);//ˢ������
                }
                swWorkState=OPT_STATE_PREH;
            }
        }
        break;
    case OPT_STATE_PREH://Ԥ��
        if(CheckKey()==0)//��ʾ�ɿ�����
        {
            HeatFlag=1;//��ʾ�������ȹ���
            KeyTime=0;
            TempFlag=0;
            if(SenTemp<TMELOW)//��ʾ����С��35��
            {
                Display(0x0D,0x0B,0x0C,0x16);//��ʾLO
            }
            else
            {
                Display(0x0D,Temp1/10,Temp1%10,0x16);//ˢ������
            }
            swWorkState=OPT_STATE_IDLE;
        }
        break;
    case OPT_STATE_IDLE://
        if(ShuiFlag==0)//
        {
            RButKey=CheckKey();
            if(RButKey==START_SW)//��ʾ��������
            {
                FlashTempVal();
                HeadMaxTmp=SenTemp;//ˢ��һ���¶� ���һ����ͼ�⵽��һ���¶�
                KeyFlag=0xAA;
                if(TempFlag==0)
                {
                    swWorkState=OPT_STATE_HEAD1;
                    KeyTime=0;
                    MkCafeTime=0;
                    DelayTime=0;
                    CT1_C=1;
                    delayNP(FA_RA);
                }
                else
                {
                    swWorkState=OPT_STATE_HEAD2;
                    KeyTime=0;
                    MkCafeTime=0;
                    DelayTime=0;
                    CT1_C=1;
                    delayNP(FA_RA);
                }
            }
            else if((RButKey==CLEAR_SW)&&(KeyFlag==0))//��ʾ���
            {

                CT1_C=1;
                delayNP(FA_RA);
                KeyFlag=0xBB;
                KeyTime=0;
                TimeCc=0;
                ClearWorkDoJob(0x01);
                swWorkState=OPT_STATE_CLERA;
            }
            else if(RButKey==TMP1_SW)//��ʾѡ���¶�1
            {
                if(TempFlag==1)
                {
                    TempFlag=0;
                }
                Display(0x0D,Temp1/10,Temp1%10,0x16);//ˢ������
                TimeCc=0;
                KeyTime++;
                if(KeyTime>SW_LONG)//��ʾ���� �����¶�1ֵ
                {
                    swWorkState=OPT_STATE_SHEAD1;
                    KeyTime=0;
                    TimeCc=0;
                    Display(0x0D,Temp1/10,Temp1%10,0x25);//ˢ������
                }
            }
            else if(RButKey==TMP2_SW)//��ʾѡ���¶�2
            {
                if(TempFlag==0)
                {
                    TempFlag=1;
                }
                Display(0x0D,Temp2/10,Temp2%10,0x1A);//ˢ������
                KeyTime++;
                TimeCc=0;
                if(KeyTime>SW_LONG)//��ʾ���� �����¶�1ֵ
                {
                    swWorkState=OPT_STATE_SHEAD2;
                    KeyTime=0;
                    Display(0x0D,Temp2/10,Temp2%10,0x29);//ˢ������
                }
            }
            else if(TimeCc>(0.8*ONES))
            {
                KeyTime=0;
                KeyFlag=0;
                TimeCc=0;
                if(TempFlag==0)
                {
                    if(((Lamp&0x10)>0)&&(HeatFa==0xAA)&&(StartMach==0xAA))
                    {
                        Lamp=0x06;
                    }
                    else
                    {
                        Lamp=0x16;
                    }
                    if(SenTemp<(TMELOW-1))//��ʾ����С��35��
                    {
                        Display(0x0D,0x0B,0x0C,Lamp);//��ʾLO
                    }
                    else if(SenTemp>TMELOW)
                    {
                        if(SenTemp<100)
                        {
                            Display(0x0D,(SenTemp+TMP_OFFSET)/10,(SenTemp+TMP_OFFSET)%10,Lamp);//ˢ������
                        }
                        else
                        {
                            Display((SenTemp+TMP_OFFSET)/100,(SenTemp+TMP_OFFSET-100)/10,(SenTemp+TMP_OFFSET)%10,Lamp);//ˢ������
                        }
                    }
                }
                else
                {
                    if(((Lamp&0x10)>0)&&(HeatFa==0xAA)&&(StartMach==0xAA))
                    {
                        Lamp=0x0A;
                    }
                    else
                    {
                        Lamp=0x1A;
                    }
                    if(SenTemp<(TMELOW-1))//��ʾ����С��35��
                    {
                        Display(0x0D,0x0B,0x0C,0x1A);//��ʾLO
                    }
                    else if(SenTemp>TMELOW)
                    {
                        if(SenTemp<100)
                        {
                            Display(0x0D,(SenTemp+TMP_OFFSET)/10,(SenTemp+TMP_OFFSET)%10,Lamp);//ˢ������
                        }
                        else
                        {
                            Display((SenTemp+TMP_OFFSET)/100,(SenTemp+TMP_OFFSET-100)/10,(SenTemp+TMP_OFFSET)%10,Lamp);//ˢ������
                        }
                    }
                }
            }
        }
        break;
    case OPT_STATE_HEAD1://��ʾ����״̬1
        if(TimeCc>(0.5*ONES))
        {
            TimeCc=0;
            DelayTime++;
            if(SenTemp<TMELOW)//��ʾ����С��35��
            {
                Display(0x0D,0x0B,0x0C,0x14);//��ʾLO
            }
            else
            {
                if(SenTemp<100)
                {
                    Display(0x0D,(SenTemp+TMP_OFFSET)/10,(SenTemp+TMP_OFFSET)%10,0x14);//ˢ������
                }
                else
                {
                    Display((SenTemp+TMP_OFFSET)/100,(SenTemp+TMP_OFFSET-100)/10,(SenTemp+TMP_OFFSET)%10,0x14);//ˢ������
                }
            }
        }
        HeadMaxTmp=SenTemp;
        if(SenTemp>=Temp1)
        {
            KeyTime++;
            if((KeyTime>6)&&(MkCafeTime>(5*ONES)))
            {
                CT1_C=0;
                delayNP(FA_RA);
                KeyTime=0;
                swWorkState=OPT_STATE_IDLE;
            }
        }
        else
        {
            KeyTime=0;
            if(DelayTime>360)//��ʾ��ʱ�˳�
            {
                CT1_C=0;
                delayNP(FA_RA);
                KeyTime=0;
                swWorkState=OPT_STATE_IDLE;
            }
        }
        KeyBut=CheckKey();
        if((KeyBut==START_SW)&&(KeyFlag==0))//��ʾ
        {
            KeyFlag=1;
        }
        else if((KeyFlag==1)&&(KeyBut==0))
        {
            CT1_C=0;
            delayNP(FA_RA);
            KeyTime=0;
            swWorkState=OPT_STATE_IDLE;
        }
        else if(KeyBut==0)
        {
            KeyFlag=0;
        }
        break;
    case OPT_STATE_HEAD2://��ʾ����״̬2
        if(TimeCc>(0.5*ONES))
        {
            TimeCc=0;
            DelayTime++;
            if(SenTemp<TMELOW)//��ʾ����С��35��
            {
                Display(0x0D,0x0B,0x0C,0x18);//��ʾLo
            }
            else
            {
                if(SenTemp<100)
                {
                    Display(0x0D,(SenTemp+TMP_OFFSET)/10,(SenTemp+TMP_OFFSET)%10,0x18);//ˢ������
                }
                else
                {
                    Display((SenTemp+TMP_OFFSET)/100,(SenTemp+TMP_OFFSET-100)/10,(SenTemp+TMP_OFFSET)%10,0x18);//ˢ������
                }
            }
        }
        HeadMaxTmp=SenTemp;
        if(SenTemp>=Temp2)
        {
            KeyTime++;
            if((KeyTime>6)&&(MkCafeTime>(5*ONES)))
            {
                CT1_C=0;
                delayNP(FA_RA);
                swWorkState=OPT_STATE_IDLE;
            }
        }
        else
        {
            KeyTime=0;
            if(DelayTime>360)//��ʾ��ʱ�˳�
            {
                CT1_C=0;
                delayNP(FA_RA);
                KeyTime=0;
                swWorkState=OPT_STATE_IDLE;
            }
        }
        KeyBut=CheckKey();
        if((KeyBut==START_SW)&&(KeyFlag==0))//��ʾ
        {
            KeyFlag=1;
        }
        else if((KeyFlag==1)&&(KeyBut==0))
        {
            CT1_C=0;
            delayNP(FA_RA);
            KeyTime=0;
            swWorkState=OPT_STATE_IDLE;
        }
        else if(KeyBut==0)
        {
            KeyFlag=0;
        }
        break;
    case OPT_STATE_CLERA://��ʾ���״̬
        KeyBut=CheckKey();
        if(KeyBut==CLEAR_SW)//��ʾ���
        {
            KeyTime++;
            if(KeyTime>SW_LONG)//��ʾ�����������
            {
                KeyTime=0;
                KeyFlag=0xBB;
                HeatFlag=0;//�ر�������ȹ���
                MkCafeTime=0;
                CT1_S=1;//������ˮ
                swWorkState=OPT_STATE_CLERB;
                if(TempFlag==0)
                {
                    Display(0x03,0x03,0x03,0x16);//��ʾLO
                }
                else
                {
                    Display(0x03,0x03,0x03,0x1A);//��ʾLO
                }
            }
        }
        else
        {
            if(TimeCc>(3*ONES))
            {
                swWorkState=OPT_STATE_IDLE;
                ClearWorkDoJob(0x02);
                CT1_C=0;
                delayNP(FA_RA);
            }
        }
        if(((KeyBut==START_SW)||(KeyBut==CLEAR_SW))&&(KeyFlag==0))//��ʾ
        {
            KeyFlag=1;
        }
        else if((KeyFlag==1)&&(KeyBut==0))
        {
            swWorkState=OPT_STATE_IDLE;
            ClearWorkDoJob(0x02);
            CT1_C=0;
            delayNP(FA_RA);
        }
        else if(KeyBut==0)
        {
            KeyFlag=0;
        }
        break;
    case OPT_STATE_CLERB://��ʾ�������
        //�������ʱ��
        if((CheckKey()==CLEAR_SW)&&(KeyFlag==0)&&(TimeCc>(2*ONES)))//��ʾ���
        {
            swWorkState=OPT_STATE_IDLE;
            ClearWorkDoJob(0x02);
            CT1_C=0;
            CT1_S=0;//�رղ�ˮ
            delayNP(FA_RA);
            HeatFlag=1;
            KeyFlag=0xBB;
        }
        else if(CheckKey()==0)
        {
            KeyFlag=0;
            TimeCc=0;
        }
        if(MkCafeTime>(10*ONES))
        {
            CT1_S=0;//������ˮ
            MkCafeTime=0;
        }
        break;
    case OPT_STATE_SHEAD1://�����¶ȼ���ֵ1
        ButAdjPro(0x01);
        if(CheckKey()==TMP1_SW)//��ʾ
        {
            KeyTime++;
            if(KeyTime>SW_LONGA)//��ʾ���� �����¶�1ֵ
            {
                //��������
                WriteTmp1Eeprom();
                swWorkState=OPT_STATE_IDLE;
                HeatFlag=1;
                KeyTime=0;
                Display(0x0D,Temp1/10,Temp1%10,0x16);//ˢ������
                TimeCc=0;
            }
        }
        else
        {
            if(TimeCc>(0.8*ONES))
            {
                TimeCc=0;
                if(LedOnOFF==0x21)
                {
                    LedOnOFF=0x25;
                }
                else
                {
                    LedOnOFF=0x21;
                }
                Display(0x0D,Temp1/10,Temp1%10,LedOnOFF);
            }
        }
        break;
    case OPT_STATE_SHEAD2://�����¶ȼ���ֵ2
        ButAdjPro(0x02);
        if(CheckKey()==TMP2_SW)//��ʾ
        {
            KeyTime++;
            if(KeyTime>SW_LONGA)//��ʾ���� �����¶�1ֵ
            {
                //��������
                WriteTmp2Eeprom();
                swWorkState=OPT_STATE_IDLE;
                HeatFlag=1;
                KeyTime=0;
                Display(0x0D,Temp2/10,Temp2%10,0x1A);//ˢ������
                TimeCc=0;
            }
        }
        else
        {
            if(TimeCc>(0.8*ONES))
            {
                TimeCc=0;
                if(LedOnOFF==0x21)
                {
                    LedOnOFF=0x29;
                }
                else
                {
                    LedOnOFF=0x21;
                }
                Display(0x0D,Temp2/10,Temp2%10,LedOnOFF);
            }
        }
        break;
    case OPT_STATE_ERR1://���ɻָ����� ��ˮʧ�� ����2����
        HeatFlag=0;
        CT1_C=0;
        CT1_Z=0;
        CT1_S=0;
        break;
    case OPT_STATE_SLEEP://�ػ�״̬
        HeatFlag=0;
        CT1_C=0;
        CT1_Z=0;
        CT1_S=0;
        break;
    default:
        swWorkState=OPT_STATE_INIT;
        break;

    }
}
void ClearBWorkDoJob(unsigned char LED)
{
    unsigned char Lmp=0;
    if(TempFlagB==0)
    {
        if(LED==0x01)
        {
            Lmp=0x02;
        }
        else
        {
            Lmp=0x16;
        }
        if(SenTempB<TMELOW)//��ʾ����С��35��
        {
            DisplayB(0x0D,0x0B,0x0C,Lmp);//��ʾLO
        }
        else
        {
            if(SenTempB<100)
            {
                DisplayB(0x0D,(SenTempB+TMP_OFFSET)/10,(SenTempB+TMP_OFFSET)%10,Lmp);//ˢ������
            }
            else
            {
                DisplayB((SenTempB+TMP_OFFSET)/100,(SenTempB+TMP_OFFSET-100)/10,(SenTempB+TMP_OFFSET)%10,Lmp);//ˢ������
            }
        }
    }
    else
    {
        if(LED==0x01)
        {
            Lmp=0x02;
        }
        else
        {
            Lmp=0x1A;
        }
        if(SenTempB<TMELOW)//��ʾ����С��35��
        {
            DisplayB(0x0D,0x0B,0x0C,Lmp);//��ʾLO
        }
        else
        {
            if(SenTempB<100)
            {
                DisplayB(0x0D,(SenTempB+TMP_OFFSET)/10,(SenTempB+TMP_OFFSET)%10,Lmp);//ˢ������
            }
            else
            {
                DisplayB((SenTempB+TMP_OFFSET)/100,(SenTempB+TMP_OFFSET-100)/10,(SenTempB+TMP_OFFSET)%10,Lmp);//ˢ������
            }
        }
    }
}
void ButAdjProB(unsigned char Type)
{
    static unsigned char  AddBFlag=0;
    static unsigned char  SubBFlag=0;
    static unsigned char  AddDelay=0;
    static unsigned char  SubDelay=0;
    static unsigned char  AddLongKey=0;
    static unsigned char  SubLongKey=0;
    static unsigned char AddUp=0;
    if((CheckKeyB()==ADD_SW)&&(AddBFlag==0xAA))//��ʾ
    {
        AddDelay++;
        AddUp=0;
        if(AddDelay>SW_L)
        {
            AddDelay=0;
            AddBFlag=0x00;
            if(Type==0x01)
            {
                if(Temp1B<STMPMAX)
                {
                    Temp1B++;
                    DisplayB(0x0D,Temp1B/10,Temp1B%10,0x25);//ˢ������
                }
            }
            else
            {
                if(Temp2B<STMPMAX)
                {
                    Temp2B++;
                    DisplayB(0x0D,Temp2B/10,Temp2B%10,0x29);//ˢ������
                }
            }
        }
    }
    else if((CheckKeyB()==ADD_SW)&&(AddBFlag==0x00))
    {
        AddDelay++;
        AddUp=0;
        if(AddDelay>SW_LONGA)
        {
            AddDelay=SW_LONGA+5;
            AddLongKey=0xAA;
            delayNP(DIS_DLY);
            if(Type==0x01)
            {
                if(Temp1B<STMPMAX)
                {
                    Temp1B++;
                    DisplayB(0x0D,Temp1B/10,Temp1B%10,0x25);//ˢ������
                }
            }
            else
            {
                if(Temp2B<STMPMAX)
                {
                    Temp2B++;
                    DisplayB(0x0D,Temp2B/10,Temp2B%10,0x29);//ˢ������
                }
            }
        }
    }
    else if(CheckKeyB()!=ADD_SW)
    {
        AddBFlag=0xAA;
        AddDelay=0;
        AddLongKey=0;
    }
    if((CheckKeyB()==SUB_SW)&&(SubBFlag==0xAA))
    {
        SubDelay++;
        if(SubDelay>SW_L)
        {
            SubDelay=0;
            SubBFlag=0x00;
            if(Type==0x01)
            {
                if(Temp1B>STMPMIN)
                {
                    Temp1B--;
                    DisplayB(0x0D,Temp1B/10,Temp1B%10,0x25);//ˢ������
                }
            }
            else
            {
                if(Temp2B>STMPMIN)
                {
                    Temp2B--;
                    DisplayB(0x0D,Temp2B/10,Temp2B%10,0x29);//ˢ������
                }
            }
        }
    }
    else if((CheckKeyB()==SUB_SW)&&(SubBFlag==0x00))
    {
        SubDelay++;
        if(SubDelay>SW_LONGA)
        {
            SubDelay=SW_LONGA+5;
            SubLongKey=0xAA;
            delayNP(DIS_DLY);
            if(Type==0x01)
            {
                if(Temp1B>STMPMIN)
                {
                    Temp1B--;
                    DisplayB(0x0D,Temp1B/10,Temp1B%10,0x25);//ˢ������
                }
            }
            else
            {
                if(Temp2B>STMPMIN)
                {
                    Temp2B--;
                    DisplayB(0x0D,Temp2B/10,Temp2B%10,0x29);//ˢ������
                }
            }
        }
    }
    else if(CheckKeyB()!=SUB_SW)
    {
        SubBFlag=0xAA;
        SubDelay=0;
        SubLongKey=0;
    }
}




void ProWorkStateB(void)
{
    unsigned char RButKey=0;

    switch(swWorkStateB)
    {
    case OPT_STATE_INIT://��ʼ��
        if(ShuiFlagB==0)
        {
            if(TimeCcB>(3*ONES))
            {
                TimeCcB=0;
                if(SenTempB<TMELOW)//��ʾ����С��35��
                {
                    DisplayB(0x0D,0x0B,0x0C,0x16);//��ʾLO
                }
                else
                {
                    DisplayB(0x0D,Temp1B/10,Temp1B%10,0x16);//ˢ������
                }
                swWorkStateB=OPT_STATE_PREH;
            }
        }
        break;
    case OPT_STATE_PREH://Ԥ��
        if(CheckKeyB()==0)//��ʾ�ɿ�����
        {
            HeatFlagB=1;//��ʾ�������ȹ���
            KeyTimeB=0;
            TempFlagB=0;
            if(SenTempB<TMELOW)//��ʾ����С��35��
            {
                DisplayB(0x0D,0x0B,0x0C,0x16);//��ʾLO
            }
            else
            {
                DisplayB(0x0D,Temp1B/10,Temp1B%10,0x16);//ˢ������
            }
            swWorkStateB=OPT_STATE_IDLE;
        }
        break;
    case OPT_STATE_IDLE://
        if(ShuiFlagB==0)//��ʾ���ڲ�ˮ
        {
            RButKey=CheckKeyB();
            if(RButKey==START_SW)//��ʾ��������
            {
                FlashTempValB();
                HeadMaxTmpB=SenTempB;//ˢ��һ���¶� ���һ����ͼ�⵽��һ���¶�
                KeyFlagB=0xAA;
                if(TempFlagB==0)
                {
                    swWorkStateB=OPT_STATE_HEAD1;
                    KeyTimeB=0;
                    MkCafeTimeB=0;
                    DelayTimeB=0;
                    CT2_C=1;
                    delayNP(FA_RA);
                }
                else
                {
                    swWorkStateB=OPT_STATE_HEAD2;
                    KeyTimeB=0;
                    MkCafeTimeB=0;
                    DelayTimeB=0;
                    CT2_C=1;
                    delayNP(FA_RA);
                }
            }
            else if((RButKey==CLEAR_SW)&&(KeyFlagB==0))//��ʾ���
            {

                CT2_C=1;
                delayNP(FA_RA);
                KeyFlagB=0xBB;
                KeyTimeB=0;
                TimeCcB=0;
                ClearBWorkDoJob(0x01);
                swWorkStateB=OPT_STATE_CLERA;

            }
            else if(RButKey==TMP1_SW)//��ʾѡ���¶�1
            {

                if(TempFlagB==1)
                {
                    TempFlagB=0;
                }
                DisplayB(0x0D,Temp1B/10,Temp1B%10,0x16);//ˢ������
                TimeCcB=0;
                KeyTimeB++;
                if(KeyTimeB>SW_LONG)//��ʾ���� �����¶�1ֵ
                {
                    swWorkStateB=OPT_STATE_SHEAD1;
                    KeyTimeB=0;
                    TimeCcB=0;
                    DisplayB(0x0D,Temp1B/10,Temp1B%10,0x25);//ˢ������
                }
            }
            else if(RButKey==TMP2_SW)//��ʾѡ���¶�2
            {
                if(TempFlagB==0)
                {
                    TempFlagB=1;
                }
                DisplayB(0x0D,Temp2B/10,Temp2B%10,0x1A);//ˢ������
                KeyTimeB++;
                TimeCcB=0;
                if(KeyTimeB>SW_LONG)//��ʾ���� �����¶�1ֵ
                {
                    swWorkStateB=OPT_STATE_SHEAD2;
                    KeyTimeB=0;
                    DisplayB(0x0D,Temp2B/10,Temp2B%10,0x29);//ˢ������
                }
            }
            else if(TimeCcB>(0.8*ONES))
            {
                KeyTimeB=0;
                KeyFlagB=0;
                TimeCcB=0;
                if(TempFlagB==0)
                {
                    if(((LampB&0x10)>0)&&(HeatFaB==0xAA)&&(StartMachB==0xAA))
                    {
                        LampB=0x06;
                    }
                    else
                    {
                        LampB=0x16;
                    }
                    if(SenTempB<(TMELOW-1))//��ʾ����С��35��
                    {
                        DisplayB(0x0D,0x0B,0x0C,LampB);//��ʾLO
                    }
                    else if(SenTempB>TMELOW)
                    {
                        if(SenTempB<100)
                        {
                            DisplayB(0x0D,(SenTempB+TMP_OFFSET)/10,(SenTempB+TMP_OFFSET)%10,LampB);//ˢ������
                        }
                        else
                        {
                            DisplayB((SenTempB+TMP_OFFSET)/100,(SenTempB+TMP_OFFSET-100)/10,(SenTempB+TMP_OFFSET)%10,LampB);//ˢ������
                        }
                    }
                }
                else
                {
                    if(((LampB&0x10)>0)&&(HeatFaB==0xAA)&&(StartMachB==0xAA))
                    {
                        LampB=0x0A;
                    }
                    else
                    {
                        LampB=0x1A;
                    }
                    if(SenTempB<(TMELOW-1))//��ʾ����С��35��
                    {
                        DisplayB(0x0D,0x0B,0x0C,0x1A);//��ʾLO
                    }
                    else if(SenTempB>TMELOW)
                    {
                        if(SenTempB<100)
                        {
                            DisplayB(0x0D,(SenTempB+TMP_OFFSET)/10,(SenTempB+TMP_OFFSET)%10,LampB);//ˢ������
                        }
                        else
                        {
                            DisplayB((SenTempB+TMP_OFFSET)/100,(SenTempB+TMP_OFFSET-100)/10,(SenTempB+TMP_OFFSET)%10,LampB);//ˢ������
                        }
                    }
                }
            }
        }
        break;
    case OPT_STATE_HEAD1://��ʾ����״̬1
        if(TimeCcB>(0.5*ONES))
        {
            TimeCcB=0;
            DelayTimeB++;
            if(SenTempB<TMELOW)//��ʾ����С��35��
            {
                DisplayB(0x0D,0x0B,0x0C,0x14);//��ʾLO
            }
            else
            {
                if(SenTempB<100)
                {
                    DisplayB(0x0D,(SenTempB+TMP_OFFSET)/10,(SenTempB+TMP_OFFSET)%10,0x14);//ˢ������
                }
                else
                {
                    DisplayB((SenTempB+TMP_OFFSET)/100,(SenTempB+TMP_OFFSET-100)/10,(SenTempB+TMP_OFFSET)%10,0x14);//ˢ������
                }
            }
        }
        HeadMaxTmpB=SenTempB;
        if(SenTempB>=Temp1B)
        {
            KeyTimeB++;
            if((KeyTimeB>6)&&(MkCafeTimeB>(5*ONES)))
            {
                CT2_C=0;
                delayNP(FA_RA);
                KeyTimeB=0;
                swWorkStateB=OPT_STATE_IDLE;
            }
        }
        else
        {
            KeyTimeB=0;
            if(DelayTimeB>360)//��ʾ��ʱ�˳�
            {
                CT2_C=0;
                delayNP(FA_RA);
                KeyTimeB=0;
                swWorkStateB=OPT_STATE_IDLE;
            }
        }
        KeyBut=CheckKeyB();
        if((KeyBut==START_SW)&&(KeyFlagB==0))//��ʾ
        {
            KeyFlagB=1;
        }
        else if((KeyFlagB==1)&&(KeyBut==0))
        {
            CT2_C=0;
            delayNP(FA_RA);
            KeyTimeB=0;
            swWorkStateB=OPT_STATE_IDLE;
        }
        else if(KeyBut==0)
        {
            KeyFlagB=0;
        }
        break;
    case OPT_STATE_HEAD2://��ʾ����״̬2
        if(TimeCcB>(0.5*ONES))
        {
            TimeCcB=0;
            DelayTimeB++;
            if(SenTempB<TMELOW)//��ʾ����С��35��
            {
                DisplayB(0x0D,0x0B,0x0C,0x18);//��ʾLo
            }
            else
            {
                if(SenTempB<100)
                {
                    DisplayB(0x0D,(SenTempB+TMP_OFFSET)/10,(SenTempB+TMP_OFFSET)%10,0x18);//ˢ������
                }
                else
                {
                    DisplayB((SenTempB+TMP_OFFSET)/100,(SenTempB+TMP_OFFSET-100)/10,(SenTempB+TMP_OFFSET)%10,0x18);//ˢ������
                }
            }
        }
        HeadMaxTmp=SenTempB;
        if(SenTempB>=Temp2B)
        {
            KeyTimeB++;
            if((KeyTimeB>6)&&(MkCafeTimeB>(5*ONES)))
            {
                CT2_C=0;
                delayNP(FA_RA);
                swWorkStateB=OPT_STATE_IDLE;
            }
        }
        else
        {
            KeyTimeB=0;
            if(DelayTimeB>360)//��ʾ��ʱ�˳�
            {
                CT2_C=0;
                delayNP(FA_RA);
                KeyTimeB=0;
                swWorkStateB=OPT_STATE_IDLE;
            }
        }
        KeyBut=CheckKeyB();
        if((KeyBut==START_SW)&&(KeyFlagB==0))//��ʾ
        {
            KeyFlagB=1;
        }
        else if((KeyFlagB==1)&&(KeyBut==0))
        {
            CT2_C=0;
            delayNP(FA_RA);
            KeyTimeB=0;
            swWorkStateB=OPT_STATE_IDLE;
        }
        else if(KeyBut==0)
        {
            KeyFlagB=0;
        }
        break;
    case OPT_STATE_CLERA://��ʾ���״̬
        KeyBut=CheckKeyB();
        if(KeyBut==CLEAR_SW)//��ʾ���
        {
            KeyTimeB++;
            if(KeyTimeB>SW_LONG)//��ʾ�����������
            {
                KeyTimeB=0;
                KeyFlagB=0xBB;
                HeatFlagB=0;//�ر�������ȹ���
                MkCafeTimeB=0;
                CT2_S=1;//������ˮ
                swWorkStateB=OPT_STATE_CLERB;
                if(TempFlagB==0)
                {
                    DisplayB(0x03,0x03,0x03,0x16);//��ʾLO
                }
                else
                {
                    DisplayB(0x03,0x03,0x03,0x1A);//��ʾLO
                }
            }
        }
        else
        {
            if(TimeCcB>(3*ONES))
            {
                swWorkStateB=OPT_STATE_IDLE;
                ClearBWorkDoJob(0x02);
                CT2_C=0;
                delayNP(FA_RA);
            }
        }
        if(((KeyBut==START_SW)||(KeyBut==CLEAR_SW))&&(KeyFlagB==0))//��ʾ
        {
            KeyFlagB=1;
        }
        else if((KeyFlagB==1)&&(KeyBut==0))
        {
            swWorkStateB=OPT_STATE_IDLE;
            ClearBWorkDoJob(0x02);
            CT2_C=0;
            delayNP(FA_RA);
        }
        else if(KeyBut==0)
        {
            KeyFlagB=0;
        }
        break;
    case OPT_STATE_CLERB://��ʾ�������
        //�������ʱ��
        if((CheckKeyB()==CLEAR_SW)&&(KeyFlagB==0)&&(TimeCcB>(2*ONES)))//��ʾ���
        {
            swWorkStateB=OPT_STATE_IDLE;
            ClearBWorkDoJob(0x02);
            CT2_C=0;
            CT2_S=0;//�رղ�ˮ
            delayNP(FA_RA);
            HeatFlagB=1;
            KeyFlagB=0xBB;
        }
        else if(CheckKeyB()==0)
        {
            KeyFlagB=0;
            TimeCcB=0;
        }
        if(MkCafeTimeB>(10*ONES))
        {
            CT2_S=0;//������ˮ
            MkCafeTimeB=0;
        }
        break;
    case OPT_STATE_SHEAD1://�����¶ȼ���ֵ1
        ButAdjProB(0x01);
        if(CheckKeyB()==TMP1_SW)//��ʾ
        {
            KeyTimeB++;
            if(KeyTimeB>SW_LONGA)//��ʾ���� �����¶�1ֵ
            {
                //��������
                WriteTmp1BEeprom();
                swWorkStateB=OPT_STATE_IDLE;
                HeatFlagB=1;
                KeyTimeB=0;
                DisplayB(0x0D,Temp1B/10,Temp1B%10,0x16);//ˢ������
                TimeCcB=0;
            }
        }
        else
        {
            if(TimeCcB>(0.8*ONES))
            {
                TimeCcB=0;
                if(LedOnOFFB==0x21)
                {
                    LedOnOFFB=0x25;
                }
                else
                {
                    LedOnOFFB=0x21;
                }
                DisplayB(0x0D,Temp1B/10,Temp1B%10,LedOnOFFB);
            }

        }
        break;
    case OPT_STATE_SHEAD2://�����¶ȼ���ֵ2
        ButAdjProB(0x02);
        if(CheckKeyB()==TMP2_SW)//��ʾ
        {
            KeyTimeB++;
            if(KeyTimeB>SW_LONGA)//��ʾ���� �����¶�1ֵ
            {
                //��������
                WriteTmp2BEeprom();
                swWorkStateB=OPT_STATE_IDLE;
                HeatFlagB=1;
                KeyTimeB=0;
                DisplayB(0x0D,Temp2B/10,Temp2B%10,0x1A);//ˢ������
                TimeCcB=0;
            }
        }
        else
        {
            if(TimeCcB>(0.8*ONES))
            {
                TimeCcB=0;
                if(LedOnOFFB==0x21)
                {
                    LedOnOFFB=0x29;
                }
                else
                {
                    LedOnOFFB=0x21;
                }
                DisplayB(0x0D,Temp2B/10,Temp2B%10,LedOnOFFB);
            }
        }
        break;
    case OPT_STATE_ERR1://���ɻָ����� ��ˮʧ�� ����2����
		HeatFlagB=0;
        CT2_C=0;
        CT2_Z=0;
        CT2_S=0;
        break;
    case OPT_STATE_SLEEP://�ػ�״̬
        HeatFlagB=0;
        CT2_C=0;
        CT2_Z=0;
        CT2_S=0;
        break;
    default:
        swWorkStateB=OPT_STATE_INIT;
        break;

    }
}

void ShuiXiangErr(void)
{
    static unsigned char xdata ErrFlag=0;
    static unsigned char xdata ErrDou=0;
    if(CheckShuiXiang()>0)
    {
        ErrDou++;
        if(ErrDou>5)
        {
            ErrFlag=0xAA;
            if(Ledflag)
            {
                Display(0x0D,0x0A,0x03,Lamp);//��ʾ��ˮʧ��
            }
            if(LedBflag)
            {
                DisplayB(0x0D,0x0A,0x03,Lamp);//��ʾ��ˮʧ��
            }
            swWorkStateB=OPT_STATE_ERR1;
            swWorkState=OPT_STATE_ERR1;
        }
    }
    else
    {
        ErrDou=0;
        if(ErrFlag==0xAA)
        {
            ErrFlag=0x00;
            HeatFlag=1;
            HeatFlagB=1;
            swWorkStateB=OPT_STATE_IDLE;
            swWorkState=OPT_STATE_IDLE;
        }
    }
}
void CheckPowerOff(void)
{
    static unsigned char xdata Pwdelay1=0;
    static unsigned char xdata Pwdelay2=0;
    static unsigned char xdata PowerS1=0;
    static unsigned char xdata PowerS2=0;
    PW1=1;
    if(PW1==1)
    {
        Pwdelay1++;
        if(Pwdelay1>10)
        {
            PowerS1=0xAA;
            Display(0x0D,0x0D,0x0D,0x00);//�ر���ʾ��
            swWorkState=OPT_STATE_SLEEP;
            Ledflag=0;
        }
    }
    else
    {
        Pwdelay1=0;
        if(PowerS1==0xAA)
        {
            Ledflag=1;
            PowerS1=0x00;
            HeatFlag=1;
            swWorkState=OPT_STATE_IDLE;
        }
    }
    PW2=1;
    if(PW2==1)
    {
        Pwdelay2++;
        if(Pwdelay2>10)
        {
            PowerS2=0xAA;
            DisplayB(0x0D,0x0D,0x0D,0x00);//�ر���ʾ��
            swWorkStateB=OPT_STATE_SLEEP;
            LedBflag=0;
        }
    }
    else
    {
        Pwdelay2=0;
        if(PowerS2==0xAA)
        {

            PowerS2=0x00;
            HeatFlagB=1;
            LedBflag=1;
            swWorkStateB=OPT_STATE_IDLE;
        }
    }

}
extern void TIMER0_initialize(void);
void main(void)
{
    LVC=0x20;
    GPIO_Init();
    TIMER0_initialize();
    WDTR_Init();
    ADC_Init();
    EA = 1;
    SDA=1;
    ReadTmp1Eeprom();
    ReadTmp2Eeprom();
    ReadTmp1BEeprom();
    ReadTmp2BEeprom();
    Display(0x0E,0x0E,0x0E,0xFF); //0x10 START  0x02 CLEAR 0x04 TMP1 0x08 TMP2
    DisplayB(0x0E,0x0E,0x0E,0xFF);
    while(1)
    {
        TemControlAuto();
        TemControlAutoB();
        ProWorkState();
        ProWorkStateB();
        distime++;
        if(distime>3)
        {
            Display(sda1,sda2,sda3,sda4);
            DisplayB(sdaB1,sdaB2,sdaB3,sdaB4);
            distime=0;
        }
		ShuiXiangErr();
        CheckPowerOff();
        WDTR_CountClear();
        delayNP(20);
    }
}
