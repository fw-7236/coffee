#include "OB38R08T1.h"
#include "UART0.h"
#include "EEPROM.h"
#include "Timer.h"
#include "ADC.h"


//1:enable, 0:disable
#define g_SleepCtrl     (*((unsigned char volatile xdata*)0x125))
//Set this flag can skip touch detection after user wakes up MCU from power down mode by external interrupt
#define ExtINTBreak     (*((unsigned char volatile xdata*)0xF7))


#define DEFAULT_SING  93  //单杯
#define DEFAULT_DBU   93  //双杯

#define  TMP1B_VAL 1//蒸汽温度1
#define  TMP2B_VAL 2//蒸汽温度2
#define  SETTEMP    3//单杯时间
//#define  DUBT      4//双杯时间

#define BU_2MIN  234  //120秒时间
#define BU_FULL  0

#define YuPaoT	   1
#define SW_LONG    100

unsigned int xdata     SenTemp=0;//争蒸汽温度热电偶
unsigned int xdata  	CafTemp=0;//咖啡煲温度NTC
unsigned char xdata  	PushFlag=0;//蒸汽煲温度NTC
unsigned char xdata  	ShuiFlag=0;//水位开关检测
unsigned char xdata  	WaterFlag=0;//满水标志位
unsigned char xdata		HeatState=0;
unsigned char   xdata   PowerFlag=0;//电源标志

unsigned char xdata  	ZetTmp=0;//蒸汽温度工作值
unsigned char xdata     Temp1 =0; //蒸汽温度1设定值
unsigned char xdata     Temp2 =0; //蒸汽温度2设定值

unsigned int  xdata 	SetTemp=0;//咖啡设定温度
unsigned char xdata  	COFE_TMP=124;//做咖啡温度
unsigned char xdata  	COFE_LIMT=93;//显示限制

unsigned int  xdata 	TimeCc=0;//状态机计时
unsigned int  xdata 	TimeCc1=0;//状态机计时
unsigned int  xdata 	AutCtrTime=0;//蒸汽自动控制计时
unsigned int  xdata 	BengCtrTime=0;//蒸汽自动控制计时
unsigned int  xdata 	AutTempTime=0;//自动温度计时
unsigned int  xdata     CofAutCtrTime=0;//咖啡自动控制计时
unsigned int  xdata     StemAutCtrTime=0;//咖啡自动控制计时
unsigned int  xdata 	MakTim=0;//制作计时
unsigned int  xdata 	MkCafeTime=0;//做咖啡时间
unsigned char xdata  	ErrCode=0;//存储咖啡任务错误代码
unsigned char xdata     ErrCode1=0;//存储蒸汽任务错误代码

unsigned char xdata	 	Cafone=0;
unsigned char xdata 	Caftwo=0;
unsigned char xdata 	Cafthr=0;
unsigned char xdata 	Zheone=0;
unsigned char xdata 	Zhetwo=0;
unsigned char xdata 	Zhethr=0;
unsigned char xdata 	disCofLed =0;
unsigned char xdata 	disStemLed =0;

unsigned char xdata 	SigFlag=0;
unsigned char xdata 	DubFlag=0;

unsigned char xdata 	ADKEY=0;//AD按键


unsigned int xdata  ShuiTime=0;//补水时间计数
unsigned char xdata   WhichBut=0;//记录哪个按键进入设置
unsigned char xdata   WhichBut1=0;//记录哪个按键进

unsigned int KeyTime=0;
unsigned int KeyTime1=0;
unsigned char LedOnOff=0;
unsigned char LedOnOff1=0;
unsigned char onoff=1;

bit HeatFlag;//蒸汽加热标志位
bit CofHeat=0;//咖啡加热标志

#define  ONES          445///循环时间为1秒
//********************************************************************
#define LED_COM1 	P3_3
#define LED_COM2 	P3_4
#define LED_COM3 	P3_5
#define LED_COM4 	P1_7
#define LED_COM5 	P1_6
#define LED_COM6 	P1_3
#define LED_COM7 	P1_2

#define LED_A	 	P0_0
#define LED_B	 	P0_1
#define LED_C	 	P0_2
#define LED_D	 	P0_3
#define LED_E	 	P0_4
#define LED_F	 	P0_5
#define LED_G		P0_6
#define LED_H		P0_7

#define SW_ADD		P3_0	//SW1
#define SW_SUB		P3_1	//SW2

#define K1	 		P1_0   //AD按键

#define RXD	 		P1_5
#define TXD	 		P1_4

#define SW_L        4
#define SW_D        4

//*******************************************************************
typedef enum _WORKState
{
    WORK_STATE_INIT     =     0,//系统初始化
    WORK_STATE_BENG     =     1,//泵水3秒钟
    WORK_STATE_RHEAT    =     2,//咖啡预热
    WORK_STATE_ERR      =     3,//系统运行错误
    WORK_STATE_DOWN     =     4,//关机状态
    //***************************************
    WORK_STATE_RUN      =     8,//显示NTC实际温度
    WORK_STATE_WAIT     =     9,//轮流开启阀门
    WORK_STATE_PER      =     10,//预泡
    WORK_STATE_PAO		=	  11,
    WORK_STATE_OUT      =     12,//出咖啡
    //************************************************
    WORK_STATE_SET     =     16,//咖啡时间设置
} WorkState;

WorkState  swWorkState=WORK_STATE_INIT;//定义状态机

typedef enum _StemWorkState
{
    OPT_STATE_INIT     =     0,//0x00 表示机器初始化状态 全部数码管显示888 1秒
    OPT_STATE_IDLE     =     1,//等待用户操作状态
    OPT_STATE_HEAD     =     2,//表示加热状态1
    OPT_STATE_CLERA    =     3,//表示清洁状态
    OPT_STATE_SETMP1   =     4,//表示温度设置1
    OPT_STATE_SETMP2   =     5,//表示温度设置2
    OPT_STATE_ERR      =     6,//出错
    OPT_STATE_DOWN     =     7,//关机状态
} _StemWorkState;

_StemWorkState  StemWork=OPT_STATE_INIT;//定义状态机

//数据头
#define  ESC_01  	0x1B
#define  ESC_02  	0x2A
//**********************************************************
#define  CMD_CTMP  		0x01  //读取咖啡温度
#define  CMD_ZTMP  		0x02  //读取蒸汽温度
#define  CMD_SHUI       0x03  //读取水位
#define  CMD_LIU        0x04  //读取流量 
#define  CMD_COFE       0x05  //读取咖啡按键 
#define  CMD_SHXI       0x06  //读取水箱状态
#define  CMD_POP        0x07  //表示压力开关状态
#define  CMD_MS         0x08  //获取蒸汽开关
#define  CMD_STEMTMP    0x09  //获取蒸汽温度(热电偶)

