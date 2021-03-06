#include "OB38R08T1.h"
#include "Timer.h"
#include "UART0.h"
#include "WDT.h"
#include "ADC.h"

// 本代码为CRM3126电源主要代码
//******************************************************
//AD0--->P04-->AN0  模拟信号输入   NTC1  检测蒸汽煲温度 使用
//AD1--->P05-->AN1  模拟信号输入   NTC2	检测咖啡温度
//AD2--->P06-->AN2  模拟信号输入   NTC3
//AD3--->P07-->AN3  模拟信号输入   SHUI  检测有水无水 使用
//PRS--->P17-->IN I/O 开关信号输入 PRS   压力开关输入
//WLS--->P02-->IN I/O 水位信号输入 WLS   水位开关输入    使用 检测外部水箱是否有水
//POS--->P00-->IN I/O 电源信号输入 POS   电源开关输入    使用
//CS---->P03-->IN I/O 咖啡信号输入 CS    咖啡开关输入    使用
//PLUS-->P35-->IN   脉冲信号输入   PLUS  流量计信号输入
//LAMP-->P24-->OUT  控制输出       LAMP  控制信号输出
//SCL--->P26
//SDA--->P25

//PWM--->P16-->OUT  可控硅1输出
//CT2--->P01-->OUT  可控硅2输出     使用
//CK1--->P10-->OUT   备用输出
//CK2--->P11-->OUT   咖啡进水电磁阀   使用  E-CI
//CK3--->P23-->OUT   咖啡出水电磁阀
//CK4--->P30-->OUT   热水电磁阀
//CK5--->P31-->OUT   蒸汽进水电磁阀  使用 E-HW
//CK6--->P32-->OUT   水泵开关        使用WP
//BUZ--->P33-->OUT   蜂鸣器
//TXD--->P14-->OUT   RS232 TXD
//RXD--->P15-->IN    RS232 RXD
//******************************************************

#define	PRS		P1_7   //读取压力开关
#define	WLS		P0_2   //检测外部水箱是否有水
#define POS     P0_0   //电源信号输入
#define CS      P0_3   //咖啡信号输入
#define PLUS    P3_5   //读取流量计
//******************************************************************
#define CS1   	P1_2
#define LAMP    P2_4
#define PWM     P1_6  //咖啡杯加热
#define CT2     P0_1  //蒸汽加热

#define CK1     P1_0  //备用输出
#define CK2     P1_1  //咖啡进水电磁阀  
#define CK3     P2_3  //咖啡出水电磁阀
#define CK4     P3_0  //热水电磁阀
#define CK5     P3_1  //蒸汽进水电磁阀
#define CK6     P3_2  //水泵开关

#define RXD		P1_5//串口输入
#define TXD		P1_4//串口输出
#define SDA		P2_5
#define SCL		P2_6
#define M_SDA   P3_4
#define M_SCK   P1_3

#define BUZ     P3_3
//波特率9600
//设置参数
//1)开机时显示屏显示NTC实际温度，按SET按键一次，“PXX”闪烁，通过“+/-”来调整预浸泡时间(XX:表示预浸泡时间,
//  调节范围：00-30,表示预浸泡时间调节范围0-30秒)
//2)再按一下SET按键,数字显示锅炉温度,通过“+/-”调整温度.调节范围：80-135℃.再按SET按键，
//  "ECX"闪烁(ECX表示休眠时间,ECX调节范围:EC1-EC5),通过”+/-”可以选择EC1(时间为28min).EC2(
//  时间为60min).EC3(时间为90min).EC4(时间为180min)以及EC5(关闭节能模式)
//3)再按SET按键,正式进入工作状态,做咖啡时,显示屏数字表示做咖啡的时间.
//4)当机器进入节能模式，显示屏显示ECO(所有用电器停止工作),按一次SET按键,显示锅炉温度,再
//   按一次SET按键,重复上述的1.2.3 点让机器再次工作.
//5) 默认设置：P02/125/EC1。
//通讯协议
// 1B 2A  表示包头
// dir    表示方向 0x01表示电源板主机向上发送 0x02 表示显示屏向下发送
// cmd    表示指令
// data1  表示参数1
// data2  表示参数2
// checkSum1  校验码1
// checkSum2  校验码2

