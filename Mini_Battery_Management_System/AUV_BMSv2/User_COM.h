/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USER_COM_H
#define __USER_COM_H

#ifdef __cplusplus
 extern "C" {
#endif
	
	 
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"	 
#include <stdbool.h>
#include "User_Delay.h"
#include <string.h>
#include "User_BMS.h"
#include "User_RTC.h"
	 
/* Exported constants/macros -------------------------------------------------*/


 /**
 * @defgroup <Group name here>
 * @{
 */	
 
 /**
 * @}
 */	 
/* Exported types ------------------------------------------------------------*/
	 
/* Exported function prototypes ----------------------------------------------*/
void UCOM_Configure(void);
void UCOM_RequiredConfigBaterry(void);
void UCOM_SendStatusBaterry(URTC_Time_Typedef* _urtc_time, UBMS_Status_Typedef* _ubms_status);
bool UCOM_Get_Flag(void);
/* Peripherals Interrupt prototypes ------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif 
