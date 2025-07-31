#include "ntc.h"
#include "public.h"
#ifdef 	LOG_TAG
#undef	LOG_TAG
#endif
#define LOG_TAG    "ntc"

EwmaFilter ntc_filter; // NTC�˲���ʵ��
/**
 * ************************************************************************
 * @brief  NTC��ʼ������
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
    // ��ʼ��ADC
    adc0_config();
    ewma_filter_init(&ntc_filter, DEFAULT_ALPHA); // ��ʼ��EWMA�˲���
}

/**
 * ************************************************************************
 * @brief  ��ȡNTC�¶�ֵ
 * 
 * @param[in] temp  ָ��洢�¶�ֵ��ָ��
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
    float temperature = 0.0f; // �¶�ֵ
    if(adc_val == 0) 
    {
        *temp = 0;    
        temperature = 0.0f; // ���ADCֵΪ0���¶�Ϊ0
    }
    else if(adc_val >= 4095)
    {
        *temp = 3000;
        temperature = 300.0f; // ���ADCֵ�������ֵ���¶���Ϊ300��
    } 
    else
    {
        ewma_filter_apply(&ntc_filter, (uint32_t)adc_val);
        /* ADCֵת��ѹ */
        float voltage = (adc_val * 3.3f) / 4095.0f;
        /* ����NTC����ֵ����λ��ŷķ��*/
        float ntc_resistance = (3000.0f * 3.3f) / voltage - 3000.0f;
        /* �¶ȼ������ */
        const float ref_temp = 298.15f;     // �ο��¶�25�棨��λ��K��
        const float ref_resistance = 10000.0f; // �ο�����10K��
        const float B_value = 3380.0f;      // B����
        /* ʹ��Steinhart-Hart���̼����¶� */
        float steinhart = log(ntc_resistance / ref_resistance) / B_value; // ln(R/R0)/B
        steinhart += 1.0f / ref_temp;       // 1/T0 + (ln(R/R0)/B)
        steinhart = 1.0f / steinhart;      // ȡ�����õ��������¶�
        temperature = steinhart - 273.15f; // ת��Ϊ���϶�
        // Ӧ��EWMA�˲���
        
        *temp = (uint16_t)(temperature * 10); // ת��Ϊ������ʾ������1λС��
    }
    

    // ����¶�ֵ
    log_i("NTC Temperature: %.1f", temperature);
}