//************************************************************
#define  CMD_CTBENG   	0x10  //表示控制泵抽水           CK6
#define  CMD_CTZHEN  	0x11  //表示控制蒸汽进水阀      	CK5
#define  CMD_CTHEAT  	0x12  //表示控制热水电磁阀      	CK4 
#define  CMD_CTCFE      0x13  //表示控制咖啡出水电磁阀   	CK3
#define  CMD_CTZSHUI 	0x14  //表示控制咖啡进水阀      	CK2
#define  CMD_BK1 		0x15  //表示控制备用            	CK1
#define  CMD_LAMP       0x16  //灯控制开关              	LAMP
#define  CMD_CAFEH      0x17  //控制咖啡煲加热           PWM
#define  CMD_ZHENH      0x18  //控制蒸气煲加热           CT2

#define  CMD_BUZ        0x19  //控制蜂鸣器
#define  CMD_SLEEP      0x1A  //表示控制休眠
#define  CMD_PWR        0x20  //电源开关
//***********************************************************

#define  P_TO_DIS   	0x01 //电源板主机向上发送
#define  DIS_TO_P   	0x02 //表示显示板向电源板发送
#define  CMD_OK     	0xAA //表示收到指令成功
#define  CMD_FAIL   	0x55 //表示失败

unsigned char  CheckCoffTemp(void);
unsigned char  CheckStemTemp(void);

#define TK0AVG  ((unsigned int volatile xdata*)0x178)				//key0平均值地址
//========================================================================================================
//  Command Read Format:
//  Start    -> Slave(W) -> Ack -> Command -> Ack ->
//  Re-Start -> Slave(R) -> Ack ->   0x00  -> Ack ->  TKSTATUS1  -> Ack -> TKSTATUS0 -> NAck -> Stop
//
//  Touch Slave(W) Address = 0x60
//  Touch Slave(R) Address = 0x61
//  Read Key Status Command Number = 0x17
//
//  [Master Example]
//  Start    ->  0x60(W) -> Ack ->   0x17  -> Ack ->
//  Re-Start ->  0x61(R) -> Ack ->   0x00  -> Ack ->  TKSTATUS1  -> Ack -> TKSTATUS0 -> NAck -> Stop
//========================================================================================================



//******************************************************************
//读取蒸汽温度1
void  WriteTmp1BEeprom(void)
{
    Write_EEPROM(TMP1B_VAL,Temp1);
}
void  ReadTmp1BEeprom(void)
{
    Temp1=Read_EEPROM(TMP1B_VAL);
    if(Temp1<50||Temp1>90)//当读取值不在范围值 恢复默认值
    {
        Temp1=60;
        WriteTmp1BEeprom();
    }
}

//读取蒸汽温度2
void  WriteTmp2BEeprom(void)
{
    Write_EEPROM(TMP2B_VAL,Temp2);
}
void  ReadTmp2BEeprom(void)
{
    Temp2=Read_EEPROM(TMP2B_VAL);
    if(Temp2<50||Temp2>90)//当读取值不在范围值 恢复默认值
    {
        Temp2=70;
        WriteTmp2BEeprom();
    }
}
//**********************************************************************************
//读写咖啡温度
void  WriteSetTempEeprom(void)
{
    Write_EEPROM(SETTEMP,SetTemp);
}
void  ReadSetTempEeprom(void)
{
    SetTemp=Read_EEPROM(SETTEMP);
    if(SetTemp>100||SetTemp<88)//当读取值不在范围值 恢复默认值
    {
        SetTemp=DEFAULT_SING;
        WriteSetTempEeprom();
    }
}
//**********************************************************************************
//读写双杯时间
//void  WriteDbuEeprom(void)
//{
//    Write_EEPROM(DUBT,DbuTemp);
//}
//void  ReadDbuEeprom(void)
//{
//    DbuTemp=Read_EEPROM(DUBT);
//    if(DbuTemp>100||DbuTemp<88)//当读取值不在范围值 恢复默认值
//    {
//        DbuTemp=DEFAULT_DBU;
//        WriteDbuEeprom();
//    }
//}

//********************************************************************************

void Delay(unsigned int tim)
{
    unsigned int  i,j;
    for (i=0; i<tim; i++)
        for (j=0; j<100; j++);
}
//void delayNP(unsigned int tim)
//{
//    unsigned int  i,j;
//    for (i=0; i<tim; i++)
//    {
//        for (j=0; j<10; j++);
//    }
//}
//*******************************************************************
#define TIME_OUT   200
unsigned char  UART_Rxd_Process(unsigned char cmd)//处理显示相关指令
{
    unsigned char xdata pcmd=0,sdata1=0,sdata2=0,schecksum1,schecksum2;
    unsigned short CheckSumDIS,CheckSumPo;
    unsigned int timeOut=0;
    while(1)
    {
        if(GetBufSize()>=8)
            break;
        timeOut++;
        Delay(10);
        if(timeOut>TIME_OUT)
            break;
    }
    if(GetBufSize()>=8)
    {
        if(GetBufData()==ESC_01)//读取0x1B
        {
            if(GetBufData()==ESC_02)//读取0x2A
            {
                if(GetBufData()==P_TO_DIS)//0x02 表示显示屏到电源板指令
                {
                    pcmd=GetBufData();
                    sdata1=GetBufData();
                    sdata2=GetBufData();
                    schecksum1=GetBufData();
                    schecksum2=GetBufData();
                    CheckSumDIS=(schecksum1<<8)+schecksum2;
                    CheckSumPo=ESC_01+ESC_02+P_TO_DIS+pcmd+sdata1+sdata2;
                    if((CheckSumPo==CheckSumDIS)&&(pcmd==cmd))//
                    {
                        switch(pcmd)
                        {
                        case CMD_CTMP://读取咖啡煲温度
                            CafTemp=sdata1;
                            break;
                        case CMD_ZTMP://读取蒸汽煲温度
                            break;
                        case CMD_STEMTMP://获取蒸汽温度
                            SenTemp=sdata1+3;
                            break;
                        case CMD_SHUI://获取水箱水位
                            ShuiFlag=sdata1;
                            break;
                        case CMD_LIU://读取流量
                            break;
                        case CMD_COFE://
                            break;
                        case CMD_SHXI:
                            break;
                        case CMD_POP:
                            PushFlag=sdata1;
                            break;
                        case CMD_MS:
                            break;
                        case CMD_PWR:
                            PowerFlag=sdata1;
                            break;
                        case CMD_CTBENG: //表示控制泵抽水
                            break;
                        case CMD_CTZHEN://表示控制蒸汽进水阀
                        case CMD_CTHEAT://表示控制热水电磁阀
                        case CMD_CTCFE://表示控制咖啡出水电磁阀
                        case CMD_CTZSHUI://表示控制咖啡进水阀
                        case CMD_BK1://表示控制备用
                        case CMD_LAMP://灯控制开关
                        case CMD_CAFEH://控制咖啡煲加热
                        case CMD_ZHENH://控制蒸气煲加热
                        case CMD_BUZ://控制蜂鸣器
                        case CMD_SLEEP://表示控制休眠
                            break;
                        default:
                            break;
                        }
                        return  0xAA;
                    }
                }
            }
        }
    }
    return 0;
}

