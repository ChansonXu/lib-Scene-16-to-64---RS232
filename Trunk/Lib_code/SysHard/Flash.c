/***************************Copyright BestFu 2014-05-14*************************
��	����	Flash.c
˵	����	STM32F103_Flash��������
��	�룺	Keil uVision4 V4.54.0.0
��	����	v1.0
��	д��	Unarty
��	�ڣ�	2013-11-21
��  ��:     ����
*******************************************************************************/
#include "stm32f10x.h"
#include "Flash.h"
#include "BF_type.h"

/* FLASH Keys */
#define RDP_Key                  ((u16)0x00A5)
#define FLASH_KEY1               ((u32)0x45670123)
#define FLASH_KEY2               ((u32)0xCDEF89AB)

#define EraseTime 		((u32)0x2000)
#define ProgramTime		((u32)0x3000)

static u8 Flash_EOP(void);
static u8 Flash_Status(u32 timeOut);

/*******************************************************************************
�� �� ����	Flash_Unlock
����˵���� 	STM32_Flash����
��	  ���� 	��
�� �� ֵ��	��
*******************************************************************************/
void Flash_Unlock(void)
{
    FLASH->KEYR = FLASH_KEY1;
    FLASH->KEYR = FLASH_KEY2;
}

/*******************************************************************************
�� �� ����	Flash_Unlock
����˵���� 	STM32_Flash����
��	  ���� 	��
�� �� ֵ��	��
*******************************************************************************/
void Flash_lock(void)
{
    FLASH->CR |= CR_LOCK;
}

/*******************************************************************************
�� �� ����	FLASH_ErasePage
����˵���� 	Flashҳ����
��	  ���� 	page_add:  ҵ��ַ
�� �� ֵ��  TRUE/FALSE
*******************************************************************************/
u8 FLASH_ErasePage(u32 page_add)
{
    u8 flag = Flash_Status(EraseTime);

    if (!flag)//û��flash����
    {
        FLASH->CR |= CR_PER; //ʹ��ҳ����
        FLASH->AR = page_add;//Ҫ����ҳ��ַ
        FLASH->CR |= CR_STRT;//����һ��ҳ���� 
        
        flag = Flash_Status(EraseTime);
        FLASH->CR &= ~CR_PER;   //���ҳ����
    }
    if (!flag)
    {
        flag  = Flash_EOP();
    }
    
    return flag;
}

/*******************************************************************************
�� �� ����	Flash_Status
����˵���� 	��ȡFlash״̬�Ĵ���ֵ
��	  ����  timeOut:    ��ⳬʱʱ��
�� �� ֵ��	Flash_status
*******************************************************************************/
static u8 Flash_Status(u32 timeOut)
{
    for (;(timeOut > 0)&&(FLASH->SR&BUSY); timeOut--)//�ȴ�Flash����
    {}
    if (!timeOut)//�ȴ���ʱ
    {
        return BUSY;
    }
    if (FLASH->SR&(PGERR|WRITEERR))
	{
		FLASH->SR = PGERR|WRITEERR;
		return 1;
	}

    return 0;
}

/*******************************************************************************
�� �� ����	Flash_ProgramHalfWord
����˵���� 	д�����ֽ�
��	  ����  add:    д���ַ
            data:   д����������
�� �� ֵ��	TRUE/FLASE
*******************************************************************************/
u8 Flash_ProgramHalfWord(u32 add, u16 data)
{
    u8 flag = Flash_Status(ProgramTime);

    if (!flag)
    {
        FLASH->CR |= CR_PG;
        *(vu16*)add = (u16)data;
        flag = Flash_Status(ProgramTime);
    
        FLASH->CR &= ~CR_PG;
    }
    if (!flag)
    {
        if (Flash_EOP())
        {
            flag = TRUE; 
        }
    }

    return flag;
}

/*******************************************************************************
�� �� ����	Flash_Write
����˵���� 	flashд������
��	  ����  add:    д���׵�ַ
            len:    д�����ݴ�С
            data:   д����������
�� �� ֵ��	TRUE/FLASE
*******************************************************************************/
u8 Flash_Write(u32 add, u32 len, u8 *data)
{
    if (!Flash_Status(ProgramTime))
    {
        FLASH->CR |= CR_PG;  //������
        while (len > 0)
        {
            *(vu16*)add = *(u16*)data;
            data += 2;
            add  += 2;
            len  -= 2;
            if (!Flash_Status(ProgramTime))//δ��ʱ
            {
                if (!Flash_EOP())
                {
                    break;
                }
            }
            else
            {
                break;
            }
        }
    
        FLASH->CR &= ~CR_PG;
    } 

    return (len == 0 ? TRUE : FALSE);
}

