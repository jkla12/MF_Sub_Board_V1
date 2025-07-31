#include "adc.h"
#include "systick.h"

uint16_t adc_val;

uint16_t adc_filter_val;

void adc0_config(void)
{
	 /*定义ADC，DMA结构体*/
    dma_parameter_struct dma_data_parameter;
	 /*使能GPIO时钟*/
	 rcu_periph_clock_enable(ADC0_CLOCK);	
	 /*使能ADC时钟*/
	 rcu_periph_clock_enable(RCU_ADC0);
	 rcu_adc_clock_config(RCU_CKADC_CKAPB2_DIV8);
	 /*使能DMA时钟*/
	 rcu_periph_clock_enable(RCU_DMA0);
	
	 gpio_init(ADC0_PORT, GPIO_MODE_AIN, GPIO_OSPEED_50MHZ, ADC0_PIN);	//ADC0_CH0,PA0
	
			/*复位DMA*/
	dma_deinit(DMA0, DMA_CH0);
	
	/* 初始化DMA参数 */
	dma_data_parameter.periph_addr  = (uint32_t)(&ADC_RDATA(ADC0));
	dma_data_parameter.periph_inc   = DMA_PERIPH_INCREASE_DISABLE;
	
	dma_data_parameter.memory_addr  = (uint32_t)(&adc_val);
	dma_data_parameter.memory_inc   = DMA_MEMORY_INCREASE_ENABLE;
	
	dma_data_parameter.periph_width = DMA_PERIPHERAL_WIDTH_16BIT;
	dma_data_parameter.memory_width = DMA_MEMORY_WIDTH_16BIT;
	
	dma_data_parameter.direction    = DMA_PERIPHERAL_TO_MEMORY;
	dma_data_parameter.number       = 1;
	dma_data_parameter.priority     = DMA_PRIORITY_HIGH;
	dma_init(DMA0, DMA_CH0, &dma_data_parameter);
	dma_circulation_enable(DMA0, DMA_CH0);

	/* 使能DMA */
	dma_channel_enable(DMA0, DMA_CH0);
	
	 /* ADC扫描模式 */
    adc_special_function_config(ADC0, ADC_SCAN_MODE, DISABLE);	//单通道关闭扫描模式
    /* 配置ADC连续转换模式 */
    adc_special_function_config(ADC0, ADC_CONTINUOUS_MODE, ENABLE);	
   
	  /* ADC触发配置 */
    adc_external_trigger_source_config(ADC0, ADC_REGULAR_CHANNEL, ADC0_1_2_EXTTRIG_REGULAR_NONE);	//软件触发
		
		
    /*ADC对齐方式，右对齐*/
    adc_data_alignment_config(ADC0, ADC_DATAALIGN_RIGHT);
		
		adc_mode_config(ADC_MODE_FREE);	//单通道模式
		
    /* ADC通道组长度，采集几路就设置为几 */
    adc_channel_length_config(ADC0, ADC_REGULAR_CHANNEL, 1);
		
		/* 使能ADC的DMA传输 */
    adc_dma_mode_enable(ADC0);
		
    /* 配置ADC规则组 */
    adc_regular_channel_config(ADC0, 0, ADC_CHANNEL_0, ADC_SAMPLETIME_239POINT5);	//239.5个时钟周期采样，



    adc_external_trigger_config(ADC0, ADC_REGULAR_CHANNEL, ENABLE);	//使能转换
    
    /* 使能ADC */
    adc_enable(ADC0);
    delay_1ms(1);
    /* 使能ADC校准*/
    adc_calibration_enable(ADC0);


    /* 使能软件转换*/
    adc_software_trigger_enable(ADC0, ADC_REGULAR_CHANNEL);

}

/**********************************************************************
**函数原型:	void height_cal(uint16_t val)
**函数作用:	高度换算
**入口参数:	ADC采样值
**出口参数:	压力值
**备    注:	无
************************************************************************/
float height_cal(uint16_t val)
{
	float height;
	height = (float)val*0.021f-12.5f;
	return height;
}

uint16_t adc_buf[10];
// 写一个滑动均值滤波代码,窗口长度为10
void adc_filter(void)
{
	uint8_t i;
	uint32_t sum = 0;
	for (i = 0; i < 9; i++)
	{
		adc_buf[i] = adc_buf[i + 1];
		sum += adc_buf[i];
	}
	adc_buf[9] = adc_val;
	sum += adc_val;
	adc_filter_val =  sum / 10;
}