//返回指令
//1B 2A   表示包头
//dir     表示方向
//cmd     表示收到指令
//data1   表示之前参数
//data2    正确或者错误 0x01表示正确  0x02表示错误
// checkSum1  校验码1
// checkSum2  校验码2
#define  ESC_01  		0x1B
#define  ESC_02  		0x2A

//**********************************************************
#define  CMD_CTMP  		0x01  //读取咖啡煲温度(NTC)
#define  CMD_ZTMP  		0x02  //读取蒸汽煲温度(NTC)
#define  CMD_SHUI       0x03  //读取水位
#define  CMD_LIU        0x04  //读取流量 
#define  CMD_COFE       0x05  //读取咖啡按键 
#define  CMD_SHXI       0x06  //读取水箱状态
#define  CMD_POP        0x07  //表示压力开关状态
#define  CMD_MS         0x08  //获取蒸汽开关
#define  CMD_STEMTMP    0x09  //获取蒸汽温度(热电偶)

//************************************************************
#define  CMD_CTBENG   	0x10  //表示控制泵抽水           CK6
#define  CMD_CTZHEN  	0x11  //表示控制蒸汽进水阀       CK5
#define  CMD_CTHEAT  	0x12  //表示控制出蒸汽电磁阀       CK4 
#define  CMD_CTCFE      0x13  //表示控制咖啡出水电磁阀   CK3
#define  CMD_CTZSHUI 	0x14  //表示控制咖啡进水阀       CK2
#define  CMD_BK1 		0x15  //表示控制备用             CK1
#define  CMD_LAMP       0x16  //灯控制开关               LAMP
#define  CMD_CAFEH      0x17  //控制咖啡煲加热           PWM
#define  CMD_ZHENH      0x18  //控制蒸气煲加热           CT2

#define  CMD_BUZ        0x19  //控制蜂鸣器
#define  CMD_SLEEP      0x1A  //表示控制休眠
#define  CMD_PWR        0x20  //电源开关
//***********************************************************


#define  P_TO_DIS   0x01 //电源板主机向上发送
#define  DIS_TO_P   0x02 //表示显示板向电源板发送
#define  CMD_OK     0xAA //表示收到指令成功
#define  CMD_FAIL   0x55 //表示失败

#define JP_DEAFE    1
#define CAF_TEM     102
#define DUCUP       40
#define SINGCUP     25
#define HEATT       5

#define  AD_CH0    0x00
#define  AD_CH1    0x01
#define  AD_CH3    0x03

unsigned char 	xdata 	SenTemp=0;//蒸汽温度热电偶
unsigned char   xdata   ErrCode=0;//错误代码
unsigned long   xdata   TimeCc=0;//状态机计时
unsigned int    xdata   BuzTime=0;//蜂鸣器响多少次
unsigned int    xdata   LiuLiangC=0;//流量计数
unsigned char   xdata   RetOK=0;
unsigned char   xdata   ShuiXFlag=0;//0x00表示无水 0x01表示有水
unsigned char   xdata   PowerFlag=0;//电源标志
unsigned char   xdata   CaffeFlag=0;//咖啡按键标志
unsigned char   xdata   PushFlag=0;//压力传感器标志
unsigned char 	xdata 	ShuiFlag=0;//水位控针
unsigned char  CafeTmp=0;
unsigned char  ZhenQiTmp=0;
unsigned char   LiuH=0;
unsigned char   LiuL=0;


//**********************************************

