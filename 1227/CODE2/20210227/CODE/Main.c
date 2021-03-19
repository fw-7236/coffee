#include "OB38R08T1.h"
#include "EEPROM.h"
#include "Timer.h"
#include "wdt.h"
#define  ONES          445///循环时间为1秒 440
//*******************************************************************

typedef enum _WORKState
{
    OPT_STATE_INIT     =     0,//0x00 表示机器初始化状态 全部数码管显示888 1秒
    OPT_STATE_PREH     =     1,//0x01 表示带机预热阶段 检测水位 超过范围值显示NG 显示0x000 进入错误状态 0x09
    OPT_STATE_IDLE     =     2,//等待用户操作状态
    OPT_STATE_HEAD1    =     3,//表示加热状态1
    OPT_STATE_HEAD2    =     4,//表示加热状态2
    OPT_STATE_CLERA    =     5,//表示清洁状态
    OPT_STATE_CLERB    =     6,//表示清空煲体
    OPT_STATE_SHEAD1   =     7,//表示温度设置1
    OPT_STATE_SHEAD2   =     8,//表示温度设置2
    OPT_STATE_ERR1     =     9,//补水状态
    OPT_STATE_SLEEP    =     10,//关机状态
    OPT_STATE_ERR2     =    11,//补水超过2分钟
} WorkState;

WorkState  swWorkState=OPT_STATE_INIT;//定义状态机
WorkState  swWorkStateB=OPT_STATE_INIT;//定义状态机

bit  HeatFlag;//加热标记值
bit  TempFlag;//温度状态

bit  HeatFlagB;//加热标记值
bit  TempFlagB;//温度状态

unsigned char xdata SenTemp=0;
unsigned int xdata  ShuiTime=0;//补水时间计数

unsigned char xdata SenTempB=0;
unsigned int xdata  ShuiTimeB=0;//补水时间计数

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
unsigned char Temp1=40;//记录T1温度值
unsigned char Temp2=60;//记录T2温度值

unsigned char Temp1B=40;//记录T1温度值
unsigned char Temp2B=60;//记录T2温度值

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


//存储
#define DFE_CAFE    25
#define DFE_HT1     5
#define DFE_HT2     5

#define MAX_ZT      300  //蒸汽时间
#define MAX_HT1     30
#define MAX_HT2     30
#define MAX_CFE     60
#define MAX_POWER   900  //15分钟关机


#define TMP1_VAL    0  //
#define TMP2_VAL     1  //

#define TMP1B_VAL    0  //
#define TMP2B_VAL     1  //

#define DAFETMP  60    //恢复到默认温度

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

#define  ADD_SW     0x06 //表示加1按键
#define  SUB_SW     0x01 //表示减1按键
#define  CLEAR_SW   0x02 //表示清洁按键
#define  TMP1_SW    0x03 //表示温度1按键
#define  TMP2_SW    0x04 //表示温度1按键
#define  START_SW   0x05 //表示启动按键

#define  START_LED  0x10
//**********************************************************************
#define  CT1_S         P1_7   //水泵电机控制1 
#define  CT1_Z         P1_6   //蒸汽发热控制或者加热可控硅控制	 PWM0
#define  CT1_C         P3_1  //电磁阀开关控制

#define  LEVE1         P0_7 //ADC7  水位控针输入
#define  SCR1          P0_6 //固态继电器控制 备用
#define  P_SW          P0_5 //压力开关

#define  CS             P1_5 //MAX6675 IC A
#define  SCK            P0_0
#define  SDA            P0_1

#define  SPI_STB        P1_2
#define  SPI_CLK        P1_3
#define  SPI_DA         P1_4
//***************************************************************************
#define  CT2_S          P1_0   //水泵电机控制   
#define  CT2_Z          P1_1   //蒸汽发热控制或者加热可控硅控制	 
#define  CT2_C          P3_0  //电磁阀开关控制

#define  LEVE2          P0_4 //ADC0  水位控针输入
#define  SCR2           P0_3 //固态继电器控制 备用
#define  P_SW2          P0_2 //压力开关

#define  CS2             P2_3//MAX6675 TWO

#define  SPI_STB2        P3_5
#define  SPI_CLK2        P3_4
#define  SPI_DA2         P3_3

