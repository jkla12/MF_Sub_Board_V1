#include "modbus.h"
#include "public.h"


/* Private variables ---------------------------------------------------------*/
//����Ĵ�������
uint16_t usRegInputBuf[REG_INPUT_NREGS] = {0x0000};
//����Ĵ�����ʼ��ַ
uint16_t usRegInputStart = REG_INPUT_START;



//���ּĴ�������
uint16_t usRegHoldingBuf[REG_HOLDING_NREGS] = {0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000,0x0000};
//���ּĴ�����ʼ��ַ
uint16_t usRegHoldingStart = REG_HOLDING_START;


//��Ȧ״̬
uint8_t ucRegCoilsBuf[REG_COILS_SIZE / 8] = {0x00,0x00};
//��������״̬
uint8_t ucRegDiscreteBuf[REG_DISCRETE_SIZE / 8] = {0x00,0x00};


/****************************************************************************
* ��	  �ƣ�eMBRegInputCB 
* ��    �ܣ���ȡ����Ĵ�������Ӧ�������� 04 eMBFuncReadInputRegister
* ��ڲ�����pucRegBuffer: ���ݻ�������������Ӧ����   
*						usAddress: �Ĵ�����ַ
*						usNRegs: Ҫ��ȡ�ļĴ�������
* ���ڲ�����
* ע	  �⣺��λ�������� ֡��ʽ��: SlaveAddr(1 Byte)+FuncCode(1 Byte)
*								+StartAddrHiByte(1 Byte)+StartAddrLoByte(1 Byte)
*								+LenAddrHiByte(1 Byte)+LenAddrLoByte(1 Byte)+
*								+CRCAddrHiByte(1 Byte)+CRCAddrLoByte(1 Byte)
*							3 ��
****************************************************************************/
eMBErrorCode
eMBRegInputCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs )
{
    eMBErrorCode    eStatus = MB_ENOERR;
    int             iRegIndex;

    if( ( usAddress >= REG_INPUT_START )
        && ( usAddress + usNRegs <= REG_INPUT_START + REG_INPUT_NREGS ) )
    {
        iRegIndex = ( int )( usAddress - usRegInputStart );
        while( usNRegs > 0 )
        {
            *pucRegBuffer++ = ( UCHAR )( usRegInputBuf[iRegIndex] >> 8 );
            *pucRegBuffer++ = ( UCHAR )( usRegInputBuf[iRegIndex] & 0xFF );
            iRegIndex++;
            usNRegs--;
        }
    }
    else
    {
        eStatus = MB_ENOREG;
    }

    return eStatus;
}

/****************************************************************************
* ��	  �ƣ�eMBRegHoldingCB 
* ��    �ܣ���Ӧ�������У�06 д���ּĴ��� eMBFuncWriteHoldingRegister 
*													16 д������ּĴ��� eMBFuncWriteMultipleHoldingRegister
*													03 �����ּĴ��� eMBFuncReadHoldingRegister
*													23 ��д������ּĴ��� eMBFuncReadWriteMultipleHoldingRegister
* ��ڲ�����pucRegBuffer: ���ݻ�������������Ӧ����   
*						usAddress: �Ĵ�����ַ
*						usNRegs: Ҫ��д�ļĴ�������
*						eMode: ������
* ���ڲ�����
* ע	  �⣺4 ��
****************************************************************************/
eMBErrorCode
eMBRegHoldingCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNRegs, eMBRegisterMode eMode )
{
	eMBErrorCode    eStatus = MB_ENOERR;
	int             iRegIndex;


	if((usAddress >= REG_HOLDING_START)&&\
		((usAddress+usNRegs) <= (REG_HOLDING_START + REG_HOLDING_NREGS)))
	{
		iRegIndex = (int)(usAddress - usRegHoldingStart);
		switch(eMode)
		{                                       
			case MB_REG_READ://�� MB_REG_READ = 0
        while(usNRegs > 0)
				{
					*pucRegBuffer++ = (uint8_t)(usRegHoldingBuf[iRegIndex] >> 8);            
					*pucRegBuffer++ = (uint8_t)(usRegHoldingBuf[iRegIndex] & 0xFF); 
          iRegIndex++;
          usNRegs--;					
				}                            
        break;
			case MB_REG_WRITE://д MB_REG_WRITE = 0
				while(usNRegs > 0)
				{         
					usRegHoldingBuf[iRegIndex] = *pucRegBuffer++ << 8;
          usRegHoldingBuf[iRegIndex] |= *pucRegBuffer++;
          iRegIndex++;
          usNRegs--;
        }				
			}
	}
	else//����
	{
		eStatus = MB_ENOREG;
	}	
	
	return eStatus;
}