/*******************************************************************************
�� �� ����	Flash_EOP
����˵���� 	��ȡFlash���������־λ
��	  ����  ��
�� �� ֵ��	FALSE/TRUE (Flashд�������������)
*******************************************************************************/
static u8 Flash_EOP(void)
{
    if (FLASH->SR&EOP)//�����ɹ�
    {
        FLASH->SR = EOP; //�����־
        return TRUE;
    }

    return FALSE;
}

/*******************************************************************************
�� �� ����	Flash_eraseOptionByte
����˵���� 	��������Flashѡ���ֽ�
��	  ����  ��
�� �� ֵ��	TRUE/FALSE (�������)
*******************************************************************************/
u8 Flash_EraseOptionByte(void)
{
    u8 flag = FALSE;
    
    if (!Flash_Status(ProgramTime))
    {
        Flash_Unlock();
        FLASH->CR |= CR_OPTER;
        FLASH->CR |= CR_STRT;
        if (!Flash_Status(ProgramTime))
        {
            if (Flash_EOP())
            {
                flag = TRUE;
            }
        }
          FLASH->CR &= ~CR_OPTER;
        
    }
    return flag;
}


/*******************************************************************************
�� �� ����	Flash_WriteOptionByte
����˵���� 	д��Flashѡ���ֽ�
��	  ����  addr:   ѡ���ֽڵ�ַ
            data:   ����
�� �� ֵ��	TRUE/FALSE (�������)
*******************************************************************************/
u8 Flash_WriteOptionByte(u32 addr, u8 data)
{
    u8 flag = FALSE;
    
    if (!Flash_Status(ProgramTime))
    {
        Flash_Unlock();
        FLASH->CR |= CR_OPTPG;
        FLASH->CR |= CR_STRT;
        *(vu8*)addr = data;
        if (!Flash_Status(ProgramTime))
        {
            if (Flash_EOP())
            {
                flag = TRUE;
            }
        }
        FLASH->CR &= ~CR_OPTER;    
    }
    return flag;
}

/*******************************************************************************
//�� �� ����	Flash_WriteProtect
//����˵���� 	д��Flashѡ���ֽ�
//��	  ����  addr:   �����������ַ
//            sta:    0(NO protect)/(~0)(protect)
//�� �� ֵ��	TRUE/FALSE (�������)
//ע    ��:   size <= 32Kb
*******************************************************************************/
//u8 Flash_WriteProtect(u32 addr, u8 sta)
//{
//    u8 seat, flag;
//    
//    addr -= 0x08000000;
//    seat = (u8)(addr>>12)&(0x1f);
//        
//
////    if (sta)
////    {
////        flag = Flash_WriteOptionByte((u32)(&OB->WRP0 + (seat>>3))
////                              ,&OB->WRP0 + ((seat>>3)|(1<<(seat&0x07))));
////    }
////    else
////    {
////       flag = Flash_WriteOptionByte((u32)(&OB->WRP0 + (seat>>3))
////                              ,&OB->WRP0 + ((seat>>3)&(~(1<<(seat&0x07)))));
////    }
//
//    return flag;
//}

#if (DEVICE_ADDR_RESET_FEATURE_EN > 0u)
/*******************************************************************************
�� �� ����	void Get_UniqueDeviceID(u8* chipidtable)
����˵���� 	��ȡstm32 flashȫ��ΨһID
��	  ����  chipidtable:�洢96λȫ��ΨһID���׵�ַ
�� �� ֵ��	��
*******************************************************************************/
void Get_UniqueDeviceID(u8* chipidtable)
{
	*(u32*)(&chipidtable[0]) = *((vu32*)(CHIPID_ADDR + 0x00));
	*(u32*)(&chipidtable[4]) = *((vu32*)(CHIPID_ADDR + 0x04));
	*(u32*)(&chipidtable[8]) = *((vu32*)(CHIPID_ADDR + 0x08));
}
#endif
/**************************Copyright BestFu 2014-05-14*************************/


