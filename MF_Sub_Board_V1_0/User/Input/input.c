#include "input.h"
#include "elog.h"
#include "modbus.h"
#include "stdio.h"
#include "mb.h"

extern uint8_t ucRegDiscreteBuf[];


/**
 * @brief 初始化液位传感器GPIO
 */
void level_sensor_init(void)
{
    // 使能GPIOC时钟
    rcu_periph_clock_enable(RCU_GPIOC);

    // 配置GPIO为输入模式，上拉
    gpio_init(LEVEL_SENSOR_PORT, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ,
              LEVEL_SENSOR_1_PIN | LEVEL_SENSOR_2_PIN | LEVEL_SENSOR_3_PIN | LEVEL_SENSOR_4_PIN);
}

/**
 * @brief 初始化输出GPIO
 */
void output_gpio_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOB);
    gpio_init(OUT_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, OUT1_PIN);
    gpio_init(OUT_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, OUT2_PIN);
    gpio_init(OUT_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, OUT3_PIN);
}

/**
 * @brief 读取液位传感器状态
 * @param status: 指向存储状态的结构体
 */
void level_sensor_read(LevelSensor_Status_t *status)
{
    // 消抖：要求连续10次读取一致才更新状态（调用周期100ms≈1秒稳定）
    #define LEVEL_DEBOUNCE_COUNT 10

    // 持久化消抖状态
    static uint8_t initialized = 0;
    static uint8_t raw_last[4];   // 最近一次原始读数
    static uint8_t stable[4];     // 已确认的稳定值
    static uint8_t cnt[4];        // 连续相同读数计数

    // 读取GPIO原始状态（上拉输入，低电平有效，取反后1=有料，0=无料）
    uint8_t raw[4];
    raw[0] = !gpio_input_bit_get(LEVEL_SENSOR_PORT, LEVEL_SENSOR_1_PIN); // pin9 -> level_1
    raw[1] = !gpio_input_bit_get(LEVEL_SENSOR_PORT, LEVEL_SENSOR_2_PIN); // pin8 -> level_2
    raw[2] = !gpio_input_bit_get(LEVEL_SENSOR_PORT, LEVEL_SENSOR_3_PIN); // pin7 -> level_3
    raw[3] = !gpio_input_bit_get(LEVEL_SENSOR_PORT, LEVEL_SENSOR_4_PIN); // pin6 -> level_4

    // 首次初始化：不延迟，直接采用当前状态为稳定值
    if (!initialized) {
        for (int i = 0; i < 4; i++) {
            raw_last[i] = raw[i];
            stable[i]   = raw[i];
            cnt[i]      = 0;
        }
        initialized = 1;
    }

    // 对每个通道做消抖判定
    for (int i = 0; i < 4; i++) {
        if (raw[i] == raw_last[i]) {
            if (cnt[i] < 0xFF) cnt[i]++; // 防溢出
        } else {
            raw_last[i] = raw[i];
            cnt[i] = 1; // 新电平开始计数
        }

        // 达到门限且与稳定值不同，则更新稳定值
        if (cnt[i] >= LEVEL_DEBOUNCE_COUNT && stable[i] != raw_last[i]) {
            stable[i] = raw_last[i];
        }
    }

    // 更新传感器状态(1=有料,0=无料) —— 使用消抖后的稳定值
    status->level_1_status = stable[0];
    status->level_2_status = stable[1];
    status->level_3_status = stable[2];
    status->level_4_status = stable[3];

    // 更新Modbus离散输入状态 —— 使用稳定值
    ucRegDiscreteBuf[0] = (stable[0] << 0) | // 10001
                          (stable[1] << 1) | // 10002
                          (stable[2] << 2) | // 10003
                          (stable[3] << 3);  // 10004
}

/**
 * @brief 输出任务函数
 */
void Output_task()
{
    // Modbus通信处理
    

    // 从线圈寄存器读取状态
    uint8_t coils = ucRegCoilsBuf[0];

    // OUT1对应线圈1 (地址00001)
    gpio_bit_write(OUT_PORT, OUT1_PIN, (coils & 0x01) ? SET : RESET);
    // OUT2对应线圈2 (地址00002)
    gpio_bit_write(OUT_PORT, OUT2_PIN, (coils & 0x02) ? SET : RESET);
    // OUT3对应线圈3 (地址00003)
    gpio_bit_write(OUT_PORT, OUT3_PIN, (coils & 0x04) ? SET : RESET);


}

