/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MY_PISTONMOTOR_H
#define __MY_PISTONMOTOR_H

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
#include "misc.h"

/* Exported constants/macros -------------------------------------------------*/
typedef enum
{
	PISTON_CW = 'R',
	PISTON_CCW = 'L'
}UPIS_Direction;

/* Exported types ------------------------------------------------------------*/

/* Exported function prototypes ----------------------------------------------*/
void UPIS_Configuration(void);
void UPIS_SetDuty(float _duty, UPIS_Direction _direction);
void UPIS_StartEncoder(void);
void UPIS_StopEncoder(void);
float UPIS_GetCurrentPosition(void);
float UPIS_GetLPFDesiredPosition(void);
void UPIS_SetGain(float kp, float ki, float kd);
void UPIS_SetDesiredPosition(float position);
bool UPIS_GetFlagUpdatePID(void);
void UPIS_ResetFlagUpdatePID(void);
void UPIS_PIDCaculation(void);

/* Peripherals Interrupt prototypes ------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif 
