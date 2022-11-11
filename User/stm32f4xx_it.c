/**
  ******************************************************************************
  * @file    FMC_SDRAM/stm32f4xx_it.c 
  * @author  MCD Application Team
  * @version V1.0.1
  * @date    11-November-2013
  * @brief   Main Interrupt Service Routines.
  *         This file provides template for all exceptions handler and
  *         peripherals interrupt service routine.
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2013 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_it.h"
#include "./systick/bsp_SysTick.h"
#include "./led/bsp_led.h"
#include "./key/bsp_exti.h"
#include "./usart/bsp_debug_usart.h"
#include "./i2c_soft/bsp_i2c_soft.h"
#include "./tim/bsp_basic_tim.h"
#include "./sys.h"

/** @addtogroup STM32F429I_DISCOVERY_Examples
  * @{
  */

/** @addtogroup FMC_SDRAM
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/******************************************************************************/
/*            Cortex-M4 Processor Exceptions Handlers                         */
/******************************************************************************/

/**
  * @brief  This function handles NMI exception.
  * @param  None
  * @retval None
  */
void NMI_Handler(void)
{
}

/**
  * @brief  This function handles Hard Fault exception.
  * @param  None
  * @retval None
  */
void HardFault_Handler(void)
{
  /* Go to infinite loop when Hard Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Memory Manage exception.
  * @param  None
  * @retval None
  */
void MemManage_Handler(void)
{
  /* Go to infinite loop when Memory Manage exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Bus Fault exception.
  * @param  None
  * @retval None
  */
void BusFault_Handler(void)
{
  /* Go to infinite loop when Bus Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Usage Fault exception.
  * @param  None
  * @retval None
  */
void UsageFault_Handler(void)
{
  /* Go to infinite loop when Usage Fault exception occurs */
  while (1)
  {}
}

/**
  * @brief  This function handles Debug Monitor exception.
  * @param  None
  * @retval None
  */
void DebugMon_Handler(void)
{}

/**
  * @brief  This function handles SVCall exception.
  * @param  None
  * @retval None
  */
void SVC_Handler(void)
{}

/**
  * @brief  This function handles PendSV_Handler exception.
  * @param  None
  * @retval None
  */
void PendSV_Handler(void)
{}

/**
  * @brief  This function handles SysTick Handler.
  * @param  None
  * @retval None
  */
void SysTick_Handler(void)
{
	
	TimingDelay_Decrement();//获取节拍程序
	
}
/******************************************************************************/
/*                 STM32F4xx Peripherals Interrupt Handlers                   */
/*  Add here the Interrupt Handler for the used peripheral(s) (PPP), for the  */
/*  available peripheral interrupt handler's name please refer to the startup */
/*  file (startup_stm32f429_439xx.s).                         */
/******************************************************************************/

/**
  * @}
  */
u8 KEY_flag,KEY1_flag;
void  BASIC_TIM_IRQHandler (void)
{
/********************KEY1************************/	
	u8 val=GPIO_ReadInputDataBit(GPIOA,GPIO_Pin_0);
	u8 val1=GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_13);
	static u16 count=0,count1=0;
	if(val==Bit_SET)//按键按下 记录按下的时间
	{
		count++;
	}
	else //按键松开
	{
		if(count>100)//按下经过1s  10*100ms=1s
		{
    		//长按
			KEY_flag = 2;
			printf("key_flag == 2\r\n");
		}
		else if(count>10 && count <100)//表示超过10ms
		{
			//短按
			KEY_flag = 1;
			printf("key_flag == 1\r\n");
		}
		else
		{
			
			//消抖时间
		}
		count=0;
	}
/********************KEY2********************/	
	if(val1==Bit_SET)//按键按下 记录按下的时间
	{
		count1++;
	}
	else //按键松开
	{
		if(count1>100)//按下经过1s  10*100ms=1s
		{
    		//长按
			KEY1_flag = 2;
//			printf("key1_flag == 2\r\n");
		}
		else if(count1>10 && count1 <100)//表示超过10ms
		{
			//短按
			KEY1_flag = 1;
//			printf("key1_flag == 1\r\n");
		}
		else
		{
			
			//消抖时间
		}
		count1=0;
	}
	
	TIM_ClearITPendingBit(BASIC_TIM , TIM_IT_Update);  
}


//extern u16 value_field[]={0};
//extern u16 Read_DATA[];

//void KEY1_IRQHandler(void)
//{
//  //确保是否产生了EXTI Line中断
//	if(EXTI_GetITStatus(KEY1_INT_EXTI_LINE) != RESET) 
//	{
//		u16 num;
//		u16 *p=value_field;
//		num=sizeof(value_field)/sizeof(value_field[0]);
//		printf("%d\r\n",num);
//		Data_write_mul_N(0x50,0x00,value_field,num);
//		printf("写入值:%d\r\n", *p);
//		Delay_ms(50);		
//		
//    //清除中断标志位
//		EXTI_ClearITPendingBit(KEY1_INT_EXTI_LINE);     
//	}  
//}

//void KEY2_IRQHandler(void)
//{
//	u16 *p1=Read_DATA;
//	Data_read_mul_N(0x50,0x00,512,Read_DATA);	
//    printf("读取值:%d\r\n", *p1);
//	Delay_ms(50);		

//    //确保是否产生了EXTI Line中断
//	if(EXTI_GetITStatus(KEY2_INT_EXTI_LINE) != RESET) 
//	{
//    //清除中断标志位
//		EXTI_ClearITPendingBit(KEY2_INT_EXTI_LINE);     
//	}  
//}
/**
  * @}
  */ 

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