//系统关机状态
//系统自检状态 全部数码管常1秒 检测NTC1 NTC2开路 短路 温度小于-1或者大于170
//水泵上水8秒
//
//休眠状态
//系统硬件错误
//数码管显示NTC1 开路显示E-1 短路显示E-2 <-1度显示E-3 >170度显示E-4
//数码管显示NTC2 开路显示E-5 短路显示E-6 <-1度显示E-7 >170度显示E-8
//1.关机状态 显示EC0
//2.系统自检  检查两个NTC是否开断 短路
//3.泵水8秒钟 停止进入下一级处理
//以下正常工作状态
//4.发热煲开始加热 蒸汽煲开始加热工作
//5.判断蒸汽煲温度是否达到138度 达到
//6.发热煲是否达到设定温度值 进行PID控制计算
//7.热水、手动、自动、蒸汽同一时间只能单独一个可以工作



extern	void 	delay(unsigned int n);

#define		isr_push_sfr	{_push_(	INSCON );INSCON = 0;_push_( FLASHCON );	FLASHCON = 0; }
#define		isr_pop_sfr		{INSCON = 0;_pop_(  FLASHCON );_pop_(	INSCON );}


typedef	bit				Bit;
typedef	bit				Bool;
typedef	unsigned char	Byte;
typedef	unsigned int	Word;
typedef	unsigned long	Dword;


#define	MAKEWORD(v1,v2)	(((Word)(v1)<<8) + (Word)(v2))
#define	HIBYTE(v1)		((Byte)((v1)>>8))
#define	LOBYTE(v1)		((Byte)((v1)&0xff))


unsigned char   EppVal[8];
extern void WriteFlash(Byte *ptr);
extern void	Flash_Of_CtrlPara(void);
extern void ReadFlash(Byte *ptr);
void ConfineSleepTime(void);
void BuzCtr(void);
void UART_TXD_Process(unsigned char cmd,unsigned char parm1,unsigned char parm2);
unsigned char GetBufSize(void);
unsigned char GetBufData(void);
unsigned char GetTempVal(unsigned char  chanel);
unsigned char GetShuiLevel(void);
void PowerSleep(void);
void BengCtr(unsigned char ctr);
unsigned char ReadStemTmp(void);

bit	gbFlag1ms;
void delay(unsigned int tim)
{
    unsigned int  i,j;
    for (i=0; i<tim; i++)
        for (j=0; j<100; j++)
        {

        }
}
void delayus(unsigned int tim)
{
    unsigned int i,j;
    for(i=0; i<tim; i++)
    {
        for(j=0; j<2; j++)
        {

        }
    }
}
void delayNP(unsigned int tim)
{
    unsigned int  i,j;
    for (i=0; i<tim; i++)
    {
        for (j=0; j<10; j++);
    }
}
//===============================================================================================
//									1ms中断
//								-----------------------------------------
//===============================================================================================
void BuzCtr(void)
{
    unsigned int xdata BuzTA=0;
    if(BuzTime>0)
    {
        for(BuzTA=0; BuzTA<BuzTime; BuzTA++)
        {
            BUZ=0;
            delayus(10);
            BUZ=1;
            delayus(10);
        }
        BuzTime=0;
    }
}
void BengCtr(unsigned char ctr)
{
    unsigned int xdata BuzTA=0;
    if(ctr==1)
    {
        CK6=1;
    }
    else
    {
        CK6=0;
    }
}


//PXM1    PXM0
// 0		0 	准双向
// 0		1	推挽
// 1		0	只输入
// 1		1	开漏