void UART_Txd_Cmd(unsigned char Cmd,unsigned char Parm1,unsigned char Parm2)
{
    unsigned short CheckSum=0;
    BufSizeClear();
    UART0_TX(ESC_01);
    UART0_TX(ESC_02);
    UART0_TX(DIS_TO_P);
    UART0_TX(Cmd);
    UART0_TX(Parm1);
    UART0_TX(Parm2);
    CheckSum=ESC_01+ESC_02+DIS_TO_P+Cmd+Parm1+Parm2;
    UART0_TX((CheckSum>>8)&0xFF);
    UART0_TX(CheckSum&0xFF);
}



//***************************************************************************
unsigned char GetCurTmp(void)//获取咖啡煲温度
{
    unsigned char i;
    UART_Txd_Cmd(CMD_CTMP,0x00,0x00);
    if(UART_Rxd_Process(CMD_CTMP)==0xAA)//返回成功
    {
        return 0x01;
    }
    else
    {
        for(i=0; i<3; i++)
        {
            UART_Txd_Cmd(CMD_CTMP,0x00,0x00);
            if(UART_Rxd_Process(CMD_CTMP)==0xAA)
            {
                return 0x01;
            }
        }
    }
    return 0x00;//不正常控制
}
//unsigned char GetZhenQiTmp(void)//获取蒸汽煲温度
//{
//    unsigned char i;
//    UART_Txd_Cmd(CMD_ZTMP,0x00,0x00);
//    if(UART_Rxd_Process(CMD_ZTMP)==0xAA)//返回成功
//    {
//        return 0x01;
//    }
//    else
//    {
//        for(i=0; i<3; i++)
//        {
//            UART_Txd_Cmd(CMD_ZTMP,0x00,0x00);
//            if(UART_Rxd_Process(CMD_ZTMP)==0xAA)
//            {
//                return 0x01;
//            }
//        }
//    }
//    return 0x00;//不正常控制
//}
unsigned char GetPop(void)//获取蒸汽煲压力
{
    unsigned char i;
    UART_Txd_Cmd(CMD_POP,0x00,0x00);
    if(UART_Rxd_Process(CMD_POP)==0xAA)//返回成功
    {
        return 0x01;
    }
    else
    {
        for(i=0; i<3; i++)
        {
            UART_Txd_Cmd(CMD_POP,0x00,0x00);
            if(UART_Rxd_Process(CMD_POP)==0xAA)
            {
                return 0x01;
            }
        }
    }
    return 0x00;//不正常控制
}


unsigned char GetSteamTmp(void)//获取蒸汽温度
{
    unsigned char i;
    UART_Txd_Cmd(CMD_STEMTMP,0x00,0x00);
    if(UART_Rxd_Process(CMD_STEMTMP)==0xAA)//返回成功
    {
        return 0x01;
    }
    else
    {
        for(i=0; i<5; i++)
        {
            UART_Txd_Cmd(CMD_STEMTMP,0x00,0x00);
            if(UART_Rxd_Process(CMD_STEMTMP)==0xAA)
            {
                return 0x01;
            }
        }
    }
    return 0x00;//不正常控制
}

unsigned char GetShuiWei(void)//取得水箱水位
{
    unsigned char i;
    UART_Txd_Cmd(CMD_SHUI,0x00,0x00);
    if(UART_Rxd_Process(CMD_SHUI)==0xAA)//返回成功
    {
        return 0x01;
    }
    else
    {
        for(i=0; i<3; i++)
        {
            UART_Txd_Cmd(CMD_SHUI,0x00,0x00);
            if(UART_Rxd_Process(CMD_SHUI)==0xAA)
            {
                return 0x01;
            }
        }
    }
    return 0x00;//不正常控制
}
unsigned char CtrK1(unsigned char ctr)//蒸汽进水电磁阀
{
    unsigned char i;
    UART_Txd_Cmd(CMD_CTZHEN,ctr,0x00);
    if(UART_Rxd_Process(CMD_CTZHEN)==0xAA)//返回成功
    {
        return 0x01;
    }
    else
    {
        for(i=0; i<5; i++)
        {
            UART_Txd_Cmd(CMD_CTZHEN,ctr,0x00);
            if(UART_Rxd_Process(CMD_CTZHEN)==0xAA)
            {
                return 0x01;
            }
        }
    }
    return 0x00;//不正常控制
}
unsigned char CtrK2(unsigned char ctr)//咖啡出水电磁阀
{
    unsigned char i;
    UART_Txd_Cmd(CMD_CTCFE,ctr,0x00);
    if(UART_Rxd_Process(CMD_CTCFE)==0xAA)//返回成功
    {
        return 0x01;
    }
    else
    {
        for(i=0; i<5; i++)
        {
            UART_Txd_Cmd(CMD_CTCFE,ctr,0x00);
            if(UART_Rxd_Process(CMD_CTCFE)==0xAA)
            {
                return 0x01;
            }
        }
    }
    return 0x00;//不正常控制
}
//*********************************************************************************
unsigned char CtrCoffeHeat(unsigned char ctr)//控制咖啡加热
{
    unsigned char i;
    UART_Txd_Cmd(CMD_CAFEH,ctr,0x00);
    if(UART_Rxd_Process(CMD_CAFEH)==0xAA)//返回成功
    {
        return 0x01;
    }
    else
    {
        for(i=0; i<5; i++)
        {
            UART_Txd_Cmd(CMD_CAFEH,ctr,0x00);
            if(UART_Rxd_Process(CMD_CAFEH)==0xAA)
            {
                return 0x01;
            }
        }
    }
    return 0x00;//不正常控制
}
unsigned char CtrZhenQI(unsigned char ctrOn)//控制蒸汽加热
{
    unsigned char i;
    UART_Txd_Cmd(CMD_ZHENH,ctrOn,0x00);
    if(UART_Rxd_Process(CMD_ZHENH)==0xAA)//返回成功
    {
        return 0x01;
    }
    else
    {
        for(i=0; i<5; i++)
        {
            UART_Txd_Cmd(CMD_ZHENH,ctrOn,0x00);
            if(UART_Rxd_Process(CMD_ZHENH)==0xAA)
            {
                return 0x01;
            }
        }
    }
    return 0x00;//不正常控制
}
//************************************************************************************
unsigned char CtrBeng(unsigned char Ontime)//控制泵
{
    unsigned char i;
    UART_Txd_Cmd(CMD_CTBENG,Ontime,0x00);
    if(UART_Rxd_Process(CMD_CTBENG)==0xAA)//返回成功
    {
        return 0x01;
    }
    else
    {
        for(i=0; i<5; i++)
        {
            UART_Txd_Cmd(CMD_CTBENG,Ontime,0x00);
            if(UART_Rxd_Process(CMD_CTBENG)==0xAA)
            {
                return 0x01;
            }
        }
    }
    return 0x00;//不正常控制
}