#define  PW1             P2_6 //电源开关1
#define  PW2             P2_5 //电源开关2
//**********************************************************************
//*******************************************************************
// 00 表示双向IO
// 01 表示推挽输出
// 10 表示高阻输入
// 11 表示开漏
void GPIO_Init()
{
    P0 = 0x00;
    P1 = 0x00;
    P2 = 0x30;
    P3 = 0x00;
//P0.0-->SCK  MAX6675 IIC
//P0.1-->SDA  MAX6675 IIC
//P0.2-->P_SW2  压力开关2
//P0.3-->SCR2	备用
//P0.4-->LEVE2  水量检测2
//P0.5-->P_SW   压力开关
//P0.6-->SCR1   固态继电器
//P0.7-->LEVE1  水位检测输入
//      7  6  5  4  3  2  1  0
//P0M1  0  0  0	 0	0  0  0	 0
//P0M0  0  0  0	 0	0  0  1	 1
    P0M1 = 0x90;
    P0M0 = 0x01;
//      7  6  5  4  3  2  1  0
//P1M1  0  0  1	 1	0  1  1	 0
//P1M0  1  1  0	 0	1  0  0	 0
//P1.0-->CT2_S  水泵电机控制
//P1.1-->CT2_Z  蒸汽发热控制或者加热可控硅控制
//P1.2-->SPI_STB
//P1.3-->SPI_CLK
//P1.4-->SPI_DA
//P1.5-->CS       MAX6675片选
//P1.6-->CT1_Z    固态继电器
//P1.7-->CT1_S    //水泵电机控制1
    P1M1 = 0x00;
    P1M0 = 0xEF;
//P2.3 输出 P2.5 输出 P2.6输入
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
//P3.0--->CT2_C   电磁阀开关控制2
//P3.1--->CT1_C   电磁阀开关控制1
//P3.2--->AD1     备用
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
//功能：向TM1628发送8位数据
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
unsigned char read_8bit(void)//下降沿输出
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
//定义数据                                                                          0x0A  0x0b                0x0f 0x10
//定义数据
unsigned char const CODE[][2] =
//   0           1         2            3          4            5           6
{   {0x3F,0x00},{0x06,0x00},{0x5b,0x00},{0x4f,0x10},{0x66,0x00},{0x6d,0x00},{0x7D,0x00},
//   7           8     		 9	   E          L            o	  空 0x0D	  0x0E
    {0x07,0x00},{0x7f,0x00},{0x6f,0x00},{0x79,0x00},{0x38,0x00},{0x5c,0x00},{0x00,0x00},{0xFF,0x00}
}; //4位8段模式下，共阴数码管0-9的编码


/*****************发送指令*************************************/
void command(unsigned char com)
{
    SPI_STB=1;
    delayns(200);
    SPI_STB=0;
    send_8bit(com);
}
//*******************************************************************************************************
/**************************************************************/
//功能：在1-4位数码管显示数字0-3
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
    command(0x01); //设置显示模式，4位13段模式
    command(0x40); //设置数据命令,采用地址自动加1模式
    command(0xc0); //设置显示地址，从00H开始
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
    command(0x8E); //显示控制命令，打开显示并设置为最亮
    SPI_STB=1;
    sda1=da1;
    sda2=da2;
    sda3=da3;
    sda4=da4;
}
unsigned char CheckKey(void)//读取按键值
{
    unsigned char  Key1,Key2,Key3,Key4,Key5;
    unsigned char KeyVal=0;
    command(0x42);//设置读取按键指令
    Key1=read_8bit(); //
    Key2=read_8bit();
    Key3=read_8bit();//SW2A
    Key4=read_8bit();//SW1A
    Key5=read_8bit();//SW1B  SW2B
    KeyVal=0x00;
    if((Key1&0x01)>0)//表示按键1
    {
        KeyVal=0x01;
    }
    else if((Key1&0x08)>0)//表示按键2
    {
        KeyVal=0x02;
    }
    else if((Key2&0x01)>0)//表示按键1
    {
        KeyVal=0x03;
    }
    else if((Key2&0x08)>0)//表示按键2
    {
        KeyVal=0x04;
    }
    else if((Key3&0x01)>0)//表示按键1
    {
        KeyVal=0x05;
    }
    else if((Key3&0x08)>0)//表示按键2
    {
        KeyVal=0x06;
    }
    return KeyVal;
}
/*****************发送指令*************************************/
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
unsigned char read_8bitB(void)//下降沿输出
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
    commandB(0x01); //设置显示模式，4位13段模式
    commandB(0x40); //设置数据命令,采用地址自动加1模式
    commandB(0xc0); //设置显示地址，从00H开始
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
    commandB(0x8E); //显示控制命令，打开显示并设置为最亮
    SPI_STB2=1;
    sdaB1=da1;
    sdaB2=da2;
    sdaB3=da3;
    sdaB4=da4;
}
//******************************************************************************************************
unsigned char CheckKeyB(void)//读取按键值
{
    unsigned char  Key1,Key2,Key3,Key4,Key5;
    unsigned char KeyVal=0;
    commandB(0x42);//设置读取按键指令
    Key1=read_8bitB(); //
    Key2=read_8bitB();
    Key3=read_8bitB();//SW2A
    Key4=read_8bitB();//SW1A
    Key5=read_8bitB();//SW1B  SW2B
    KeyVal=0x00;
    if((Key1&0x01)>0)//表示按键1
    {
        KeyVal=0x01;
    }
    else if((Key1&0x08)>0)//表示按键2
    {
        KeyVal=0x02;
    }
    else if((Key2&0x01)>0)//表示按键1
    {
        KeyVal=0x03;
    }
    else if((Key2&0x08)>0)//表示按键2
    {
        KeyVal=0x04;
    }
    else if((Key3&0x01)>0)//表示按键1
    {
        KeyVal=0x05;
    }
    else if((Key3&0x08)>0)//表示按键2
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
    if(Temp1>STMPMAX)//当读取值不在范围值 恢复默认值
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
    if(Temp2>STMPMAX)//当读取值不在范围值 恢复默认值
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
    if(Temp1B>STMPMAX)//当读取值不在范围值 恢复默认值
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
    if(Temp2B>STMPMAX)//当读取值不在范围值 恢复默认值
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
//新特服龙线不干扰 金属屏蔽线易干扰
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
            Display(0x0D,0x0A,0x01,Lamp);//显示传感器开路E3
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
            DisplayB(0x0D,0x0A,0x01,Lamp);//显示传感器开路E3
        }
        //swWorkState=OPT_STATE_ERR1;
    }
}




