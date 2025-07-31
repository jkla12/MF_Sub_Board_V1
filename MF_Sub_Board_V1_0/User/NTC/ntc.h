#ifndef __NTC_H
#define __NTC_H

#include "gd32f30x.h"

void NTC_Init(void);    // NTC初始化函数
void NTC_Read(uint16_t *temp); // 读取NTC温度值

#endif