//unsigned char CtrJinShuiF(unsigned char ctr)//控制咖啡进水电磁阀
//{
//    unsigned char i;
//    UART_Txd_Cmd(CMD_CTZSHUI,ctr,0x00);//
//    if(UART_Rxd_Process(CMD_CTZSHUI)==0xAA)//返回成功
//    {
//        return 0x01;
//    }
//    else
//    {
//        for(i=0; i<3; i++)
//        {
//            UART_Txd_Cmd(CMD_CTZSHUI,ctr,0x00);
//            if(UART_Rxd_Process(CMD_CTZSHUI)==0xAA)
//            {
//                return 0x01;
//            }
//        }
//    }
//    return 0x00;//不正常控制
//}
//************************************************************************************
unsigned char CtrBuz(unsigned char ctr)//蜂鸣器
{
    unsigned char i;
    UART_Txd_Cmd(CMD_BUZ,ctr,0x00);
    if(UART_Rxd_Process(CMD_BUZ)==0xAA)//返回成功
    {
        return 0x01;
    }
    else
    {
        for(i=0; i<5; i++)
        {
            UART_Txd_Cmd(CMD_BUZ,ctr,0x00);
            if(UART_Rxd_Process(CMD_BUZ)==0xAA)
            {
                return 0x01;
            }
        }
    }
    return 0x00;//不正常控制
}

unsigned char CtrSteam(unsigned char ctr)//控制出蒸汽阀
{
    unsigned char i;
    UART_Txd_Cmd(CMD_CTHEAT,ctr,0x00);
    if(UART_Rxd_Process(CMD_CTHEAT)==0xAA)//返回成功
    {
        return 0x01;
    }
    else
    {
        for(i=0; i<5; i++)
        {
            UART_Txd_Cmd(CMD_CTHEAT,ctr,0x00);
            if(UART_Rxd_Process(CMD_CTHEAT)==0xAA)
            {
                return 0x01;
            }
        }
    }
    return 0x00;//不正常控制
}

unsigned char GetPower()//获取电源
{
    unsigned char i;
    UART_Txd_Cmd(CMD_PWR,0x00,0x00);
    if(UART_Rxd_Process(CMD_PWR)==0xAA)//返回成功
    {
        return 0x01;
    }
    else
    {
        for(i=0; i<5; i++)
        {
            UART_Txd_Cmd(CMD_PWR,0x00,0x00);
            if(UART_Rxd_Process(CMD_PWR)==0xAA)
            {
                return 0x01;
            }
        }
    }
    return 0x00;//不正常控制
}


//***********************************************************************************



// 00 表示双向IO
// 01 表示推挽输出
// 10 表示高阻输入
// 11 表示开漏
void GPIO_Init()
{
    P0 = 0x00;
    P1 = 0x00;
    P2 = 0x00;
    P3 = 0x00;

//      7  6  5  4  3  2  1  0
//P0M1  0  0  0	 0	0  0  0	 0
//P0M0  1  1  1	 1	1  1  1	 1
//	    H  G  F	 E  D  C  B  A
    P0M1 = 0x00;
    P0M0 = 0xFF;
//      7  	6 	5	4  	3  	2  	1  	0
//P1M1  0  	0  	0	0	0  	0  	0	0
//P1M0  1  	1  	0	1	1  	1  	0	0
//   COM4 COM5 RX  TX COM6 COM7     K1
    P1M1 = 0x00;
    P1M0 = 0xDC;
//      7  	 6 	 5	 4   3   2   1   0
//P2M1  0  	 0   0	 0	 0   0   0	 0
//P2M0  0  	 0   1	 0	 0   0   0	 0
//         RX1 TX1

    P2M1 = 0x00;
    P2M0 = 0x20;
//      7  	6 	5	4  	 3    2  	1  	0
//P3M1  0  	0  	0	0	 0    0  	0	0
//P3M0  0  	0  	1	1	 1    0  	0	0
//	          COM3 COM2 COM1    SW_SUB  SW_ADD
//******************************************************
    P3M1 = 0x03;
    P3M0 = 0x38;
//********************************************************
    K1 = 0;
    LED_A=0;
    LED_B=0;
    LED_C=0;
    LED_D=0;
    LED_E=0;
    LED_F=0;
    LED_G=0;
    LED_H=0;
    LED_COM1=0;
    LED_COM2=0;
    LED_COM3=0;
    LED_COM4=0;
    LED_COM5=0;
    LED_COM6=0;
    LED_COM7=0;
    TXD=1;
    RXD=1;
//********************************************************
}
void DisplayC(unsigned char Tmp,unsigned char tag)
{
    if(tag==0)//正常数值显示
    {
        if(Tmp>=100)
        {
            Cafone=Tmp/100;//
            Caftwo=(Tmp%100)/10;
            Cafthr=(Tmp%100)%10;
        } else if(Tmp>=10)
        {
            Cafone=0x0E;//
            Caftwo=Tmp/10;
            Cafthr=Tmp%10;
        }
        else
        {
            Cafone=0x0E;//
            Caftwo=0x0E;
            Cafthr=Tmp%10;
        }
    }
    else if(tag==1)//显示LO
    {
        Cafone=0x0E;//
        Caftwo=0x10;
        Cafthr=0x00;

    }
    else if(tag==2)//显示000
    {
        Cafone=0x00;//
        Caftwo=0x00;
        Cafthr=0x00;
    }
    else if(tag==3)//出错显示
    {
        Cafone=0x0E;//
        Caftwo=0x0B;
        Cafthr=ErrCode;
    }
    else if(tag==4)//息屏
    {
        Cafone=0x0E;
        Caftwo=0x0E;
        Cafthr=0x0E;
    }
}
void DisplayZ(unsigned char Tmp,unsigned char tag)
{
    if(tag==0)
    {
        if(Tmp>=100)
        {
            Zheone=Tmp/100;//
            Zhetwo=(Tmp%100)/10;
            Zhethr=(Tmp%100)%10;
        } else if(Tmp>=10)
        {
            Zheone=0x0E;//
            Zhetwo=Tmp/10;
            Zhethr=Tmp%10;
        }
        else
        {
            Zheone=0x0E;//
            Zhetwo=0x0E;
            Zhethr=Tmp%10;
        }
    }
    else if(tag==1)//显示LO
    {
        Zheone=0x0E;
        Zhetwo=0x10;
        Zhethr=0x00;
    }
    else if(tag==2)//补水显示000
    {
        Zheone=0x00;
        Zhetwo=0x00;
        Zhethr=0x00;
    }
    else if(tag==3)//出错显示
    {
        Zheone=0x0E;
        Zhetwo=0x0B;
        Zhethr=ErrCode1;
    }
    else if(tag==4)//息屏
    {
        Zheone=0x0E;
        Zhetwo=0x0E;
        Zhethr=0x0E;
    }
}


