#include "uart.h"
#include "public.h"


#if UART0_ENABLE
uart_TypeDef uart0_struct;
queue_t tx0_queue = {{{{0}, 0}}, 0, 0, 0};
#endif

#if UART1_ENABLE
uart_TypeDef uart1_struct;
queue_t tx1_queue = {{{{0}, 0}}, 0, 0, 0};
#endif

#if UART2_ENABLE
uart_TypeDef uart2_struct;
queue_t tx2_queue = {{{{0}, 0}}, 0, 0, 0};
#endif

#if UART3_ENABLE
uart_TypeDef uart3_struct;
#endif

#if UART4_ENABLE
//uart_TypeDef uart4_struct;
#endif





/**********************************************************************
**����ԭ��:	void uart0_config(uint32_t baud)
**��������:	����0��ʼ��
**��ڲ���:	uint32_t baud ������
**          
**���ڲ���:	��
**��    ע:	��
************************************************************************/
void uart0_config(uint32_t baud)
{
    /* ʹ�ܴ���ʱ�� */
    rcu_periph_clock_enable(RCU_USART0);
    /* ʹ��GPIOʱ�� */
    rcu_periph_clock_enable(RCU_GPIOA);
    
    /* ����TX���� */
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_9);
    /* ����RX���� */
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_10);

    /* �������� */
    usart_deinit(USART0);
    usart_baudrate_set(USART0, baud);
    usart_word_length_set(USART0, USART_WL_8BIT);
    usart_stop_bit_set(USART0, USART_STB_1BIT);
    usart_parity_config(USART0, USART_PM_NONE);
    usart_hardware_flow_rts_config(USART0, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(USART0, USART_CTS_DISABLE);
    usart_receive_config(USART0, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART0, USART_TRANSMIT_ENABLE);
	//����DMA����
	usart_dma_receive_config(USART0, USART_RECEIVE_DMA_ENABLE);
    usart_dma_transmit_config(USART0, USART_TRANSMIT_DMA_ENABLE);
		
    nvic_irq_enable(USART0_IRQn,1,2);
    usart_interrupt_enable(USART0,USART_INT_IDLE);  //���������ж�
    usart_enable(USART0);
}

/**********************************************************************
**����ԭ��:	void uart0_dma_init(void)
**��������:	����0 DMA ����
**��ڲ���:	��
**          
**���ڲ���:	��
**��    ע:	��
************************************************************************/
void uart0_dma_init(void)
{
    dma_parameter_struct dma_init_struct;
    rcu_periph_clock_enable(RCU_DMA0);

    /*���ô���0����DMAͨ��3 */
    dma_deinit(DMA0, DMA_CH3);
    dma_struct_para_init(&dma_init_struct);

    dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_addr = (uint32_t)NULL;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number = 0;
    dma_init_struct.periph_addr = (uint32_t)(&USART_DATA(USART0));
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA0, DMA_CH3, &dma_init_struct);

	dma_circulation_disable(DMA0, DMA_CH3);
    dma_memory_to_memory_disable(DMA0, DMA_CH3);
		
	dma_interrupt_enable(DMA0, DMA_CH3, DMA_INT_FTF);
	dma_channel_disable(DMA0, DMA_CH3);
	dma_flag_clear(DMA0, DMA_CH3, DMA_FLAG_G);
	nvic_irq_enable(DMA0_Channel3_IRQn, 0, 4);
		
		
    /*���ô���0����DMAͨ��4 */
    dma_deinit(DMA0, DMA_CH4);
    dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_addr = (uint32_t)uart0_struct.rx_buf;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number = 256;
    dma_init_struct.periph_addr = (uint32_t)(&USART_DATA(USART0));
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA0, DMA_CH4, &dma_init_struct);


    /* ����DMAѭ��ģʽ */
    dma_circulation_disable(DMA0, DMA_CH4);
    /* ����DMAͨ��4 */
    dma_channel_enable(DMA0, DMA_CH4);


    
}
/**********************************************************************
**����ԭ��:	void USART0_IRQHandler(void)
**��������:	����0�жϺ���
**��ڲ���:	��
**          
**���ڲ���:	��
**��    ע:	��
************************************************************************/
void USART0_IRQHandler(void)
{
    if(RESET != usart_interrupt_flag_get(USART0, USART_INT_FLAG_IDLE)) {
        /* clear IDLE flag */
        usart_data_receive(USART0);

        /* number of data received */
        uart0_struct.rx_count = 256 - (dma_transfer_number_get(DMA0, DMA_CH4));
        uart0_struct.rx_flag = 1;

        /* disable DMA and reconfigure */
        dma_channel_disable(DMA0, DMA_CH4);
        dma_transfer_number_config(DMA0, DMA_CH4, 256);
        dma_channel_enable(DMA0, DMA_CH4);
    }
}

