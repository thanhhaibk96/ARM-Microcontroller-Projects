/* Includes ------------------------------------------------------------------*/
#include "User_RTC.h"

/**
 * @defgroup Module Pin define
 * @{
 */
		/** 
		* @brief   RTC Pin define 
		*/
				
			#define 	URTC_BKP_CLK 				  RCC_APB1Periph_BKP
			#define 	URTC_PWR_CLK					RCC_APB1Periph_PWR				
			#define		URTC_CLK_Cmd		  	 	RCC_APB1PeriphClockCmd			

			#define 	URTC_RTC_IRQn										RTC_IRQn
			#define		URTC_RTC_PreemptionPriority			0x00
			#define		URTC_USART_SubPriority					0x02
			
			#define		URTC_RTC_IRQHandler			RTC_IRQHandler
			
/**
 * @}
 */
 
/* Public variables ----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private const/macros ------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Exported function body ----------------------------------------------------*/

/* Private functions body ----------------------------------------------------*/
void URTC_Configure(URTC_Time_Typedef* _urtc_inittime)
{
	if (BKP_ReadBackupRegister(BKP_DR1) != 0xA5A5)
  {
    /* RTC Configuration */
    /* Enable PWR and BKP clocks */
		URTC_CLK_Cmd(URTC_PWR_CLK | URTC_BKP_CLK, ENABLE);

		/* Allow access to BKP Domain */
		PWR_BackupAccessCmd(ENABLE);

		/* Reset Backup Domain */
		BKP_DeInit();

		/* Enable LSE */
		RCC_LSEConfig(RCC_LSE_ON);
		/* Wait till LSE is ready */
		while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET)
		{}

		/* Select LSE as RTC Clock Source */
		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

		/* Enable RTC Clock */
		RCC_RTCCLKCmd(ENABLE);

		/* Wait for RTC registers synchronization */
		RTC_WaitForSynchro();

		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();

//		/* Enable the RTC Second */
//		RTC_ITConfig(RTC_IT_SEC, ENABLE);

//		/* Wait until last write operation on RTC registers has finished */
//		RTC_WaitForLastTask();

		/* Set RTC prescaler: set RTC period to 1sec */
		RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();

    /* Adjust time by values entered by the user on the hyperterminal */
		
		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();
		/* Change the current time */
		RTC_SetCounter(_urtc_inittime->hours*3600 + _urtc_inittime->minutes*60 + _urtc_inittime->seconds);
		//	RTC_SetCounter(14*3600 + 06*60 + 00);
		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();
    
    BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
  }
  else
  {
    /* Wait for RTC registers synchronization */
    RTC_WaitForSynchro();

    /* Enable the RTC Second */
    RTC_ITConfig(RTC_IT_SEC, ENABLE);
    /* Wait until last write operation on RTC registers has finished */
    RTC_WaitForLastTask();
		RCC_ClearFlag();
  }
}

void URTC_GetTime(URTC_Time_Typedef* _urtc_time)
{
	uint32_t TimeVar = 0;
  /* Reset RTC Counter when Time is 23:59:59 */
  if (RTC_GetCounter() == 0x0001517F)
  {
		TimeVar = RTC_GetCounter();
		RTC_SetCounter(0x00);
		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();
  }
	else
	{
		TimeVar = RTC_GetCounter();
	}
  
  /* Compute  hours */
  _urtc_time->hours = (uint8_t)(TimeVar / 3600);
  /* Compute minutes */
  _urtc_time->minutes = (uint8_t)((TimeVar % 3600) / 60);
  /* Compute seconds */
  _urtc_time->seconds = (uint8_t)((TimeVar % 3600) % 60);

//  printf("Time: %0.2d:%0.2d:%0.2d\r", _urtc_time->hours, _urtc_time->minutes, _urtc_time->seconds);
}

void URTC_AdjustTime(URTC_Time_Typedef* _urtc_time)
{
	/* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
  /* Change the current time */
  RTC_SetCounter(_urtc_time->hours*3600 + _urtc_time->minutes*60 + _urtc_time->seconds);
  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
}



/******************************************************************************/
/* Peripherals Interrupt Handlers --------------------------------------------*/
/******************************************************************************/
