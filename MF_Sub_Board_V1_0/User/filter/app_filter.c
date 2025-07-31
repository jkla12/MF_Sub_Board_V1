#include "app_filter.h"


/**
 * ************************************************************************
 * @brief  ��ʼ���˲���
 * 
 * @param[in,out] filter �˲���ָ��
 * @param[in] alpha      �˲�ϵ�� (0.0 ~ 1.0)
 * 
 * @return ��
 * 
 * @version 2.0
 * @author jiaokai 
 * @date 2025-04-29
 * 
 * ************************************************************************
 */
void ewma_filter_init(EwmaFilter *filter, float alpha) {
    if (filter) {
        filter->filtered_value = 0;
        filter->initialized = 0;
        filter->alpha = (alpha > 0.0f && alpha <= 1.0f) ? alpha : DEFAULT_ALPHA;
    }
}



/**
 * ************************************************************************
 * @brief  Ӧ���ƶ�ָ����Ȩ�˲�
 * 
 * @param[in,out] filter    �˲���ָ��
 * @param[in] new_value     ������
 * 
 * @return �˲�������
 * 
 * @version 2.0
 * @author jiaokai 
 * @date 2025-04-29
 * 
 * ************************************************************************
 */
uint32_t ewma_filter_apply(EwmaFilter *filter, uint32_t new_value) {
    if (!filter) {
        return new_value;  // ����˲���ָ����Ч��ֱ�ӷ���ԭʼֵ
    }

    // ����˲���δ��ʼ�������ʼ��
    if (!filter->initialized) {
        filter->filtered_value = new_value;
        filter->initialized = 1;
    }

    // Ӧ���˲��㷨
    filter->filtered_value = (uint32_t)(filter->alpha * new_value + (1 - filter->alpha) * filter->filtered_value);
    
    return filter->filtered_value;
}

