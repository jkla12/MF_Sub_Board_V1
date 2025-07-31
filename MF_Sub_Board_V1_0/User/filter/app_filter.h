#ifndef __APP_FILTER_H__
#define __APP_FILTER_H__

#include "gd32f30x.h"

#define DEFAULT_ALPHA 0.1f

/**
 * �˲����ṹ��
 */
typedef struct {
    uint32_t filtered_value;    // �˲����ֵ
    int initialized;            // ��ʼ����־
    float alpha;                // �˲�ϵ��
} EwmaFilter;

void ewma_filter_init(EwmaFilter *filter, float alpha);
uint32_t ewma_filter_apply(EwmaFilter *filter, uint32_t new_value);

#endif

