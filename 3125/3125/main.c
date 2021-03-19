#include "OB38R08T1.h"
#include "Timer.h"
#include "UART0.h"
#include "WDT.h"
#include "ADC.h"

// ������ΪCRM3126��Դ��Ҫ����
//******************************************************
//AD0--->P04-->AN0  ģ���ź�����   NTC1  ����������¶� ʹ��
//AD1--->P05-->AN1  ģ���ź�����   NTC2	��⿧���¶�
//AD2--->P06-->AN2  ģ���ź�����   NTC3
//AD3--->P07-->AN3  ģ���ź�����   SHUI  �����ˮ��ˮ ʹ��
//PRS--->P17-->IN I/O �����ź����� PRS   ѹ����������
//WLS--->P02-->IN I/O ˮλ�ź����� WLS   ˮλ��������    ʹ�� ����ⲿˮ���Ƿ���ˮ
//POS--->P00-->IN I/O ��Դ�ź����� POS   ��Դ��������    ʹ��
//CS---->P03-->IN I/O �����ź����� CS    ���ȿ�������    ʹ��
//PLUS-->P35-->IN   �����ź�����   PLUS  �������ź�����
//LAMP-->P24-->OUT  �������       LAMP  �����ź����
//SCL--->P26
//SDA--->P25

//PWM--->P16-->OUT  �ɿع�1���
//CT2--->P01-->OUT  �ɿع�2���     ʹ��
//CK1--->P10-->OUT   �������
//CK2--->P11-->OUT   ���Ƚ�ˮ��ŷ�   ʹ��  E-CI
//CK3--->P23-->OUT   ���ȳ�ˮ��ŷ�
//CK4--->P30-->OUT   ��ˮ��ŷ�
//CK5--->P31-->OUT   ������ˮ��ŷ�  ʹ�� E-HW
//CK6--->P32-->OUT   ˮ�ÿ���        ʹ��WP
//BUZ--->P33-->OUT   ������
//TXD--->P14-->OUT   RS232 TXD
//RXD--->P15-->IN    RS232 RXD
//******************************************************

#define	PRS		P1_7   //��ȡѹ������
#define	WLS		P0_2   //����ⲿˮ���Ƿ���ˮ
#define POS     P0_0   //��Դ�ź�����
#define CS      P0_3   //�����ź�����
#define PLUS    P3_5   //��ȡ������
//******************************************************************
#define CS1   	P1_2
#define LAMP    P2_4
#define PWM     P1_6  //���ȱ�����
#define CT2     P0_1  //��������

#define CK1     P1_0  //�������
#define CK2     P1_1  //���Ƚ�ˮ��ŷ�  
#define CK3     P2_3  //���ȳ�ˮ��ŷ�
#define CK4     P3_0  //��ˮ��ŷ�
#define CK5     P3_1  //������ˮ��ŷ�
#define CK6     P3_2  //ˮ�ÿ���

#define RXD		P1_5//��������
#define TXD		P1_4//�������
#define SDA		P2_5
#define SCL		P2_6
#define M_SDA   P3_4
#define M_SCK   P1_3