/**********************************************************************
**����ԭ��:	void DMA0_Channel3_IRQHandler(void)
**��������:	DMA0ͨ��3�жϺ��� 
**��ڲ���:	��
**          
**���ڲ���:	��
**��    ע:	��
************************************************************************/
void DMA0_Channel3_IRQHandler(void)
{
     if(dma_interrupt_flag_get(DMA0,DMA_CH3, DMA_INT_FTF)){
        /* ����DMA��������¼� */
        dma_interrupt_flag_clear(DMA0,DMA_CH3, DMA_INT_FTF); // ���������ɱ�־
        // ... ������������Ӵ������������ݷ�����ϵ���� ...
        uart0_struct.tx_flag = 0;
         // ���������Ƿ��д����͵�����
         uint8_t *data;
         uint32_t size;
         if (dequeue(&tx0_queue, &data, &size) == 0) {
             dma_channel_disable(DMA0, DMA_CH3); // �ر�DMA
             dma_memory_address_config(DMA0, DMA_CH3, (uint32_t)data); // ����DMA�ڴ��ַ
             dma_transfer_number_config(DMA0, DMA_CH3, size); // ����DMA��������
             usart_dma_transmit_config(USART0, USART_TRANSMIT_DMA_ENABLE);
             dma_channel_enable(DMA0, DMA_CH3); // ʹ��DMAͨ��
             uart0_struct.tx_flag = 1;
         }
    }
}

/**********************************************************************
**����ԭ��:	void uart0_dma_send(uint8_t *data,uint32_t size)
**��������:	����0ʹ��DMA���ͺ���
**��ڲ���:	uint8_t *data   ��������
**          uint32_t size   ���鳤��
**���ڲ���:	��
**��    ע:	��
************************************************************************/
void uart0_dma_send(uint8_t *data,uint32_t size)
{
	// dma_channel_disable(DMA0, DMA_CH3);//�ر�DMA
    // dma_memory_address_config(DMA0,DMA_CH3, (uint32_t)data); // ����DMA�ڴ��ַ
    // dma_transfer_number_config(DMA0,DMA_CH3, size); // ����DMA��������
	// usart_dma_transmit_config(USART0, USART_TRANSMIT_DMA_ENABLE);
    // dma_channel_enable(DMA0,DMA_CH3); // ʹ��DMAͨ��

    enqueue(&tx0_queue, data, size);
    
    if (uart0_struct.tx_flag == 0) {
        uint8_t *next_data;
        uint32_t next_size;
        if (dequeue(&tx0_queue, &next_data, &next_size) == 0) {
            dma_channel_disable(DMA0, DMA_CH3); // �ر�DMA
            dma_memory_address_config(DMA0, DMA_CH3, (uint32_t)next_data); // ����DMA�ڴ��ַ
            dma_transfer_number_config(DMA0, DMA_CH3, next_size); // ����DMA��������
            usart_dma_transmit_config(USART0, USART_TRANSMIT_DMA_ENABLE);
            dma_channel_enable(DMA0, DMA_CH3); // ʹ��DMAͨ��
            uart0_struct.tx_flag = 1;
        }
    }
}


/**********************************************************************
**����ԭ��:	void uart0_receive_process(void)
**��������:	UART0�������ݴ���
**��ڲ���:	��
**���ڲ���:	��
**��    ע:	��
************************************************************************/
void uart0_receive_process(void)
{
//    uint8_t tx_cnt;
    if(uart0_struct.rx_flag == 1)
    {
        //���յ����ݷ��ͻ�ȥ
        // for(tx_cnt = 0;tx_cnt<uart0_struct.rx_count;tx_cnt++)
        // {
        //     while(RESET == usart_flag_get(USART0, USART_FLAG_TBE));
        //         usart_data_transmit(USART0, uart0_struct.rx_buf[tx_cnt]);
        // }

		uart0_struct.rx_flag = 0;
    }
}



