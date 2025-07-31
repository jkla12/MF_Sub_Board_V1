#include "rtc.h"
#include "public.h"

rtctime_struct RTCTime = {0};
struct tm Cache_TimeStandard = {0};

uint8_t  RTCIRQHandler_Flag;
/**
 * ************************************************************************
 * @brief  RTC初始化函数
 * 
 * 
 * 
 * @version 1.0
 * @author jiaokai 
 * @date 2025-07-17
 * 
 * ************************************************************************
 */
void RTC_Init(void)
{
    nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);
    nvic_irq_enable(RTC_IRQn,1,0);
    if((bkp_read_data(BKP_DATA_0) != 0xA5A5) || (0x00 == GET_BITS(RCU_BDCTL, 8, 9)))
    {
        rcu_periph_clock_enable(RCU_BKPI);
        rcu_periph_clock_enable(RCU_PMU);
        pmu_backup_write_enable();
        bkp_deinit();
        rcu_osci_on(RCU_LXTAL);
        rcu_osci_stab_wait(RCU_LXTAL);
        rcu_rtc_clock_config(RCU_RTCSRC_LXTAL);
        rcu_periph_clock_enable(RCU_RTC);
        rtc_register_sync_wait();
        rtc_lwoff_wait();
        rtc_interrupt_enable(RTC_INT_SECOND);
        rtc_lwoff_wait();
        rtc_prescaler_set(32767);
        rtc_lwoff_wait();
        
        SetRTCTimeFormTimeStamp(1752800400);

        bkp_write_data(BKP_DATA_0, 0xA5A5);
    }
    else
    {
        if (rcu_flag_get(RCU_FLAG_PORRST) != RESET)             //发生开机重置
        {
            
        }
        else if (rcu_flag_get(RCU_FLAG_SWRST) != RESET)         //发生外部重置
        {
            
        }
        
        rcu_periph_clock_enable(RCU_PMU);
        pmu_backup_write_enable();
        rtc_register_sync_wait();
        rtc_interrupt_enable(RTC_INT_SECOND);
        rtc_lwoff_wait();
    }
    rcu_all_reset_flag_clear();
}

/**
 * ************************************************************************
 * @brief  RTC中断处理函数
 * 
 * 
 * 
 * @version 1.0
 * @author jiaokai 
 * @date 2025-07-18
 * 
 * ************************************************************************
 */
void RTC_IRQHandler(void)
{
    if (rtc_flag_get(RTC_FLAG_SECOND) != RESET)
    {
        rtc_flag_clear(RTC_FLAG_SECOND);
        RTCIRQHandler_Flag = 1;
        rtc_lwoff_wait();
    }
}

/**
 * ************************************************************************
 * @brief  计算星期几
 * 
 * @param[in] year  Comment
 * @param[in] month  Comment
 * @param[in] day  Comment
 * 
 * @return 
 * 
 * @version 1.0
 * @author jiaokai 
 * @date 2025-07-18
 * 
 * ************************************************************************
 */
int Calculate_Week(int year, int month, int day)
{
	int c,y,week;
    year = year + 2000;
	if (month == 1 || month == 2)
	year--, month += 12;
	c = year / 100;
	y = year - c * 100;
	week = y + y / 4 + c / 4 - 2 * c + 26 * (month + 1) / 10 + day - 1;
	while (week < 0)
	week += 7;
	week %= 7;
	return week;
}
/**
 * ************************************************************************
 * @brief  将时间标准转换为时间戳
 * 
 * @param[in] stm  Comment
 * 
 * @return 
 * 
 * @version 1.0
 * @author jiaokai 
 * @date 2025-07-18
 * 
 * ************************************************************************
 */
