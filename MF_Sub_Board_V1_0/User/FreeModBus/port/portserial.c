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
        usart_interrupt_enable(UART3, USART_INT_RBNE); // ʹ�ܴ���1�����ж�
    }
    else
    {
        usart_interrupt_disable(UART3, USART_INT_RBNE); // ���ô���1�����ж�
    }
    if(xTxEnable == TRUE)
    {
        usart_interrupt_enable(UART3, USART_INT_TC); // ʹ�ܴ���1�����ж�
    }
    else
    {
        usart_interrupt_disable(UART3, USART_INT_TC); // ���ô���1�����ж�
    }
}

BOOL
xMBPortSerialInit( UCHAR ucPORT, ULONG ulBaudRate, UCHAR ucDataBits, eMBParity eParity )
{
    uart3_config(ulBaudRate); // ��ʼ������3    
    return TRUE;
}

BOOL
xMBPortSerialPutByte( CHAR ucByte )
{
    while(RESET == usart_flag_get(UART3, USART_FLAG_TBE));
    usart_data_transmit(UART3, ucByte); // ͨ�����ڷ�������
    return TRUE;
}

BOOL
xMBPortSerialGetByte( CHAR * pucByte )
{
    *pucByte = usart_data_receive(UART3); // �Ӵ��ڽ�������
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
**����ԭ��:	void UART3_IRQHandler(void)
**��������:	����1�жϺ���
**��ڲ���:	��
**          
**���ڲ���:	��
**��    ע:	��
************************************************************************/
void UART3_IRQHandler(void)
{
   if(RESET != usart_interrupt_flag_get(UART3, USART_INT_FLAG_RBNE)) {
       /* ����жϱ�־λ */
       prvvUARTRxISR(); //���ڽ����жϵ��ú���
       usart_interrupt_flag_clear(UART3, USART_INT_FLAG_RBNE);
   }
   if(RESET != usart_interrupt_flag_get(UART3, USART_INT_FLAG_RBNE_ORERR)) {
       /* ����жϱ�־λ */
       prvvUARTRxISR(); //���ڷ����жϵ��ú���
       usart_interrupt_flag_clear(UART3, USART_INT_FLAG_RBNE_ORERR);
   }
   if(RESET != usart_interrupt_flag_get(UART3, USART_INT_FLAG_TC)) {
       /* ����жϱ�־λ */
       prvvUARTTxReadyISR(); //���ڷ����жϵ��ú���
       usart_interrupt_flag_clear(UART3, USART_INT_FLAG_TC);
   }
}

