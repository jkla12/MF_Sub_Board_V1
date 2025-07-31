#ifndef __DEBUG_H
#define __DEBUG_H

#include "gd32f30x.h"


#define SN_MAX_LEN    		 16    // ���� 15 �ֽ��ַ��� + ������ 
#define VER_LEN              4     // ��4�ֽڴ����֣����� 10��20��uint32��
#define DATE_LEN             10    // ��ʽ yyyy/mm/dd
/* ���� Э�鳣�� ���� */
#define FRAME_HEADER_WRITE   0xFA    // ��λ��������д/������֡ͷ
#define FRAME_END_WRITE      0xAF    // ��λ�������֡β
#define FRAME_HEADER_READ    0xFB    // MCU ������λ������Ӧ֡ͷ
#define FRAME_END_READ       0xBF    // MCU ��Ӧ��֡β

#define DATA_TYPE_SN         0x01    // �������ͣ�SN ��
#define DATA_TYPE_HW     	 0x02    // Ӳ���汾��
#define DATA_TYPE_SW     	 0x03    // ����汾��
#define DATA_TYPE_DATE       0x04    // ��������

#define OPERATION_READ       0x00    // ��������
#define OPERATION_WRITE      0x01    // ������д

void debugProcess(void);


uint8_t calc_checksum(const uint8_t *buf, uint16_t len) ;

#endif
