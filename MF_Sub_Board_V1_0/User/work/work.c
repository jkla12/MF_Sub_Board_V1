#include "work.h"
#include "public.h"



#ifdef 	LOG_TAG
#undef	LOG_TAG
#endif
#define LOG_TAG    "work"

static led_typedef_enum led1 = LED1;

uint8_t deviceRunState = 0 ; // 设备状态,0空闲，1运行，2异常

State_TypeDef machine_state = {0}; // 机器状态

/* 全局变量 */
/* 全局变量 */
TaskQueue_t task_queue = {NULL, NULL, 0};  // 任务队列
uint8_t current_state = 0;                 // 当前状态

/* 系统状态机 */
State_t* state_machine = NULL;          // 状态机
uint8_t state_count = 0;                // 状态数量


/* 内存分配失败处理函数 */
void memory_allocation_failed(void) {
    // 处理内存分配失败，可以点亮错误LED，进入安全模式等
    while(1) {
        // 安全循环
        LED_ON(LED2);
    }
}

/* 初始化任务队列 */
void init_task_queue(void) {
    task_queue.head = NULL;
    task_queue.tail = NULL;
    task_queue.count = 0;
}

/* 硬件初始化函数 */
void hw_init(void)
{
    //初始化时钟系统
    systick_config();
    nvic_priority_group_set(NVIC_PRIGROUP_PRE1_SUB3);
    //初始化GPIO
    led_init();
    Button_gpio_init();
    DR_Key_Init();
    //初始化串口
    uart0_config(115200);
    uart0_dma_init();
    eMBInit(MB_RTU, 0x02, 0x01, 115200, MB_PAR_NONE);
    eMBEnable();
    Timer = (Timer_TypeDef){0}; // 初始化定时器结构体
    // 其他外设初始化
    dial_init();
    //relay_init();		//MOS输出初始化
    NTC_Init(); // NTC温度传感器初始化
    level_sensor_init();
    output_gpio_init(); // 输出GPIO初始化
    elog_init();
	elog_set_fmt(ELOG_LVL_ASSERT, ELOG_FMT_ALL);
	elog_set_fmt(ELOG_LVL_ERROR, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
	elog_set_fmt(ELOG_LVL_WARN, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
	elog_set_fmt(ELOG_LVL_INFO, ELOG_FMT_LVL | ELOG_FMT_TAG | ELOG_FMT_TIME);
    //除了时间，进程线程信息之外，其余全部输出
	elog_set_fmt(ELOG_LVL_DEBUG, ELOG_FMT_ALL & ~(ELOG_FMT_FUNC | ELOG_FMT_T_INFO | ELOG_FMT_P_INFO));
	elog_set_fmt(ELOG_LVL_VERBOSE, ELOG_FMT_ALL & ~(ELOG_FMT_FUNC | ELOG_FMT_T_INFO | ELOG_FMT_P_INFO));
    // 设置日志过滤级别为 DEBUG，只输出 DEBUG 及以上等级的日志
   //elog_set_filter_lvl(ELOG_LVL_ERROR);
	//开启log输出
	elog_start();

    elog_set_output_enabled(true);
}

/**
 * ************************************************************************
 * @brief  初始化任务
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
    add_task_with_param(led_toggle_param,&led1,NULL, 100, true);    //LED运行指示灯
    add_task(temp_read_task, NULL, 1000, true);                     // 温度读取任务
    add_task(level_modbus_task, NULL, 100, true);                    // 液位传感器状态读取任务
    add_task(Output_task, NULL, 100, true);                          // 输出任务
}

/**
 * ************************************************************************
 * @brief  添加任务
 * 
 * @param[in] func  任务函数指针
 * @param[in] cond   任务条件函数指针
 * @param[in] period  任务周期（毫秒）
 * @param[in] enabled  任务是否启用
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
    // 分配新任务节点内存
    TaskNode_t* new_task = (TaskNode_t*)malloc(sizeof(TaskNode_t));
    if (new_task == NULL) {
        memory_allocation_failed();
        return false;
    }
    
    // 初始化任务节点
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
    
    // 添加到队列
    if (task_queue.head == NULL) {
        // 空队列
        task_queue.head = new_task;
        task_queue.tail = new_task;
    } else {
        // 添加到队列尾部
        task_queue.tail->next = new_task;
        task_queue.tail = new_task;
    }
    
    task_queue.count++;
    return true;
}

/**
 * ************************************************************************
 * @brief  添加带参数的任务
 * 
 * @param[in] func  任务函数指针
 * @param[in] param  任务参数指针
 * @param[in] cond  任务条件函数指针
 * @param[in] period  任务周期（毫秒）
 * @param[in] enabled  任务是否启用
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
    // 分配新任务节点内存
    TaskNode_t* new_task = (TaskNode_t*)malloc(sizeof(TaskNode_t));
    if (new_task == NULL) {
        memory_allocation_failed();
        return false;
    }
    
    // 初始化任务节点
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
    
    // 添加到队列
    if (task_queue.head == NULL) {
        // 空队列
        task_queue.head = new_task;
        task_queue.tail = new_task;
    } else {
        // 添加到队列尾部
        task_queue.tail->next = new_task;
        task_queue.tail = new_task;
    }
    
    task_queue.count++;
    return true;
}

/**
 * ************************************************************************
 * @brief  删除任务
 * 
 * @param[in] func  任务函数指针
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
            // 找到要删除的任务
            if (prev == NULL) {
                // 删除头节点
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
 * @brief  删除带参数的任务
 * 
 * @param[in] func  任务函数指针
 * @param[in] param  任务参数指针
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
            // 找到要删除的任务
            if (prev == NULL) {
                // 删除头节点
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
 * @brief  启用任务
 * 
 * @param[in] func  任务函数指针
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

/* 启用带参数的任务 */
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
 * @brief  禁用任务
 * 
 * @param[in] func  任务函数指针
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
 * @brief  禁用带参数的任务
 * 
 * @param[in] func  任务函数指针
 * @param[in] param     任务参数指针
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
 * @brief  设置状态机
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
 * @brief  执行状态机
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
    
    // 执行当前状态动作
    if (state_machine[current_state].action != NULL) {
        state_machine[current_state].action();
    }
    
    // 检查状态转换
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
 * @brief  任务调度器
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
            // 检查执行条件
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
    
    eMBPoll(); // 调用Modbus轮询函数
}



/**
 * ************************************************************************
 * @brief  LED闪烁函数
 * 
 * @param[in] led_num  LED编号
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
 * @brief  读取温度任务函数
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
    NTC_Read(&usRegInputBuf[0]); // 读取NTC温度值
}

/**
 * ************************************************************************
 * @brief  液位传感器读取任务函数
 * 
 * ************************************************************************
 */
void level_modbus_task(void)
{
    static LevelSensor_Status_t sensor_status;
    level_sensor_read(&sensor_status);
}


