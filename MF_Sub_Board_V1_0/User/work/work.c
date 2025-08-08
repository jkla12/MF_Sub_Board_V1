#include "work.h"
#include "public.h"



#ifdef 	LOG_TAG
#undef	LOG_TAG
#endif
#define LOG_TAG    "work"

static led_typedef_enum led1 = LED1;

uint8_t deviceRunState = 0 ; // �豸״̬,0���У�1���У�2�쳣

State_TypeDef machine_state = {0}; // ����״̬

/* ȫ�ֱ��� */
/* ȫ�ֱ��� */
TaskQueue_t task_queue = {NULL, NULL, 0};  // �������
uint8_t current_state = 0;                 // ��ǰ״̬

/* ϵͳ״̬�� */
State_t* state_machine = NULL;          // ״̬��
uint8_t state_count = 0;                // ״̬����


/* �ڴ����ʧ�ܴ����� */
void memory_allocation_failed(void) {
    // �����ڴ����ʧ�ܣ����Ե�������LED�����밲ȫģʽ��
    while(1) {
        // ��ȫѭ��
        LED_ON(LED2);
    }
}

/* ��ʼ��������� */
void init_task_queue(void) {
    task_queue.head = NULL;
    task_queue.tail = NULL;
    task_queue.count = 0;
}

/* Ӳ����ʼ������ */
void hw_init(void)
{
    //��ʼ��ʱ��ϵͳ
    systick_config();
    nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);
    //��ʼ��GPIO
    led_init();
    Button_gpio_init();
    DR_Key_Init();
    //��ʼ������
    uart0_config(115200);
    uart0_dma_init();
    eMBInit(MB_RTU, 0x02, 0x01, 115200, MB_PAR_NONE);
    eMBEnable();
    Timer = (Timer_TypeDef){0}; // ��ʼ����ʱ���ṹ��
    // ���������ʼ��
    dial_init();
    //relay_init();		//MOS�����ʼ��
    NTC_Init(); // NTC�¶ȴ�������ʼ��
    level_sensor_init();
    output_gpio_init(); // ���GPIO��ʼ��
    elog_init();
	elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
	elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
	elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
	elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    //����ʱ�䣬�����߳���Ϣ֮�⣬����ȫ�����
	elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL & ~(ELOG_FMT_FUNC | ELOG_FMT_T_INFO | ELOG_FMT_P_INFO));
	elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~(ELOG_FMT_FUNC | ELOG_FMT_T_INFO | ELOG_FMT_P_INFO));
    // ������־���˼���Ϊ DEBUG��ֻ��� DEBUG �����ϵȼ�����־
   //elog_set_filter_lvl(ELOG_LVL_ERROR);
	//����log���
	elog_start();

    elog_set_output_enabled(true);
}

/**
 * ************************************************************************
 * @brief  ��ʼ������
 * 
 * 
 * 
 * @version 1.0
 * @author jiaokai 
 * @date 2025-06-20
 * 
 * ************************************************************************
 */
void task_init(void)
{
    add_task_with_param(led_toggle_param,&led1,NULL, 100, true);    //LED����ָʾ��
    add_task(temp_read_task, NULL, 1000, true);                     // �¶ȶ�ȡ����
    add_task(level_modbus_task, NULL, 100, true);                    // Һλ������״̬��ȡ����
    add_task(Output_task, NULL, 100, true);                          // �������
}

/**
 * ************************************************************************
 * @brief  �������
 * 
 * @param[in] func  ������ָ��
 * @param[in] cond   ������������ָ��
 * @param[in] period  �������ڣ����룩
 * @param[in] enabled  �����Ƿ�����
 * 
 * @return 
 * 
 * @version 1.0
 * @author jiaokai 
 * @date 2025-07-25
 * 
 * ************************************************************************
 */
uint8_t add_task(task_func_t func, task_condition_t cond, uint16_t period, uint8_t enabled)
{
    // ����������ڵ��ڴ�
    TaskNode_t* new_task = (TaskNode_t*)malloc(sizeof(TaskNode_t));
    if (new_task == NULL) {
        memory_allocation_failed();
        return false;
    }
    
    // ��ʼ������ڵ�
    new_task->function = func;
    new_task->param_function = NULL;
    new_task->param = NULL;
    new_task->has_param = false;
    new_task->condition = cond;
    new_task->period = period;
    new_task->counter = 0;
    new_task->ready = false;
    new_task->enabled = enabled;
    new_task->next = NULL;
    
    // ��ӵ�����
    if (task_queue.head == NULL) {
        // �ն���
        task_queue.head = new_task;
        task_queue.tail = new_task;
    } else {
        // ��ӵ�����β��
        task_queue.tail->next = new_task;
        task_queue.tail = new_task;
    }
    
    task_queue.count++;
    return true;
}