uint32_t TimeStandardToTimeStamp(struct tm stm)
{
    struct tm timeinfo = {0}; 

    timeinfo.tm_year = stm.tm_year - 1900;  // Year since 1900  
    timeinfo.tm_mon = stm.tm_mon - 1;     // Month since January. 0-11  
    timeinfo.tm_mday = stm.tm_mday;  
    timeinfo.tm_hour = stm.tm_hour;  
    timeinfo.tm_min = stm.tm_min;  
    timeinfo.tm_sec = stm.tm_sec;  
    timeinfo.tm_isdst = -1;         // Let mktime() determine if DST is in effect
    
    return (uint32_t)(mktime(&timeinfo) - 28800);
}
/**
 * ************************************************************************
 * @brief  将时间戳转换为时间标准
 * 
 * @param[in] timestamp  Comment
 * 
 * @return 
 * 
 * @version 1.0
 * @author jiaokai 
 * @date 2025-07-18
 * 
 * ************************************************************************
 */
struct tm TimeStampToTimeStandard(uint32_t timestamp)
{
    struct tm stm;
    time_t tick = (time_t)(timestamp + 28800);
    char s[100];
    stm = *localtime(&tick);

    strftime(s,sizeof(s),"%Y-%m-%d %H:%M:%S",&stm);

    stm.tm_year = atoi(s);
    stm.tm_mon = atoi(s+5);
    stm.tm_yday = atoi(s+8);
    stm.tm_hour = atoi(s+11);
    stm.tm_min = atoi(s+14);
    stm.tm_sec = atoi(s+17);

    return stm;
}
/**
 * ************************************************************************
 * @brief  设置RTC时间戳
 * 
 * @param[in] timestamp   时间戳，单位为秒
 * 
 * 
 * @version 1.0
 * @author jiaokai 
 * @date 2025-07-18
 * 
 * ************************************************************************
 */
void SetRTCTimeFormTimeStamp(uint32_t timestamp)
{
    rtc_counter_set(timestamp);
}
/**
 * ************************************************************************
 * @brief  设置RTC时间标准
 * 
 * @param[in] stm  时间标准结构体，包含年、月、日、时、分、秒等信息
 * 
 * 
 * @version 1.0
 * @author jiaokai 
 * @date 2025-07-18
 * 
 * ************************************************************************
 */
void SetRTCTimeFormTimeStandard(struct tm stm)
{
    uint32_t timestamp = 0;
    timestamp = TimeStandardToTimeStamp(stm);
    SetRTCTimeFormTimeStamp(timestamp);
}

/**
 * ************************************************************************
 * @brief  读取RTC时间戳
 * 
 * 
 * @return 
 * 
 * @version 1.0
 * @author jiaokai 
 * @date 2025-07-18
 * 
 * ************************************************************************
 */
uint32_t ReadRTCTimeStamp(void)
{
    return rtc_counter_get();
}

/**
 * ************************************************************************
 * @brief  读取RTC缓存时间
 * 
 * 
 * 
 * @version 1.0
 * @author jiaokai 
 * @date 2025-07-18
 * 
 * ************************************************************************
 */
void ReadRTCCacheTime(void)
{
    Cache_TimeStandard.tm_year = RTCTime.TimeStandard.tm_year;
    Cache_TimeStandard.tm_mon = RTCTime.TimeStandard.tm_mon;
    Cache_TimeStandard.tm_mday = RTCTime.TimeStandard.tm_mday;
    Cache_TimeStandard.tm_hour = RTCTime.TimeStandard.tm_hour;
    Cache_TimeStandard.tm_min = RTCTime.TimeStandard.tm_min;
    Cache_TimeStandard.tm_sec = RTCTime.TimeStandard.tm_sec;
}

/**
 * ************************************************************************
 * @brief  读取RTC时间任务
 * 
 * 
 * 
 * @version 1.0
 * @author jiaokai 
 * @date 2025-07-18
 * 
 * ************************************************************************
 */
void ReadRTCTime_Task(void)
{
    if(RTCIRQHandler_Flag == 1)
    {
        RTCIRQHandler_Flag = 0;
        RTCTime.TimeStamp = ReadRTCTimeStamp();
        RTCTime.TimeStandard = TimeStampToTimeStandard(RTCTime.TimeStamp);
    }
}


