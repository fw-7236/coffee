#include "OB38R08A1.h"
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
    OPT_STATE_ERR2     =    10,//��ˮ����2����
}WorkState;

WorkState  swWorkState=OPT_STATE_INIT;//����״̬��

bit  HeatFlag;//���ȱ��ֵ
bit  TempFlag;//�¶�״̬

unsigned char xdata SenTemp=0;
unsigned int xdata  ShuiTime=0;//��ˮʱ�����

unsigned int xdata MkCafeTime=0;
unsigned int xdata ZhenTime=0;
unsigned char xdata PowerState=0;
unsigned char xdata LedOnOFF=0;


#define FA_RA   100

#define TAR_CAFE    95
#define TAR_ZHEN    110
unsigned char xdata TagTmp=0;//�����¶Ȼ��������¶�ֵ

unsigned int AutCtrTime=0;
unsigned int TimeCc=0;

unsigned char Lamp=0;
unsigned char Temp1=40;//��¼T1�¶�ֵ
unsigned char Temp2=60;//��¼T2�¶�ֵ
unsigned char HeadMaxTmp=0;
unsigned char ShuiFlag=0;
unsigned int KeyTime=0;
unsigned char KeyFlag=0;
unsigned char KeyBut=0;
unsigned char HeatFa=0;
unsigned char ErrCode=0;
unsigned char StartMach=0xAA;
unsigned int DelayTime=0;

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

#define DAFETMP  60    //�ָ���Ĭ���¶�

#define KeyLong   20
#define KeyNor    0
#define STMPMAX   90
#define STMPMIN   40

#define TMELOW    35
#define SW_L  1
#define SW_LONG  280
#define SW_LONGA  50


void Display(unsigned char da1,unsigned char da2,unsigned char da3,unsigned char da4);
void CheckPowerKey(void);
unsigned char GetMsKey(void);
void BuzCtr(unsigned int BuzTime);
void ADC_Init(void);

#define  ADD_SW     0x06 //��ʾ��1����
#define  SUB_SW     0x01 //��ʾ��1����
#define  CLEAR_SW   0x02 //��ʾ��ఴ��
#define  TMP1_SW    0x03 //��ʾ�¶�1����
#define  TMP2_SW    0x04 //��ʾ�¶�1����
#define  START_SW   0x05 //��ʾ��������

#define  START_LED  0x10
//******************************************************************************
void Delay(unsigned int tim)
{
    unsigned int  i,j;
    for (i=0; i<tim; i++)
    for (j=0; j<100; j++);
}


#define  BUZ            P0_6

#define  CT1_S         P1_0   //ˮ�õ������  IO3 
#define  CT1_Z         P0_5   //�������ȿ��ƻ��߼��ȿɿع����	 PWM0
#define  CT1_C         P1_1  //��ŷ����ؿ���

#define  LEVE1         P0_7 //ADC7  ˮλ��������
#define  SCR1          P1_4 //��̬�̵������� ����
#define  P_SW          P3_0 //ѹ������

#define  CS             P3_1
#define  SCK            P1_6
#define  SDA            P1_5 

#define  SPI_STB        P0_4
#define  SPI_CLK        P0_3
#define  SPI_DA         P0_6

