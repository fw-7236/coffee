/*--------------------------------------------------------------------------
Modify on 2018/03/30
--------------------------------------------------------------------------*/
#ifndef _OB38R08T1_H_
#define _OB38R08T1_H_

/*  BYTE Registers  */
sfr P0     = 0x80;
sfr P1     = 0x90;
sfr P2     = 0xA0;
sfr P3     = 0xB0;
sfr PSW    = 0xD0;
sfr ACC    = 0xE0;
sfr B      = 0xF0;
sfr SP     = 0x81;
sfr DPL    = 0x82;
sfr DPH    = 0x83;
sfr RCON   = 0x86;
sfr PCON   = 0x87;
sfr CKCON  = 0x8E;
sfr IFCON  = 0x8F;
sfr AUX    = 0x91;
sfr RSTS   = 0xA1;
sfr LVC    = 0xE6;
sfr SWRES  = 0xE7;
sfr TAKEY  = 0xF7;
sfr IP0    = 0xA9;
sfr IP1    = 0xB9;
sfr IE     = 0xA8;
sfr IP     = 0xA9;
sfr IEN0   = 0xA8;
sfr IEN1   = 0xB8;
sfr IEN2   = 0x9A;
sfr IRCON  = 0xC0;
sfr IRCON2 = 0x97;

//---------GPIO Type ----------------
sfr P0M0   = 0xD2;
sfr P0M1   = 0xD3;
sfr P1M0   = 0xD4;
sfr P1M1   = 0xD5;
sfr P2M0   = 0xD6;
sfr P2M1   = 0xD7;
sfr P3M0   = 0xDA;
sfr P3M1   = 0xDB;

//---------Timer---------------------
sfr TCON   = 0x88;
sfr T2CON  = 0xC8;
sfr TMOD   = 0x89;
sfr TL0    = 0x8A;
sfr TL1    = 0x8B;
sfr TH0    = 0x8C;
sfr TH1    = 0x8D;
sfr TL2    = 0xCC;
sfr TH2    = 0xCD;
sfr PFCON  = 0xD9;
sfr ENHIT  = 0xE5;
sfr INTDEG = 0xEE;
sfr16 T2   = 0xCC;

//----------CCU----------------------
sfr CCEN   = 0xC1;
sfr CCEN2  = 0xD1;
sfr CCCON  = 0xC9;
sfr CRCL   = 0xCA;
sfr CRCH   = 0xCB;
sfr CCL1   = 0xC2;
sfr CCH1   = 0xC3;
sfr CCL2   = 0xC4;
sfr CCH2   = 0xC5;
sfr CCL3   = 0xC6;
sfr CCH3   = 0xC7;
sfr16 CRC  = 0xCA;
sfr16 CC1  = 0xC2;
sfr16 CC2  = 0xC4;
sfr16 CC3  = 0xC6;

//----------PWM----------------------
sfr PWMC        = 0xB5;
sfr PWMMDL      = 0xCE;
sfr PWMMDH      = 0xCF;
sfr PWMD0L      = 0xBC;
sfr PWMD0H      = 0xBD;
sfr PWMD1L      = 0xBE;
sfr PWMD1H      = 0xBF;
sfr PWMD2L      = 0xB1;
sfr PWMD2H      = 0xB2;
sfr PWMD3L      = 0xB3;
sfr PWMD3H      = 0xB4;
sfr PWMC2       = 0xDD;
sfr PWMDT0      = 0xDE;
sfr PWMDT1      = 0xDF;
sfr16 PWMMD     = 0xCE;
sfr16 PWMD0     = 0xBC;
sfr16 PWMD1     = 0xBE;
sfr16 PWMD2     = 0xB1;
sfr16 PWMD3     = 0xB3;

//----------WDT----------------------
sfr WDTRC   = 0xB6;
sfr WDTIC   = 0xFF;
sfr WDTK    = 0xB7;