extern void xMBUtilSetBits( UCHAR * ucByteBuf, USHORT usBitOffset, UCHAR ucNBits,
                UCHAR ucValue );
extern UCHAR xMBUtilGetBits( UCHAR * ucByteBuf, USHORT usBitOffset, UCHAR ucNBits );
/****************************************************************************
* ��	  �ƣ�eMBRegCoilsCB 
* ��    �ܣ���Ӧ�������У�01 ����Ȧ eMBFuncReadCoils
*													05 д��Ȧ eMBFuncWriteCoil
*													15 д�����Ȧ eMBFuncWriteMultipleCoils
* ��ڲ�����pucRegBuffer: ���ݻ�������������Ӧ����   
*						usAddress: ��Ȧ��ַ
*						usNCoils: Ҫ��д����Ȧ����
*						eMode: ������
* ���ڲ�����
* ע	  �⣺��̵��� 
*						0 ��
****************************************************************************/
eMBErrorCode
eMBRegCoilsCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNCoils,
              eMBRegisterMode eMode )
{
  //����״̬
  eMBErrorCode eStatus = MB_ENOERR;
  //�Ĵ�������
  int16_t iNCoils = ( int16_t )usNCoils;
  //�Ĵ���ƫ����
  int16_t usBitOffset;
  
  //���Ĵ����Ƿ���ָ����Χ��
  if( ( (int16_t)usAddress >= REG_COILS_START ) &&
     ( usAddress + usNCoils <= REG_COILS_START + REG_COILS_SIZE ) )
  {
    //����Ĵ���ƫ����
    usBitOffset = ( int16_t )( usAddress - REG_COILS_START );
    switch ( eMode )
    {
      //������
    case MB_REG_READ:
      while( iNCoils > 0 )
      {
        *pucRegBuffer++ = xMBUtilGetBits( ucRegCoilsBuf, usBitOffset,
                                         ( uint8_t )( iNCoils > 8 ? 8 : iNCoils ) );
        iNCoils -= 8;
        usBitOffset += 8;
      }
      break;
      
      //д����
    case MB_REG_WRITE:
      while( iNCoils > 0 )
      {
        xMBUtilSetBits( ucRegCoilsBuf, usBitOffset,
                       ( uint8_t )( iNCoils > 8 ? 8 : iNCoils ),
                       *pucRegBuffer++ );
        iNCoils -= 8;
      }
      break;
    }
    
  }
  else
  {
    eStatus = MB_ENOREG;
  }
  return eStatus;
}

/****************************************************************************
* ��	  �ƣ�eMBRegDiscreteCB 
* ��    �ܣ���ȡ��ɢ�Ĵ�������Ӧ�������У�02 ����ɢ�Ĵ��� eMBFuncReadDiscreteInputs
* ��ڲ�����pucRegBuffer: ���ݻ�������������Ӧ����   
*						usAddress: �Ĵ�����ַ
*						usNDiscrete: Ҫ��ȡ�ļĴ�������
* ���ڲ�����
* ע	  �⣺1 ��
****************************************************************************/
eMBErrorCode
eMBRegDiscreteCB( UCHAR * pucRegBuffer, USHORT usAddress, USHORT usNDiscrete )
{
  //����״̬
  eMBErrorCode eStatus = MB_ENOERR;
  //�����Ĵ�������
  int16_t iNDiscrete = ( int16_t )usNDiscrete;
  //ƫ����
  uint16_t usBitOffset;
  
  //�жϼĴ���ʱ�����ƶ���Χ��
  if( ( (int16_t)usAddress >= REG_DISCRETE_START ) &&
     ( usAddress + usNDiscrete <= REG_DISCRETE_START + REG_DISCRETE_SIZE ) )
  {
    //���ƫ����
    usBitOffset = ( uint16_t )( usAddress - REG_DISCRETE_START );
    
    while( iNDiscrete > 0 )
    {
      *pucRegBuffer++ = xMBUtilGetBits( ucRegDiscreteBuf, usBitOffset,
                                       ( uint8_t)( iNDiscrete > 8 ? 8 : iNDiscrete ) );
      iNDiscrete -= 8;
      usBitOffset += 8;
    }
    
  }
  else
  {
    eStatus = MB_ENOREG;
  }
  return eStatus;
}

