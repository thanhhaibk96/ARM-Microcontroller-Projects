/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USER_CAN_H
#define __USER_CAN_H

#ifdef __cplusplus
 extern "C" {
#endif
	
	 
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"	 
#include <stdbool.h>

	 
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
void UCAN_Configure(void);
void UCAN_Transmit(int _IDstd, uint8_t _data[]);

/* Peripherals Interrupt prototypes ------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif 