void PowerSleep(void)
{
    CtrBeng(0x00);		//关闭水泵
    CtrK2(0x00);	  //关出水
    CtrCoffeHeat(0x00);//关闭咖啡加热
}
void PowerSleep1(void)
{
    HeatFlag=0;
    CtrZhenQI(0x00);//关加热
    HeatState=0;
    CtrK1(0x00);	//关进水
    CtrSteam(0x00);//关出蒸汽
}
void CheckCofExit(void)
{
    static unsigned char set=0;
    static unsigned char down=0;
    static unsigned char temp=0;
	static unsigned char temp1=0;
    ADKEY = GetADKey();
    switch(ADKEY)
    {
    case AD_SIG_KEY:
        temp=3;
        down=1;
        break;
    case AD_DU_KEY:
        temp=4;
        down=1;
        break;
    case AD_SET_KEY:
        if(set&&swWorkState==WORK_STATE_SET)
        {
            KeyTime++;
            if(KeyTime>SW_LONG)
            {
                temp1=1;
                down=1;
                set=0;
                KeyTime=0;
                if(CafTemp>COFE_LIMT)
                {
                    DisplayC(COFE_LIMT,0);
                }
                else
                {
                    DisplayC(CafTemp,0);
                }
				onoff=0;
            }
        }
        break;
    default:
        if(down)
        {
            down=0;
            if(temp=WhichBut&&swWorkState!=WORK_STATE_SET)
            {
                CtrBeng(0x00);
                CtrK2(0x00);
                swWorkState=WORK_STATE_RHEAT;
            }
            if(temp1==1)
            {
				temp1=0;
				onoff=1;
                swWorkState=WORK_STATE_RHEAT;
            }
        }
        set=1;
        KeyTime=0;
        break;
    }
}
void CheckStemExit(void)
{
    static unsigned char set1=0;
    static unsigned char timer1=0;
    static unsigned char temp=0;
    ADKEY = GetADKey();
    switch(ADKEY)
    {
    case AD_TMP1_KEY:
        temp=1;
        set1=1;
        break;
    case AD_TMP2_KEY:
        temp=2;
        set1=1;
        break;
    default:
        if(set1==1)
        {
            timer1++;
            set1=0;
            if(temp==WhichBut1)
            {
                if(timer1>=2)
                {
                    timer1=0;
                    StemWork=OPT_STATE_IDLE;
                }
                else if(StemWork<OPT_STATE_SETMP1)
                {
                    timer1=0;
                    CtrSteam(0x00);
                    StemWork=OPT_STATE_IDLE;
                }
            }
        }
        break;
    }
}

void CheckCofButSet(void)
{
    static unsigned char SING,DUB=0;  	//按下标志位
    ADKEY = GetADKey();
    switch(ADKEY)
    {
    case AD_SIG_KEY://C1
        disCofLed=0x01;
        SING=1;
        break;
    case AD_DU_KEY://C2
        disCofLed=0x02;
        DUB=1;
        break;
    case AD_SET_KEY:
        KeyTime++;
        disCofLed=0x03;
        if(KeyTime>SW_LONG&&(StemWork<OPT_STATE_SETMP1||StemWork>OPT_STATE_SETMP2))
        {
            KeyTime=0;
            DisplayC(SetTemp,0);
            swWorkState=WORK_STATE_SET;
        }
        break;
    default:
        if(SING)
        {
            TimeCc=0;
            SING=0;
            WhichBut=3;
            SigFlag=0xAA;
            swWorkState=WORK_STATE_WAIT;
        }
        else if(DUB)
        {
            DUB=0;
            TimeCc=0;
            WhichBut=4;
            DubFlag=0xAA;
            swWorkState=WORK_STATE_WAIT;
        }
        else
        {
            KeyTime=0;
            disCofLed=0x03;
        }
        break;
    }
}
void CheckSteamButSet(void)
{
    static unsigned char TMP1,TMP2,CLEAR=0;  	//按下标志位
    ADKEY = GetADKey();
    switch(ADKEY)
    {
    case AD_TMP1_KEY://T1
        TMP1=1;
        disStemLed=0x01;
        KeyTime1++;
        if(KeyTime1>SW_LONG&&swWorkState<WORK_STATE_SET)
        {
            WhichBut1=1;
            TMP1=0;
            KeyTime1=0;
            DisplayZ(Temp1,0);
            StemWork=OPT_STATE_SETMP1;
        }
        break;
    case AD_TMP2_KEY://T2
        TMP2=1;
        disStemLed=0x02;
        KeyTime1++;
        if(KeyTime1>SW_LONG&&swWorkState<WORK_STATE_SET)
        {
            WhichBut1=2;
            TMP2=0;
            KeyTime1=0;
            DisplayZ(Temp2,0);
            StemWork=OPT_STATE_SETMP2;
        }
        break;
    case AD_CLEAR_KEY://清洗
        CLEAR=1;
        disStemLed=0x04;
        break;
    default:
        KeyTime1=0;
        if(TMP1)
        {
            TMP1=0;
            TimeCc1=0;
            ZetTmp=Temp1;
            WhichBut1=1;
            CtrSteam(0x01);
            StemWork=OPT_STATE_HEAD;
        }
        else if(TMP2)
        {
            TMP2=0;
            TimeCc1=0;
            ZetTmp=Temp2;
            WhichBut1=2;
            CtrSteam(0x01);
            StemWork=OPT_STATE_HEAD;
        }
        else if(CLEAR)
        {
            CLEAR=0;
            TimeCc1=0;
            CtrSteam(0x01);
            StemWork=OPT_STATE_CLERA;
        }
        else
        {
            disStemLed=0x07;
        }
        break;
    }
}

