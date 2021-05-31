/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USER_RTC_H
#define __USER_RTC_H

#ifdef __cplusplus
 extern "C" {
#endif
	
	 
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"	 
#include <stdbool.h>
#include "User_Delay.h"
#include <string.h>
#include <stdio.h>
	 
/* Exported constants/macros -------------------------------------------------*/


 /**
 * @defgroup <Group name here>
 * @{
 */	
 
 /**
 * @}
 */	 
/* Exported types ------------------------------------------------------------*/
typedef struct
{
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
}URTC_Time_Typedef;

/* Exported function prototypes ----------------------------------------------*/
void URTC_Configure(URTC_Time_Typedef* _urtc_inittime);
void URTC_GetTime(URTC_Time_Typedef* _urtc_time);
void URTC_AdjustTime(URTC_Time_Typedef* _urtc_time);

/* Peripherals Interrupt prototypes ------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif 
