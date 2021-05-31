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
#include "User_Delay.h"
#include "User_GSM.h"
#include "User_RCServo.h"
#include "User_XBEE.h"
#include "User_GPS_INS.h"
#include "User_CAN.h"

/* Private typedef -----------------------------------------------------------*/
RCC_ClocksTypeDef         RCC_ClockFreq;
ErrorStatus               HSEStartUpStatus;

void Clock_HSE_Configuration(uint32_t clock_in);
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t TimingDelay;
uint32_t clock_source = 0;

uint8_t test_CAN[8] = {0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08};
UGSM_FlagStatus _check;
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
  /* Add your application code here
     */
  Clock_HSE_Configuration(clock_72Mhz);
  RCC_GetClocksFreq(&RCC_ClockFreq);      
  clock_source = RCC_ClockFreq.SYSCLK_Frequency; 
  
  // Update SystemCoreClock value
  SystemCoreClockUpdate();
  // Configure the SysTick timer to overflow every 1 us
  //SysTick_Config(SystemCoreClock / 1000000);
  UDELAY_Configure();
	
	UGSM_Configure();
	UDELAY_ms(5000);
	UGSM_InitGSM(9600);
	while(UGSM_Ping() != _OK);
	
	UXBEE_Configure();
	UDELAY_ms(1000);
	
//	URC_Configure();
	
//	UNAVI_Configure();

	UCAN_Configure();
	UDELAY_ms(1000);
//	UDELAY_ms(20000);
		
	//_check = UGSM_Ping();
//	GPIO_InitTypeDef GPIO_InitStruct;
//	
//	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);
//	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
//	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13;
//	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
//	GPIO_Init(GPIOC,&GPIO_InitStruct);
//	UGSM_MakeCall("0383191679");

//	UDELAY_ms(2000);
//	UGSM_MakeSMS("0707651686", "Hello Em yeu ngu ngon");
//	UGSM_MakeCall("0383191679");
//	(void)UGSM_Ping();
//	UDELAY_ms(2000);
//	(void)UGSM_Ping();
//	UDELAY_ms(2000);
	/* Infinite loop */
  while (1)
  {
//		GPIO_SetBits(GPIOC, GPIO_Pin_13);
//		UDELAY_ms(1000);
//		GPIO_ResetBits(GPIOC, GPIO_Pin_13);
//		UGSM_MakeSMS("Chau Thanh Hai");
//		UXBEE_Transmit("Chau Thanh Hai\r\n");
//		URC_SetAngle(90.0f);
//		URC_SwitchON();
//		UDELAY_ms(5000);
//		URC_SwitchOFF();
//		UDELAY_ms(5000);
//		UNAVI_Transmit("Chau Thanh Hai-29/03/1996\r\n");
//		test_CAN[1] += 1;
//		UCAN_Transmit(0x123, test_CAN);
//		UDELAY_ms(1000);
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

//Well the functions are RCC_HCLKConfig() for AHB, RCC_PCLK1Config() for APB1 and RCC_PCLK2Config() for APB2. 
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
