#include "input.h"
#include "elog.h"
#include "modbus.h"
#include "stdio.h"
#include "mb.h"

extern uint8_t ucRegDiscreteBuf[];


/**
 * @brief ��ʼ��Һλ������GPIO
 */
void level_sensor_init(void)
{
    // ʹ��GPIOCʱ��
    rcu_periph_clock_enable(RCU_GPIOC);

    // ����GPIOΪ����ģʽ������
    gpio_init(LEVEL_SENSOR_PORT, GPIO_MODE_IPU, GPIO_OSPEED_50MHZ,
              LEVEL_SENSOR_1_PIN | LEVEL_SENSOR_2_PIN | LEVEL_SENSOR_3_PIN | LEVEL_SENSOR_4_PIN);
}

/**
 * @brief ��ʼ�����GPIO
 */
void output_gpio_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOB);
    gpio_init(OUT_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, OUT1_PIN);
    gpio_init(OUT_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, OUT2_PIN);
    gpio_init(OUT_PORT, GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, OUT3_PIN);
}

/**
 * @brief ��ȡҺλ������״̬
 * @param status: ָ��洢״̬�Ľṹ��
 */
void level_sensor_read(LevelSensor_Status_t *status)
{
    // ������Ҫ������10�ζ�ȡһ�²Ÿ���״̬����������100ms��1���ȶ���
    #define LEVEL_DEBOUNCE_COUNT 10

    // �־û�����״̬
    static uint8_t initialized = 0;
    static uint8_t raw_last[4];   // ���һ��ԭʼ����
    static uint8_t stable[4];     // ��ȷ�ϵ��ȶ�ֵ
    static uint8_t cnt[4];        // ������ͬ��������

    // ��ȡGPIOԭʼ״̬���������룬�͵�ƽ��Ч��ȡ����1=���ϣ�0=���ϣ�
    uint8_t raw[4];
    raw[0] = !gpio_input_bit_get(LEVEL_SENSOR_PORT, LEVEL_SENSOR_1_PIN); // pin9 -> level_1
    raw[1] = !gpio_input_bit_get(LEVEL_SENSOR_PORT, LEVEL_SENSOR_2_PIN); // pin8 -> level_2
    raw[2] = !gpio_input_bit_get(LEVEL_SENSOR_PORT, LEVEL_SENSOR_3_PIN); // pin7 -> level_3
    raw[3] = !gpio_input_bit_get(LEVEL_SENSOR_PORT, LEVEL_SENSOR_4_PIN); // pin6 -> level_4

    // �״γ�ʼ�������ӳ٣�ֱ�Ӳ��õ�ǰ״̬Ϊ�ȶ�ֵ
    if (!initialized) {
        for (int i = 0; i < 4; i++) {
            raw_last[i] = raw[i];
            stable[i]   = raw[i];
            cnt[i]      = 0;
        }
        initialized = 1;
    }

    // ��ÿ��ͨ���������ж�
    for (int i = 0; i < 4; i++) {
        if (raw[i] == raw_last[i]) {
            if (cnt[i] < 0xFF) cnt[i]++; // �����
        } else {
            raw_last[i] = raw[i];
            cnt[i] = 1; // �µ�ƽ��ʼ����
        }

        // �ﵽ���������ȶ�ֵ��ͬ��������ȶ�ֵ
        if (cnt[i] >= LEVEL_DEBOUNCE_COUNT && stable[i] != raw_last[i]) {
            stable[i] = raw_last[i];
        }
    }

    // ���´�����״̬(1=����,0=����) ���� ʹ����������ȶ�ֵ
    status->level_1_status = stable[0];
    status->level_2_status = stable[1];
    status->level_3_status = stable[2];
    status->level_4_status = stable[3];

    // ����Modbus��ɢ����״̬ ���� ʹ���ȶ�ֵ
    ucRegDiscreteBuf[0] = (stable[0] << 0) | // 10001
                          (stable[1] << 1) | // 10002
                          (stable[2] << 2) | // 10003
                          (stable[3] << 3);  // 10004
}

/**
 * @brief ���������
 */
void Output_task()
{
    // Modbusͨ�Ŵ���
    

    // ����Ȧ�Ĵ�����ȡ״̬
    uint8_t coils = ucRegCoilsBuf[0];

    // OUT1��Ӧ��Ȧ1 (��ַ00001)
    gpio_bit_write(OUT_PORT, OUT1_PIN, (coils & 0x01) ? SET : RESET);
    // OUT2��Ӧ��Ȧ2 (��ַ00002)
    gpio_bit_write(OUT_PORT, OUT2_PIN, (coils & 0x02) ? SET : RESET);
    // OUT3��Ӧ��Ȧ3 (��ַ00003)
    gpio_bit_write(OUT_PORT, OUT3_PIN, (coils & 0x04) ? SET : RESET);


}