//*******************************************************************
// 00 ��ʾ˫��IO 
// 01 ��ʾ�������
// 10 ��ʾ��������
// 11 ��ʾ��©
void GPIO_Init()
{
		P0 = 0x00;
		P1 = 0x00;
		P3 = 0x00;
//P0.2 P0.3 P0.4 P0.5 P0.6 P0.7 ���ݴ������� ׼˫��� �����
//P0.0 P0.1 ΪLEDCOM1 LEDCOM2
//      7  6  5  4  3  2  1  0
//P0M1  0  0  0	 0	0  0  0	 0
//P0M0  0  0  0	 0	0  0  1	 1
		P0M1 = 0x80;
		P0M0 = 0xA8;
//      7  6  5  4  3  2  1  0
//P1M1  0  0  1	 1	0  1  1	 0
//P1M0  1  1  0	 0	1  0  0	 0
		P1M1 = 0x00;
		P1M0 = 0xDF;
//P2.3 ��� P2.5 ��� P2.6����
//      7  6  5  4  3  2  1  0
//P2M1  0  0  0	 0	0  0  0	 0
//P2M0  0  0  0	 0	1  0  0	 0
	   
//P3.0 P3.1 P3.2Ϊ���������� P3.3 P3.4 P3.5Ϊ���
//      7  6  5  4  3  2  1  0
//P3M1  0  0  0	 0	0  0  0	 0
//P3M0  0  0  1	 1	1  0  0	 0
//******************************************************
		P3M1 = 0x00;
		P3M0 = 0x0E;
//********************************************************
       
        BUZ=1;
        CT1_S=0;
        CT1_Z=0; //PWM0
		CT1_C=0;
        P_SW=1;
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
/**************************************************************/
//���ܣ���TM1628����8λ����
/**************************************************************/
void send_8bit(unsigned char dat)
{
	unsigned char i;
	for(i=0;i<8;i++)
	{
		if(dat&0x01)
		SPI_DA=1;
		else SPI_DA=0;
		SPI_CLK=0;
		delayNP(2);
		SPI_CLK=1;
		delayNP(2);
		dat=dat>>1;
	}
}

unsigned char read_8bit(void)//�½������
{
	unsigned char xdata reData,i;
	 SPI_DA=1;
	 reData=0;
	for(i=0;i<8;i++)
	{
		SPI_CLK=0;
		delayNP(2);
		SPI_CLK=1;
		delayNP(2);
		reData=reData>>1;
		if(SPI_DA)
		{
	     	reData=reData|0x80;
	     }
	}
	return reData;
}



//************************************************************************************************************************
unsigned int MAX6675_ReadReg(void)
{ 
	unsigned char i;   
	unsigned int dat;
	i   = 0;   
	dat = 0; 
	SCK = 0; 
	CS=0;
	Delay(20);  
	for(i=0; i<16; i++)		//get D15-D0 from 6675  
	{      
		SCK = 1;    
		delayNP(20); 
		dat = dat<<1;    
		if( SDA==1 )   
		dat = dat|0x01;   
		SCK = 0;    
		delayNP(20);
	} 
	CS=1;
	return dat; 
} 
//************************************************************************************
unsigned char GetCurTemp(void)
{
	unsigned int MAX6675_Temp=0;
	unsigned int CurTemCal=0;
	unsigned int t=0;
	  t=MAX6675_ReadReg();
   	  MAX6675_Temp = t/4;				//��õ��¶ȵ�λ��0.25������Ҫ����0.25��������4�����ܵõ��Զ�Ϊ��λ���¶�ֵ
	return MAX6675_Temp;
}
//*********************************************************************************************************************
//��������                                                                          0x0A  0x0b                0x0f 0x10
//��������
unsigned char const CODE[][2] =
//   0           1         2            3          4            5		    6
{{0x3F,0x00},{0x06,0x00},{0x5b,0x00},{0x4f,0x10},{0x66,0x00},{0x6d,0x00},{0x7D,0x00},
//   7           8     		 9		       E          L            o		�� 0x0D	  0x0E
{0x07,0x00},{0x7f,0x00},{0x6f,0x00},{0x79,0x00},{0x38,0x00},{0x5c,0x00},{0x00,0x00},{0xFF,0x00}}; //4λ8��ģʽ�£����������0-9�ı���


/*****************����ָ��*************************************/
void command(unsigned char com)
{
   SPI_STB=1;
   Delay(2);
   SPI_STB=0;
   send_8bit(com);
}
//*******************************************************************************************************
/**************************************************************/
//���ܣ���1-4λ�������ʾ����0-3
/**************************************************************/
void Display(unsigned char da1,unsigned char da2,unsigned char da3,unsigned char da4)
{
	static unsigned char xdata sda1=0;
   	static unsigned char xdata sda2=0;
   	static unsigned char xdata sda3=0;
   	static unsigned char xdata sda4=0;
   	if((sda1==da1)&&(sda2==da2)&&(sda3==da3)&&(sda4==da4))
   	return;
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
    unsigned char xdata Key1,Key2,Key3,Key4,Key5;
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

void TimCal(void)
{
   TimeCc++;
   if(MkCafeTime<60000)
   MkCafeTime++;
   AutCtrTime++;
   WDT_CountClear();
}

//*******************************************************************************
//���ط����߲����� �����������׸���
#define TMP_OFFSET  0
void FlashTempVal(void)
{
    unsigned int t=0;
	static unsigned char  CurSenErr=0;
	unsigned char Flag_connect=0;
	t=MAX6675_ReadReg();
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


extern unsigned char CheckShuiState(void); 
#define ZHEQITMP  130
#define CAFETMP   124
#define OFFSET_TMP  7

#define ZQVALL  120
#define BU_2MIN  234  //120��ʱ��
#define BU_FULL  20 //��ˮʱ�� ��⵽ˮ֮��10��
#define BEN_RA   20 //��ˮ�ø���
#define ZQVALH  125

#define SHUI_FULL  0
#define SHUI_EMPT  1
void TemControlAuto(void)
{
     unsigned char TemVal=0;
     unsigned char ShuiResult=0;
     unsigned int CalTemm=0;
     static unsigned char ShuiErrTime=0;
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
	  	else if(ShuiFlag==0)  //��ʾ�����¶ȵ���130�� ��ʼ����
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
	   		ShuiErrTime=0;
		break;
		case SHUI_EMPT://��Ҫ��ˮ
	  	if((ShuiFlag==0)&&((swWorkState==OPT_STATE_INIT)||(swWorkState==OPT_STATE_IDLE)))
	  	{
	   		CT1_S=1;//������ˮ
	   		Delay(BEN_RA);//��ʱ �����źŸ���
	   		ShuiFlag=1;
	   		ShuiTime=0;
	  		Display(0x00,0x00,0x00,0x00);//
	  	}
	  	ShuiTime++;
	  	Delay(100);
	  	ShuiErrTime=0;
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
void  ExidWorkJob(void)
{
	
}
void CafeWorkDoJob(void)
{
	
}
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
#define DIS_DLY 170
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
		Delay(DIS_DLY);
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
		Delay(DIS_DLY);
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
		    if(ShuiFlag==0)//��ʾ���ڲ�ˮ
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
						Delay(FA_RA);
				  }
				  else
				  {
				        swWorkState=OPT_STATE_HEAD2;
				        KeyTime=0;
						MkCafeTime=0;
						DelayTime=0;
				        CT1_C=1;
						Delay(FA_RA);
			     }
			  }
			  else if((RButKey==CLEAR_SW)&&(KeyFlag==0))//��ʾ���
			  {
			  	
				    CT1_C=1;
					Delay(FA_RA);
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
				   Delay(FA_RA);
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
				   Delay(FA_RA);
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
				   Delay(FA_RA);
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
				      Delay(FA_RA);
				      swWorkState=OPT_STATE_IDLE;
				   }	  
			   }
			   else
			   {
			      KeyTime=0;
				 if(DelayTime>360)//��ʾ��ʱ�˳�
			  	 {
				   CT1_C=0;
				   Delay(FA_RA);
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
					  Delay(FA_RA);
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
					  Delay(FA_RA);	
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
				  Delay(FA_RA);
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
				  Delay(FA_RA);
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
	      default:
		       swWorkState=OPT_STATE_INIT;   
          break;
   	
   }
}

extern void TIMER0_initialize(void);
void main(void)
{ 
     LVC=0x20;
        GPIO_Init();
	TIMER0_initialize();
	WDT_initialize();
	ADC_Init();
	EA = 1;
	SDA=1;
	ReadTmp1Eeprom();
	ReadTmp2Eeprom();
	Display(0x0E,0x0E,0x0E,0xFF); //0x10 START  0x02 CLEAR 0x04 TMP1 0x08 TMP2 
	//
    	while(1)
    	{
			TemControlAuto();
	        ProWorkState();
			WDT_CountClear();
	        Delay(20);
        }
}  	