/**********************************************************************
**����ԭ��:	void uart1_config(uint32_t baud)
**��������:	����1��ʼ��
**��ڲ���:	uint32_t baud ������
**          
**���ڲ���:	��
**��    ע:	��
************************************************************************/
void uart1_config(uint32_t baud)
{
    /* ʹ�ܴ���ʱ�� */
    rcu_periph_clock_enable(RCU_USART1);
    /* ʹ��GPIOʱ�� */
    rcu_periph_clock_enable(RCU_GPIOA);
    
    /* ����TX���� */
    gpio_init(GPIOA, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_2);
    /* ����RX���� */
    gpio_init(GPIOA, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_3);

    /* �������� */
    usart_deinit(USART1);
    usart_baudrate_set(USART1, baud);
    usart_word_length_set(USART1, USART_WL_8BIT);
    usart_stop_bit_set(USART1, USART_STB_1BIT);
    usart_parity_config(USART1, USART_PM_NONE);
    usart_hardware_flow_rts_config(USART1, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(USART1, USART_CTS_DISABLE);
    usart_receive_config(USART1, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART1, USART_TRANSMIT_ENABLE);

    //����DMA����
	usart_dma_receive_config(USART1, USART_RECEIVE_DMA_ENABLE);
    usart_dma_transmit_config(USART1, USART_TRANSMIT_DMA_ENABLE);
		
    nvic_irq_enable(USART1_IRQn,1,1);
    usart_interrupt_enable(USART1,USART_INT_IDLE);  //���������ж�
    usart_enable(USART1);
}


/**********************************************************************
**����ԭ��:	void uart1_dma_init(void)
**��������:	����1 DMA ����
**��ڲ���:	��
**          
**���ڲ���:	��
**��    ע:	��
************************************************************************/
void uart1_dma_init(void)
{
    dma_parameter_struct dma_init_struct;
    rcu_periph_clock_enable(RCU_DMA0);

    /*���ô���1����DMAͨ��6 */
    dma_deinit(DMA0, DMA_CH6);
    dma_struct_para_init(&dma_init_struct);

    dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_addr = (uint32_t)NULL;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number = 0;
    dma_init_struct.periph_addr = (uint32_t)(&USART_DATA(USART1));
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA0, DMA_CH6, &dma_init_struct);

	dma_circulation_disable(DMA0, DMA_CH6);
    dma_memory_to_memory_disable(DMA0, DMA_CH6);
		
	dma_interrupt_enable(DMA0, DMA_CH6, DMA_INT_FTF);
	dma_channel_disable(DMA0, DMA_CH6);
	dma_flag_clear(DMA0, DMA_CH6, DMA_FLAG_G);
	nvic_irq_enable(DMA0_Channel6_IRQn, 0, 3);
		
		
    /*���ô���1����DMAͨ��5 */
    dma_deinit(DMA0, DMA_CH5);
    dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_addr = (uint32_t)uart1_struct.rx_buf;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number = 256;
    dma_init_struct.periph_addr = (uint32_t)(&USART_DATA(USART1));
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA0, DMA_CH5, &dma_init_struct);


    /* ����DMAѭ��ģʽ */
    dma_circulation_disable(DMA0, DMA_CH5);
    /* ����DMAͨ��5 */
    dma_channel_enable(DMA0, DMA_CH5);
}
/**********************************************************************
**����ԭ��:	void USART1_IRQHandler(void)
**��������:	����1�жϺ���
**��ڲ���:	��
**          
**���ڲ���:	��
**��    ע:	��
************************************************************************/
void USART1_IRQHandler(void)
{
    if(RESET != usart_interrupt_flag_get(USART1, USART_INT_FLAG_IDLE)) {
        /* ����жϱ�־λ */
        usart_data_receive(USART1);

        /* ���� */
        uart1_struct.rx_count = 256 - (dma_transfer_number_get(DMA0, DMA_CH5));
        for(uint8_t i = 0;i<uart1_struct.rx_count;i++)
        {
            //queue_push2(uart1_struct.rx_buf[i]);
        }
        uart1_struct.rx_flag = 1;

        /* disable DMA and reconfigure */
        dma_channel_disable(DMA0, DMA_CH5);
        dma_transfer_number_config(DMA0, DMA_CH5, 256);
        dma_channel_enable(DMA0, DMA_CH5);
    }
}

