/*--------------------------------------------------------------------------
Header file for OB38R08A1 microcontroller.
Modify on 2017/03/01
--------------------------------------------------------------------------*/
#ifndef __OB38R08A1_H__
#define __OB38R08A1_H__

/*  BYTE Registers  */
sfr P0      = 0x80;
sfr P1      = 0x90;
sfr P3      = 0xB0;
sfr PSW     = 0xD0;
sfr ACC     = 0xE0;
sfr B       = 0xF0;
sfr SP      = 0x81;
sfr DPL     = 0x82;
sfr DPH     = 0x83;

sfr PCON    = 0x87;    /*  SMOD,X,X,X,X,X,STOP,IDLE  */
sfr TCON    = 0x88;
sfr TMOD    = 0x89;
sfr TL0     = 0x8A;
sfr TL1     = 0x8B;
sfr TH0     = 0x8C;
sfr TH1     = 0x8D;
sfr CKCON   = 0x8E;
sfr SCON    = 0x98;
sfr SBUF    = 0x99;
sfr IE      = 0xA8;    /*  Separate IEN0&IEN1  */
sfr IP      = 0xA9;    /*  Separate IP0&IP1  */

/*  Dual DPTR control register  */
sfr DPL1    = 0x84;
sfr DPH1    = 0x85;
sfr AUX     = 0x91;    /*  BRS,x,x,x,x,DPS  */

/*  ISP SFR  */
sfr IFCON   = 0x8F;    /*  X,CDPR,X,X,X,X,X,ISPE  */
sfr TAKEY   = 0xF7;
sfr ISPFAH  = 0xE1;
sfr ISPFAL  = 0xE2;
sfr ISPFDL  = 0xE3;
sfr ISPFDH  = 0xEB;
sfr ISPFC   = 0xE4;

/*  Enhance UART control register  */
sfr S0CON   = 0x98;
sfr S0BUF   = 0x99;
sfr S0RELL  = 0xAA;
sfr S0RELH  = 0xBA;
sfr SRELL   = 0xAA;
sfr SRELH   = 0xBA;
sfr PFCON   = 0xD9;

/*  Enhance Interrupt control register  */
sfr IEN2    = 0x9A;
sfr IEN0    = 0xA8;
sfr IEN1    = 0xB8;
sfr IP0     = 0xA9;
sfr IP1     = 0xB9;
sfr IRCON   = 0xC0;
sfr INTDEG  = 0xEE;
sfr ENHIT   = 0xE5;

/*  KBI  */
sfr KBLS    = 0x93;
sfr KBE     = 0x94;
sfr KBF     = 0x95;
sfr KBD     = 0x96;

/*  IRCON2  */
sfr IRCON2  = 0x97;

/*  Reset rule  */
sfr RSTS    = 0xA1;
sfr LVC     = 0xE6;    /*  LVI_EN,X,LVRXE,X,X,X,X,X  */
sfr SWRES   = 0xE7;

/*  ADC  */
sfr ADCC1   = 0xAB;
sfr ADCC2   = 0xAC;
sfr ADCDH   = 0xAD;
sfr ADCDL   = 0xAE;
sfr ADCCS   = 0xAF;
sfr ADCSH   = 0xEF;
#define ADCCAL (*((unsigned char volatile xdata*)0xFFFF))

/*  PWM  */
sfr PWMC    = 0xB5;
sfr PWMD0H  = 0xBC;
sfr PWMD0L  = 0xBD;
sfr PWMD1H  = 0xBE;
sfr PWMD1L  = 0xBF;
sfr PWMD2H  = 0xB1;
sfr PWMD2L  = 0xB2;
sfr PWMD3H  = 0xB3;
sfr PWMD3L  = 0xB4;
sfr PWMMDH  = 0xCE;
sfr PWMMDL  = 0xCF;

/*  WDT  */
sfr WDTC    = 0xB6;
sfr WDTK    = 0xB7;

/*  Timer2 Compare/Capture control register  */
sfr CCEN    = 0xC1;
sfr CCEN2   = 0xD1;
sfr CCL1    = 0xC2;
sfr CCH1    = 0xC3;
sfr CCL2    = 0xC4;
sfr CCH2    = 0xC5;
sfr CCL3    = 0xC6;
sfr CCH3    = 0xC7;
sfr T2CON   = 0XC8;
sfr CCCON   = 0xC9;
sfr CRCL    = 0xCA;
sfr CRCH    = 0xCB;
sfr TL2     = 0xCC;
sfr TH2     = 0xCD;

sfr16 CC1   = 0xC2;
sfr16 CC2   = 0xC4;
sfr16 CC3   = 0xC6;
sfr16 CRC   = 0xCA;
sfr16 T2    = 0xCC;    // TH2 | TL2

