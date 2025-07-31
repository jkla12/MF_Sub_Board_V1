#ifndef __CONFIG_H
#define __CONFIG_H

#define VERSION 1

#define WATERTAP_NUMBER_OF_PARAMETERS 14

#define CONFIG_ADDRESS 0x0000   //配置数据保存地址 0开始
#define RECORD_ADDRESS 0x400   //记录数据保存地址 1024开始
#define DEVICE_INFO_ADDRESS 0x800   //设备信息保存地址 2048开始

//默认参数
#define IS_LOCK                         0       //是否锁定

//deviceInfo相关默认参数


#define HARDWARE_VERSION                20     //硬件版本
#define SOFTWARE_VERSION                20     //软件版本


//MQTT相关
#define REGISTER_ADDR                   "http://www.sparkinger.com:30020/api/Equipment/AddDevice"   //设备注册地址
#define UPDATE_ADDR                     "http://www.sparkinger.com:30020/api/Equipment/UEDV"         //设备更新版本号地址
#define MQTT_PUBTOPIC                    "$oc/devices/%s/sys/properties/report"        //设备上报属性地址
#define MQTT_SUBTOPIC                    "$oc/devices/%s/sys/messages/down"        //设备接收命令地址
#define MQTT_ADDR                       "c867b1d505.st1.iotda-device.ap-southeast-1.myhuaweicloud.com"   //MQTT服务器地址
#define MQTT_PORT                       1883    //MQTT端口
#define MQTT_SUBSCRIBE_TOPIC            "/%s/thing/service/property/set"   //MQTT订阅主题

#define ETID_                            "MF05"  //设备类别ID
#define DV_                              "1.0"   //设备版本号
#define ESN_                             "MF05_%s"   //设备序列号


//加热块温度控制PID参数
#define PID_Kp                        0.5    // PID比例系数
#define PID_Ki                        0.1    // PID积分系数
#define PID_Kd                        0.01   // PID微分系数

#define PASSWORD_ "123456" //默认密码

#define MATERIALS_NAME_1  "牛奶"   //物料1名称
#define MATERIALS_NAME_2  "浓缩1" //物料2名称
#define MATERIALS_NAME_3  "浓缩2" //物料3名称

#define MATERIALS_TIME_  100      //默认物料时间 10秒
#define MATERIALS_AIR_   15       //默认物料气量
#define MATERIALS_HEAT_  0       //默认物料是否加热
#define ESPRESSO_FIRST_  0        //默认是否先打浓缩咖啡
#define WATER_TIME_     150      //默认水量时间
#define WATER_HEAT_     0       //默认水量是否加热


#define MATERIAL1_TIME_MAX  600.0f //物料1时间最大值 60秒
#define MATERIAL1_TIME_MIN  0   //物料1时间最小值
#define MATERIAL2_TIME_MAX  100 //物料2时间最大值 10秒
#define MATERIAL2_TIME_MIN  0   //物料2时间最小值
#define MATERIAL3_TIME_MAX  100 //物料3时间最大值 10秒
#define MATERIAL3_TIME_MIN  0   //物料3时间最小值

#define MATERIAL1_AIR_MAX  100 //物料1气量最大值
#define MATERIAL1_AIR_MIN  0   //物料1气量最小值
#define MATERIAL2_AIR_MAX  80 //物料2气量最大值
#define MATERIAL2_AIR_MIN  0   //物料2气量最小值
#define MATERIAL3_AIR_MAX  80 //物料3气量最大值
#define MATERIAL3_AIR_MIN  0   //物料3气量最小值


#define STANDBY_CLEANING_TIME_ 6 //默认待机清洗时间 单位分钟
#define STANDBY_CLEANING_TIME_MAX 99 //待机清洗时间最大值
#define STANDBY_CLEANING_TIME_MIN 0  //待机清洗时间 //待机时间为0禁用待机清洗

#define PUMP_PRE_START_TIME_ 15 //默认预抽液时间 1.5秒
#define PUMP_PRE_START_TIME_MAX 100 //预抽液时间最大值
#define PUMP_PRE_START_TIME_MIN 0 //预抽液时间最小值

#define PIPELINE_FILLING_TIME_ 50 //默认预排空时间 5秒
#define PIPELINE_FILLING_TIME_MAX 100 //预排空时间最大值
#define PIPELINE_FILLING_TIME_MIN 0 //预排空时间最小值

#define PIPELINE_DRAIN_TIME_ 50 //默认结束排空时间 5秒
#define PIPELINE_DRAIN_TIME_MAX 100 //结束排空时间最大值
#define PIPELINE_DRAIN_TIME_MIN 0 //结束排空时间最小值

#define LIQUID_FLOW_RATE_ 50 //默认液体流速 50%
#define LIQUID_FLOW_RATE_MAX 100 //液体流速最大值
#define LIQUID_FLOW_RATE_MIN 0 //液体流速最小值

#define HeatingBlickTemp_ 50 //默认加热块温度 50℃
#define HeatingBlockTempMax          70  // 加热块温度最大值
#define HeatingBlockTempMin          1  // 加热块温度最小值

#define WATER_TEMP_ 50 //默认水温 50℃
#define WATER_TEMP_MAX 100 //水温最大值
#define WATER_TEMP_MIN 1   //水温最小值

#define AIR_ 80 //默认空气比例 80%
#define AIR_MAX 100 //空气比例最大值
#define AIR_MIN 0   //空气比例最小值



#endif

/*
匚贰甘捌:
项目参数
奶：60秒 单位0.1秒
奶泡比例：80%
浓缩液：10秒 单位0.1秒
油脂量：80%
水：60秒 单位1秒

高级参数
预抽液：最大10秒 ，单位0.1秒
预排空：最大10秒， 单位0.1秒
结束排空：最大10秒，单位0.1秒
液体流速：最大100%
奶的温度：最高70℃，最低1℃，单位1
水的温度：最高100℃，最低1℃


出厂设置
项目名称：1-14
默认密码：123456
原料名称：奶 浓缩1 浓缩2 水
长时间清洗时间：6分钟

预抽液：1.5秒
预排空：5秒
结束排空：5秒
液体流速：50%
项目参数
奶：1.5秒
浓缩1：3秒
浓缩2：3秒
水：15秒
奶泡比例：15%
油脂量：15%
温度：50℃

*/