/**********************************************************************
**����ԭ��:	void DMA0_Channel6_IRQHandler(void)
**��������:	DMA0ͨ��6�жϺ���
**��ڲ���:	��
**          
**���ڲ���:	��
**��    ע:	��
************************************************************************/
void DMA0_Channel6_IRQHandler(void)
{
     if(dma_interrupt_flag_get(DMA0,DMA_CH6, DMA_INT_FTF)){
        /* ����DMA��������¼� */
        dma_interrupt_flag_clear(DMA0,DMA_CH6, DMA_INT_FTF); // ���������ɱ�־
        // ... ������������Ӵ������������ݷ�����ϵ���� ...
		uart1_struct.tx_flag = 0;

         // ���������Ƿ��д����͵�����
        uint8_t *data;
        uint32_t size;
        if (dequeue(&tx1_queue, &data, &size) == 0) {
            dma_channel_disable(DMA0, DMA_CH6); // �ر�DMA
            dma_memory_address_config(DMA0, DMA_CH6, (uint32_t)data); // ����DMA�ڴ��ַ
            dma_transfer_number_config(DMA0, DMA_CH6, size); // ����DMA��������
            usart_dma_transmit_config(USART1, USART_TRANSMIT_DMA_ENABLE);
            dma_channel_enable(DMA0, DMA_CH6); // ʹ��DMAͨ��
            uart1_struct.tx_flag = 1;
        }
    }
}

/**********************************************************************
**����ԭ��:	void uart1_dma_send(uint8_t *data,uint32_t size)
**��������:	����1��DMA����
**��ڲ���:	uint8_t *data ��������
**          uint32_t size   �������ݳ���
**���ڲ���:	��
**��    ע:	��
************************************************************************/
void uart1_dma_send(uint8_t *data,uint32_t size)
{
     enqueue(&tx1_queue, data, size);
    
    if (uart1_struct.tx_flag == 0) {
        uint8_t *next_data;
        uint32_t next_size;
        if (dequeue(&tx1_queue, &next_data, &next_size) == 0) {
            dma_channel_disable(DMA0, DMA_CH6); // �ر�DMA
            dma_memory_address_config(DMA0, DMA_CH6, (uint32_t)next_data); // ����DMA�ڴ��ַ
            dma_transfer_number_config(DMA0, DMA_CH6, next_size); // ����DMA��������
            usart_dma_transmit_config(USART1, USART_TRANSMIT_DMA_ENABLE);
            dma_channel_enable(DMA0, DMA_CH6); // ʹ��DMAͨ��
            uart1_struct.tx_flag = 1;
        }
    }
}

/**********************************************************************
**����ԭ��:	void uart1_receive_process(void)
**��������:	UART1�������ݴ���
**��ڲ���:	��
**���ڲ���:	��
**��    ע:	��
************************************************************************/
void uart1_receive_process(void)
{
    // uint8_t tx_cnt;
    if(uart1_struct.rx_flag == 1)
    {

		uart1_struct.rx_flag = 0;
    }
}


void uart1_send_char(unsigned char t)
{
    usart_data_transmit(USART1, t);
    while(RESET == usart_flag_get(USART1, USART_FLAG_TBE));
}


#if UART2_ENABLE
/**********************************************************************
**����ԭ��:	void uart2_config(uint32_t baud)
**��������:	����2��ʼ��
**��ڲ���:	uint32_t baud ������
**          
**���ڲ���:	��
**��    ע:	��
************************************************************************/
void uart2_config(uint32_t baud)
{
    /*ʹ�ܴ���ʱ��*/
    rcu_periph_clock_enable(RCU_USART2);
    /*ʹ��GPIOʱ��*/
    rcu_periph_clock_enable(RCU_GPIOB);
    
    /* ����TX���� */
    gpio_init(GPIOB, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
    /* ����RX���� */
    gpio_init(GPIOB, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_11);

    /* �������� */
    usart_deinit(USART2);
    usart_baudrate_set(USART2, baud);
    usart_word_length_set(USART2, USART_WL_8BIT);
    usart_stop_bit_set(USART2, USART_STB_1BIT);
    usart_parity_config(USART2, USART_PM_NONE);
    usart_hardware_flow_rts_config(USART2, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(USART2, USART_CTS_DISABLE);
    usart_receive_config(USART2, USART_RECEIVE_ENABLE);
    usart_transmit_config(USART2, USART_TRANSMIT_ENABLE);

    //����DMA����
	usart_dma_receive_config(USART2, USART_RECEIVE_DMA_ENABLE);
    usart_dma_transmit_config(USART2, USART_TRANSMIT_DMA_ENABLE);
		
    nvic_irq_enable(USART2_IRQn,1,4);
    usart_interrupt_enable(USART2,USART_INT_IDLE);  //���������ж�
    usart_enable(USART2);
}

/**********************************************************************
**����ԭ��:	void uart2_dma_init(void)
**��������:	����2 DMA ����
**��ڲ���:	��
**          
**���ڲ���:	��
**��    ע:	��
************************************************************************/
void uart2_dma_init(void)
{
    dma_parameter_struct dma_init_struct;
    rcu_periph_clock_enable(RCU_DMA0);

    /*���ô���2����DMAͨ��1 */
    dma_deinit(DMA0, DMA_CH1);
    dma_struct_para_init(&dma_init_struct);

    dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_addr = (uint32_t)NULL;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number = 0;
    dma_init_struct.periph_addr = (uint32_t)(&USART_DATA(USART2));
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA0, DMA_CH1, &dma_init_struct);

	dma_circulation_disable(DMA0, DMA_CH1);
    dma_memory_to_memory_disable(DMA0, DMA_CH1);
		
	dma_interrupt_enable(DMA0, DMA_CH1, DMA_INT_FTF);
	dma_channel_disable(DMA0, DMA_CH1);
	dma_flag_clear(DMA0, DMA_CH1, DMA_FLAG_G);
	nvic_irq_enable(DMA0_Channel1_IRQn, 0, 5);
		
		
    /*���ô���2����DMAͨ��2 */
    dma_deinit(DMA0, DMA_CH2);
    dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_addr = (uint32_t)uart2_struct.rx_buf;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number = 256;
    dma_init_struct.periph_addr = (uint32_t)(&USART_DATA(USART2));
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA0, DMA_CH2, &dma_init_struct);


    /* ����DMAѭ��ģʽ */
    dma_circulation_disable(DMA0, DMA_CH2);
    /* ����DMAͨ��5 */
    dma_channel_enable(DMA0, DMA_CH2);
}

