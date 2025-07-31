#include "timer.h"
#include  "public.h"

Timer_TypeDef Timer;

#ifdef 	LOG_TAG
#undef	LOG_TAG
#endif
#define LOG_TAG    "Timer"

// void timer1_config(void)
// {
//     timer_parameter_struct timer_initpara;
//     rcu_periph_clock_enable(RCU_TIMER1);
//     timer_deinit(TIMER1);

//     timer_struct_para_init(&timer_initpara);
//     /* TIMER1 configuration */
//     timer_initpara.prescaler = 119; // 120��Ƶ
//     timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
//     timer_initpara.counterdirection = TIMER_COUNTER_UP;
//     timer_initpara.period = 999; // ��ʱ1ms
//     timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
//     timer_init(TIMER1, &timer_initpara);
// 	nvic_irq_enable(TIMER1_IRQn, 0, 1);
//     timer_interrupt_flag_clear(TIMER1, TIMER_INT_FLAG_UP);
//     timer_interrupt_enable(TIMER1, TIMER_INT_UP);
//     timer_enable(TIMER1);
// }

// void TIMER1_IRQHandler(void)
// {
//     if (SET == timer_interrupt_flag_get(TIMER1, TIMER_INT_FLAG_UP))
//     {
//         /* clear channel 0 interrupt bit */
//         timer_interrupt_flag_clear(TIMER1, TIMER_INT_FLAG_UP);
//         // gd_eval_led_toggle(LED1);
//         (Timer.cnt < 1000) ? (Timer.cnt++) : (Timer.cnt = 1); // cnt����
//         Timer.flag = 1;                                       // 1ms
//         Timer.systemTime++;
//         if (Timer.cnt % 10 == 0)
//         {
//             Timer._10msFlag = 1; // 10ms
//         }
//         if (Timer.cnt % 100 == 0)
//         {
//             Timer._100msFlag = 1; // 100ms

//         }
//         if (Timer.cnt == 1000)
//         {
//             Timer._1sFlag = 1; // 1s
//             Timer.system1Sec++;
//             Timer.cnt = 0;
//         }
//     }
// }

void timer3_config(uint32_t period)
{
	timer_parameter_struct timer_initpara;

	rcu_periph_clock_enable(RCU_TIMER3);

	timer_deinit(TIMER3);
	/* initialize TIMER init parameter struct */
	timer_struct_para_init(&timer_initpara);
	/* TIMER1 configuration */
	timer_initpara.prescaler = 11999; // ʱ��50us
	timer_initpara.alignedmode = TIMER_COUNTER_EDGE;
	timer_initpara.counterdirection = TIMER_COUNTER_UP;
	timer_initpara.period = period-1; 
	timer_initpara.clockdivision = TIMER_CKDIV_DIV1;
	timer_init(TIMER3, &timer_initpara);

	timer_interrupt_flag_clear(TIMER3, TIMER_INT_FLAG_UP);
	timer_interrupt_enable(TIMER3, TIMER_INT_UP);
	timer_enable(TIMER3);
	nvic_irq_enable(TIMER3_IRQn, 0, 3);
}