/**
 * ************************************************************************
 * @brief  ��Ӵ�����������
 * 
 * @param[in] func  ������ָ��
 * @param[in] param  �������ָ��
 * @param[in] cond  ������������ָ��
 * @param[in] period  �������ڣ����룩
 * @param[in] enabled  �����Ƿ�����
 * 
 * @return 
 * 
 * @version 1.0
 * @author jiaokai 
 * @date 2025-07-25
 * 
 * ************************************************************************
 */
uint8_t add_task_with_param(task_func_param_t func, void* param, task_condition_t cond, uint16_t period, uint8_t enabled)
{
    // ����������ڵ��ڴ�
    TaskNode_t* new_task = (TaskNode_t*)malloc(sizeof(TaskNode_t));
    if (new_task == NULL) {
        memory_allocation_failed();
        return false;
    }
    
    // ��ʼ������ڵ�
    new_task->function = NULL;
    new_task->param_function = func;
    new_task->param = param;
    new_task->has_param = true;
    new_task->condition = cond;
    new_task->period = period;
    new_task->counter = 0;
    new_task->ready = false;
    new_task->enabled = enabled;
    new_task->next = NULL;
    
    // ��ӵ�����
    if (task_queue.head == NULL) {
        // �ն���
        task_queue.head = new_task;
        task_queue.tail = new_task;
    } else {
        // ��ӵ�����β��
        task_queue.tail->next = new_task;
        task_queue.tail = new_task;
    }
    
    task_queue.count++;
    return true;
}

/**
 * ************************************************************************
 * @brief  ɾ������
 * 
 * @param[in] func  ������ָ��
 * 
 * @return 
 * 
 * @version 1.0
 * @author jiaokai 
 * @date 2025-07-25
 * 
 * ************************************************************************
 */
uint8_t delete_task(task_func_t func) {
    TaskNode_t* current = task_queue.head;
    TaskNode_t* prev = NULL;
    
    while (current != NULL) {
        if (!current->has_param && current->function == func) {
            // �ҵ�Ҫɾ��������
            if (prev == NULL) {
                // ɾ��ͷ�ڵ�
                task_queue.head = current->next;
                if (task_queue.head == NULL) {
                    task_queue.tail = NULL;
                }
            } else {
                prev->next = current->next;
                if (current == task_queue.tail) {
                    task_queue.tail = prev;
                }
            }
            
            free(current);
            task_queue.count--;
            return true;
        }
        
        prev = current;
        current = current->next;
    }
    
    return false;
}

/**
 * ************************************************************************
 * @brief  ɾ��������������
 * 
 * @param[in] func  ������ָ��
 * @param[in] param  �������ָ��
 * 
 * @return 
 * 
 * @version 1.0
 * @author jiaokai 
 * @date 2025-07-25
 * 
 * ************************************************************************
 */
uint8_t delete_task_with_param(task_func_param_t func, void* param) {
    TaskNode_t* current = task_queue.head;
    TaskNode_t* prev = NULL;
    
    while (current != NULL) {
        if (current->has_param && current->param_function == func && current->param == param) {
            // �ҵ�Ҫɾ��������
            if (prev == NULL) {
                // ɾ��ͷ�ڵ�
                task_queue.head = current->next;
                if (task_queue.head == NULL) {
                    task_queue.tail = NULL;
                }
            } else {
                prev->next = current->next;
                if (current == task_queue.tail) {
                    task_queue.tail = prev;
                }
            }
            
            free(current);
            task_queue.count--;
            return true;
        }
        
        prev = current;
        current = current->next;
    }
    
    return false;
}

/**
 * ************************************************************************
 * @brief  ��������
 * 
 * @param[in] func  ������ָ��
 * 
 * @return 
 * 
 * @version 1.0
 * @author jiaokai 
 * @date 2025-07-25
 * 
 * ************************************************************************
 */
uint8_t enable_task(task_func_t func) {
    TaskNode_t* current = task_queue.head;
    
    while (current != NULL) {
        if (!current->has_param && current->function == func) {
            current->enabled = true;
            return true;
        }
        current = current->next;
    }
    
    return false;
}