/*  GPIO mode control register  */
sfr P0M0    = 0xD2;
sfr P0M1    = 0xD3;
sfr P1M0    = 0xD4;
sfr P1M1    = 0xD5;
sfr P3M0    = 0xDA;
sfr P3M1    = 0xDB;

/*  SPI  */
sfr SPIC1   = 0xF1;
sfr SPIC2   = 0xF2;
sfr SPITXD  = 0xF3;
sfr SPIRXD  = 0xF4;
sfr SPIS    = 0xF5;

/*  IIC  */
sfr IICS    = 0xF8;
sfr IICCTL  = 0xF9;
sfr IICA1   = 0xFA;
sfr IICA2   = 0xFB;
sfr IICRWD  = 0xFC;
sfr IICEBT  = 0xFD;

/*  Comparator  */
sfr OpPin   = 0xF6;
sfr Cmp0CON = 0xFE;
sfr Cmp1CON = 0xFF;

/*  BIT Registers  */
sbit P0_0   = 0x80;
sbit P0_1   = 0x81;
sbit P0_2   = 0x82;
sbit P0_3   = 0x83;
sbit P0_4   = 0x84;
sbit P0_5   = 0x85;
sbit P0_6   = 0x86;
sbit P0_7   = 0x87;
sbit P1_0   = 0x90;
sbit P1_1   = 0x91;
sbit P1_2   = 0x92;
sbit P1_3   = 0x93;
sbit P1_4   = 0x94;
sbit P1_5   = 0x95;
sbit P1_6   = 0x96;
sbit P1_7   = 0x97;
sbit P3_0   = 0xB0;
sbit P3_1   = 0xB1;

/*  PSW  */
sbit CY     = PSW^7;
sbit AC     = PSW^6;
sbit F0     = PSW^5;
sbit RS1    = PSW^4;
sbit RS0    = PSW^3;
sbit OV     = PSW^2;
sbit P      = PSW^0;    //8052 only

/*  TCON  */
sbit TF1    = TCON^7;
sbit TR1    = TCON^6;
sbit TF0    = TCON^5;
sbit TR0    = TCON^4;
sbit IE1    = TCON^3;
sbit IT1    = TCON^2;
sbit IE0    = TCON^1;
sbit IT0    = TCON^0;

/*  IE  */
sbit EA     = IE^7;
sbit ET2    = IE^5;
sbit ES     = IE^4;
sbit ET1    = IE^3;
sbit EX1    = IE^2;
sbit ET0    = IE^1;
sbit EX0    = IE^0;

/*  S0CON  */
sbit SM0    = S0CON^7;
sbit SM1    = S0CON^6;
sbit SM2    = S0CON^5;
sbit REN    = S0CON^4;
sbit TB8    = S0CON^3;
sbit RB8    = S0CON^2;
sbit TI     = S0CON^1;
sbit RI     = S0CON^0;

/*  IICS  */
sbit MPIF   = IICS^6;
sbit LAIF   = IICS^5;
sbit RXIF   = IICS^4;
sbit TXIF   = IICS^3;
sbit RXAK   = IICS^2;
sbit TXAK   = IICS^1;
sbit RW     = IICS^0;
sbit BB     = IICS^0;

/*  IRCON  */
sbit EXF2   = IRCON^7;
sbit TF2    = IRCON^6;
sbit IICIF  = IRCON^5;
sbit LVIIF  = IRCON^4;
sbit KBIIF  = IRCON^3;
sbit ADCIF  = IRCON^2;
sbit SPIIF  = IRCON^1;
sbit PWMIF  = IRCON^0;

/*  IEN1  */
sbit EXEN2  = IEN1^7;
sbit IEIIC  = IEN1^5;
sbit IELVI  = IEN1^4;
sbit IEKBI  = IEN1^3;
sbit IEADC  = IEN1^2;
sbit IESPI  = IEN1^1;
sbit IEPWM  = IEN1^0;

/*  Interrupt Define  */
#define d_INT0_Vector       0   // 0003h
#define d_T0_Vector         1   // 000Bh
#define d_INT1_Vector       2   // 0013h
#define d_T1_Vector         3   // 001Bh
#define d_UART0_Vector      4   // 0023h
#define d_T2_Vector         5   // 002Bh
#define d_PWM_Vector        8   // 0043h
#define d_SPI_Vector        9   // 004Bh
#define d_ADC_Vector        10  // 0053h
#define d_KBI_Vector        11  // 005Bh
#define d_LVI_Vector        12  // 0063h
#define d_IIC_Vector        13  // 006Bh
#define d_WDT_Vector        17  // 008Bh
#define d_Comparator_Vector 18  // 0093h

#endif
      