/* ----------------------- Platform includes --------------------------------*/
#include "port.h"
#include "timer.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"

/* ----------------------- static functions ---------------------------------*/
static void prvvTIMERExpiredISR( void );

/* ----------------------- Start implementation -----------------------------*/
BOOL
xMBPortTimersInit( USHORT usTim1Timerout50us )
{
    timer3_config(usTim1Timerout50us);
    return TRUE;
}


 void
vMBPortTimersEnable(  )
{
    // 重新设置定时器计数值并启动
    timer_counter_value_config(TIMER3, 0);
    timer_enable(TIMER3);
}

 void
vMBPortTimersDisable(  )
{
    // 禁用定时器
    timer_disable(TIMER3);
}

/* Create an ISR which is called whenever the timer has expired. This function
 * must then call pxMBPortCBTimerExpired( ) to notify the protocol stack that
 * the timer has expired.
 */
static void prvvTIMERExpiredISR( void )
{
    ( void )pxMBPortCBTimerExpired(  );
}

void TIMER3_IRQHandler(void)
{
    if (SET == timer_interrupt_flag_get(TIMER3, TIMER_INT_FLAG_UP))
    {
        /* clear channel 0 interrupt bit */
        prvvTIMERExpiredISR(); // Call the ISR to handle the timer expiration
        timer_interrupt_flag_clear(TIMER3, TIMER_INT_FLAG_UP);
        
    }
}

