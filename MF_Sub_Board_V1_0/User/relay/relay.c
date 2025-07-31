#include "relay.h"
#include "public.h"

#ifdef 	LOG_TAG
#undef	LOG_TAG
#endif
#define LOG_TAG    "relay"


/**
 * ************************************************************************
 * @brief  初始化输出
 * 
 * 
 * 
 * @version 1.0
 * @author jiaokai 
 * @date 2025-06-20
 * 
 * ************************************************************************
 */
void relay_init(void)
{
    rcu_periph_clock_enable(RCU_GPIOB);
    for(uint8_t i = 0; i < RELAY_NUM; i++)
    {
        gpio_init(RELAY_GPIO_PORT[i], GPIO_MODE_OUT_PP, GPIO_OSPEED_50MHZ, RELAY_GPIO_PIN[i]);
        GPIO_BC(RELAY_GPIO_PORT[i]) = RELAY_GPIO_PIN[i];
    }
}

/**
 * ************************************************************************
 * @brief  打开输出
 * 
 * @param[in] number  输出编号
 * 
 * 
 * @version 1.0
 * @author jiaokai 
 * @date 2025-06-20
 * 
 * ************************************************************************
 */
void relay_ON(Relay_typedef_enum number)
{
    GPIO_BOP(RELAY_GPIO_PORT[number]) = RELAY_GPIO_PIN[number];
}

/**
 * ************************************************************************
 * @brief  关闭输出
 * 
 * @param[in] number  输出编号
 * 
 * 
 * @version 1.0
 * @author jiaokai 
 * @date 2025-06-20
 * 
 * ************************************************************************
 */
void relay_OFF(Relay_typedef_enum number)
{
    GPIO_BC(RELAY_GPIO_PORT[number]) = RELAY_GPIO_PIN[number];
}
