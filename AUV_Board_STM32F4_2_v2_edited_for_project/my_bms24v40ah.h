/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MY_BMS24V40AH_H
#define __MY_BMS24V40AH_H

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
	uint8_t Hours;
	uint8_t Minutes;
	uint8_t Seconds;
	float BatteryTotal; //mAh
	float BatteryCapacity; // mAh
	float BatteryUsed; // mAh
	float BatteryPercentage; // %
	float BatteryCurrent; // mA
	float BaterryVoltage; // V	
}UBMS40_Status_Typedef;
/* Exported types ------------------------------------------------------------*/
	 
/* Exported function prototypes ----------------------------------------------*/
void UBMS40_Configuration(void);
void UBMS40_Transmit(char *szData);

/* Peripherals Interrupt prototypes ------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif 