//----------ISP----------------------
sfr ISPFAH  = 0xE1;
sfr ISPFAL  = 0xE2;
sfr ISPFDL  = 0xE3;
sfr ISPFDH  = 0xEB;
sfr ISPFC   = 0xE4;

//----------ADC---------------------
sfr ADCC1   = 0xAB;
sfr ADCC2   = 0xAC;
sfr ADCDH   = 0xAD;
sfr ADCDL   = 0xAE;
sfr ADCCS   = 0xAF;
sfr ADCSH   = 0xEF;

//------------UART-------------------------
sfr SCON    = 0x98;
sfr SBUF    = 0x99;
sfr S0CON   = 0x98;
sfr S0BUF   = 0x99;
sfr S0RELL  = 0xAA;
sfr S0RELH  = 0xBA;
sfr SRELL   = 0xAA;
sfr SRELH   = 0xBA;

//-------------IIC----------------------
sfr IICS    = 0xF8;
sfr IICCTL  = 0xF9;
sfr IICA1   = 0xFA;
sfr IICA2   = 0xFB;
sfr IICRWD  = 0xFC;
sfr IICEBT  = 0xFD;

//----------Touch-------------------
sfr TKEN0      = 0x93;
sfr TKEN1      = 0x94;
sfr TKCON      = 0x9B;
sfr TKRUNTIME  = 0x9C;
sfr TKCHN      = 0x9D;
sfr TKCDL      = 0x9E;
sfr TKCDH      = 0x9F;
sfr16 TKCD_16  = 0x9E;
sfr TKSTATUS0  = 0xBB;
sfr TKSTATUS1  = 0x85;
sfr TKPSSR     = 0xF2;
sfr TKWKTRICNT = 0xF3;
sfr TKSW       = 0xFE;

//---------Xdata -----------
#define pTKTRIG_16  ((unsigned int volatile xdata*)0x01D0)
#define TK0TRIG_16  (*((unsigned int volatile xdata*)0x01D0))
#define TK1TRIG_16  (*((unsigned int volatile xdata*)0x01D2))
#define TK2TRIG_16  (*((unsigned int volatile xdata*)0x01D4))
#define TK3TRIG_16  (*((unsigned int volatile xdata*)0x01D6))
#define TK4TRIG_16  (*((unsigned int volatile xdata*)0x01D8))
#define TK5TRIG_16  (*((unsigned int volatile xdata*)0x01DA))
#define TK6TRIG_16  (*((unsigned int volatile xdata*)0x01DC))
#define TK7TRIG_16  (*((unsigned int volatile xdata*)0x01DE))
#define TK8TRIG_16  (*((unsigned int volatile xdata*)0x01E0))
#define TK9TRIG_16  (*((unsigned int volatile xdata*)0x01E2))
#define TK10TRIG_16 (*((unsigned int volatile xdata*)0x01E4))
#define TK11TRIG_16 (*((unsigned int volatile xdata*)0x01E6))
#define TK12TRIG_16 (*((unsigned int volatile xdata*)0x01E8))
#define TK13TRIG_16 (*((unsigned int volatile xdata*)0x01EA))
#define TK14TRIG_16 (*((unsigned int volatile xdata*)0x01EC))
#define TK15TRIG_16 (*((unsigned int volatile xdata*)0x01EE))

