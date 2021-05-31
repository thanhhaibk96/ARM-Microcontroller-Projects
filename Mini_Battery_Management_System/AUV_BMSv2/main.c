/**
  ******************************************************************************
  * @file    Project/STM32F10x_StdPeriph_Template/main.c 
  * @author  MCD Application Team
  * @version V3.5.0
  * @date    08-April-2011
  * @brief   Main program body
  ******************************************************************************
  * @attention 
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2011 STMicroelectronics</center></h2>
  ******************************************************************************
  */  

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include <stdio.h>
#include "User_BMS.h"
#include "User_Delay.h"
#include "User_RTC.h"
#include "User_COM.h"
#include "User_FlashData.h"

/* Private typedef -----------------------------------------------------------*/
RCC_ClocksTypeDef         RCC_ClockFreq;
ErrorStatus               HSEStartUpStatus;

#define URTC_HOUR					13
#define URTC_MINUTE				10
#define URTC_SECOND				00

int a=0;
int dem2=0;
static URTC_Time_Typedef					URTC_Time_Now, URTC_Time_Init;
static UBMS_Status_Typedef UBMS_Status_Now;
void Clock_HSE_Configuration(uint32_t clock_in);
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t TimingDelay;

uint32_t clock_source = 0;
/* Private function prototypes -----------------------------------------------*/
void Delay(__IO uint32_t nTime);
/* Private functions ---------------------------------------------------------*/
#define clock_16Mhz  0
#define clock_24Mhz  0x00040000
#define clock_32Mhz  0x00080000
#define clock_40Mhz  0x000C0000
#define clock_48Mhz  0x00100000
#define clock_56Mhz  0x00140000
#define clock_64Mhz  0x00180000
#define clock_72Mhz  0x001C0000


int main(void)
{
  /* 
		Add your application code here
  */
  Clock_HSE_Configuration(clock_72Mhz);
  RCC_GetClocksFreq(&RCC_ClockFreq);      
  clock_source = RCC_ClockFreq.SYSCLK_Frequency; 
  
  SystemCoreClockUpdate();
	UDELAY_Configure();
	UBMS_Configure();	
	UDELAY_ms(100);
	
//	URTC_Time_Init.hours   = (uint8_t)URTC_HOUR;
//	URTC_Time_Init.minutes = (uint8_t)URTC_MINUTE;
//	URTC_Time_Init.seconds = (uint8_t)URTC_SECOND;
	
	//URTC_Configure(&URTC_Time_Init);
	UCOM_Configure();
	UDELAY_ms(100);
//	GPIO_SetBits(GPIOB, GPIO_Pin_0);
//	GPIO_SetBits(GPIOA, GPIO_Pin_5);
//	GPIO_SetBits(GPIOA, GPIO_Pin_6);
//	GPIO_SetBits(GPIOA, GPIO_Pin_7);
//	while(UBMS_CheckConfiguration() == CONFIG_NO)
//	{
//		UCOM_RequiredConfigBaterry();	
//		UBMS_Display();
//		UDELAY_ms(100);
//		dem2++;
//	}		
//	UBMS_StartBMS();
//	UDELAY_ms(100);
	
  while (1)
  {
//		UCOM_SendStatusBaterry(&URTC_Time_Now,&UBMS_Status_Now);
//		UDELAY_ms(500);
		if(UCOM_Get_Flag())
		{
			UBMS_GetStatus(&UBMS_Status_Now);
			UCOM_SendStatusBaterry(&URTC_Time_Now,&UBMS_Status_Now);
			UDELAY_ms(500);
		}
  }
}

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void Delay(__IO uint32_t nTime)
{ 
  TimingDelay = nTime;
  while(TimingDelay != 0);
}

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (TimingDelay != 0x00)
  { 
    TimingDelay--;
  }
}

void Clock_HSE_Configuration(uint32_t clock_in)
{
  RCC_DeInit();                             
  /* Enable HSE */
  RCC_HSEConfig(RCC_HSE_ON);                  
  /* Wait till HSE is ready */
  HSEStartUpStatus = RCC_WaitForHSEStartUp();
  if (HSEStartUpStatus == SUCCESS)
  {
    /* Enable Prefetch Buffer */
    FLASH_PrefetchBufferCmd(FLASH_PrefetchBuffer_Enable);
    /* Flash 2 wait state */
    FLASH_SetLatency(FLASH_Latency_2);
    /* HCLK = SYSCLK */
    RCC_HCLKConfig(RCC_SYSCLK_Div1); 
    /* PCLK2 = HCLK */
    RCC_PCLK2Config(RCC_HCLK_Div1); 
    /* PCLK1 = HCLK/2 */
    RCC_PCLK1Config(RCC_HCLK_Div2);
    /* PLLCLK = clock_in */
      RCC_PLLConfig(RCC_PLLSource_HSE_Div1, clock_in);  
    /* Enable PLL */ 
    RCC_PLLCmd(ENABLE);
    /* Wait till PLL is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) {}
    /* Select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    /* Wait till PLL is used as system clock source */
    while(RCC_GetSYSCLKSource() != 0x08){ }
  }
  else{ while (1){}}
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/******************* (C) COPYRIGHT 2011 STMicroelectronics *****END OF FILE****/
