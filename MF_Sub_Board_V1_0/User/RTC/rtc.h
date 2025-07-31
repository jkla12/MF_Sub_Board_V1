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

void RTC_Init(void); // RTC��ʼ������
int Calculate_Week(int year, int month, int day);
uint32_t TimeStandardToTimeStamp(struct tm stm); // ��ʱ���׼ת��Ϊʱ���
struct tm TimeStampToTimeStandard(uint32_t timestamp); // ��ʱ���ת��Ϊʱ���׼
void SetRTCTimeFormTimeStamp(uint32_t timestamp); // ����RTCʱ���
void SetRTCTimeFormTimeStandard(struct tm stm); // ����RTCʱ���׼
uint32_t ReadRTCTimeStamp(void);

#endif