void Init_GPIO(void)
{
    P0 = 0x00;
    P1 = 0x00;
    P2 = 0x00;
    P3 = 0x00;
//P0.0-->POS	-->电源输入
//P0.1-->CT2	-->蒸汽加热
//P0.2-->WLS	-->检测外部水箱是否有水
//P0.3-->CS		-->咖啡信号输入
//P0.4-->AD0	-->模拟信号输入	检测蒸汽煲温度
//P0.5-->AD1	-->模拟信号输入
//P0.6-->AD2	-->模拟信号输入
//P0.7-->AD3	-->模拟信号输入	检测有无水
    P0M1 = 0xF0;
    P0M0 = 0x02;

//P1.0-->CK1	-->备用输出
//P1.1-->CK2	-->咖啡进水电磁阀
//P1.2-->SLEEP	-->
//P1.3-->AC_C	-->SCK1
//P1.4-->TXD	-->
//P1.5-->RXD	-->
//P1.6-->PWM	-->咖啡杯加热
//P1.7-->PRS	-->读取压力开关
    P1M1 = 0x00;
    P1M0 = 0x4F;

//P2.3-->CK3	-->咖啡出水电磁阀
//P2.4-->LAMP	-->
//P2.5-->SDA	-->
//P2.6-->SCL	-->
    P2M1 = 0x00;
    P2M0 = 0x18;

//P3.0-->CK4	-->热水电磁阀
//P3.1-->CK5	-->蒸汽进水电磁阀
//P3.2-->CK6	-->水泵开关
//P3.3-->BUZ	-->蜂鸣器
//P3.4-->SDA1
//P3.5-->PLUS	-->读取流量计
    P3M1 = 0x20;
    P3M0 = 0x0F;

    RXD=1;
    TXD=1;
    SCL=1;
    SDA=1;
    M_SDA=1;
    M_SCK=1;
    LAMP=0;
    PowerSleep();
}

void GetPowerKey(void)
{
    static unsigned char ButL=0,ButH=0;
    POS=1;
    if((POS==0)&&(PowerFlag==0x00))
    {
        ButL++;
        if(ButL>30)
        {
            LAMP=1;//开灯
            PowerFlag=0xAA;
        }
    }
    else if(POS==1)
    {
        ButL=0;
        ButH++;
        if((PowerFlag==0xAA)&&(ButH>30))
        {
            LAMP=0;//关灯
            PowerFlag=0x00;
        }
    }
}

void GetCofeKey(void)
{
    static unsigned char ButL=0,ButH=0;
    CS=1;
    if((CS==0)&&(CaffeFlag==0x00))
    {
        ButL++;
        if(ButL>30)
        {
            CaffeFlag=0xAA;
        }
    }
    else	if(CS==1)
    {
        ButL=0;
        ButH++;
        if((CaffeFlag==0xAA)&&(ButH>30))
        {
            CaffeFlag=0x00;
        }
    }
}

void GetPopKey(void)
{
    static unsigned char ButL=0,ButH=0;
    PRS=1;
    if(PRS==1)//
    {
        ButH++;
        if(ButH>50)
        {
            PushFlag=0xAA;
            ButH=0;
        }
    }
    else
    {
        ButH=0;
        ButL++;
        if(ButL>50)
        {
            PushFlag=0x00;
        }
    }
}

//===============================================================================================
//功能:串口	(9600bps)
//===============================================================================================

void UART_TXD_Process(unsigned char Cmd,unsigned char Parm1,unsigned char Parm2)
{
    unsigned short CheckSum=0;
    BufSizeClear();
    UART0_TX(ESC_01);
    UART0_TX(ESC_02);
    UART0_TX(P_TO_DIS);
    UART0_TX(Cmd);
    UART0_TX(Parm1);
    UART0_TX(Parm2);
    CheckSum=ESC_01+ESC_02+P_TO_DIS+Cmd+Parm1+Parm2;
    UART0_TX((CheckSum>>8)&0xFF);
    UART0_TX(CheckSum&0xFF);
}


