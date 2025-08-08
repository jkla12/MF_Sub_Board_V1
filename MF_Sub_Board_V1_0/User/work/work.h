#ifndef __WORK_H
#define __WORK_H

#include "gd32f30x.h"

#define true 1
#define false 0

/* 系统配置 */
#define SYSTEM_TICK_MS 1 // 系统时钟节拍(ms)

extern uint8_t deviceRunState; 

/* 类型定义 */
typedef void (*task_func_t)(void);         // 任务函数指针类型
typedef void (*task_func_param_t)(void *); // 带参数的任务函数指针类型
typedef uint8_t (*task_condition_t)(void); // 任务条件函数指针类型

/* 任务节点结构体 */
typedef struct TaskNode {
    task_func_t     function;      // 无参数任务函数
    task_func_param_t param_function; // 带参数任务函数
    void*           param;         // 函数参数
    uint8_t            has_param;     // 是否有参数
    task_condition_t condition;    // 执行条件
    uint16_t        period;        // 周期(ms)
    uint16_t        counter;       // 计数器
    uint8_t            ready;         // 就绪标志
    uint8_t            enabled;       // 使能标志
    struct TaskNode* next;         // 下一个任务节点
} TaskNode_t;


/* 任务队列 */
typedef struct {
    TaskNode_t* head;              // 队列头
    TaskNode_t* tail;              // 队列尾
    uint16_t    count;             // 任务计数
} TaskQueue_t;



/* 状态机类型定义 */
typedef void (*state_func_t)(void);         // 状态函数指针类型
typedef uint8_t (*transition_func_t)(void); // 状态转换条件函数指针类型

/* 状态转换结构体 */
typedef struct
{
    uint8_t next_state;          // 下一个状态
    transition_func_t condition; // 转换条件
} StateTransition_t;

/* 状态结构体 */
typedef struct
{
    state_func_t action;            // 状态动作
    StateTransition_t *transitions; // 状态转换表
    uint8_t num_transitions;        // 转换数量
} State_t;

extern TaskQueue_t task_queue;
static uint8_t current_state;


//方案参数结构体
typedef struct {
    uint8_t planIndex;          // 方案索引 0-13
    uint8_t changeFlag;         // 方案参数变化标志
    char    name[13];           // 方案名称 最大12个字母或者6中文
    uint16_t materialsTime[3];   // 物料时间
    uint8_t materialsAir[3];    // 物料1气量
    uint8_t materialsHeat[3];   // 物料1加热
    uint8_t espressoFirst;      // 浓缩先出
    uint16_t waterTime;          // 水时间
    uint8_t waterHeat;          // 水加热 
}Plan_Parameter_TypeDef;

typedef struct{
    uint8_t pumpPreStartTime;       //水泵预抽取时间，单位为.1秒 放大10倍
    uint8_t pipelineFillingTime;    //管道填充时间，单位为.1秒  放大10倍
    uint8_t pipelineDrainTime;      //管道排空时间，单位为.1秒  放大10倍
    uint8_t pumpSpeed;              //水泵速度，0-100
    uint8_t temp;                   //物料温度，单位为摄氏度
}material_Parameter_TypeDef;

typedef struct {
    uint8_t workMode;                                   // 工作模式 0,空闲模式；1，开机清洗；2，三分钟待机清洗；3，长待机清洗；4，药片清洗；5，出品；6，排空
    uint8_t workStep;                                   //工作步骤
    uint8_t makeIndex;                                  // 当前制作饮品的索引 0-13
    Plan_Parameter_TypeDef planParameter;               // 当前出品方案参数
    uint8_t standbyCleanTimeCache;                      // 待机清洗时间缓存 
    material_Parameter_TypeDef materialParameterCache;  // 物料参数缓存
    char materialNameCache[3][13];                      // 物料名称缓存
    uint8_t waterTempCache;                             // 热水温度缓存
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
