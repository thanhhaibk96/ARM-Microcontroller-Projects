/*
 * User_CAN.h
 *
 *  Created on: Apr 9, 2018
 *      Author: Chau Thanh Hai
 */
 #ifndef MY_CAN_H_
#define MY_CAN_H_

#include "misc.h"
#include "stm32f4xx_can.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_tim.h"
#include "my_bms24v40ah.h"
#include "my_bms24v10ah.h"
#include "my_altimeter.h"
#include <stdbool.h>
#include "my_io.h"
#include "my_delay.h"
#include "my_straingauge.h"

typedef enum
{
	MOTOR_CW		= 'R',
	MOTOR_CCW		= 'L',
}UCAN_Direction_of_Motor_TypeDef;

typedef struct
{
	float Encoder;
	uint8_t LS_AtHead;
	uint8_t LS_AtTail;
}Motor_StatusTypeDef;

typedef struct
{
	Motor_StatusTypeDef Mass_Status;
	Motor_StatusTypeDef Pistol_Status;
	UBMS40_Status_Typedef BMS40_Status;
	UBMS10_Status_Typedef BMS10_Status;
	Altimeter_StatusTypeDef ALTI_Status;
	Strain_Gauge_StatusTypeDef Status_Hull;
	uint8_t Leak_Status;
}CAN_DataTypeDef;

typedef enum
{
		//--------FUNCTION--------//
		WRITE_DATA = 0x00,
		READ_DATA = 0x01,
		STATUS_DATA = 0x02,
}Function_CANBUS_TypeDef;

typedef enum
{
		//--------ID BOARD ARM 1--------//
		ARM1_MASS_SHIFTER = 0x00,
		ARM1_PISTOL = 0x01,
		ARM1_LEAK_SENSOR = 0x02,
		ARM1_STRAIN_GAUGE = 0x03,
		ARM1_BMS24V10AH = 0x04,
		ARM1_BMS24V40AH = 0x05,
		ARM1_ALTIMETER = 0x06,
		ARM1_POWER_INT = 0x07,
		ARM1_LIGHT = 0x08,
		ARM1_ALL_DATA = 0xFF,		
}ID_CANBUS_TypeDef;

typedef enum
{
		//--------REGISTER LIGHT--------//
		LIGHT_ENABLE = 0xFF,
		//--------REGISTER MOTOR--------//
		REQ_ENCODER = 0x10,
		REQ_PISTOL_LIMIT_SWITCH = 0x11,
		REQ_MASS_LIMIT_SWITCH = 0x12,
		//--------REGISTER LEAK-SENSOR--------//
		LEAK_POSITION = 0xFF,
		//--------REGISTER STRAIN-GAUGE--------//
		STATUS_HULL = 0xFF,
		//--------BMS STATUS--------//
		HOURS = 0x01,
		MINUTES = 0x02,
		SECONDS = 0x03,
		BATTERY_TOTAL = 0x04,
		BATTERY_CAPACITY = 0x05,
		BATTERY_USED = 0x06,
		BATTERY_PERCENTAGE = 0x07,
		BATTERY_CURRENT = 0x08, 
		BATTERY_VOLTAGE = 0x09,
		//--------ALTIMETER STATUS--------//
		ALTIMETER_IN_FEET = 0x01,
		ALTIMETER_IN_METRES = 0x02,
		ALTIMETER_IN_FATHOMS = 0x03,
		//--------POWER INT--------//
		INT_24V40AH = 0xFE,
		INT_24V10AH = 0xFF,
}REGISTER_CANBUS_TypeDef;

void UCAN_Configure(void);
void UCAN_Respond_ALLData(CAN_DataTypeDef *_can_data);
bool UCAN_NeedRespondData(void);
void UCAN_AllowRespondData(FunctionalState NewState);
void UCAN_AllowRespondCheckSystem(FunctionalState NewState);
void UCAN_SystemReady(void);
void UCAN_SystemNotReady(void);
bool UCAN_IsSystemReady(void);
bool UCAN_IsNeedCheckSystem(void);
void UCAN_PassSystemReady(FunctionalState NewState);
void UCAN_Transmit(int _IDstd,int _length, uint8_t _data[]);

//-------Function For Mass Shifter-------//
void UCAN_Mass_OLoop_Duty(float _duty, UCAN_Direction_of_Motor_TypeDef _direction);

//-------Function For Pistol-------//
void UCAN_Pistol_OLoop_Duty(float _duty, UCAN_Direction_of_Motor_TypeDef _direction);

#endif /* USER_CAN_H_ */
