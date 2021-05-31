/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/main.c 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
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
#include "main.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_flash.h"
#include "my_delay.h"
#include "my_io.h"
#include "my_can.h"
#include "my_bms24v10ah.h"
#include "my_bms24v40ah.h"
#include "my_camera.h"
#include "my_altimeter.h"
#include "my_hardwarereset.h"
//#include "my_encoder.h"
#include "my_straingauge.h"
#include "my_pistonmotor.h"

/** @addtogroup Template_Project
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
static __IO uint32_t uwTimingDelay;
RCC_ClocksTypeDef RCC_Clocks;
static bool Motors_Start_Up = false;

/* Private function prototypes -----------------------------------------------*/
static void SystemClock_Config(void);
uint8_t CAN_DataTest[8] = {0x01,0x02,0x03,0x04,0x05,0x06,0x07,0x08};
int16_t _count_encoder = 0;
CAN_DataTypeDef CAN_Data;

// Test Hold Position of Piston
float __Kp = 15.0f, __Ki = 0.01f, __Kd = 0.05f;
float __Desired_Position = 0.0f, __Desired_Position_LPF = 0.0f, __position = 0.0f;

/* Private functions ---------------------------------------------------------*/

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{ 
 /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       files before to branch to application main.
       To reconfigure the default setting of SystemInit() function, 
       refer to system_stm32f4xx.c file */
	SystemClock_Config();
  /* SysTick end of count event each 10ms */
  RCC_GetClocksFreq(&RCC_Clocks);
  //SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);
  
	UDELAY_Configuration();
	UDELAY_ms(2000);
	
	UIO_Configuration();
	UDELAY_ms(100);
		
	UCAN_Configure();
	UDELAY_ms(500);
	
	UPIS_Configuration();
	UDELAY_ms(500);
	
	UPIS_StartEncoder();
	UDELAY_ms(500);
	
//	UPIS_SetDuty(20.0, PISTON_CW); 
//	for(int i = 0; i <= 40; i = i + 5)
//	{
//		UPIS_SetDuty((float)i, PISTON_CW); 
//		UDELAY_ms(1000);
//	}
//		
//	for(int i = 40; i >= 0; i = i - 5)
//	{
//		UPIS_SetDuty((float)i, PISTON_CW); 
//		UDELAY_ms(1000);
//	}

//	USTR_Configuration();
//	UDELAY_ms(1000);
	
//	UBMS40_Configuration();
//	UDELAY_ms(500);
//	UBMS40_Cmd(ENABLE);
//	UDELAY_ms(500);
	
//	UBMS10_Configuration();

//	UCAM_Configuration();
//	UDELAY_ms(500);
//	UCAM_Focus_Mode(CAM_FOCUS_BOTH);
//	UDELAY_ms(500);
//	
//	UALTI_Configuration();
//	UDELAY_ms(500);
	
//	UHRST_Configuration();
//	UENC_Configuration();
//	UENC_StartEncoder();
	
//	UCAN_PassSystemReady(ENABLE);
  /* Infinite loop */
  while (1)
  {
//		CAN_Data.Pistol_Status.Encoder = UENC_GetNbrRoundPistol();
//		CAN_Data.Pistol_Status.LS_AtHead = UIO_CheckPistolAtHead();
//		CAN_Data.Pistol_Status.LS_AtTail = UIO_CheckPistolAtTail();
//		
//		CAN_Data.Mass_Status.Encoder = UENC_GetNbrRoundMass();
//		CAN_Data.Mass_Status.LS_AtHead = UIO_CheckMassAtHead();
//		CAN_Data.Mass_Status.LS_AtTail = UIO_CheckMassAtTail();
//		
////		CAN_Data.Leak_Status = UIO_LeakSensorPosition();
//		
////		USTR_GetStatus(&CAN_Data.Status_Hull);
//		
//		UBMS40_GetStatus(&CAN_Data.BMS40_Status);
//		
//		UALTI_GetStatus(&CAN_Data.ALTI_Status);
//		
//		if(UCAN_NeedRespondData() == true)
//		{
//			UCAN_Respond_ALLData(&CAN_Data);
//			UCAN_AllowRespondData(DISABLE);
//		}
//		
//		if(/*(UIO_CheckPistolAtTail() == 0x01) && */(Motors_Start_Up == false) && (UIO_CheckMassAtHead() == 0x01))
//		{
//			UENC_StartEncoder();
//			Motors_Start_Up = true;
//		}
//		
//		if(UCAN_IsNeedCheckSystem())
//		{
//			if(UCAN_IsSystemReady()) UCAN_SystemReady();
//			else UCAN_SystemNotReady();
//			UCAN_AllowRespondCheckSystem(DISABLE);
//		}
//		UIO_LEDRED_ON();
//		UIO_LEDBLUE_ON();
//		UIO_LEDGREEN_ON();
//		UIO_LEDORANGE_ON();
//		UDELAY_ms(200);
//		UIO_LEDRED_OFF();
//		UIO_LEDBLUE_OFF();
//		UIO_LEDGREEN_OFF();
//		UIO_LEDORANGE_OFF();
//		UDELAY_ms(200);
		
		UPIS_SetGain(__Kp, __Ki, __Kd);
		UPIS_SetDesiredPosition(__Desired_Position);
		__Desired_Position_LPF = UPIS_GetLPFDesiredPosition();
		__position = UPIS_GetCurrentPosition();
		
		if(UPIS_GetFlagUpdatePID() == true)
		{
			UPIS_PIDCaculation();
			UPIS_ResetFlagUpdatePID();
		}
//		_count_encoder = TIM_GetCounter(TIM4);
//		UDELAY_ms(10);
//		_count_encoder = UENC_GetNbrRoundPistol();
//		UCAN_Transmit(0x123,8,CAN_DataTest);
//		UBMS40_Transmit("Chau Thanh Hai");
//		UBMS10_Transmit("Chau Thanh Hai");
//		UALTI_Transmit("Chau Thanh Hai");
		
  }
}