/**********************************************************************
**����ԭ��:	void USART2_IRQHandler(void)
**��������:	����2�жϺ���
**��ڲ���:	��
**          
**���ڲ���:	��
**��    ע:	��
************************************************************************/
void USART2_IRQHandler(void)
{
    if(RESET != usart_interrupt_flag_get(USART2, USART_INT_FLAG_IDLE)) {
        /* clear IDLE flag */
        usart_data_receive(USART2);

        /* number of data received */
        uart2_struct.rx_count = 256 - (dma_transfer_number_get(DMA0, DMA_CH2));
        for(uint8_t i = 0;i<uart2_struct.rx_count;i++)
        {
            //queue_push(uart2_struct.rx_buf[i]);
        }
        uart2_struct.rx_flag = 1;
				
        /* disable DMA and reconfigure */
        dma_channel_disable(DMA0, DMA_CH2);
        dma_transfer_number_config(DMA0, DMA_CH2, 256);
        dma_channel_enable(DMA0, DMA_CH2);

    }
}

/**********************************************************************
**����ԭ��:	void DMA0_Channel1_IRQHandler(void)
**��������:	DMA0��ͨ��1�жϺ���
**��ڲ���:	��
**          
**���ڲ���:	��
**��    ע:	��
************************************************************************/
void DMA0_Channel1_IRQHandler(void)
{
     if(dma_interrupt_flag_get(DMA0,DMA_CH1, DMA_INT_FTF)){
        /* ����DMA��������¼� */
        dma_interrupt_flag_clear(DMA0,DMA_CH1, DMA_INT_FLAG_G); // ���������ɱ�־
        // ... ������������Ӵ������������ݷ�����ϵ���� ...
        uart2_struct.tx_flag = 0;

         // ���������Ƿ��д����͵�����
        uint8_t *data;
        uint32_t size;
        if (dequeue(&tx2_queue, &data, &size) == 0) {
            dma_channel_disable(DMA0, DMA_CH1); // �ر�DMA
            dma_memory_address_config(DMA0, DMA_CH1, (uint32_t)data); // ����DMA�ڴ��ַ
            dma_transfer_number_config(DMA0, DMA_CH1, size); // ����DMA��������
            usart_dma_transmit_config(USART2, USART_TRANSMIT_DMA_ENABLE);
            dma_channel_enable(DMA0, DMA_CH1); // ʹ��DMAͨ��
            uart2_struct.tx_flag = 1;
        }
    }
}

/**********************************************************************
**����ԭ��:	void uart2_dma_send(uint8_t *data,uint32_t size)
**��������:	����2��DMA����
**��ڲ���:	uint8_t *data ��������
**          uint32_t size ���ݳ���
**���ڲ���:	��
**��    ע:	��
************************************************************************/
void uart2_dma_send(uint8_t *data,uint32_t size)
{
    enqueue(&tx2_queue, data, size);
    
    if (uart2_struct.tx_flag == 0) {
        uint8_t *next_data;
        uint32_t next_size;
        if (dequeue(&tx2_queue, &next_data, &next_size) == 0) {
            dma_channel_disable(DMA0, DMA_CH1); // �ر�DMA
            dma_memory_address_config(DMA0, DMA_CH1, (uint32_t)next_data); // ����DMA�ڴ��ַ
            dma_transfer_number_config(DMA0, DMA_CH1, next_size); // ����DMA��������
            usart_dma_transmit_config(USART2, USART_TRANSMIT_DMA_ENABLE);
            dma_channel_enable(DMA0, DMA_CH1); // ʹ��DMAͨ��
            uart2_struct.tx_flag = 1;
        }
    }
}

