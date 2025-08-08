#ifndef __WORK_H
#define __WORK_H

#include "gd32f30x.h"

#define true 1
#define false 0

/* ϵͳ���� */
#define SYSTEM_TICK_MS 1 // ϵͳʱ�ӽ���(ms)

extern uint8_t deviceRunState; 

/* ���Ͷ��� */
typedef void (*task_func_t)(void);         // ������ָ������
typedef void (*task_func_param_t)(void *); // ��������������ָ������
typedef uint8_t (*task_condition_t)(void); // ������������ָ������

/* ����ڵ�ṹ�� */
typedef struct TaskNode {
    task_func_t     function;      // �޲���������
    task_func_param_t param_function; // ������������
    void*           param;         // ��������
    uint8_t            has_param;     // �Ƿ��в���
    task_condition_t condition;    // ִ������
    uint16_t        period;        // ����(ms)
    uint16_t        counter;       // ������
    uint8_t            ready;         // ������־
    uint8_t            enabled;       // ʹ�ܱ�־
    struct TaskNode* next;         // ��һ������ڵ�
} TaskNode_t;


/* ������� */
typedef struct {
    TaskNode_t* head;              // ����ͷ
    TaskNode_t* tail;              // ����β
    uint16_t    count;             // �������
} TaskQueue_t;



/* ״̬�����Ͷ��� */
typedef void (*state_func_t)(void);         // ״̬����ָ������
typedef uint8_t (*transition_func_t)(void); // ״̬ת����������ָ������

/* ״̬ת���ṹ�� */
typedef struct
{
    uint8_t next_state;          // ��һ��״̬
    transition_func_t condition; // ת������
} StateTransition_t;

/* ״̬�ṹ�� */
typedef struct
{
    state_func_t action;            // ״̬����
    StateTransition_t *transitions; // ״̬ת����
    uint8_t num_transitions;        // ת������
} State_t;

extern TaskQueue_t task_queue;
static uint8_t current_state;


//���������ṹ��
typedef struct {
    uint8_t planIndex;          // �������� 0-13
    uint8_t changeFlag;         // ���������仯��־
    char    name[13];           // �������� ���12����ĸ����6����
    uint16_t materialsTime[3];   // ����ʱ��
    uint8_t materialsAir[3];    // ����1����
    uint8_t materialsHeat[3];   // ����1����
    uint8_t espressoFirst;      // Ũ���ȳ�
    uint16_t waterTime;          // ˮʱ��
    uint8_t waterHeat;          // ˮ���� 
}Plan_Parameter_TypeDef;

typedef struct{
    uint8_t pumpPreStartTime;       //ˮ��Ԥ��ȡʱ�䣬��λΪ.1�� �Ŵ�10��
    uint8_t pipelineFillingTime;    //�ܵ����ʱ�䣬��λΪ.1��  �Ŵ�10��
    uint8_t pipelineDrainTime;      //�ܵ��ſ�ʱ�䣬��λΪ.1��  �Ŵ�10��
    uint8_t pumpSpeed;              //ˮ���ٶȣ�0-100
    uint8_t temp;                   //�����¶ȣ���λΪ���϶�
}material_Parameter_TypeDef;

typedef struct {
    uint8_t workMode;                                   // ����ģʽ 0,����ģʽ��1��������ϴ��2�������Ӵ�����ϴ��3����������ϴ��4��ҩƬ��ϴ��5����Ʒ��6���ſ�
    uint8_t workStep;                                   //��������
    uint8_t makeIndex;                                  // ��ǰ������Ʒ������ 0-13
    Plan_Parameter_TypeDef planParameter;               // ��ǰ��Ʒ��������
    uint8_t standbyCleanTimeCache;                      // ������ϴʱ�仺�� 
    material_Parameter_TypeDef materialParameterCache;  // ���ϲ�������
    char materialNameCache[3][13];                      // �������ƻ���
    uint8_t waterTempCache;                             // ��ˮ�¶Ȼ���
} State_TypeDef;





extern State_TypeDef machine_state;






void memory_allocation_failed(void);
void init_task_queue(void);
void hw_init(void);
void task_init(void);
uint8_t add_task(task_func_t func, task_condition_t cond, uint16_t period, uint8_t enabled);
uint8_t add_task_with_param(task_func_param_t func, void *param, task_condition_t cond, uint16_t period, uint8_t enabled);
uint8_t delete_task(task_func_t func);
uint8_t delete_task_with_param(task_func_param_t func, void* param);

uint8_t enable_task(task_func_t func);
uint8_t enable_task_with_param(task_func_param_t func, void* param);
uint8_t disable_task(task_func_t func);
uint8_t disable_task_with_param(task_func_param_t func, void* param);
void set_state_machine(State_t *states, uint8_t count);
void execute_state_machine(void);
void scheduler(void);

void led_toggle_param(void *led_num);
void temp_read_task(void);

void level_modbus_task(void);

#endif