void ButAdjPTo(unsigned char Type,unsigned char Cal)
{
    switch(Type)
    {
    case TMP1B_VAL://蒸汽温度1
        if((Temp1<90)&&(Cal==1))
        {
            Temp1++;
        }
        else if((Temp1>50)&&(Cal==2))
        {
            Temp1--;
        }
        WriteTmp1BEeprom();
        DisplayZ(Temp1,0);
        break;
    case TMP2B_VAL://蒸汽温度2
        if((Temp2<90)&&(Cal==1))
        {
            Temp2++;
        }
        else if((Temp2>50)&&(Cal==2))
        {
            Temp2--;
        }
        WriteTmp2BEeprom();
        DisplayZ(Temp2,0);
        break;
    case SETTEMP://温度设置
        if((SetTemp<100)&&(Cal==1))
        {
            SetTemp++;
            COFE_LIMT=SetTemp;
            COFE_TMP=COFE_TMP+31;
        }
        else if((SetTemp>88)&&(Cal==2))
        {
            SetTemp--;
            COFE_LIMT=SetTemp;
            COFE_TMP=COFE_TMP+31;
        }
        WriteSetTempEeprom();
        DisplayC(SetTemp,0);
        break;
    }
}


void ButAdjPro(unsigned char Type)
{
    static unsigned char AddDelay=0;
    static unsigned char SubDelay=0;
    static unsigned char AddBFlag=0;
    static unsigned char SubBFlag=0;
    SW_ADD=1;
    if((SW_ADD==0x00)&&(AddBFlag==0xAA))
    {
        AddDelay++;
        if(AddDelay>SW_L)
        {
            AddDelay=0;
//            CtrBuz(0x01);
            AddBFlag=0x00;
            ButAdjPTo(Type,0x01);//表示当前参数增加
        }
    }
    else
    {
        AddBFlag=0xAA;
    }
    SW_SUB=1;
    if((SW_SUB==0x00)&&(SubBFlag==0xAA))
    {
        SubDelay++;
        if(SubDelay>SW_L)
        {
            ButAdjPTo(Type,0x02);//表示当前参数减少
//            CtrBuz(0x01);
            SubDelay=0;
            SubBFlag=0x00;
        }
    }
    else
    {
        SubBFlag=0xAA;
    }
}

//void SetModeLEDCtr1()
//{
//    static unsigned char temp=0;
//    if(TimeCc1>(ONES/2))
//    {
//        TimeCc1=0;
//        if(LedOnOff1==0)
//        {
//            temp = disStemLed;
//            disStemLed=0;
//            LedOnOff1=0x01;
//        }
//        else
//        {
//            disStemLed=temp;
//            LedOnOff1=0x00;
//        }
//    }
//}

//void SetModeLEDCtr()
//{
//    static unsigned char temp=0;
//    if(TimeCc>(ONES/2))
//    {
//        TimeCc=0;
//        if(LedOnOff==0)
//        {
//            temp = disCofLed;
//            disCofLed=0;
//            LedOnOff=0x01;
//        }
//        else
//        {
//            disCofLed=temp;
//            LedOnOff=0x00;
//        }
//    }
//}

void SetModeLEDCtr1(unsigned char which)
{
    unsigned char temp;
    if(which)
    {
        temp=Temp1;
    }
    else
    {
        temp=Temp2;
    }
    if(TimeCc1>(ONES/2))
    {
        TimeCc1=0;
        if(LedOnOff1==0)
        {
            DisplayZ(temp,0);
            LedOnOff1=0x01;
        }
        else
        {
            DisplayZ(0,4);
            LedOnOff1=0x00;
        }
    }
}

void SetModeLEDCtr(unsigned char onoff)
{
    if(onoff)
    {
        if(TimeCc>(ONES/2))
        {
            TimeCc=0;
            if(LedOnOff==0)
            {
                DisplayC(SetTemp,0);
                LedOnOff=0x01;
            }
            else
            {
                DisplayC(0,4);
                LedOnOff=0x00;
            }
        }
    }
}


void BuzCtr()
{
    static unsigned char On=0;
    if(swWorkState==WORK_STATE_DOWN||StemWork==OPT_STATE_DOWN)
    {
        return;
    }
    switch(ADKEY)
    {
    case AD_SIG_KEY:
    case AD_DU_KEY:
    case AD_TMP1_KEY:
    case AD_TMP2_KEY:
    case AD_CLEAR_KEY:
    case AD_SET_KEY:
        if(On==0)
        {
            On=1;
            CtrBuz(0x01);
        }
        break;
    default:
        On=0;
        break;
    }

}


//void RunningLamp()
//{
//    static unsigned char Down=0;
//    ADKEY = GetADKey();
//    switch(ADKEY)
//    {
//    case AD_SIG_KEY:
//        Down=1;
//        disCofLed=0x01;
//        break;
//    case AD_DU_KEY:
//        Down=1;
//        disCofLed=0x02;
//        break;
//    }
//    if(Down==0)
//    {
//        if(CtrLedTime<1*ONES)//灯交替闪烁
//        {
//            disCofLed=0x01;
//        }
//        else if(CtrLedTime<2*ONES)
//        {
//            disCofLed=0x02;
//        }
//        else
//        {
//            CtrLedTime=0;
//        }
//    }
//}