/**********************************************************************
**����ԭ��:	void uart2_receive_process(void)
**��������:	UART2�������ݴ���
**��ڲ���:	��
**���ڲ���:	��
**��    ע:	��
************************************************************************/
void uart2_receive_process(void)
{
//    uint8_t tx_cnt;
    if(uart2_struct.rx_flag == 1)
    {

		uart2_struct.rx_flag = 0;
    }
}

void uart2_send_char(unsigned char t)
{
    usart_data_transmit(USART2, t);
    while(RESET == usart_flag_get(USART1, USART_FLAG_TBE));
}


#endif // UART2_ENABLE

/**********************************************************************
**����ԭ��:	void uart3_config(uint32_t baud)
**��������:	����3��ʼ��
**��ڲ���:	uint32_t baud ������
**          
**���ڲ���:	��
**��    ע:	��
************************************************************************/
void uart3_config(uint32_t baud)
{
    /* ʹ�ܴ���ʱ�� */
    rcu_periph_clock_enable(RCU_UART3);
    /* ʹ��GPIOʱ�� */
    rcu_periph_clock_enable(RCU_GPIOC);
    /* ����TX���� */
    gpio_init(GPIOC, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_10);
    /* ����RX���� */
    gpio_init(GPIOC, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_11);

    /* �������� */
    usart_deinit(UART3);
    usart_baudrate_set(UART3, baud);
    usart_word_length_set(UART3, USART_WL_8BIT);
    usart_stop_bit_set(UART3, USART_STB_1BIT);
    usart_parity_config(UART3, USART_PM_NONE);
    usart_hardware_flow_rts_config(UART3, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(UART3, USART_CTS_DISABLE);
    usart_receive_config(UART3, USART_RECEIVE_ENABLE);
    usart_transmit_config(UART3, USART_TRANSMIT_ENABLE);
    
   //����DMA����
	// usart_dma_receive_config(UART3, USART_RECEIVE_DMA_ENABLE);
    // usart_dma_transmit_config(UART3, USART_TRANSMIT_DMA_ENABLE);
		
    nvic_irq_enable(UART3_IRQn,1,1);
    //usart_interrupt_enable(UART3,USART_INT_IDLE);  //���������ж�
    usart_enable(UART3);
}

/**********************************************************************
**����ԭ��:	void uart3_dma_init(void)
**��������:	����3 DMA ����
**��ڲ���:	��
**          
**���ڲ���:	��
**��    ע:	��
************************************************************************/
void uart3_dma_init(void)
{
    dma_parameter_struct dma_init_struct;
    rcu_periph_clock_enable(RCU_DMA1);

    /*���ô���3����DMAͨ��4 */
    dma_deinit(DMA1, DMA_CH4);
    dma_struct_para_init(&dma_init_struct);

    dma_init_struct.direction = DMA_MEMORY_TO_PERIPHERAL;
    dma_init_struct.memory_addr = (uint32_t)NULL;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number = 0;
    dma_init_struct.periph_addr = (uint32_t)(&USART_DATA(UART3));
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA1, DMA_CH4, &dma_init_struct);

	dma_circulation_disable(DMA1, DMA_CH4);
    dma_memory_to_memory_disable(DMA1, DMA_CH4);
		
	dma_interrupt_enable(DMA1, DMA_CH4, DMA_INT_FTF);
	dma_channel_disable(DMA1, DMA_CH4);
	dma_flag_clear(DMA1, DMA_CH4, DMA_FLAG_G);
	nvic_irq_enable(DMA1_Channel3_Channel4_IRQn, 0, 5);
		
		
    /*���ô���2����DMAͨ��2 */
    dma_deinit(DMA1, DMA_CH2);
    dma_init_struct.direction = DMA_PERIPHERAL_TO_MEMORY;
    dma_init_struct.memory_addr = (uint32_t)uart3_struct.rx_buf;
    dma_init_struct.memory_inc = DMA_MEMORY_INCREASE_ENABLE;
    dma_init_struct.memory_width = DMA_MEMORY_WIDTH_8BIT;
    dma_init_struct.number = 256;
    dma_init_struct.periph_addr = (uint32_t)(&USART_DATA(UART3));
    dma_init_struct.periph_inc = DMA_PERIPH_INCREASE_DISABLE;
    dma_init_struct.periph_width = DMA_PERIPHERAL_WIDTH_8BIT;
    dma_init_struct.priority = DMA_PRIORITY_ULTRA_HIGH;
    dma_init(DMA1, DMA_CH2, &dma_init_struct);


    /* ����DMAѭ��ģʽ */
    dma_circulation_disable(DMA1, DMA_CH2);
    /* ����DMAͨ��5 */
    dma_channel_enable(DMA1, DMA_CH2);
}

