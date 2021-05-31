/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MY_RCSERVO_H
#define __MY_RCSERVO_H

#ifdef __cplusplus
 extern "C" {
#endif
	
	 
/* Includes ------------------------------------------------------------------*/
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "stm32f4xx.h"	 
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_usart.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_dma.h" 
#include "misc.h"

/* Exported constants/macros -------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
	 
/* Exported function prototypes ----------------------------------------------*/
void URC_Configuration(void);
void URC_SetAngle(float _angle); // _angle --> 0-180 degree

/* Peripherals Interrupt prototypes ------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif 