void  UART_Rxd_Process(void)//处理显示相关指令
{
    unsigned char xdata pcmd=0,sdata1=0,sdata2=0,schecksum1,schecksum2;
    unsigned short CheckSumDIS,CheckSumPo;
    if(GetBufSize()>=8)
    {
        if(GetBufData()==ESC_01)//读取0x1B
        {
            if(GetBufData()==ESC_02)//读取0x2A
            {
                if(GetBufData()==DIS_TO_P)//0x02 表示显示屏到电源板指令
                {
                    pcmd=GetBufData();
                    sdata1=GetBufData();
                    sdata2=GetBufData();
                    schecksum1=GetBufData();
                    schecksum2=GetBufData();
                    CheckSumDIS=(schecksum1<<8)+schecksum2;
                    CheckSumPo=ESC_01+ESC_02+DIS_TO_P+pcmd+sdata1+sdata2;
                    if(CheckSumPo==CheckSumDIS)
                    {
                        switch(pcmd)
                        {
                        case  CMD_CTMP://读取咖啡煲温度
                            CafeTmp=GetCafeTmp();//咖啡煲温度
                            UART_TXD_Process(CMD_CTMP,CafeTmp,0x00);
                            break;
                        case CMD_ZTMP://读取蒸汽煲温度
                            ZhenQiTmp=GetZhenQiTmp();//咖啡煲温度
                            UART_TXD_Process(CMD_ZTMP,ZhenQiTmp,0x00);
                            break;
                        case CMD_STEMTMP://获取蒸汽温度热电偶
                            SenTemp=ReadStemTmp();
                            UART_TXD_Process(CMD_STEMTMP,SenTemp,0x00);
                            break;
                        case CMD_SHUI://读取水位
                            ShuiFlag=GetShuiLevel();
                            UART_TXD_Process(CMD_SHUI,ShuiFlag,0x00);
                            break;
                        case CMD_LIU://读取流量
                            if(sdata1==0)
                            {
                                LiuH=TH1;
                                LiuL=TL1;
                                UART_TXD_Process(CMD_LIU,LiuH,LiuL);
                            }
                            else
                            {
                                LiuH=TH1;
                                LiuL=TL1;
                                UART_TXD_Process(CMD_LIU,LiuH,LiuL);
                                TH1=0;
                                TL1=0;
                            }
                            break;
                        case CMD_COFE://咖啡按键
                            UART_TXD_Process(CMD_COFE,CaffeFlag,0x00);
                            break;
                        case CMD_PWR://确认收到电源开关
                            UART_TXD_Process(CMD_PWR,PowerFlag,0x00);
                            break;
                        case CMD_POP://读取压力传感器是否闭合
                            UART_TXD_Process(CMD_POP,PushFlag,0x00);
                            break;
                        case CMD_CTBENG://表示控制泵抽水           CK6
                            if(sdata1==1)
                            {
                                BengCtr(0x01);
                            }
                            else
                            {
                                BengCtr(0x00);
                            }
                            UART_TXD_Process(CMD_CTBENG,0x00,0x00);
                            break;
                        case CMD_CTZHEN://表示控制蒸汽进水阀       CK5
                            if(sdata1==1)
                            {
                                CK5=1;
                            }
                            else
                            {
                                CK5=0;
                            }
                            UART_TXD_Process(CMD_CTZHEN,0x00,0x00);
                            break;
                        case CMD_CTHEAT://表示控制蒸汽电磁阀(热水改)      CK4
                            if(sdata1==1)
                            {
                                CK4=1;
                            }
                            else
                            {
                                CK4=0;
                            }
                            UART_TXD_Process(CMD_CTHEAT,0x00,0x00);
                            break;
                        case CMD_CTCFE://表示控制咖啡出水电磁阀    CK3
                            if(sdata1==1)
                            {
                                CK3=1;
                            }
                            else
                            {
                                CK3=0;
                            }
                            UART_TXD_Process(CMD_CTCFE,0x00,0x00);
                            break;
                        case CMD_CTZSHUI://表示控制咖啡进水阀       CK2
                            if(sdata1==1)
                            {
                                CK2=1;
                            }
                            else
                            {
                                CK2=0;
                            }
                            UART_TXD_Process(CMD_CTZSHUI,0x00,0x00);
                            break;
                        case CMD_BK1://表示控制备用             CK1
                            if(sdata1==1)
                            {
                                CK1=1;
                            }
                            else
                            {
                                CK1=0;
                            }
                            UART_TXD_Process(CMD_BK1,0x00,0x00);
                            break;
                        case CMD_LAMP://灯控制开关
                            if(sdata1==1)
                            {
                                LAMP=1;
                            }
                            else
                            {
                                LAMP=0;
                            }
                            UART_TXD_Process(CMD_LAMP,0x00,0x00);
                            break;
                        case CMD_BUZ://蜂鸣器
                            if(sdata1==1)
                            {
                                BuzTime=500;
                            }
                            else if(sdata1==2)
                            {
                                BuzTime=1000;
                            }
                            else if(sdata1==3)
                            {
                                BuzTime=5000;
                            }
                            UART_TXD_Process(CMD_BUZ,0x00,0x00);
                            break;
//                        case CMD_SLEEP://休眠模式
//                            if(sdata1==1)
//                            {
//                                PowerSleep();
//                            }
//                            else
//                            {
//                                SLEEP=1;//取消休眠
//                            }
//                            UART_TXD_Process(CMD_SLEEP,0x00,0x00);
//                            break;
                        case CMD_CAFEH://咖啡煲加热
                            if(sdata1==1)
                            {
                                PWM=1;
                            }
                            else
                            {
                                PWM=0;//
                            }
                            UART_TXD_Process(CMD_CAFEH,0x00,0x00);
                            break;
                        case CMD_ZHENH://蒸汽煲加热
                            if(sdata1==1)
                            {
                                CT2=1;
                            }
                            else
                            {
                                CT2=0;//
                            }
                            UART_TXD_Process(CMD_ZHENH,0x00,0x00);
                            break;
                        case CMD_SHXI://水箱状态
                            UART_TXD_Process(CMD_SHXI,ShuiXFlag,0x00);
                            break;
                        default:
                            break;
                        }
                    }
                }
            }
        }
    }
}