/**
  * @brief  System Clock Configuration
  *         The system Clock is configured as follow : 
  *            System Clock source            = PLL (HSE)
  *            SYSCLK(Hz)                     = 168000000
  *            HCLK(Hz)                       = 168000000
  *            AHB Prescaler                  = 1
  *            APB1 Prescaler                 = 4
  *            APB2 Prescaler                 = 2
  *            HSE Frequency(Hz)              = 8000000
  *            PLL_M                          = 8
  *            PLL_N                          = 336
  *            PLL_P                          = 2
  *            PLL_Q                          = 7
  *            VDD(V)                         = 3.3
  *            Main regulator output voltage  = Scale1 mode
  *            Flash Latency(WS)              = 7
  * @param  None
  * @retval None
  */
static void SystemClock_Config(void)
{
  // Resets the clock configuration to the default reset state
    RCC_DeInit();

    // Enable external crystal (HSE)
    RCC_HSEConfig(RCC_HSE_ON);
    // Wait until HSE ready to use or not
    ErrorStatus errorStatus = RCC_WaitForHSEStartUp();

    if (errorStatus == SUCCESS)
    {
        // Configure the PLL for 168MHz SysClk and 48MHz for USB OTG, SDIO
        RCC_PLLConfig(RCC_PLLSource_HSE, 8, 336, 2, 7);
        // Enable PLL
        RCC_PLLCmd(ENABLE);
        // Wait until main PLL clock ready
        while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

        // Set flash latency
        FLASH_SetLatency(FLASH_Latency_5);

        // AHB 168MHz
        RCC_HCLKConfig(RCC_SYSCLK_Div1);
        // APB1 42MHz
        RCC_PCLK1Config(RCC_HCLK_Div4);
        // APB2 84 MHz
        RCC_PCLK2Config(RCC_HCLK_Div2);

        // Set SysClk using PLL
        RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    }
    else
    {
        // Do something to indicate that error clock configuration
        while (1);
    }

    SystemCoreClockUpdate();
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


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
