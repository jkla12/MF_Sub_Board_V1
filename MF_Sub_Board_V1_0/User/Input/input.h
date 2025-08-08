#ifndef __INPUT_H
#define __INPUT_H

#include "gd32f30x.h"

// 定义液位传感器通道
#define LEVEL_SENSOR_1_PIN     GPIO_PIN_9
#define LEVEL_SENSOR_2_PIN     GPIO_PIN_8
#define LEVEL_SENSOR_3_PIN     GPIO_PIN_7
#define LEVEL_SENSOR_4_PIN     GPIO_PIN_6
#define LEVEL_SENSOR_PORT      GPIOC

// 输出
#define OUT_PORT               GPIOB
#define OUT1_PIN               GPIO_PIN_12
#define OUT2_PIN               GPIO_PIN_13
#define OUT3_PIN               GPIO_PIN_14


// 液位状态结构体
typedef struct {
    uint8_t level_1_status;    // 传感器1状态
    uint8_t level_2_status;    // 传感器2状态
    uint8_t level_3_status;    // 传感器3状态
    uint8_t level_4_status;    // 传感器4状态
} LevelSensor_Status_t;

void level_sensor_read(LevelSensor_Status_t *status);
void level_sensor_init(void);
void output_gpio_init(void);
void Output_task(void);


#endif