void ShuiWLS(void)
{
    static unsigned char ShH=0;
    static unsigned char ShL=0;
    WLS=1;
    if(WLS==1)//
    {
        ShH++;
        if(ShH>50)
        {
            ShuiXFlag=0x00;//表示无水
            ShH=0;
        }
    }
    else
    {
        ShH=0;
        ShL++;
        if(ShL>50)
        {
            ShuiXFlag=0x01;//表示有水
        }
    }
}

void PowerSleep(void)
{
    PWM=0;
    CT2=0;
    CK1=0;
    CK2=0;
    CK3=0;
    CK4=0;
    CK5=0;
    CK6=0;
    BengCtr(0x00);
    CS1=1;
    PLUS=0;
    BUZ=1;

}
#define TMP_OFFSET  6
unsigned char ReadStemTmp(void)
{
    unsigned char i;
    unsigned int dat;
    i   = 0;
    dat = 0;
    CS1=0;
    M_SCK = 0;
    delayNP(200);
    for(i=0; i<16; i++)		//get D15-D0 from 6675
    {
        M_SCK = 1;
        delayNP(20);
        dat = dat<<1;
        if(M_SDA==1)
            dat = dat|0x01;
        M_SCK = 0;
        delayNP(20);
    }
    dat >>=3;
    CS1=1;
    return dat/4-TMP_OFFSET;
}

//===============================================================================================
//									主循环
//								-----------------------------------------
//===============================================================================================
void	main(void)
{
    EA =0;				//关中断
    Init_GPIO();
    init_UART0();
    ADC_Init();
    CafeTmp=GetCafeTmp();
    ZhenQiTmp=GetZhenQiTmp();
    WDTR_Init();
    TIMER1_initialize();
    ReadStemTmp();
    EA =1;
    BuzCtr();
    //主循环
    while(1)
    {
        UART_Rxd_Process();//处理指令
        GetPowerKey();
        BuzCtr();
        ShuiWLS();//实际检测水箱
        GetCofeKey();
        GetPopKey();//检询压力开关
        WDTR_CountClear();
        delay(10);
    }
}

