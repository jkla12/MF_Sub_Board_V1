#ifndef __TIMER_H
#define __TIMER_H

#include "gd32f30x.h"

typedef struct
{
	uint32_t systemTime;		//ϵͳʱ��, 1ms����
	uint32_t system1Sec;		//ϵͳʱ�䣬1s����
	uint8_t _1sFlag;		    //��ʱ��1_1S_������־λ
	uint8_t _100msFlag;	        //��ʱ��1_100ms_������־λ
	uint8_t _10msFlag;	        //��ʱ��1_10ms_������־λ
	uint8_t flag;			    //��ʱ��1_1ms_������־λ
	uint16_t cnt;		        //��ʱ��1������־λ
}Timer_TypeDef;

extern Timer_TypeDef Timer;

void timer1_config(void);

void timer3_config(uint32_t period);

#endif


