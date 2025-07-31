#ifndef __RTC_H
#define __RTC_H

#include "gd32f30x.h"
#include "time.h"

typedef struct _rtctime_struct_
{
    struct tm TimeStandard;
    uint32_t TimeStamp;
}rtctime_struct;

extern rtctime_struct RTCTime;
extern struct tm Cache_TimeStandard;

void RTC_Init(void); // RTC初始化函数
int Calculate_Week(int year, int month, int day);
uint32_t TimeStandardToTimeStamp(struct tm stm); // 将时间标准转换为时间戳
struct tm TimeStampToTimeStandard(uint32_t timestamp); // 将时间戳转换为时间标准
void SetRTCTimeFormTimeStamp(uint32_t timestamp); // 设置RTC时间戳
void SetRTCTimeFormTimeStandard(struct tm stm); // 设置RTC时间标准
uint32_t ReadRTCTimeStamp(void);

#endif

