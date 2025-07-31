#ifndef __RELAY_H
#define __RELAY_H

#include "gd32f30x.h"

#define Relay1_PORT GPIOB
#define Relay1_PIN  GPIO_PIN_12
#define Relay2_PORT GPIOB
#define Relay2_PIN  GPIO_PIN_13
#define Relay3_PORT GPIOB
#define Relay3_PIN  GPIO_PIN_14


#define RELAY_NUM 3 // �̵�������
static uint32_t RELAY_GPIO_PORT[RELAY_NUM] = {
    Relay1_PORT,Relay2_PORT,Relay3_PORT
};
static uint32_t RELAY_GPIO_PIN[RELAY_NUM] = {
    Relay1_PIN,Relay2_PIN,Relay3_PIN
};
typedef enum{
    Relay1 = 0,	//��Һѡ����
    Relay2 = 1,	//Ԥ��
    Relay3 = 2,	//Ԥ��
}Relay_typedef_enum;




void relay_init(void);
void relay_ON(Relay_typedef_enum number);
void relay_OFF(Relay_typedef_enum number);



#endif


