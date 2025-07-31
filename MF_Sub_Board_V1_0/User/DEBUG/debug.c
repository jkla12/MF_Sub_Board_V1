/**
 *
 * @file debug.c
 * @author jiaokai
 * @brief  调试程序
 *
 * @copyright Copyright (c) 2025
 */
#include "debug.h"
#include "public.h"


#ifdef LOG_TAG
#undef LOG_TAG
#endif
#define LOG_TAG "debug"

/**
 * ************************************************************************
 * @brief 处理上位机发送的配置命令
 *
 *
 *
 * @version 1.0
 * @author jiaokai
 * @date 2025-05-16
 * 10 ms执行一次
 * ************************************************************************
 */
void debugProcess(void)
{
    if (uart0_struct.rx_flag == 1)
    {
        uart0_struct.rx_flag = 0;
        
    }
}

/**
 * ************************************************************************
 * @brief  计算校验和
 *
 * @param[in] buf  待计算校验和的缓冲区
 * @param[in] len  缓冲区长度
 *
 * @return 校验和
 *
 * @version 1.0
 * @author jiaokai
 * @date 2025-05-16
 *
 * ************************************************************************
 */
uint8_t calc_checksum(const uint8_t *buf, uint16_t len)
{
    uint8_t sum = 0;
    for (uint16_t i = 0; i < len; i++)
        sum += buf[i];
    return sum;
}
