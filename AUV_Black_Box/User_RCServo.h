/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USER_RCSERVO_H
#define __USER_RCSERVO_H

#ifdef __cplusplus
 extern "C" {
#endif
	
	 
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"	 
#include <stdbool.h>
#include "User_Delay.h"
#include <string.h>
	 
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
void URC_Configure(void);
void URC_SwitchON(void); //degree
void URC_SwitchOFF(void); //degree
/* Peripherals Interrupt prototypes ------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif 
