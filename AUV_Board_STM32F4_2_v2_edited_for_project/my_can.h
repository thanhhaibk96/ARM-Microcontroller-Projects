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
#include <stdbool.h>
#include "my_mx28.h"
#include "my_bms24v40ah.h"
#include "my_keller_pa3.h"
#include "my_io.h"

typedef enum
{
	MOTOR_CW		= 'L',
	MOTOR_CCW		= 'R',
}UCAN_Direction_of_Motor_TypeDef;

typedef struct
{
	float ActualSpeed;
	float DutyCycle;
	float DesiredSpeed;
	float TempOnChip;
	float TempOnLM35;
	float iMotor;
	float encoder;
}Motor_StatusTypeDef;

typedef struct
{
	Motor_StatusTypeDef Thruster;
	MX28_Status_Typedef MX28_Status;
	UBMS40_Status_Typedef BMS40_Status;
	uint8_t Leak_Status;
	UKellerPA3_Status_Typedef KellerPA3_Status;
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
		//--------ID BOARD ARM 2--------//
		ARM2_THRUSTER = 0x00,
		ARM2_LEAK_SENSOR = 0x01,
		ARM2_STRAIN_GAUGE = 0x02,
		ARM2_BMS24V40AH = 0x03,
		ARM2_RUDDER = 0x04,
		ARM2_POWER_INT = 0x05,
		ARM2_PRESSURE = 0x06,
		ARM2_ALL_DATA = 0xFF,
}ID_CANBUS_TypeDef;

typedef enum
{
		//--------REGISTER MOTOR--------//
		COM = 0x00,
		OPENLOOP_DUTY = 0x01,
		PID_DESIREDSPEED = 0x02,
		M_KP = 0x03,
		M_KI = 0x04,
		M_KD = 0x05,
		FUZZY_DESIREDSPEED = 0x06,
		M_GE = 0x07,
		M_GDE = 0x08,
		M_GDU = 0x09,
		REQ_IMOTOR = 0x0A,
		REQ_TEMP_ON_CHIP = 0x0B,
		REQ_TEMP_LM35 = 0x0C,
		REQ_DUTY = 0x0D,
		REQ_DESIREDSPEED = 0x0E,
		REQ_AUTUALSPEED = 0x0F,
		REQ_ENCODER = 0x10,
		//--------REGISTER RUDDER--------//
		MX28_GOAL_POSITION = 0x00,
		MX28_MOVING_SPEED = 0x01,
		MX28_KP = 0x02,
		MX28_KI = 0x03,
		MX28_KD = 0x04,
		MX28_BAUDRATE = 0x05,
		MX28_PRESENT_POSITION = 0x06,
		MX28_PRESENT_SPEED = 0x07,
		MX28_PRESENT_LOAD = 0x08,
		MX28_PRESENT_VOL = 0x09,
		MX28_PRESENT_TEMP = 0x0A,
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
		//--------REGISTER PRESSURE--------//
		DEPTH_DATA = 0xFE,
		TEMP_DATA = 0xFF,
		//--------POWER INT--------//
		INT_24V40AH = 0xFE,
		INT_24V10AH = 0xFF,
}REGISTER_CANBUS_TypeDef;

void UCAN_Configure(void);
void UCAN_Transmit(int _IDstd,int _length, uint8_t _data[]);
void UCAN_Respond_ALLData(CAN_DataTypeDef *_can_data);
bool UCAN_NeedRespondData(void);
void UCAN_SystemReady(void);
bool UCAN_IsSystemReady(void);
void UCAN_AllowRespondData(FunctionalState NewState);

//-------Function For Thruster-------//
void UCAN_Thruster_StartCom(void);
void UCAN_Thruster_CloseCom(void);
void UCAN_Thruster_OLoop_Duty(float _duty, UCAN_Direction_of_Motor_TypeDef _direction);
void UCAN_Thruster_PID_SetPoint(float _SP, UCAN_Direction_of_Motor_TypeDef _direction);
void UCAN_Thruster_PID_SetKp(float _Kp);
void UCAN_Thruster_PID_SetKi(float _Ki);
void UCAN_Thruster_PID_SetKd(float _Kd);
void UCAN_Thruster_Fuzzy_SetPoint(float _SP, UCAN_Direction_of_Motor_TypeDef _direction);
void UCAN_Thruster_Fuzzy_SetGe(float _Ge);
void UCAN_Thruster_Fuzzy_SetDe(float _De);
void UCAN_Thruster_Fuzzy_SetDu(float _Du);

float UCAN_Thruster_Get_ActualSpeed(void);
float UCAN_Thruster_Get_DutyCycle(void);
float UCAN_Thruster_Get_DesiredSpeed(void);
float UCAN_Thruster_Get_TempOnChip(void);
float UCAN_Thruster_Get_TempOnLM35(void);
float UCAN_Thruster_Get_iMotor(void);

#endif /* USER_CAN_H_ */