#define pTKTRICNT   ((unsigned char volatile xdata*)0x01F0)
#define TK0TRICNT   (*((unsigned char volatile xdata*)0x01F0))
#define TK1TRICNT   (*((unsigned char volatile xdata*)0x01F1))
#define TK2TRICNT   (*((unsigned char volatile xdata*)0x01F2))
#define TK3TRICNT   (*((unsigned char volatile xdata*)0x01F3))
#define TK4TRICNT   (*((unsigned char volatile xdata*)0x01F4))
#define TK5TRICNT   (*((unsigned char volatile xdata*)0x01F5))
#define TK6TRICNT   (*((unsigned char volatile xdata*)0x01F6))
#define TK7TRICNT   (*((unsigned char volatile xdata*)0x01F7))
#define TK8TRICNT   (*((unsigned char volatile xdata*)0x01F8))
#define TK9TRICNT   (*((unsigned char volatile xdata*)0x01F9))
#define TK10TRICNT  (*((unsigned char volatile xdata*)0x01FA))
#define TK11TRICNT  (*((unsigned char volatile xdata*)0x01FB))
#define TK12TRICNT  (*((unsigned char volatile xdata*)0x01FC))
#define TK13TRICNT  (*((unsigned char volatile xdata*)0x01FD))
#define TK14TRICNT  (*((unsigned char volatile xdata*)0x01FE))
#define TK15TRICNT  (*((unsigned char volatile xdata*)0x01FF))
//---------ADC---------------
#define ADCCAL (*((unsigned char volatile xdata*)0xFFFF))

//---------LED---------------
#define COMEN       (*((unsigned char volatile xdata*)0xFF1C))
#define SEGEN0      (*((unsigned char volatile xdata*)0xFF1D))
#define SEGEN1      (*((unsigned char volatile xdata*)0xFF1E))
#define LEDCON      (*((unsigned char volatile xdata*)0xFF1F))
#define LEDCLK      (*((unsigned char volatile xdata*)0xFF20))

#define COM0_A_16 (*((unsigned int volatile xdata*)0xFF00))
#define COM1_A_16 (*((unsigned int volatile xdata*)0xFF02))
#define COM2_A_16 (*((unsigned int volatile xdata*)0xFF04))
#define COM3_A_16 (*((unsigned int volatile xdata*)0xFF06))
#define COM4_A_16 (*((unsigned int volatile xdata*)0xFF08))
#define COM5_A_16 (*((unsigned int volatile xdata*)0xFF0A))
#define COM6_A_16 (*((unsigned int volatile xdata*)0xFF0C))
#define COM0_C_16 (*((unsigned int volatile xdata*)0xFF0E))
#define COM1_C_16 (*((unsigned int volatile xdata*)0xFF10))
#define COM2_C_16 (*((unsigned int volatile xdata*)0xFF12))
#define COM3_C_16 (*((unsigned int volatile xdata*)0xFF14))
#define COM4_C_16 (*((unsigned int volatile xdata*)0xFF16))
#define COM5_C_16 (*((unsigned int volatile xdata*)0xFF18))
#define COM6_C_16 (*((unsigned int volatile xdata*)0xFF1A))
#define pCOM_A_16  ((unsigned int volatile xdata*)0xFF00)
#define pCOM_C_16  ((unsigned int volatile xdata*)0xFF0E)

//---------Xdata End-----------


sbit ACC0   = ACC^0;
sbit ACC1   = ACC^1;
sbit ACC2   = ACC^2;
sbit ACC3   = ACC^3;
sbit ACC4   = ACC^4;
sbit ACC5   = ACC^5;
sbit ACC6   = ACC^6;
sbit ACC7   = ACC^7;

sbit P0_0   = P0^0;
sbit P0_1   = P0^1;
sbit P0_2   = P0^2;
sbit P0_3   = P0^3;
sbit P0_4   = P0^4;
sbit P0_5   = P0^5;
sbit P0_6   = P0^6;
sbit P0_7   = P0^7;

sbit P1_0   = P1^0;
sbit P1_1   = P1^1;
sbit P1_2   = P1^2;
sbit P1_3   = P1^3;
sbit P1_4   = P1^4;
sbit P1_5   = P1^5;
sbit P1_6   = P1^6;
sbit P1_7   = P1^7;

