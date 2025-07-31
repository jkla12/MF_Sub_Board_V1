#include "port.h"
#include "uart.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
/* ----------------------- static functions ---------------------------------*/
static void prvvUARTTxReadyISR( void );
static void prvvUARTRxISR( void );

/* ----------------------- Start implementation -----------------------------*/
void
vMBPortSerialEnable( BOOL xRxEnable, BOOL xTxEnable )
{
    if(xRxEnable == TRUE)
    {
        usart_interrupt_enable(UART3, USART_INT_RBNE); // 使能串口1接收中断
    }
    else
    {
        usart_interrupt_disable(UART3, USART_INT_RBNE); // 禁用串口1接收中断
    }
    if(xTxEnable == TRUE)
    {
        usart_interrupt_enable(UART3, USART_INT_TC); // 使能串口1发送中断
    }
    else
    {
        usart_interrupt_disable(UART3, USART_INT_TC); // 禁用串口1发送中断
    }
}

BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
    uart3_config(ulBaudRate); // 初始化串口3    
    return TRUE;
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    while(RESET == usart_flag_get(UART3, USART_FLAG_TBE));
    usart_data_transmit(UART3, ucByte); // 通过串口发送数据
    return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    *pucByte = usart_data_receive(UART3); // 从串口接收数据
    return TRUE;
}

/* Create an interrupt handler for the transmit buffer empty interrupt
 * (or an equivalent) for your target processor. This function should then
 * call pxMBFrameCBTransmitterEmpty( ) which tells the protocol stack that
 * a new character can be sent. The protocol stack will then call 
 * xMBPortSerialPutByte( ) to send the character.
 */
static void prvvUARTTxReadyISR( void )
{
    pxMBFrameCBTransmitterEmpty(  );
}

/* Create an interrupt handler for the receive interrupt for your target
 * processor. This function should then call pxMBFrameCBByteReceived( ). The
 * protocol stack will then call xMBPortSerialGetByte( ) to retrieve the
 * character.
 */
static void prvvUARTRxISR( void )
{
    pxMBFrameCBByteReceived(  );
}

/**********************************************************************
**函数原型:	void UART3_IRQHandler(void)
**函数作用:	串口1中断函数
**入口参数:	无
**          
**出口参数:	无
**备    注:	无
************************************************************************/
void UART3_IRQHandler(void)
{
   if(RESET != usart_interrupt_flag_get(UART3, USART_INT_FLAG_RBNE)) {
       /* 清除中断标志位 */
       prvvUARTRxISR(); //串口接收中断调用函数
       usart_interrupt_flag_clear(UART3, USART_INT_FLAG_RBNE);
   }
   if(RESET != usart_interrupt_flag_get(UART3, USART_INT_FLAG_RBNE_ORERR)) {
       /* 清除中断标志位 */
       prvvUARTRxISR(); //串口发送中断调用函数
       usart_interrupt_flag_clear(UART3, USART_INT_FLAG_RBNE_ORERR);
   }
   if(RESET != usart_interrupt_flag_get(UART3, USART_INT_FLAG_TC)) {
       /* 清除中断标志位 */
       prvvUARTTxReadyISR(); //串口发送中断调用函数
       usart_interrupt_flag_clear(UART3, USART_INT_FLAG_TC);
   }
}