extern unsigned char CheckShuiState(void);
#define ZHEQITMP  130
#define CAFETMP   124
#define OFFSET_TMP  7

#define ZQVALL  120
#define BU_2MIN  234  //120秒时间
#define BU_FULL  20 //补水时间 检测到水之后补10秒
#define BEN_RA   200 //抽水泵干扰
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
        if(HeatFlag==1)//表示要进行加热、检测温度、检测水位 循环控制
        {
            if(P_SW==1)//检测压力开关
            {
                CT1_Z=0;
                HeatFa=0x00;
                StartMach=0x00;
            }
            else //表示蒸汽温度低于130度 开始加热
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
        case SHUI_FULL:	  //表示已经满水
            if(ShuiFlag==1)
            {
                ShuiTime=0;
                ShuiFlag=0x55;
            }
            if(ShuiFlag==0x55)
            {
                ShuiTime++;
                if(ShuiTime>BU_FULL)//补水10秒
                {
                    ShuiFlag=0;
                    ShuiTime=0;
                    CT1_S=0;
                }
            }
            break;
        case SHUI_EMPT://需要补水
            if((ShuiFlag==0)&&((swWorkState==OPT_STATE_INIT)||(swWorkState==OPT_STATE_IDLE)))
            {
                CT1_S=1;//启动补水
                delayNP(BEN_RA);//延时 减少信号干扰
                ShuiFlag=1;
                ShuiTime=0;
                Display(0x00,0x00,0x00,0x00);//
            }
            ShuiTime++;
            delayNP(1000);
            if(ShuiTime>BU_2MIN)//表示补水超过2分钟
            {
                CT1_S=0;
                ShuiFlag=0;
                ShuiTime=0;
                Display(0x0D,0x0A,0x02,Lamp);//显示补水失败
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
        if(HeatFlagB==1)//表示要进行加热、检测温度、检测水位 循环控制
        {
            if(P_SW2==1)//检测压力开关
            {
                CT2_Z=0;
                HeatFaB=0x00;
                StartMachB=0x00;
            }
            else //表示蒸汽温度低于130度 开始加热
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
        case SHUI_FULL:	  //表示已经满水
            if(ShuiFlagB==1)
            {
                ShuiTimeB=0;
                ShuiFlagB=0x55;
            }
            if(ShuiFlagB==0x55)
            {
                ShuiTimeB++;
                if(ShuiTimeB>BU_FULL)//补水10秒
                {
                    ShuiFlagB=0;
                    ShuiTimeB=0;
                    CT2_S=0;
                }
            }
            break;
        case SHUI_EMPT://需要补水
            if((ShuiFlagB==0)&&((swWorkStateB==OPT_STATE_INIT)||(swWorkStateB==OPT_STATE_IDLE)))
            {
                CT2_S=1;//启动补水
                delayNP(BEN_RA);//延时 减少信号干扰
                ShuiFlagB=1;
                ShuiTimeB=0;
                DisplayB(0x00,0x00,0x00,0x00);//
            }
            ShuiTime++;
            delayNP(1000);
            if(ShuiTime>BU_2MIN)//表示补水超过2分钟
            {
                CT2_S=0;
                ShuiFlagB=0;
                ShuiTimeB=0;
                DisplayB(0x0D,0x0A,0x02,Lamp);//显示补水失败
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
        if(SenTemp<TMELOW)//表示蒸汽小于35度
        {
            Display(0x0D,0x0B,0x0C,Lmp);//显示LO
        }
        else
        {
            if(SenTemp<100)
            {
                Display(0x0D,(SenTemp+TMP_OFFSET)/10,(SenTemp+TMP_OFFSET)%10,Lmp);//刷新数据
            }
            else
            {
                Display((SenTemp+TMP_OFFSET)/100,(SenTemp+TMP_OFFSET-100)/10,(SenTemp+TMP_OFFSET)%10,Lmp);//刷新数据
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
        if(SenTemp<TMELOW)//表示蒸汽小于35度
        {
            Display(0x0D,0x0B,0x0C,Lmp);//显示LO
        }
        else
        {
            if(SenTemp<100)
            {
                Display(0x0D,(SenTemp+TMP_OFFSET)/10,(SenTemp+TMP_OFFSET)%10,Lmp);//刷新数据
            }
            else
            {
                Display((SenTemp+TMP_OFFSET)/100,(SenTemp+TMP_OFFSET-100)/10,(SenTemp+TMP_OFFSET)%10,Lmp);//刷新数据
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
    if((CheckKey()==ADD_SW)&&(AddBFlag==0xAA))//表示
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
                    Display(0x0D,Temp1/10,Temp1%10,0x25);//刷新数据
                }
            }
            else
            {
                if(Temp2<STMPMAX)
                {
                    Temp2++;
                    Display(0x0D,Temp2/10,Temp2%10,0x29);//刷新数据
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
                    Display(0x0D,Temp1/10,Temp1%10,0x25);//刷新数据
                }
            }
            else
            {
                if(Temp2<STMPMAX)
                {
                    Temp2++;
                    Display(0x0D,Temp2/10,Temp2%10,0x29);//刷新数据
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
                    Display(0x0D,Temp1/10,Temp1%10,0x25);//刷新数据
                }
            }
            else
            {
                if(Temp2>STMPMIN)
                {
                    Temp2--;
                    Display(0x0D,Temp2/10,Temp2%10,0x29);//刷新数据
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
                    Display(0x0D,Temp1/10,Temp1%10,0x25);//刷新数据
                }
            }
            else
            {
                if(Temp2>STMPMIN)
                {
                    Temp2--;
                    Display(0x0D,Temp2/10,Temp2%10,0x29);//刷新数据
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
    case OPT_STATE_INIT://初始化
        if(ShuiFlag==0)
        {
            if(TimeCc>(3*ONES))
            {
                TimeCc=0;
                if(SenTemp<TMELOW)//表示蒸汽小于35度
                {
                    Display(0x0D,0x0B,0x0C,0x16);//显示LO
                }
                else
                {
                    Display(0x0D,Temp1/10,Temp1%10,0x16);//刷新数据
                }
                swWorkState=OPT_STATE_PREH;
            }
        }
        break;
    case OPT_STATE_PREH://预热
        if(CheckKey()==0)//表示松开按键
        {
            HeatFlag=1;//表示启动加热功能
            KeyTime=0;
            TempFlag=0;
            if(SenTemp<TMELOW)//表示蒸汽小于35度
            {
                Display(0x0D,0x0B,0x0C,0x16);//显示LO
            }
            else
            {
                Display(0x0D,Temp1/10,Temp1%10,0x16);//刷新数据
            }
            swWorkState=OPT_STATE_IDLE;
        }
        break;
    case OPT_STATE_IDLE://
        if(ShuiFlag==0)//
        {
            RButKey=CheckKey();
            if(RButKey==START_SW)//表示开启按键
            {
                FlashTempVal();
                HeadMaxTmp=SenTemp;//刷新一个温度 解决一进入就检测到上一次温度
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
            else if((RButKey==CLEAR_SW)&&(KeyFlag==0))//表示清洁
            {

                CT1_C=1;
                delayNP(FA_RA);
                KeyFlag=0xBB;
                KeyTime=0;
                TimeCc=0;
                ClearWorkDoJob(0x01);
                swWorkState=OPT_STATE_CLERA;
            }
            else if(RButKey==TMP1_SW)//表示选择温度1
            {
                if(TempFlag==1)
                {
                    TempFlag=0;
                }
                Display(0x0D,Temp1/10,Temp1%10,0x16);//刷新数据
                TimeCc=0;
                KeyTime++;
                if(KeyTime>SW_LONG)//表示长按 设置温度1值
                {
                    swWorkState=OPT_STATE_SHEAD1;
                    KeyTime=0;
                    TimeCc=0;
                    Display(0x0D,Temp1/10,Temp1%10,0x25);//刷新数据
                }
            }
            else if(RButKey==TMP2_SW)//表示选择温度2
            {
                if(TempFlag==0)
                {
                    TempFlag=1;
                }
                Display(0x0D,Temp2/10,Temp2%10,0x1A);//刷新数据
                KeyTime++;
                TimeCc=0;
                if(KeyTime>SW_LONG)//表示长按 设置温度1值
                {
                    swWorkState=OPT_STATE_SHEAD2;
                    KeyTime=0;
                    Display(0x0D,Temp2/10,Temp2%10,0x29);//刷新数据
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
                    if(SenTemp<(TMELOW-1))//表示蒸汽小于35度
                    {
                        Display(0x0D,0x0B,0x0C,Lamp);//显示LO
                    }
                    else if(SenTemp>TMELOW)
                    {
                        if(SenTemp<100)
                        {
                            Display(0x0D,(SenTemp+TMP_OFFSET)/10,(SenTemp+TMP_OFFSET)%10,Lamp);//刷新数据
                        }
                        else
                        {
                            Display((SenTemp+TMP_OFFSET)/100,(SenTemp+TMP_OFFSET-100)/10,(SenTemp+TMP_OFFSET)%10,Lamp);//刷新数据
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
                    if(SenTemp<(TMELOW-1))//表示蒸汽小于35度
                    {
                        Display(0x0D,0x0B,0x0C,0x1A);//显示LO
                    }
                    else if(SenTemp>TMELOW)
                    {
                        if(SenTemp<100)
                        {
                            Display(0x0D,(SenTemp+TMP_OFFSET)/10,(SenTemp+TMP_OFFSET)%10,Lamp);//刷新数据
                        }
                        else
                        {
                            Display((SenTemp+TMP_OFFSET)/100,(SenTemp+TMP_OFFSET-100)/10,(SenTemp+TMP_OFFSET)%10,Lamp);//刷新数据
                        }
                    }
                }
            }
        }
        break;
    case OPT_STATE_HEAD1://表示加热状态1
        if(TimeCc>(0.5*ONES))
        {
            TimeCc=0;
            DelayTime++;
            if(SenTemp<TMELOW)//表示蒸汽小于35度
            {
                Display(0x0D,0x0B,0x0C,0x14);//显示LO
            }
            else
            {
                if(SenTemp<100)
                {
                    Display(0x0D,(SenTemp+TMP_OFFSET)/10,(SenTemp+TMP_OFFSET)%10,0x14);//刷新数据
                }
                else
                {
                    Display((SenTemp+TMP_OFFSET)/100,(SenTemp+TMP_OFFSET-100)/10,(SenTemp+TMP_OFFSET)%10,0x14);//刷新数据
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
            if(DelayTime>360)//表示超时退出
            {
                CT1_C=0;
                delayNP(FA_RA);
                KeyTime=0;
                swWorkState=OPT_STATE_IDLE;
            }
        }
        KeyBut=CheckKey();
        if((KeyBut==START_SW)&&(KeyFlag==0))//表示
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
    case OPT_STATE_HEAD2://表示加热状态2
        if(TimeCc>(0.5*ONES))
        {
            TimeCc=0;
            DelayTime++;
            if(SenTemp<TMELOW)//表示蒸汽小于35度
            {
                Display(0x0D,0x0B,0x0C,0x18);//显示Lo
            }
            else
            {
                if(SenTemp<100)
                {
                    Display(0x0D,(SenTemp+TMP_OFFSET)/10,(SenTemp+TMP_OFFSET)%10,0x18);//刷新数据
                }
                else
                {
                    Display((SenTemp+TMP_OFFSET)/100,(SenTemp+TMP_OFFSET-100)/10,(SenTemp+TMP_OFFSET)%10,0x18);//刷新数据
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
            if(DelayTime>360)//表示超时退出
            {
                CT1_C=0;
                delayNP(FA_RA);
                KeyTime=0;
                swWorkState=OPT_STATE_IDLE;
            }
        }
        KeyBut=CheckKey();
        if((KeyBut==START_SW)&&(KeyFlag==0))//表示
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
    case OPT_STATE_CLERA://表示清洁状态
        KeyBut=CheckKey();
        if(KeyBut==CLEAR_SW)//表示清洁
        {
            KeyTime++;
            if(KeyTime>SW_LONG)//表示进入清空煲体
            {
                KeyTime=0;
                KeyFlag=0xBB;
                HeatFlag=0;//关闭煲体加热功能
                MkCafeTime=0;
                CT1_S=1;//启动补水
                swWorkState=OPT_STATE_CLERB;
                if(TempFlag==0)
                {
                    Display(0x03,0x03,0x03,0x16);//显示LO
                }
                else
                {
                    Display(0x03,0x03,0x03,0x1A);//显示LO
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
        if(((KeyBut==START_SW)||(KeyBut==CLEAR_SW))&&(KeyFlag==0))//表示
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
    case OPT_STATE_CLERB://表示清空煲体
        //清空煲体时间
        if((CheckKey()==CLEAR_SW)&&(KeyFlag==0)&&(TimeCc>(2*ONES)))//表示清洁
        {
            swWorkState=OPT_STATE_IDLE;
            ClearWorkDoJob(0x02);
            CT1_C=0;
            CT1_S=0;//关闭补水
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
            CT1_S=0;//启动补水
            MkCafeTime=0;
        }
        break;
    case OPT_STATE_SHEAD1://设置温度加热值1
        ButAdjPro(0x01);
        if(CheckKey()==TMP1_SW)//表示
        {
            KeyTime++;
            if(KeyTime>SW_LONGA)//表示长按 设置温度1值
            {
                //保存数据
                WriteTmp1Eeprom();
                swWorkState=OPT_STATE_IDLE;
                HeatFlag=1;
                KeyTime=0;
                Display(0x0D,Temp1/10,Temp1%10,0x16);//刷新数据
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
    case OPT_STATE_SHEAD2://设置温度加热值2
        ButAdjPro(0x02);
        if(CheckKey()==TMP2_SW)//表示
        {
            KeyTime++;
            if(KeyTime>SW_LONGA)//表示长按 设置温度1值
            {
                //保存数据
                WriteTmp2Eeprom();
                swWorkState=OPT_STATE_IDLE;
                HeatFlag=1;
                KeyTime=0;
                Display(0x0D,Temp2/10,Temp2%10,0x1A);//刷新数据
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
    case OPT_STATE_ERR1://不可恢复错误 补水失败 超过2分钟
        HeatFlag=0;
        CT1_C=0;
        CT1_Z=0;
        CT1_S=0;
        break;
    case OPT_STATE_SLEEP://关机状态
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
        if(SenTempB<TMELOW)//表示蒸汽小于35度
        {
            DisplayB(0x0D,0x0B,0x0C,Lmp);//显示LO
        }
        else
        {
            if(SenTempB<100)
            {
                DisplayB(0x0D,(SenTempB+TMP_OFFSET)/10,(SenTempB+TMP_OFFSET)%10,Lmp);//刷新数据
            }
            else
            {
                DisplayB((SenTempB+TMP_OFFSET)/100,(SenTempB+TMP_OFFSET-100)/10,(SenTempB+TMP_OFFSET)%10,Lmp);//刷新数据
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
        if(SenTempB<TMELOW)//表示蒸汽小于35度
        {
            DisplayB(0x0D,0x0B,0x0C,Lmp);//显示LO
        }
        else
        {
            if(SenTempB<100)
            {
                DisplayB(0x0D,(SenTempB+TMP_OFFSET)/10,(SenTempB+TMP_OFFSET)%10,Lmp);//刷新数据
            }
            else
            {
                DisplayB((SenTempB+TMP_OFFSET)/100,(SenTempB+TMP_OFFSET-100)/10,(SenTempB+TMP_OFFSET)%10,Lmp);//刷新数据
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
    if((CheckKeyB()==ADD_SW)&&(AddBFlag==0xAA))//表示
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
                    DisplayB(0x0D,Temp1B/10,Temp1B%10,0x25);//刷新数据
                }
            }
            else
            {
                if(Temp2B<STMPMAX)
                {
                    Temp2B++;
                    DisplayB(0x0D,Temp2B/10,Temp2B%10,0x29);//刷新数据
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
                    DisplayB(0x0D,Temp1B/10,Temp1B%10,0x25);//刷新数据
                }
            }
            else
            {
                if(Temp2B<STMPMAX)
                {
                    Temp2B++;
                    DisplayB(0x0D,Temp2B/10,Temp2B%10,0x29);//刷新数据
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
                    DisplayB(0x0D,Temp1B/10,Temp1B%10,0x25);//刷新数据
                }
            }
            else
            {
                if(Temp2B>STMPMIN)
                {
                    Temp2B--;
                    DisplayB(0x0D,Temp2B/10,Temp2B%10,0x29);//刷新数据
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
                    DisplayB(0x0D,Temp1B/10,Temp1B%10,0x25);//刷新数据
                }
            }
            else
            {
                if(Temp2B>STMPMIN)
                {
                    Temp2B--;
                    DisplayB(0x0D,Temp2B/10,Temp2B%10,0x29);//刷新数据
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
    case OPT_STATE_INIT://初始化
        if(ShuiFlagB==0)
        {
            if(TimeCcB>(3*ONES))
            {
                TimeCcB=0;
                if(SenTempB<TMELOW)//表示蒸汽小于35度
                {
                    DisplayB(0x0D,0x0B,0x0C,0x16);//显示LO
                }
                else
                {
                    DisplayB(0x0D,Temp1B/10,Temp1B%10,0x16);//刷新数据
                }
                swWorkStateB=OPT_STATE_PREH;
            }
        }
        break;
    case OPT_STATE_PREH://预热
        if(CheckKeyB()==0)//表示松开按键
        {
            HeatFlagB=1;//表示启动加热功能
            KeyTimeB=0;
            TempFlagB=0;
            if(SenTempB<TMELOW)//表示蒸汽小于35度
            {
                DisplayB(0x0D,0x0B,0x0C,0x16);//显示LO
            }
            else
            {
                DisplayB(0x0D,Temp1B/10,Temp1B%10,0x16);//刷新数据
            }
            swWorkStateB=OPT_STATE_IDLE;
        }
        break;
    case OPT_STATE_IDLE://
        if(ShuiFlagB==0)//表示正在补水
        {
            RButKey=CheckKeyB();
            if(RButKey==START_SW)//表示开启按键
            {
                FlashTempValB();
                HeadMaxTmpB=SenTempB;//刷新一个温度 解决一进入就检测到上一次温度
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
            else if((RButKey==CLEAR_SW)&&(KeyFlagB==0))//表示清洁
            {

                CT2_C=1;
                delayNP(FA_RA);
                KeyFlagB=0xBB;
                KeyTimeB=0;
                TimeCcB=0;
                ClearBWorkDoJob(0x01);
                swWorkStateB=OPT_STATE_CLERA;

            }
            else if(RButKey==TMP1_SW)//表示选择温度1
            {

                if(TempFlagB==1)
                {
                    TempFlagB=0;
                }
                DisplayB(0x0D,Temp1B/10,Temp1B%10,0x16);//刷新数据
                TimeCcB=0;
                KeyTimeB++;
                if(KeyTimeB>SW_LONG)//表示长按 设置温度1值
                {
                    swWorkStateB=OPT_STATE_SHEAD1;
                    KeyTimeB=0;
                    TimeCcB=0;
                    DisplayB(0x0D,Temp1B/10,Temp1B%10,0x25);//刷新数据
                }
            }
            else if(RButKey==TMP2_SW)//表示选择温度2
            {
                if(TempFlagB==0)
                {
                    TempFlagB=1;
                }
                DisplayB(0x0D,Temp2B/10,Temp2B%10,0x1A);//刷新数据
                KeyTimeB++;
                TimeCcB=0;
                if(KeyTimeB>SW_LONG)//表示长按 设置温度1值
                {
                    swWorkStateB=OPT_STATE_SHEAD2;
                    KeyTimeB=0;
                    DisplayB(0x0D,Temp2B/10,Temp2B%10,0x29);//刷新数据
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
                    if(SenTempB<(TMELOW-1))//表示蒸汽小于35度
                    {
                        DisplayB(0x0D,0x0B,0x0C,LampB);//显示LO
                    }
                    else if(SenTempB>TMELOW)
                    {
                        if(SenTempB<100)
                        {
                            DisplayB(0x0D,(SenTempB+TMP_OFFSET)/10,(SenTempB+TMP_OFFSET)%10,LampB);//刷新数据
                        }
                        else
                        {
                            DisplayB((SenTempB+TMP_OFFSET)/100,(SenTempB+TMP_OFFSET-100)/10,(SenTempB+TMP_OFFSET)%10,LampB);//刷新数据
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
                    if(SenTempB<(TMELOW-1))//表示蒸汽小于35度
                    {
                        DisplayB(0x0D,0x0B,0x0C,0x1A);//显示LO
                    }
                    else if(SenTempB>TMELOW)
                    {
                        if(SenTempB<100)
                        {
                            DisplayB(0x0D,(SenTempB+TMP_OFFSET)/10,(SenTempB+TMP_OFFSET)%10,LampB);//刷新数据
                        }
                        else
                        {
                            DisplayB((SenTempB+TMP_OFFSET)/100,(SenTempB+TMP_OFFSET-100)/10,(SenTempB+TMP_OFFSET)%10,LampB);//刷新数据
                        }
                    }
                }
            }
        }
        break;
    case OPT_STATE_HEAD1://表示加热状态1
        if(TimeCcB>(0.5*ONES))
        {
            TimeCcB=0;
            DelayTimeB++;
            if(SenTempB<TMELOW)//表示蒸汽小于35度
            {
                DisplayB(0x0D,0x0B,0x0C,0x14);//显示LO
            }
            else
            {
                if(SenTempB<100)
                {
                    DisplayB(0x0D,(SenTempB+TMP_OFFSET)/10,(SenTempB+TMP_OFFSET)%10,0x14);//刷新数据
                }
                else
                {
                    DisplayB((SenTempB+TMP_OFFSET)/100,(SenTempB+TMP_OFFSET-100)/10,(SenTempB+TMP_OFFSET)%10,0x14);//刷新数据
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
            if(DelayTimeB>360)//表示超时退出
            {
                CT2_C=0;
                delayNP(FA_RA);
                KeyTimeB=0;
                swWorkStateB=OPT_STATE_IDLE;
            }
        }
        KeyBut=CheckKeyB();
        if((KeyBut==START_SW)&&(KeyFlagB==0))//表示
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
    case OPT_STATE_HEAD2://表示加热状态2
        if(TimeCcB>(0.5*ONES))
        {
            TimeCcB=0;
            DelayTimeB++;
            if(SenTempB<TMELOW)//表示蒸汽小于35度
            {
                DisplayB(0x0D,0x0B,0x0C,0x18);//显示Lo
            }
            else
            {
                if(SenTempB<100)
                {
                    DisplayB(0x0D,(SenTempB+TMP_OFFSET)/10,(SenTempB+TMP_OFFSET)%10,0x18);//刷新数据
                }
                else
                {
                    DisplayB((SenTempB+TMP_OFFSET)/100,(SenTempB+TMP_OFFSET-100)/10,(SenTempB+TMP_OFFSET)%10,0x18);//刷新数据
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
            if(DelayTimeB>360)//表示超时退出
            {
                CT2_C=0;
                delayNP(FA_RA);
                KeyTimeB=0;
                swWorkStateB=OPT_STATE_IDLE;
            }
        }
        KeyBut=CheckKeyB();
        if((KeyBut==START_SW)&&(KeyFlagB==0))//表示
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
    case OPT_STATE_CLERA://表示清洁状态
        KeyBut=CheckKeyB();
        if(KeyBut==CLEAR_SW)//表示清洁
        {
            KeyTimeB++;
            if(KeyTimeB>SW_LONG)//表示进入清空煲体
            {
                KeyTimeB=0;
                KeyFlagB=0xBB;
                HeatFlagB=0;//关闭煲体加热功能
                MkCafeTimeB=0;
                CT2_S=1;//启动补水
                swWorkStateB=OPT_STATE_CLERB;
                if(TempFlagB==0)
                {
                    DisplayB(0x03,0x03,0x03,0x16);//显示LO
                }
                else
                {
                    DisplayB(0x03,0x03,0x03,0x1A);//显示LO
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
        if(((KeyBut==START_SW)||(KeyBut==CLEAR_SW))&&(KeyFlagB==0))//表示
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
    case OPT_STATE_CLERB://表示清空煲体
        //清空煲体时间
        if((CheckKeyB()==CLEAR_SW)&&(KeyFlagB==0)&&(TimeCcB>(2*ONES)))//表示清洁
        {
            swWorkStateB=OPT_STATE_IDLE;
            ClearBWorkDoJob(0x02);
            CT2_C=0;
            CT2_S=0;//关闭补水
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
            CT2_S=0;//启动补水
            MkCafeTimeB=0;
        }
        break;
    case OPT_STATE_SHEAD1://设置温度加热值1
        ButAdjProB(0x01);
        if(CheckKeyB()==TMP1_SW)//表示
        {
            KeyTimeB++;
            if(KeyTimeB>SW_LONGA)//表示长按 设置温度1值
            {
                //保存数据
                WriteTmp1BEeprom();
                swWorkStateB=OPT_STATE_IDLE;
                HeatFlagB=1;
                KeyTimeB=0;
                DisplayB(0x0D,Temp1B/10,Temp1B%10,0x16);//刷新数据
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
    case OPT_STATE_SHEAD2://设置温度加热值2
        ButAdjProB(0x02);
        if(CheckKeyB()==TMP2_SW)//表示
        {
            KeyTimeB++;
            if(KeyTimeB>SW_LONGA)//表示长按 设置温度1值
            {
                //保存数据
                WriteTmp2BEeprom();
                swWorkStateB=OPT_STATE_IDLE;
                HeatFlagB=1;
                KeyTimeB=0;
                DisplayB(0x0D,Temp2B/10,Temp2B%10,0x1A);//刷新数据
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
    case OPT_STATE_ERR1://不可恢复错误 补水失败 超过2分钟
		HeatFlagB=0;
        CT2_C=0;
        CT2_Z=0;
        CT2_S=0;
        break;
    case OPT_STATE_SLEEP://关机状态
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
                Display(0x0D,0x0A,0x03,Lamp);//显示补水失败
            }
            if(LedBflag)
            {
                DisplayB(0x0D,0x0A,0x03,Lamp);//显示补水失败
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
            Display(0x0D,0x0D,0x0D,0x00);//关闭显示屏
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
            DisplayB(0x0D,0x0D,0x0D,0x00);//关闭显示屏
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