#define BUZ     P3_3
//������9600
//���ò���
//1)����ʱ��ʾ����ʾNTCʵ���¶ȣ���SET����һ�Σ���PXX����˸��ͨ����+/-��������Ԥ����ʱ��(XX:��ʾԤ����ʱ��,
//  ���ڷ�Χ��00-30,��ʾԤ����ʱ����ڷ�Χ0-30��)
//2)�ٰ�һ��SET����,������ʾ��¯�¶�,ͨ����+/-�������¶�.���ڷ�Χ��80-135��.�ٰ�SET������
//  "ECX"��˸(ECX��ʾ����ʱ��,ECX���ڷ�Χ:EC1-EC5),ͨ����+/-������ѡ��EC1(ʱ��Ϊ28min).EC2(
//  ʱ��Ϊ60min).EC3(ʱ��Ϊ90min).EC4(ʱ��Ϊ180min)�Լ�EC5(�رս���ģʽ)
//3)�ٰ�SET����,��ʽ���빤��״̬,������ʱ,��ʾ�����ֱ�ʾ�����ȵ�ʱ��.
//4)�������������ģʽ����ʾ����ʾECO(�����õ���ֹͣ����),��һ��SET����,��ʾ��¯�¶�,��
//   ��һ��SET����,�ظ�������1.2.3 ���û����ٴι���.
//5) Ĭ�����ã�P02/125/EC1��
//ͨѶЭ��
// 1B 2A  ��ʾ��ͷ
// dir    ��ʾ���� 0x01��ʾ��Դ���������Ϸ��� 0x02 ��ʾ��ʾ�����·���
// cmd    ��ʾָ��
// data1  ��ʾ����1
// data2  ��ʾ����2
// checkSum1  У����1
// checkSum2  У����2

//����ָ��
//1B 2A   ��ʾ��ͷ
//dir     ��ʾ����
//cmd     ��ʾ�յ�ָ��
//data1   ��ʾ֮ǰ����
//data2    ��ȷ���ߴ��� 0x01��ʾ��ȷ  0x02��ʾ����
// checkSum1  У����1
// checkSum2  У����2
#define  ESC_01  		0x1B
#define  ESC_02  		0x2A

//**********************************************************
#define  CMD_CTMP  		0x01  //��ȡ�������¶�(NTC)
#define  CMD_ZTMP  		0x02  //��ȡ�������¶�(NTC)
#define  CMD_SHUI       0x03  //��ȡˮλ
#define  CMD_LIU        0x04  //��ȡ���� 
#define  CMD_COFE       0x05  //��ȡ���Ȱ��� 
#define  CMD_SHXI       0x06  //��ȡˮ��״̬
#define  CMD_POP        0x07  //��ʾѹ������״̬
#define  CMD_MS         0x08  //��ȡ��������
#define  CMD_STEMTMP    0x09  //��ȡ�����¶�(�ȵ�ż)

//************************************************************
#define  CMD_CTBENG   	0x10  //��ʾ���Ʊó�ˮ           CK6
#define  CMD_CTZHEN  	0x11  //��ʾ����������ˮ��       CK5
#define  CMD_CTHEAT  	0x12  //��ʾ���Ƴ�������ŷ�       CK4 
#define  CMD_CTCFE      0x13  //��ʾ���ƿ��ȳ�ˮ��ŷ�   CK3
#define  CMD_CTZSHUI 	0x14  //��ʾ���ƿ��Ƚ�ˮ��       CK2
#define  CMD_BK1 		0x15  //��ʾ���Ʊ���             CK1
#define  CMD_LAMP       0x16  //�ƿ��ƿ���               LAMP
#define  CMD_CAFEH      0x17  //���ƿ����Ҽ���           PWM
#define  CMD_ZHENH      0x18  //���������Ҽ���           CT2

#define  CMD_BUZ        0x19  //���Ʒ�����
#define  CMD_SLEEP      0x1A  //��ʾ��������
#define  CMD_PWR        0x20  //��Դ����
//***********************************************************


#define  P_TO_DIS   0x01 //��Դ���������Ϸ���
#define  DIS_TO_P   0x02 //��ʾ��ʾ�����Դ�巢��
#define  CMD_OK     0xAA //��ʾ�յ�ָ��ɹ�
#define  CMD_FAIL   0x55 //��ʾʧ��

#define JP_DEAFE    1
#define CAF_TEM     102
#define DUCUP       40
#define SINGCUP     25
#define HEATT       5

#define  AD_CH0    0x00
#define  AD_CH1    0x01
#define  AD_CH3    0x03