//做咖啡任务
void ProWorkState(void)
{
    switch(swWorkState)
    {
    case WORK_STATE_INIT://初始化
        GetCurTmp();
        CheckCoffTemp();
        disCofLed=0x03;
        if(CafTemp>COFE_LIMT)
        {
            DisplayC(COFE_LIMT,0);
        }
        else
        {
            DisplayC(CafTemp,0);
        }
        CtrBeng(0x01);//开水泵
        swWorkState=WORK_STATE_BENG;
        TimeCc=0;
        break;
    case WORK_STATE_BENG:
        if(TimeCc>(3*ONES))
        {
            if(WaterFlag==1)
            {
                CtrBeng(0x00);
            }
            TimeCc=0;
            swWorkState=WORK_STATE_RHEAT;
        }
        break;
    case WORK_STATE_RHEAT:
        if(BengCtrTime>0.5*ONES)
        {
            BengCtrTime=0;
            GetCurTmp();
        }
        if(CafTemp<COFE_TMP)//咖啡温度小于设定
        {
            if(CafTemp<85&&CofHeat==0)
            {
                CofHeat=1;
                CtrCoffeHeat(0x01);//全功率
            }
            else if(CafTemp>85)
            {
                if(TimeCc<3*ONES&&CofHeat==0)
                {
                    CofHeat=1;
                    CtrCoffeHeat(0x01);
                }
                else if(TimeCc<5*ONES&&CofHeat==1)
                {
                    CofHeat=0;
                    CtrCoffeHeat(0x00);
                }
                else
                {
                    TimeCc=0;
                }
            }
            if(CafTemp>COFE_LIMT)//实时显示温度值
            {
                DisplayC(COFE_LIMT,0);
            }
            else
            {
                DisplayC(CafTemp,0);
            }
            CheckCofButSet();
        }
        else
        {
            TimeCc=0;
            CtrCoffeHeat(0x00);
            swWorkState=WORK_STATE_RUN;
        }
        break;
    case WORK_STATE_RUN://等待操作
        if(CafTemp>COFE_LIMT)
        {
            DisplayC(COFE_LIMT,0);
        }
        else
        {
            DisplayC(CafTemp,0);
        }
        CheckCofButSet();
        break;
    case WORK_STATE_WAIT:
        CtrBeng(0x01);
        CtrK2(0x01);
        MakTim=0;
        TimeCc=0;
        CheckCofExit();
        swWorkState=WORK_STATE_PER;
        break;
    case WORK_STATE_PER:
        if(TimeCc>(1*ONES))
        {
            CtrBeng(0x00);//关水泵
            CtrK2(0x00);//关咖啡电磁阀
            swWorkState= WORK_STATE_PAO;
            TimeCc=0;
        }
        if(CafTemp>COFE_LIMT)
        {
            DisplayC(COFE_LIMT,0);
        }
        else
        {
            DisplayC(CafTemp,0);
        }

        CheckCofExit();
        break;
    case WORK_STATE_PAO:
        if(TimeCc>(YuPaoT*ONES))//浸泡时间
        {
            CtrBeng(0x01);//开启水泵
            CtrK2(0x01);
            swWorkState= WORK_STATE_OUT;
            if(SigFlag==0xAA)
            {

                MkCafeTime=22*ONES;
            }
            else if(DubFlag==0xAA)
            {

                MkCafeTime=37*ONES;
            }
            TimeCc=0;
        }
        if(CafTemp>COFE_LIMT)
        {
            DisplayC(COFE_LIMT,0);
        }
        else
        {
            DisplayC(CafTemp,0);
        }
        CheckCofExit();
        break;
    case WORK_STATE_OUT://出咖啡
        if(TimeCc>MkCafeTime)
        {
            CtrBeng(0x00);
            CtrK2(0x00);
            SigFlag=0x00;
            DubFlag=0x00;
            TimeCc=0;
            swWorkState= WORK_STATE_RUN;
        }
        if(CafTemp>COFE_LIMT)
        {
            DisplayC(COFE_LIMT,0);
        }
        else
        {
            DisplayC(CafTemp,0);
        }
        CheckCofExit();
        break;
    case WORK_STATE_SET://设置单杯时间
        SetModeLEDCtr(onoff);//动态显示
        ButAdjPro(SETTEMP);//参数值调整 加或者减
        CheckCofExit();//检查是否有杯数按键模式设置
        break;
    case WORK_STATE_ERR://系统错误
        PowerSleep();
        DisplayC(0,3);
        if(ErrCode==1)
        {
            if(TimeCc>0.5*ONES)
            {
                TimeCc=0;
                GetCurTmp();
            }
            if(CheckCoffTemp()==0)
            {
                swWorkState=WORK_STATE_INIT;
            }
        }
        break;
    case WORK_STATE_DOWN:
        PowerSleep();
        DisplayC(0,4);
        disCofLed=0;
        break;
    default:
        break;
    }
}

//蒸汽任务
void SteamWork(void)
{
    switch(StemWork)
    {
    case OPT_STATE_INIT:
        disStemLed=0x07;
        TimeCc1=0;
        StemWork=OPT_STATE_IDLE;
        break;
    case OPT_STATE_IDLE:
        if(WaterFlag==1)
        {
            if(SenTemp<35)
            {
                DisplayZ(0,1);
            }
            else
            {
                DisplayZ(SenTemp,0);
            }
        }
        else
        {
            DisplayZ(0,2);
        }
        CheckSteamButSet();
        break;
    case OPT_STATE_HEAD:
        if(TimeCc1>(0.5*ONES))
        {
            TimeCc1=0;
            GetSteamTmp();
        }
        if(SenTemp<35)
        {
            DisplayZ(0,1);
        }
        else
        {
            DisplayZ(SenTemp,0);
        }
        if(SenTemp>=ZetTmp)
        {
            CtrSteam(0x00);		//关闭出蒸汽阀
            StemWork=OPT_STATE_IDLE;
        }
        CheckStemExit();
        break;
    case OPT_STATE_CLERA:		//清洁
        if(TimeCc1>1.5*ONES)
        {
            TimeCc1=0;
            CtrSteam(0x00);
            StemWork=OPT_STATE_IDLE;
        }
        CheckStemExit();
        break;
    case OPT_STATE_SETMP1:
        SetModeLEDCtr1(1);//动态显示
        ButAdjPro(TMP1B_VAL);//参数值调整 加或者减
        CheckStemExit();
        break;
    case OPT_STATE_SETMP2:
        SetModeLEDCtr1(0);//动态显示
        ButAdjPro(TMP2B_VAL);//参数值调整 加或者减
        CheckStemExit();
        break;
    case OPT_STATE_ERR:
        DisplayZ(0,3);
        PowerSleep1();
        if(ErrCode1==2)
        {
            if(TimeCc1>0.5*ONES)
            {
                TimeCc1=0;
                GetSteamTmp();
            }
            if(CheckStemTemp()==0)
            {
                StemWork=OPT_STATE_INIT;
            }
        }
        break;
    case OPT_STATE_DOWN:
        PowerSleep1();
        DisplayZ(0,4);
        disStemLed=0;
        break;
    }
}