/* ���ô����������� */
uint8_t enable_task_with_param(task_func_param_t func, void* param) {
    TaskNode_t* current = task_queue.head;
    
    while (current != NULL) {
        if (current->has_param && current->param_function == func && current->param == param) {
            current->enabled = true;
            return true;
        }
        current = current->next;
    }
    
    return false;
}
/**
 * ************************************************************************
 * @brief  ��������
 * 
 * @param[in] func  ������ָ��
 * 
 * @return 
 * 
 * @version 1.0
 * @author jiaokai 
 * @date 2025-07-25
 * 
 * ************************************************************************
 */
uint8_t disable_task(task_func_t func) {
    TaskNode_t* current = task_queue.head;
    
    while (current != NULL) {
        if (!current->has_param && current->function == func) {
            current->enabled = false;
            return true;
        }
        current = current->next;
    }
    
    return false;
}
/**
 * ************************************************************************
 * @brief  ���ô�����������
 * 
 * @param[in] func  ������ָ��
 * @param[in] param     �������ָ��
 * 
 * @return 
 * 
 * @version 1.0
 * @author jiaokai 
 * @date 2025-07-25
 * 
 * ************************************************************************
 */
uint8_t disable_task_with_param(task_func_param_t func, void* param) {
    TaskNode_t* current = task_queue.head;
    
    while (current != NULL) {
        if (current->has_param && current->param_function == func && current->param == param) {
            current->enabled = false;
            return true;
        }
        current = current->next;
    }
    
    return false;
}
/**
 * ************************************************************************
 * @brief  ����״̬��
 * 
 * @param[in] states  Comment
 * @param[in] count  Comment
 * 
 * 
 * @version 1.0
 * @author jiaokai 
 * @date 2025-07-25
 * 
 * ************************************************************************
 */
void set_state_machine(State_t* states, uint8_t count)
{
    state_machine = states;
    state_count = count;
    current_state = 0;
}
/**
 * ************************************************************************
 * @brief  ִ��״̬��
 * 
 * 
 * 
 * @version 1.0
 * @author jiaokai 
 * @date 2025-07-25
 * 
 * ************************************************************************
 */
void execute_state_machine(void)
{
    if (state_machine == NULL || current_state >= state_count) {
        return;
    }
    
    // ִ�е�ǰ״̬����
    if (state_machine[current_state].action != NULL) {
        state_machine[current_state].action();
    }
    
    // ���״̬ת��
    uint8_t i;
    for (i = 0; i < state_machine[current_state].num_transitions; i++) {
        StateTransition_t* trans = &state_machine[current_state].transitions[i];
        if (trans->condition != NULL && trans->condition()) {
            current_state = trans->next_state;
            break;
        }
    }
}
/**
 * ************************************************************************
 * @brief  ���������
 * 
 * 
 * 
 * @version 1.0
 * @author jiaokai 
 * @date 2025-07-25
 * 
 * ************************************************************************
 */
void scheduler(void)
{
    TaskNode_t* current = task_queue.head;
    
    while (current != NULL) {
        if (current->ready && current->enabled) {
            // ���ִ������
            if (current->condition == NULL || current->condition()) {
                if (current->has_param) {
                    current->param_function(current->param);
                } else {
                    current->function();
                }
            }
            current->ready = false;
        }
        current = current->next;
    }
    
    eMBPoll(); // ����Modbus��ѯ����
}



/**
 * ************************************************************************
 * @brief  LED��˸����
 * 
 * @param[in] led_num  LED���
 * 
 * 
 * @version 1.0
 * @author jiaokai 
 * @date 2025-06-20
 * 
 * ************************************************************************
 */
void led_toggle_param(void* led_num) {
    gd_eval_led_toggle(*(led_typedef_enum*)led_num);
}

/**
 * ************************************************************************
 * @brief  ��ȡ�¶�������
 * 
 * 
 * 
 * @version 1.0
 * @author jiaokai 
 * @date 2025-07-25
 * 
 * ************************************************************************
 */
void temp_read_task(void)
{
    NTC_Read(&usRegInputBuf[0]); // ��ȡNTC�¶�ֵ
}

/**
 * ************************************************************************
 * @brief  Һλ��������ȡ������
 * 
 * ************************************************************************
 */
void level_modbus_task(void)
{
    static LevelSensor_Status_t sensor_status;
    level_sensor_read(&sensor_status);
}