/**********************************************************************
**����ԭ��:	void UART3_IRQHandler(void)
**��������:	����3�жϺ���
**��ڲ���:	��
**          
**���ڲ���:	��
**��    ע:	��
************************************************************************/
//void UART3_IRQHandler(void)
//{
//    if(RESET != usart_interrupt_flag_get(UART3, USART_INT_FLAG_IDLE)) {
//        /* clear IDLE flag */
//        usart_data_receive(UART3);

//        /* number of data received */
//        uart3_struct.rx_count = 256 - (dma_transfer_number_get(DMA1, DMA_CH2));
//        uart3_struct.rx_flag = 1;

//        /* disable DMA and reconfigure */
//        dma_channel_disable(DMA1, DMA_CH2);
//        dma_transfer_number_config(DMA1, DMA_CH2, 256);
//        dma_channel_enable(DMA1, DMA_CH2);
//    }
//}

/**********************************************************************
**����ԭ��:	void DMA1_Channel3_4_IRQHandler(void)
**��������:	DMA1��ͨ��3��4�жϺ���
**��ڲ���:	��
**          
**���ڲ���:	��
**��    ע:	��
************************************************************************/
void DMA1_Channel3_4_IRQHandler(void)
{
     if(dma_interrupt_flag_get(DMA1,DMA_CH4, DMA_INT_FTF)){
        /* ����DMA��������¼� */
        dma_interrupt_flag_clear(DMA1,DMA_CH4, DMA_INT_FTF); // ���������ɱ�־
        // ... ������������Ӵ������������ݷ�����ϵ���� ...
			 
    }
}
/**********************************************************************
**����ԭ��:	void uart3_dma_send(uint8_t *data,uint32_t size)
**��������:	DMA1��ͨ��3��4�жϺ���
**��ڲ���:	uint8_t *data   ��������
**          uint32_t size   ���ݳ���
**���ڲ���:	��
**��    ע:	��
************************************************************************/
void uart3_dma_send(uint8_t *data,uint32_t size)
{
    dma_channel_disable(DMA1, DMA_CH4);//�ر�DMA
    dma_memory_address_config(DMA1,DMA_CH4, (uint32_t)data); // ����DMA�ڴ��ַ
    dma_transfer_number_config(DMA1,DMA_CH4, size); // ����DMA��������
	usart_dma_transmit_config(UART3, USART_TRANSMIT_DMA_ENABLE);
    dma_channel_enable(DMA1,DMA_CH4); // ʹ��DMAͨ��
}

/**********************************************************************
**����ԭ��:	void uart3_receive_process(void)
**��������:	UART3�������ݴ���
**��ڲ���:	��
**���ڲ���:	��
**��    ע:	��
************************************************************************/
void uart3_receive_process(void)
{
    if(uart3_struct.rx_flag == 1)
    {

		uart3_struct.rx_flag = 0;
    }
}

/**********************************************************************
**����ԭ��:	void uart4_config(uint32_t baud)
**��������:	����4��ʼ��
**��ڲ���:	uint32_t baud ������
**          
**���ڲ���:	��
**��    ע:	��
************************************************************************/
void uart4_config(uint32_t baud)
{
    /* ʹ�ܴ���ʱ�� */
    rcu_periph_clock_enable(RCU_UART4);
    /* ʹ��GPIOʱ�� */
    rcu_periph_clock_enable(RCU_GPIOC);
    rcu_periph_clock_enable(RCU_GPIOD);
    /* ����TX���� */
    gpio_init(GPIOC, GPIO_MODE_AF_PP, GPIO_OSPEED_50MHZ, GPIO_PIN_12);
    /* ����RX���� */
    gpio_init(GPIOD, GPIO_MODE_IN_FLOATING, GPIO_OSPEED_50MHZ, GPIO_PIN_2);

    /* �������� */
    usart_deinit(UART4);
    usart_baudrate_set(UART4, baud);
    usart_word_length_set(UART4, USART_WL_8BIT);
    usart_stop_bit_set(UART4, USART_STB_1BIT);
    usart_parity_config(UART4, USART_PM_NONE);
    usart_hardware_flow_rts_config(UART4, USART_RTS_DISABLE);
    usart_hardware_flow_cts_config(UART4, USART_CTS_DISABLE);
    usart_receive_config(UART4, USART_RECEIVE_ENABLE);
    usart_transmit_config(UART4, USART_TRANSMIT_ENABLE);
		
    nvic_irq_enable(UART4_IRQn,0,2);
    usart_interrupt_enable(UART4,USART_INT_RBNE);  //���������ж�,
	// usart_interrupt_enable(UART4,USART_INT_IDLE);
    usart_enable(UART4);
}


