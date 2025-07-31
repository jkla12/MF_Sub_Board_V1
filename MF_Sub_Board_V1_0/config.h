#ifndef __CONFIG_H
#define __CONFIG_H

#define VERSION 1

#define WATERTAP_NUMBER_OF_PARAMETERS 14

#define CONFIG_ADDRESS 0x0000   //�������ݱ����ַ 0��ʼ
#define RECORD_ADDRESS 0x400   //��¼���ݱ����ַ 1024��ʼ
#define DEVICE_INFO_ADDRESS 0x800   //�豸��Ϣ�����ַ 2048��ʼ

//Ĭ�ϲ���
#define IS_LOCK                         0       //�Ƿ�����

//deviceInfo���Ĭ�ϲ���


#define HARDWARE_VERSION                20     //Ӳ���汾
#define SOFTWARE_VERSION                20     //����汾


//MQTT���
#define REGISTER_ADDR                   "http://www.sparkinger.com:30020/api/Equipment/AddDevice"   //�豸ע���ַ
#define UPDATE_ADDR                     "http://www.sparkinger.com:30020/api/Equipment/UEDV"         //�豸���°汾�ŵ�ַ
#define MQTT_PUBTOPIC                    "$oc/devices/%s/sys/properties/report"        //�豸�ϱ����Ե�ַ
#define MQTT_SUBTOPIC                    "$oc/devices/%s/sys/messages/down"        //�豸���������ַ
#define MQTT_ADDR                       "c867b1d505.st1.iotda-device.ap-southeast-1.myhuaweicloud.com"   //MQTT��������ַ
#define MQTT_PORT                       1883    //MQTT�˿�
#define MQTT_SUBSCRIBE_TOPIC            "/%s/thing/service/property/set"   //MQTT��������

#define ETID_                            "MF05"  //�豸���ID
#define DV_                              "1.0"   //�豸�汾��
#define ESN_                             "MF05_%s"   //�豸���к�


//���ȿ��¶ȿ���PID����
#define PID_Kp                        0.5    // PID����ϵ��
#define PID_Ki                        0.1    // PID����ϵ��
#define PID_Kd                        0.01   // PID΢��ϵ��

#define PASSWORD_ "123456" //Ĭ������

#define MATERIALS_NAME_1  "ţ��"   //����1����
#define MATERIALS_NAME_2  "Ũ��1" //����2����
#define MATERIALS_NAME_3  "Ũ��2" //����3����

#define MATERIALS_TIME_  100      //Ĭ������ʱ�� 10��
#define MATERIALS_AIR_   15       //Ĭ����������
#define MATERIALS_HEAT_  0       //Ĭ�������Ƿ����
#define ESPRESSO_FIRST_  0        //Ĭ���Ƿ��ȴ�Ũ������
#define WATER_TIME_     150      //Ĭ��ˮ��ʱ��
#define WATER_HEAT_     0       //Ĭ��ˮ���Ƿ����


#define MATERIAL1_TIME_MAX  600.0f //����1ʱ�����ֵ 60��
#define MATERIAL1_TIME_MIN  0   //����1ʱ����Сֵ
#define MATERIAL2_TIME_MAX  100 //����2ʱ�����ֵ 10��
#define MATERIAL2_TIME_MIN  0   //����2ʱ����Сֵ
#define MATERIAL3_TIME_MAX  100 //����3ʱ�����ֵ 10��
#define MATERIAL3_TIME_MIN  0   //����3ʱ����Сֵ

#define MATERIAL1_AIR_MAX  100 //����1�������ֵ
#define MATERIAL1_AIR_MIN  0   //����1������Сֵ
#define MATERIAL2_AIR_MAX  80 //����2�������ֵ
#define MATERIAL2_AIR_MIN  0   //����2������Сֵ
#define MATERIAL3_AIR_MAX  80 //����3�������ֵ
#define MATERIAL3_AIR_MIN  0   //����3������Сֵ


#define STANDBY_CLEANING_TIME_ 6 //Ĭ�ϴ�����ϴʱ�� ��λ����
#define STANDBY_CLEANING_TIME_MAX 99 //������ϴʱ�����ֵ
#define STANDBY_CLEANING_TIME_MIN 0  //������ϴʱ�� //����ʱ��Ϊ0���ô�����ϴ

#define PUMP_PRE_START_TIME_ 15 //Ĭ��Ԥ��Һʱ�� 1.5��
#define PUMP_PRE_START_TIME_MAX 100 //Ԥ��Һʱ�����ֵ
#define PUMP_PRE_START_TIME_MIN 0 //Ԥ��Һʱ����Сֵ

#define PIPELINE_FILLING_TIME_ 50 //Ĭ��Ԥ�ſ�ʱ�� 5��
#define PIPELINE_FILLING_TIME_MAX 100 //Ԥ�ſ�ʱ�����ֵ
#define PIPELINE_FILLING_TIME_MIN 0 //Ԥ�ſ�ʱ����Сֵ

#define PIPELINE_DRAIN_TIME_ 50 //Ĭ�Ͻ����ſ�ʱ�� 5��
#define PIPELINE_DRAIN_TIME_MAX 100 //�����ſ�ʱ�����ֵ
#define PIPELINE_DRAIN_TIME_MIN 0 //�����ſ�ʱ����Сֵ

#define LIQUID_FLOW_RATE_ 50 //Ĭ��Һ������ 50%
#define LIQUID_FLOW_RATE_MAX 100 //Һ���������ֵ
#define LIQUID_FLOW_RATE_MIN 0 //Һ��������Сֵ

#define HeatingBlickTemp_ 50 //Ĭ�ϼ��ȿ��¶� 50��
#define HeatingBlockTempMax          70  // ���ȿ��¶����ֵ
#define HeatingBlockTempMin          1  // ���ȿ��¶���Сֵ

#define WATER_TEMP_ 50 //Ĭ��ˮ�� 50��
#define WATER_TEMP_MAX 100 //ˮ�����ֵ
#define WATER_TEMP_MIN 1   //ˮ����Сֵ

#define AIR_ 80 //Ĭ�Ͽ������� 80%
#define AIR_MAX 100 //�����������ֵ
#define AIR_MIN 0   //����������Сֵ



#endif

/*
�η��ʰ�:
��Ŀ����
�̣�60�� ��λ0.1��
���ݱ�����80%
Ũ��Һ��10�� ��λ0.1��
��֬����80%
ˮ��60�� ��λ1��

�߼�����
Ԥ��Һ�����10�� ����λ0.1��
Ԥ�ſգ����10�룬 ��λ0.1��
�����ſգ����10�룬��λ0.1��
Һ�����٣����100%
�̵��¶ȣ����70�棬���1�棬��λ1
ˮ���¶ȣ����100�棬���1��


��������
��Ŀ���ƣ�1-14
Ĭ�����룺123456
ԭ�����ƣ��� Ũ��1 Ũ��2 ˮ
��ʱ����ϴʱ�䣺6����

Ԥ��Һ��1.5��
Ԥ�ſգ�5��
�����ſգ�5��
Һ�����٣�50%
��Ŀ����
�̣�1.5��
Ũ��1��3��
Ũ��2��3��
ˮ��15��
���ݱ�����15%
��֬����15%
�¶ȣ�50��

*/



