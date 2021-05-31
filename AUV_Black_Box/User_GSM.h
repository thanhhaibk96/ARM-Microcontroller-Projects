/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USER_GSM_H
#define __USER_GSM_H

#ifdef __cplusplus
 extern "C" {
#endif
	
	 
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"	 
#include <stdbool.h>
#include <stdio.h>
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
typedef enum {_ERROR = 0, _OK = !RESET} UGSM_FlagStatus;

/* Exported function prototypes ----------------------------------------------*/
void UGSM_Configure(void);
void UGSM_InitGSM(int _baudrate);
void UGSM_MakeSMS(char *_number, char *_text);
UGSM_FlagStatus UGSM_Ping(void);
void UGSM_MakeCall(char *_number);

/* Peripherals Interrupt prototypes ------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif 