unsigned char 	xdata 	SenTemp=0;//�����¶��ȵ�ż
unsigned char   xdata   ErrCode=0;//�������
unsigned long   xdata   TimeCc=0;//״̬����ʱ
unsigned int    xdata   BuzTime=0;//����������ٴ�
unsigned int    xdata   LiuLiangC=0;//��������
unsigned char   xdata   RetOK=0;
unsigned char   xdata   ShuiXFlag=0;//0x00��ʾ��ˮ 0x01��ʾ��ˮ
unsigned char   xdata   PowerFlag=0;//��Դ��־
unsigned char   xdata   CaffeFlag=0;//���Ȱ�����־
unsigned char   xdata   PushFlag=0;//ѹ����������־
unsigned char 	xdata 	ShuiFlag=0;//ˮλ����
unsigned char  CafeTmp=0;
unsigned char  ZhenQiTmp=0;
unsigned char   LiuH=0;
unsigned char   LiuL=0;


//**********************************************

//ϵͳ�ػ�״̬
//ϵͳ�Լ�״̬ ȫ������ܳ�1�� ���NTC1 NTC2��· ��· �¶�С��-1���ߴ���170
//ˮ����ˮ8��
//
//����״̬
//ϵͳӲ������
//�������ʾNTC1 ��·��ʾE-1 ��·��ʾE-2 <-1����ʾE-3 >170����ʾE-4
//�������ʾNTC2 ��·��ʾE-5 ��·��ʾE-6 <-1����ʾE-7 >170����ʾE-8
//1.�ػ�״̬ ��ʾEC0
//2.ϵͳ�Լ�  �������NTC�Ƿ񿪶� ��·
//3.��ˮ8���� ֹͣ������һ������
//������������״̬
//4.�����ҿ�ʼ���� �����ҿ�ʼ���ȹ���
//5.�ж��������¶��Ƿ�ﵽ138�� �ﵽ
//6.�������Ƿ�ﵽ�趨�¶�ֵ ����PID���Ƽ���
//7.��ˮ���ֶ����Զ�������ͬһʱ��ֻ�ܵ���һ�����Թ���



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
//									1ms�ж�
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
// 0		0 	׼˫��
// 0		1	����
// 1		0	ֻ����
// 1		1	��©

