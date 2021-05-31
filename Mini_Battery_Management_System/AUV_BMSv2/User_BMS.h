/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USER_BMS_H
#define __USER_BMS_H

#ifdef __cplusplus
 extern "C" {
#endif
	
	 
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"	 
#include <stdbool.h>
#include "User_Delay.h"
#include <string.h>
#include "stm32f10x_tim.h"
	 
/* Exported constants/macros -------------------------------------------------*/


 /**
 * @defgroup <Group name here>
 * @{
 */	
 
 /**
 * @}
 */
typedef struct
{
	float BatteryTotal; //mAh
	float BatteryCapacity; // mAh
	float BatteryUsed; // mAh
	float BatteryPercentage; // %
	float BatteryCurrent; // mA
	float BaterryVoltage; // V	
}UBMS_Status_Typedef;

typedef enum {CONFIG_NO = 0, CONFIG_DONE = 1} UBMS_ConfigStatus;



/* Exported types ------------------------------------------------------------*/

/* Exported function prototypes ----------------------------------------------*/
void UBMS_Configure(void);
void UBMS_GetStatus(UBMS_Status_Typedef* _ubms_status);
void UBMS_SetBatteryTotal(float _battery_total);
void UBMS_RealoadDataFromFlash(void);
void UBMS_Display(void);
void UBMS_StartBMS(void);
uint8_t UBMS_CheckConfiguration(void);
void UBMS_SetConfiguration(UBMS_ConfigStatus _set_config_status);
	
/* Peripherals Interrupt prototypes ------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif 
