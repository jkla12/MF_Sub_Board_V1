#ifndef __ADC_H
#define __ADC_H

#include "gd32f30x.h"

#define ADC0_CLOCK				RCU_GPIOA

#define ADC0_PORT  GPIOA
#define ADC0_PIN   GPIO_PIN_0

extern uint16_t adc_val;          // ADC采样值

void adc0_config(void);
float height_cal(uint16_t val);
void adc_filter(void);

#endif

