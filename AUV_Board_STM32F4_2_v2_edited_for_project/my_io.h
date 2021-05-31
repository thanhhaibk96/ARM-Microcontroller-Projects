/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MY_IO_H
#define __MY_IO_H

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
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_exti.h"
#include "stm32f4xx_syscfg.h"
#include "misc.h"

/* Exported constants/macros -------------------------------------------------*/

/* Exported types ------------------------------------------------------------*/
	 
/* Exported function prototypes ----------------------------------------------*/
void UIO_Configuration(void);
void UIO_LEDRED_ON(void);
void UIO_LEDRED_OFF(void);
void UIO_LEDGREEN_ON(void);
void UIO_LEDGREEN_OFF(void);
void UIO_LEDBLUE_ON(void);
void UIO_LEDBLUE_OFF(void);
void UIO_LEDORANGE_ON(void);
void UIO_LEDORANGE_OFF(void);
void UIO_Emergency24V40Ah(FunctionalState NewState);
uint8_t UIO_LeakSensorPosition(void);

/* Peripherals Interrupt prototypes ------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif 
