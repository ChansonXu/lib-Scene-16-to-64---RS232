/***************************Copyright BestFu 2014-05-14*************************
文	件：	main.c
说	明：	系统主函数
编	译：	Keil uVision4 V4.54.0.0
版	本：	v1.0
编	写：	Unarty
日	期：	2014.06.22 
修　改：	暂无
*******************************************************************************/
#include "SysHard.h"
#include "Thread.h"
#include "wdg.h"
#include "cmdProcess.h"
#include "Instruct.h"
#include "DeviceReset.h"
#include "Test.h"

#include "USARTx.h"

/*******************************************************************************
函 数 名：	main
功能说明： 	主函数
参	  数： 	无
返 回 值：	无
*******************************************************************************/
int main(void)
{
	Thread_Init();
	SignEncrypt_Init(); //通讯加密标记初始化
 	SysHard_Init();		//系统相关硬件初始化
	WDG_Init(5,2000);
	FIFOInit();			//初始化队列缓冲区
	Instruct_Init();
	DeviceResetCheck_Init();
	Test_Init();
	Unit_Init();
	
	//USARTx_Send("1234567890", 10); 
//	t = DecryptionFlash();
//	if(0==t)
	while(1)
	{
		Process_FIFOData(&rcv433fifo, &rcv_433); 		//处理接收FIFO	
		Process_FIFOData(&rcv433fifo, &rcv_433); 		//处理接收FIFO		
 		Thread_Process();
	}
}

/*
SceneEEPROM_Init();
void SceneEEPROM_Clr(u8 unit, u8 area, u8 num);
void SceneEEPROMUnit_Init(u8 unit);




*/








////__asm void wait()
////{
////      BX lr
////}
///** 
//  * @brief  This function handles Hard Fault exception. 
//  * @param  None 
//  * @retval None 
//  */  
//void HardFault_Handler(void)  
//{ 
////		 wait();
//  /* Go to infinite loop when Hard Fault exception occurs */  
//  if (CoreDebug->DHCSR & 1) {  //check C_DEBUGEN == 1 -> Debugger Connected  
//      __breakpoint(0);  // halt program execution here         
//  }  
//  while (1)  
//  {  
//  }  
//} 

/**************************Copyright BestFu 2014-05-14*************************/
