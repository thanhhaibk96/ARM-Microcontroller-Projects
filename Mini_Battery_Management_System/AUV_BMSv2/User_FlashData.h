/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USER_FLASHDATA_H
#define __USER_FLASHDATA_H

#ifdef __cplusplus
 extern "C" {
#endif
	
	 
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"	 
#include <stdbool.h>
#include "User_Delay.h"
#include <string.h>
#include "User_BMS.h"
	 
/* Exported constants/macros -------------------------------------------------*/

typedef enum {FAILED = 0, PASSED = !FAILED} CheckStatus;
 /**
 * @defgroup <Group name here>
 * @{
 */	
 
 /**
 * @}
 */	 
/* Exported types ------------------------------------------------------------*/
	 
/* Exported function prototypes ----------------------------------------------*/
void UFLASH_WriteFlash(UBMS_Status_Typedef* _ubms_status);
void UFLASH_ReadFlash(UBMS_Status_Typedef* _ubms_status);

/* Peripherals Interrupt prototypes ------------------------------------------*/

#ifdef __cplusplus
}
#endif

#endif 
