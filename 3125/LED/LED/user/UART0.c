#include "OB38R08T1.h"
#include "UART0.h"

//===============================================================
//DEFINITIONs
//===============================================================
#define d_S0RELH        0x03
#define d_S0RELL        0xCC

#define BUF_SIZE  64
//接收寄存器
unsigned char	xdata	gBUartRxdReadCount=0;
unsigned char 	xdata	gBUartRxdByteCount=0;
unsigned char 	xdata	gBUartRxdBuf[BUF_SIZE];		//***一帧最多收64个字节
//===============================================================
//GLOBAL VARIABLE
//===============================================================
bit bU0TX = 0;

//=========================================================================
void init_UART0(void)
{
    S0RELH   = d_S0RELH;
    S0RELL   = d_S0RELL;
    AUX     |= 0x80;            // BRS = 1
    PCON    |= 0x80;            // SMOD =1
    IEN0    |= 0x90;            // EA=1, ES0=1, interrupt enable
    S0CON    = 0x50;            // mode 1, SM20=0, Revice enable
    TI       = 0;
}

void UART0_ISR(void) interrupt d_UART0_Vector
{
    if (RI)                     // INT_RX
    {
        RI = 0;                 // RI clear
        gBUartRxdBuf[gBUartRxdByteCount] = S0BUF;         // Read BUF, user code...
        gBUartRxdByteCount++;
        if(gBUartRxdByteCount>=BUF_SIZE)
            gBUartRxdByteCount=0;
    }
    else                        // INT_TX
    {
        bU0TX  = 0;
        TI     = 0;             // TI clear
    }
}

void UART0_TX(unsigned char n_temp0)
{
    bU0TX = 1;
    S0BUF = n_temp0;
    while(bU0TX)
    {
        ;
    }
}

//***************************************************************************
unsigned char GetBufSize(void)
{
    unsigned char BSize=0;
    if(gBUartRxdByteCount>=gBUartRxdReadCount)
    {
        BSize=gBUartRxdByteCount-gBUartRxdReadCount;
    }
    else
    {
        BSize=(BUF_SIZE-gBUartRxdReadCount)+gBUartRxdByteCount;
    }
    return BSize;
}
//************************************************************************
unsigned char BufSizeClear(void)
{
    gBUartRxdByteCount=0;
    gBUartRxdReadCount=0;
    return 0;
}

//*************************************************************************
unsigned char GetBufData(void)
{
    unsigned char reData=0;
    if(gBUartRxdReadCount!=gBUartRxdByteCount)
    {
        reData=gBUartRxdBuf[gBUartRxdReadCount];
        gBUartRxdReadCount++;
        if(gBUartRxdReadCount>=BUF_SIZE)
            gBUartRxdReadCount=0;
    }
    else
    {
        reData=0;
    }
    return reData;
}
//********************************************************************************************