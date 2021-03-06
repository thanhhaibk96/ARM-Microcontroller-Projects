/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USER_XBEE_H
#define __USER_XBEE_H

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
void UXBEE_Configure(void);
void UXBEE_Transmit(char *szData);

/* Peripherals Interrupt prototypes ------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif 