void Init_GPIO(void)
{
    P0 = 0x00;
    P1 = 0x00;
    P2 = 0x00;
    P3 = 0x00;
//P0.0-->POS	-->��Դ����
//P0.1-->CT2	-->��������
//P0.2-->WLS	-->����ⲿˮ���Ƿ���ˮ
//P0.3-->CS		-->�����ź�����
//P0.4-->AD0	-->ģ���ź�����	����������¶�
//P0.5-->AD1	-->ģ���ź�����
//P0.6-->AD2	-->ģ���ź�����
//P0.7-->AD3	-->ģ���ź�����	�������ˮ
    P0M1 = 0xF0;
    P0M0 = 0x02;

//P1.0-->CK1	-->�������
//P1.1-->CK2	-->���Ƚ�ˮ��ŷ�
//P1.2-->SLEEP	-->
//P1.3-->AC_C	-->SCK1
//P1.4-->TXD	-->
//P1.5-->RXD	-->
//P1.6-->PWM	-->���ȱ�����
//P1.7-->PRS	-->��ȡѹ������
    P1M1 = 0x00;
    P1M0 = 0x4F;

//P2.3-->CK3	-->���ȳ�ˮ��ŷ�
//P2.4-->LAMP	-->
//P2.5-->SDA	-->
//P2.6-->SCL	-->
    P2M1 = 0x00;
    P2M0 = 0x18;

//P3.0-->CK4	-->��ˮ��ŷ�
//P3.1-->CK5	-->������ˮ��ŷ�
//P3.2-->CK6	-->ˮ�ÿ���
//P3.3-->BUZ	-->������
//P3.4-->SDA1
//P3.5-->PLUS	-->��ȡ������
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
            LAMP=1;//����
            PowerFlag=0xAA;
        }
    }
    else if(POS==1)
    {
        ButL=0;
        ButH++;
        if((PowerFlag==0xAA)&&(ButH>30))
        {
            LAMP=0;//�ص�
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
//����:����	(9600bps)
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


void  UART_Rxd_Process(void)//������ʾ���ָ��
{
    unsigned char xdata pcmd=0,sdata1=0,sdata2=0,schecksum1,schecksum2;
    unsigned short CheckSumDIS,CheckSumPo;
    if(GetBufSize()>=8)
    {
        if(GetBufData()==ESC_01)//��ȡ0x1B
        {
            if(GetBufData()==ESC_02)//��ȡ0x2A
            {
                if(GetBufData()==DIS_TO_P)//0x02 ��ʾ��ʾ������Դ��ָ��
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
                        case  CMD_CTMP://��ȡ�������¶�
                            CafeTmp=GetCafeTmp();//�������¶�
                            UART_TXD_Process(CMD_CTMP,CafeTmp,0x00);
                            break;
                        case CMD_ZTMP://��ȡ�������¶�
                            ZhenQiTmp=GetZhenQiTmp();//�������¶�
                            UART_TXD_Process(CMD_ZTMP,ZhenQiTmp,0x00);
                            break;
                        case CMD_STEMTMP://��ȡ�����¶��ȵ�ż
                            SenTemp=ReadStemTmp();
                            UART_TXD_Process(CMD_STEMTMP,SenTemp,0x00);
                            break;
                        case CMD_SHUI://��ȡˮλ
                            ShuiFlag=GetShuiLevel();
                            UART_TXD_Process(CMD_SHUI,ShuiFlag,0x00);
                            break;
                        case CMD_LIU://��ȡ����
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
                        case CMD_COFE://���Ȱ���
                            UART_TXD_Process(CMD_COFE,CaffeFlag,0x00);
                            break;
                        case CMD_PWR://ȷ���յ���Դ����
                            UART_TXD_Process(CMD_PWR,PowerFlag,0x00);
                            break;
                        case CMD_POP://��ȡѹ���������Ƿ�պ�
                            UART_TXD_Process(CMD_POP,PushFlag,0x00);
                            break;
                        case CMD_CTBENG://��ʾ���Ʊó�ˮ           CK6
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
                        case CMD_CTZHEN://��ʾ����������ˮ��       CK5
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
                        case CMD_CTHEAT://��ʾ����������ŷ�(��ˮ��)      CK4
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
                        case CMD_CTCFE://��ʾ���ƿ��ȳ�ˮ��ŷ�    CK3
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
                        case CMD_CTZSHUI://��ʾ���ƿ��Ƚ�ˮ��       CK2
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
                        case CMD_BK1://��ʾ���Ʊ���             CK1
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
                        case CMD_LAMP://�ƿ��ƿ���
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
                        case CMD_BUZ://������
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
//                        case CMD_SLEEP://����ģʽ
//                            if(sdata1==1)
//                            {
//                                PowerSleep();
//                            }
//                            else
//                            {
//                                SLEEP=1;//ȡ������
//                            }
//                            UART_TXD_Process(CMD_SLEEP,0x00,0x00);
//                            break;
                        case CMD_CAFEH://�����Ҽ���
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
                        case CMD_ZHENH://�����Ҽ���
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
                        case CMD_SHXI://ˮ��״̬
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
            ShuiXFlag=0x00;//��ʾ��ˮ
            ShH=0;
        }
    }
    else
    {
        ShH=0;
        ShL++;
        if(ShL>50)
        {
            ShuiXFlag=0x01;//��ʾ��ˮ
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
//									��ѭ��
//								-----------------------------------------
//===============================================================================================
void	main(void)
{
    EA =0;				//���ж�
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
    //��ѭ��
    while(1)
    {
        UART_Rxd_Process();//����ָ��
        GetPowerKey();
        BuzCtr();
        ShuiWLS();//ʵ�ʼ��ˮ��
        GetCofeKey();
        GetPopKey();//��ѯѹ������
        WDTR_CountClear();
        delay(10);
    }
}