sbit P2_3   = P2^3;
sbit P2_4   = P2^4;
sbit P2_5   = P2^5;
sbit P2_6   = P2^6;
sbit P3_0   = P3^0;
sbit P3_1   = P3^1;
sbit P3_2   = P3^2;
sbit P3_3   = P3^3;
sbit P3_4   = P3^4;
sbit P3_5   = P3^5;

sbit EA     = IEN0^7;
sbit ET2    = IEN0^5;
sbit ES     = IEN0^4;
sbit ET1    = IEN0^3;
sbit EX1    = IEN0^2;
sbit ET0    = IEN0^1;
sbit EX0    = IEN0^0;

sbit EXEN2  = IEN1^7;
sbit IEIIC  = IEN1^5;
sbit IELVI  = IEN1^4;
sbit IEADC  = IEN1^2;
sbit IEPWM  = IEN1^0;

sbit EXF2   = IRCON^7;
sbit TF2    = IRCON^6;
sbit IICIF  = IRCON^5;
sbit LVIIF  = IRCON^4;
sbit ADCIF  = IRCON^2;
sbit PWMIF  = IRCON^0;

sbit SM0    = SCON^7;
sbit SM1    = SCON^6;
sbit SM2    = SCON^5;
sbit REN    = SCON^4;
sbit TB8    = SCON^3;
sbit RB8    = SCON^2;
sbit TI     = SCON^1;
sbit RI     = SCON^0;


/* PSW */
sbit CY     = PSW^7;
sbit AC     = PSW^6;
sbit F0     = PSW^5;
sbit RS1    = PSW^4;
sbit RS0    = PSW^3;
sbit OV     = PSW^2;
sbit F1     = PSW^1;
sbit P      = PSW^0;

/*  TCON  */
sbit TF1    = TCON^7;
sbit TR1    = TCON^6;
sbit TF0    = TCON^5;
sbit TR0    = TCON^4;
sbit IE1    = TCON^3;
sbit IT1    = TCON^2;
sbit IE0    = TCON^1;
sbit IT0    = TCON^0;

//-------------------------------------------
sbit MPIF   = IICS^6;
sbit LAIF   = IICS^5;
sbit RXIF   = IICS^4;
sbit TXIF   = IICS^3;
sbit RXAK   = IICS^2;
sbit TXAK   = IICS^1;
sbit RW     = IICS^0;
sbit BB     = IICS^0;
//---------------------------------------------
#define dCOM0       P1_5
#define dCOM1       P1_4
#define dCOM2       P1_3
#define dCOM3       P1_2
#define dCOM4       P0_0
#define dCOM5       P0_1
#define dCOM6       P2_4

#define dSEG0_0     P0_0
#define dSEG1_0     P0_1
#define dSEG0_1     P3_0
#define dSEG1_1     P3_1
#define dSEG2       P0_2
#define dSEG3       P0_3
#define dSEG4       P0_4
#define dSEG5       P0_5
#define dSEG6       P0_6

#define dSEG7       P0_7
#define dSEG8       P1_0
#define dSEG9       P1_1
#define dSEG10      P2_3
#define dSEG11      P3_2
#define dSEG12      P3_3
#define dSEG13      P3_4
#define dSEG14      P1_6
#define dSEG15      P1_7


//Interrupt Define
#define        d_INT0_Vector    0       // 0003h
#define        d_T0_Vector      1       // 000Bh
#define        d_INT1_Vector    2       // 0013h
#define        d_T1_Vector      3       // 001Bh
#define        d_UART_Vector    4       // 0023h
#define        d_UART0_Vector   4       // 0023h
#define        d_T2_Vector      5       // 002Bh
#define        d_PWM_Vector     8       // 0043h
#define        d_ADC_Vector     10      // 0053h
#define        d_LVI_Vector     12      // 0063h
#define        d_IIC0_Vector    13      // 006Bh
#define        d_IIC_Vector     13      // 006Bh
#define        d_WDT_Vector     17      // 008Bh
#define        d_TK_Vector      19      // 009Bh


#endif
    