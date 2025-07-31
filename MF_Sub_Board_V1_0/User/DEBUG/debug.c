/**
 *
 * @file debug.c
 * @author jiaokai
 * @brief  ���Գ���
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
 * @brief ������λ�����͵���������
 *
 *
 *
 * @version 1.0
 * @author jiaokai
 * @date 2025-05-16
 * 10 msִ��һ��
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
 * @brief  ����У���
 *
 * @param[in] buf  ������У��͵Ļ�����
 * @param[in] len  ����������
 *
 * @return У���
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
