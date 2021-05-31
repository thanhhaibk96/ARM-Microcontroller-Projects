/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MY_ALTIMETER_H
#define __MY_ALTIMETER_H

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
typedef struct
{
	float ALTI_in_feet;
	float ALTI_in_metres;
	float ALTI_in_fathoms;
}Altimeter_StatusTypeDef;
/* Exported types ------------------------------------------------------------*/
	 
/* Exported function prototypes ----------------------------------------------*/
void UALTI_Configuration(void);
void UALTI_Transmit(char *szData);
void UALTI_GetStatus(Altimeter_StatusTypeDef *_status_data);

/* Peripherals Interrupt prototypes ------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif 
