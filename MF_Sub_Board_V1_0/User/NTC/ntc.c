#include "ntc.h"
#include "public.h"
#ifdef 	LOG_TAG
#undef	LOG_TAG
#endif
#define LOG_TAG    "ntc"

EwmaFilter ntc_filter; // NTC滤波器实例
/**
 * ************************************************************************
 * @brief  NTC初始化函数
 * 
 * 
 * 
 * @version 1.0
 * @author jiaokai 
 * @date 2025-07-25
 * 
 * ************************************************************************
 */
void NTC_Init(void)
{
    // 初始化ADC
    adc0_config();
    ewma_filter_init(&ntc_filter, DEFAULT_ALPHA); // 初始化EWMA滤波器
}

/**
 * ************************************************************************
 * @brief  读取NTC温度值
 * 
 * @param[in] temp  指向存储温度值的指针
 * 
 * 
 * @version 1.0
 * @author jiaokai 
 * @date 2025-07-25
 * 
 * ************************************************************************
 */
void NTC_Read(uint16_t *temp)
{
    float temperature = 0.0f; // 温度值
    if(adc_val == 0) 
    {
        *temp = 0;    
        temperature = 0.0f; // 如果ADC值为0，温度为0
    }
    else if(adc_val >= 4095)
    {
        *temp = 3000;
        temperature = 300.0f; // 如果ADC值超过最大值，温度设为300℃
    } 
    else
    {
        ewma_filter_apply(&ntc_filter, (uint32_t)adc_val);
        /* ADC值转电压 */
        float voltage = (adc_val * 3.3f) / 4095.0f;
        /* 计算NTC电阻值（单位：欧姆）*/
        float ntc_resistance = (3000.0f * 3.3f) / voltage - 3000.0f;
        /* 温度计算参数 */
        const float ref_temp = 298.15f;     // 参考温度25℃（单位：K）
        const float ref_resistance = 10000.0f; // 参考电阻10KΩ
        const float B_value = 3380.0f;      // B常数
        /* 使用Steinhart-Hart方程计算温度 */
        float steinhart = log(ntc_resistance / ref_resistance) / B_value; // ln(R/R0)/B
        steinhart += 1.0f / ref_temp;       // 1/T0 + (ln(R/R0)/B)
        steinhart = 1.0f / steinhart;      // 取倒数得到开尔文温度
        temperature = steinhart - 273.15f; // 转换为摄氏度
        // 应用EWMA滤波器
        
        *temp = (uint16_t)(temperature * 10); // 转换为整数表示，保留1位小数
    }
    

    // 输出温度值
    log_i("NTC Temperature: %.1f", temperature);
}