void UpdateLED(unsigned char disDat)
{
    if((disDat&0x01)>0)
    {
        LED_A=1;
    }
    else
    {
        LED_A=0;
    }
    if((disDat&0x02)>0)
    {
        LED_B=1;
    }
    else
    {
        LED_B=0;
    }
    if((disDat&0x04)>0)
    {
        LED_C=1;
    }
    else
    {
        LED_C=0;
    }
    if((disDat&0x08)>0)
    {
        LED_D=1;
    }
    else
    {
        LED_D=0;
    }
    if((disDat&0x10)>0)
    {
        LED_E=1;
    }
    else
    {
        LED_E=0;
    }
    if((disDat&0x20)>0)
    {
        LED_F=1;
    }
    else
    {
        LED_F=0;
    }
    if((disDat&0x40)>0)
    {
        LED_G=1;
    }
    else
    {
        LED_G=0;
    }
    if((disDat&0x80)>0)
    {
        LED_H=1;
    }
    else
    {
        LED_H=0;
    }
}
//显示位置                          0   1    2    3    4    5    6    7    8    9    P    E    C    -    0x0E   F    L
unsigned char xdata distable[18]= {0x3f,0x06,0x5b,0x4f,0x66,0x6d,0x7d,0x07,0x7f,0x6f,0x73,0x79,0x39,0x40,0x00,0x71,0x38,0xFF};
void FlashLed(void)
{
    static unsigned char dispos=0;
    unsigned char dat1;
    TimeCc++;
    TimeCc1++;
    AutCtrTime++;
    AutTempTime++;
    CofAutCtrTime++;
    StemAutCtrTime++;
    BengCtrTime++;
    MakTim++;
    LED_COM1=0;
    LED_COM2=0;
    LED_COM3=0;
    LED_COM4=0;
    LED_COM5=0;
    LED_COM6=0;
    LED_COM7=0;
    dispos++;
    switch(dispos)
    {
    case 1://表示咖啡显示第一位
        dat1=distable[Cafone];
        UpdateLED(dat1);
        LED_COM3=1;
        break;
    case 2://表示咖啡显示第二位
        dat1=distable[Caftwo];
        UpdateLED(dat1);
        LED_COM2=1;
        break;
    case 3://表示咖啡显示第三位
        dat1=distable[Cafthr];
        UpdateLED(dat1);
        LED_COM1=1;
        break;
    case 4://表示蒸汽显示第一位
        dat1=distable[Zheone];
        UpdateLED(dat1);
        LED_COM4=1;
        break;
    case 5://表示蒸汽显示第二位
        dat1=distable[Zhetwo];
        UpdateLED(dat1);
        LED_COM5=1;
        break;
    case 6://表示蒸汽显示第三位
        dat1=distable[Zhethr];
        UpdateLED(dat1);
        LED_COM6=1;
        break;
    case 7://按键灯显示
        LED_A=0;
        LED_B=0;
        LED_C=0;
        LED_D=0;
        LED_E=0;
        if((disCofLed&0x01)>0)
        {
            LED_E=1;
        }
        if((disCofLed&0x02)>0)
        {
            LED_D=1;
        }
        if((disStemLed&0x01)>0)
        {
            LED_C=1;
        }
        if((disStemLed&0x02)>0)
        {
            LED_B=1;
        }
        if((disStemLed&0x04)>0)
        {
            LED_A=1;
        }
        LED_COM7=1;
        break;
    default:
        dispos=0;
        break;
    }
}
void CheckTemp()//热电偶检测
{
    if(AutTempTime>0.5*ONES)
    {
        AutTempTime=0;
        GetSteamTmp();
    }
    CheckStemTemp();
}

unsigned char  CheckStemTemp(void)
{
    if((SenTemp<1)||(SenTemp>200))
    {
        ErrCode1=2;//表示热电偶故障
        StemWork=OPT_STATE_ERR;
        return 1;
    }
    return 0;
}

unsigned char  CheckCoffTemp(void)
{
    if((CafTemp<1)||(CafTemp>200))
    {
        ErrCode=1;//表示NTC故障
        swWorkState=WORK_STATE_ERR;
        return 1;
    }
    return 0;
}

void Start(void)
{
    Cafone=0x11;
    Caftwo=0x11;
    Cafthr=0x11;
    Zheone=0x11;
    Zhetwo=0x11;
    Zhethr=0x11;
    disCofLed =0x03;
    disStemLed =0x07;
}

void CheckPowerKey(void)//电源检测
{
    if(GetPower()==0x01)
    {
        if((PowerFlag==0xAA)&&(swWorkState==WORK_STATE_DOWN))
        {
            TimeCc=0;
            Start();
            Delay(5000);
            swWorkState=WORK_STATE_INIT;
            StemWork=OPT_STATE_INIT;
        }
        else if(PowerFlag==0)
        {
            StemWork=OPT_STATE_DOWN;
            swWorkState=WORK_STATE_DOWN;
        }
    }
}

void CofeTemCtrAuto(void)//咖啡煲温度自动控制
{
    if(CofAutCtrTime>0.5*ONES)
    {
        CofAutCtrTime=0;
        GetCurTmp();
        CheckCoffTemp();
    }
    if(swWorkState<WORK_STATE_SET&&swWorkState>WORK_STATE_RUN)
    {
        if(CofHeat==0)
        {
            CofHeat=1;
            CtrCoffeHeat(0x01);
        }
        else if(CofHeat==1)
        {
            CofHeat=0;
            CtrCoffeHeat(0x00);
        }
    }
    else if(swWorkState==WORK_STATE_RUN)
    {
        if(CofHeat==0&&CafTemp<COFE_TMP)
        {
            CofHeat=1;
            CtrCoffeHeat(0x01);
        }
        else if(CofHeat==1&&CafTemp>=COFE_TMP)
        {
            CofHeat=0;
            CtrCoffeHeat(0x00);
        }
    }
}

void SteamCtrAuto(void)//蒸汽水位和蒸汽煲温度实时控制
{

    static unsigned char ShuiTime=0;
    static unsigned char ShuiTime1=0;
    static unsigned char AddWater=0;
//***************************************************************
    if(StemAutCtrTime>(0.5*ONES))
    {
        StemAutCtrTime=0;
        if(StemWork<OPT_STATE_ERR)//表示要进行加热、检测温度、检测水位 循环控制
        {
            GetPop();
            if(PushFlag==0x00&&HeatState==0&&WaterFlag==1)//检测压力开关
            {
                HeatState=1;
                CtrZhenQI(0x01);
            }
            else if(PushFlag==0xAA&&HeatState==1)
            {
                HeatState=0;
                CtrZhenQI(0x00);
            }
            GetShuiWei();
            if(swWorkState>WORK_STATE_OUT&&swWorkState<WORK_STATE_RUN)
            {
                CtrK1(0x00);
                return;
            }
            switch(ShuiFlag)
            {
            case 1:	  //表示已经满水
                if(WaterFlag==0)
                {
                    ShuiTime1++;
                    if(ShuiTime1>BU_FULL)//补水10秒
                    {
                        AddWater=0;
                        WaterFlag=1;
                        ShuiTime1=0;
                        CtrK1(0x00);
                    }
                }
                else if(swWorkState>WORK_STATE_OUT||swWorkState==WORK_STATE_RHEAT||swWorkState==WORK_STATE_RUN)
                {
                    CtrBeng(0x00);
                }
                break;
            case 0:	//需要补水
                if(AddWater==0)
                {
                    ShuiTime=0;
                    AddWater=1;
                    WaterFlag=0;
                    CtrBeng(0x01);
                    CtrK1(0x01);
                }
                ShuiTime++;
                if(ShuiTime>BU_2MIN)//表示补水超过2分钟
                {
                    CtrK1(0x00);
                    AddWater=0;
                    ShuiTime=0;
                    ErrCode1=3;
                    StemWork=OPT_STATE_ERR;
                }
                break;
            default:
                break;
            }
        }
    }
}

void main(void)
{
    GPIO_Init();
    init_UART0();
    ADC_Init();
    TIMER0_initialize();
    EA   = 1;
    ReadTmp1BEeprom();
    ReadTmp2BEeprom();
    ReadSetTempEeprom();
    DisplayC(0,4);
    DisplayZ(0,4);
    while(1)
    {
        CheckPowerKey();
        ProWorkState();
        SteamWork();
        BuzCtr();
        SteamCtrAuto();
        CofeTemCtrAuto();
        CheckTemp();
        Delay(10);
    }
}