/**********************************************************************
**����ԭ��:	void Uart4_send_buf(uint8_t *buf,uint8_t len)
**��������:	����4�����ַ���
**��ڲ���:	uint8_t *buf ��������
            uint8_t len ���ݳ���
**���ڲ���:	��
**��    ע:	��
************************************************************************/
void uart4_send_buf(uint8_t *buf,uint8_t len)
{
	while(len>0)
	{
		usart_data_transmit(UART4, *buf);
        while(RESET == usart_flag_get(UART4, USART_FLAG_TBE));
        buf++;
            len--;
	}
}
/**********************************************************************
**����ԭ��:	void uart4_send_str(const char *str)
**��������:	����4�����ַ���
**��ڲ���:	const char *str �����ַ���
**���ڲ���:	��
**��    ע:	��
************************************************************************/
void uart4_send_str(const char *str)
{
    while (*str != '\0') {
        usart_data_transmit(UART4, *str);
        while(RESET == usart_flag_get(UART4, USART_FLAG_TBE));
        str++;
    }
}


/**********************************************************************
**����ԭ��:	void uart3_receive_process(void)
**��������:	UART3�������ݴ���
**��ڲ���:	��
**���ڲ���:	��
**��    ע:	��
************************************************************************/
void uart4_receive_process(void)
{
    // uint8_t tx_cnt;
    // if(uart4_struct.rx_flag == 1)
    // {
       
	// 	uart4_struct.rx_flag = 0;
    // }
}




/**********************************************************************
**����ԭ��:	void uart1_send_str(const char *str)
**��������:	����1�����ַ���
**��ڲ���:	const char *str �����ַ���
**���ڲ���:	��
**��    ע:	��
************************************************************************/
void uart1_send_str(const char *str)
{
    while (*str != '\0') {
        usart_data_transmit(USART1, *str);
        while(RESET == usart_flag_get(USART1, USART_FLAG_TBE));
        str++;
    }
}

/**********************************************************************
**����ԭ��:	void Uart3_send_buf(uint8_t *buf,uint8_t len)
**��������:	����3�����ַ���
**��ڲ���:	uint8_t *buf ��������
            uint8_t len ���ݳ���
**���ڲ���:	��
**��    ע:	��
************************************************************************/
void Uart3_send_buf(uint8_t *buf,uint8_t len)
{
	while(len>0)
	{
		usart_data_transmit(UART3, *buf);
    while(RESET == usart_flag_get(UART3, USART_FLAG_TBE));
    buf++;
		len--;
	}
}


int enqueue(queue_t *queue, uint8_t *data, uint32_t length) {
    if (length > MAX_DATA_SIZE) {
        return -1; // ����̫��
    }

    if (queue->count == QUEUE_SIZE) {
        // �����������ǵ�����ͷ������
        queue->front = (queue->front + 1) % QUEUE_SIZE;
        queue->count--;
    }

    memcpy(queue->items[queue->rear].data, data, length);
    queue->items[queue->rear].length = length;
    queue->rear = (queue->rear + 1) % QUEUE_SIZE;
    queue->count++;
    return 0;
}

int dequeue(queue_t *queue, uint8_t **data, uint32_t *length) {
    if (queue->count == 0) {
        return -1; // ���п�
    }
    *data = queue->items[queue->front].data;
    *length = queue->items[queue->front].length;
    queue->front = (queue->front + 1) % QUEUE_SIZE;
    queue->count--;
    return 0;
}

/* retarget the C library printf function to the USART */
int fputc(int ch, FILE *f)
{
    usart_data_transmit(USART0, (uint8_t)ch);
    while (RESET == usart_flag_get(USART0, USART_FLAG_TBE))
        ;

    return ch;
}

