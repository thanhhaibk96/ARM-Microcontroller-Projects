/*
 * User_CAN.c
 *
 *  Created on: Apr 9, 2018
 *      Author: Chau Thanh Hai
 */
#include "my_can.h"

/**
 * @defgroup Module Pin define
 * @{
 */
		/** 
		* @brief   CAN Pin define 
		*/
			#define 	UCAN_TX_PIN  				   	GPIO_Pin_9			
			#define 	UCAN_TX_PORT 				  	GPIOB
			#define 	UCAN_TX_CLK							RCC_AHB1Periph_GPIOB			
			#define 	UCAN_TX_SOURCE				 	GPIO_PinSource9				
			#define		UCAN_TX_CLK_Cmd		  	 	RCC_AHB1PeriphClockCmd			

			#define 	UCAN_RX_PIN  				   	GPIO_Pin_8
			#define 	UCAN_RX_PORT 				  	GPIOB
			#define 	UCAN_RX_CLK							RCC_AHB1Periph_GPIOB
			#define 	UCAN_RX_SOURCE				 	GPIO_PinSource8
			#define		UCAN_RX_CLK_Cmd		  	 	RCC_AHB1PeriphClockCmd		

			#define		UCAN_CAN								CAN1
			#define 	UCAN_CAN_CLK						RCC_APB1Periph_CAN1
			#define		UCAN_CAN_CLK_Cmd		  	RCC_APB1PeriphClockCmd
			#define		UCAN_CAN_AF							GPIO_AF_CAN1
			#define 	UCAN_CAN_IRQn						CAN1_RX0_IRQn
			
			#define		UCAN_CAN_IRQHandler			CAN1_RX0_IRQHandler
			
#define MODULE_CAN_MCP251x 

#ifdef MODULE_CAN_MCP251x
	static const uint8_t num_frames_revc4req = 2;
#else
	static const uint8_t num_frames_revc4req = 1;
#endif


static bool _need_respond_data = false;
static bool _system_ready = false;
static bool _need_system_ready = false;
uint8_t _can_respond_count = 0;

/**
 * @}
 */
static int _IDCANBUS_ARM_1 = 0x121;
static int _IDCANBUS_ARM_2 = 0x122;
static int _IDCANBUS_MASS_SHIFTER = 0x123;
static int _IDCANBUS_PISTOL = 0x124;
static int _IDCANBUS_THRUSTER = 0x125;
static int _IDCANBUS_EPC = 0x126;

uint8_t CAN_RxMessage[8];
uint8_t CAN_TxMessage[8];

CanRxMsg RxMessage;

void UCAN_Configure(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	CAN_InitTypeDef        CAN_InitStruct;
	CAN_FilterInitTypeDef  CAN_FilterInitStruct;
	NVIC_InitTypeDef	NVIC_InitStruct;
	
	UCAN_TX_CLK_Cmd(UCAN_TX_CLK,ENABLE);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = UCAN_TX_PIN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	GPIO_Init(UCAN_TX_PORT,&GPIO_InitStruct);

	UCAN_RX_CLK_Cmd(UCAN_RX_CLK,ENABLE);
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = UCAN_RX_PIN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	GPIO_Init(UCAN_RX_PORT,&GPIO_InitStruct);
	
	GPIO_PinAFConfig(UCAN_TX_PORT,UCAN_TX_SOURCE,UCAN_CAN_AF);
	GPIO_PinAFConfig(UCAN_RX_PORT,UCAN_RX_SOURCE,UCAN_CAN_AF);
	
	UCAN_CAN_CLK_Cmd(UCAN_CAN_CLK,ENABLE);

	/* CAN register init */
	CAN_DeInit(UCAN_CAN);

/* CAN cell init */
	CAN_InitStruct.CAN_TTCM = DISABLE;
	CAN_InitStruct.CAN_ABOM = DISABLE;
	CAN_InitStruct.CAN_AWUM = DISABLE;
	CAN_InitStruct.CAN_NART = ENABLE;
	CAN_InitStruct.CAN_RFLM = DISABLE;
	CAN_InitStruct.CAN_TXFP = ENABLE;
	CAN_InitStruct.CAN_Mode = CAN_Mode_Normal;
	CAN_InitStruct.CAN_SJW = CAN_SJW_1tq;

	/* CAN Baudrate = 1 MBps (CAN clocked at 42 MHz) */
	// Baudrate = 1/(tq + tBS1 + tBS2)
	//http://www.bittiming.can-wiki.info/
	CAN_InitStruct.CAN_BS1 = CAN_BS1_12tq;
	CAN_InitStruct.CAN_BS2 = CAN_BS2_8tq;
	CAN_InitStruct.CAN_Prescaler = 2;
	CAN_Init(UCAN_CAN, &CAN_InitStruct);

	/* CAN filter init */
	CAN_FilterInitStruct.CAN_FilterNumber = 0;
	CAN_FilterInitStruct.CAN_FilterMode = CAN_FilterMode_IdList;
	CAN_FilterInitStruct.CAN_FilterScale = CAN_FilterScale_16bit;
	CAN_FilterInitStruct.CAN_FilterIdHigh = _IDCANBUS_ARM_1 << 5;
	CAN_FilterInitStruct.CAN_FilterIdLow = _IDCANBUS_ARM_1 << 5;
	CAN_FilterInitStruct.CAN_FilterMaskIdHigh = _IDCANBUS_ARM_1 << 5;
	CAN_FilterInitStruct.CAN_FilterMaskIdLow = _IDCANBUS_ARM_1 << 5;
	CAN_FilterInitStruct.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStruct.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStruct);

//	/* Enable FIFO 0 message pending Interrupt */
	CAN_ITConfig(UCAN_CAN, CAN_IT_FMP0, ENABLE);

	NVIC_InitStruct.NVIC_IRQChannel = CAN1_RX0_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x00;
	NVIC_Init(&NVIC_InitStruct);
}

void UCAN_Transmit(int _IDstd,int _length, uint8_t _data[])
{
	CanTxMsg _TxMessage;
  uint8_t mailbox;
	uint8_t status;
  _TxMessage.StdId = _IDstd;
	_TxMessage.ExtId = 0x00;
  _TxMessage.RTR = CAN_RTR_DATA;
  _TxMessage.IDE = CAN_ID_STD;
  _TxMessage.DLC = _length;
	for(int j = 0;j < _length; j++)
	{
		_TxMessage.Data[j] = _data[j];
	}

  mailbox = CAN_Transmit(UCAN_CAN,&_TxMessage);

	//wait until CAN transmission is OK
	int32_t i = 0;
  while((status != CANTXOK) && (i != 0xFFFF))
  {
    status = CAN_TransmitStatus(UCAN_CAN,mailbox);
    i++;
  }
	
//	UDELAY_us(100);
	
}

uint8_t UCAN_Checksum(uint8_t *_data)
{
	uint8_t value = 0;

	//Calculate CheckSum (Byte)
	for (int i = 0; i < 7; i++)
	{
		value += _data[i];
	}
	value = ~value;
	value++;
	return (uint8_t)value;
}

void UCAN_Convert_Float_to_Bytes(float _data_in, uint8_t* _data_out)
{
	union
	{
		float _value;
		uint8_t _byte[4];
	}_part;
	
	_part._value = _data_in;
	
	_data_out[0] = _part._byte[0];
	_data_out[1] = _part._byte[1];
	_data_out[2] = _part._byte[2];
	_data_out[3] = _part._byte[3];
}

void UCAN_Convert_Bytes_to_Float(uint8_t* _data_in, float *_data_out)
{
	union
	{
		float _value;
		uint8_t _byte[4];
	}_part;
		
	_part._byte[0] = _data_in[3];
	_part._byte[1] = _data_in[2];
	_part._byte[2] = _data_in[1];
	_part._byte[3] = _data_in[0];
	
	*_data_out = _part._value;
}

//--------Mass Shifter--------//
void UCAN_Mass_OLoop_Duty(float _duty, UCAN_Direction_of_Motor_TypeDef _direction)
{
	CAN_TxMessage[0] = 'O';
	CAN_TxMessage[1] = 'L';
	CAN_TxMessage[2] = _direction;
	UCAN_Convert_Float_to_Bytes(_duty, &CAN_TxMessage[3]);
	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);
	
	UCAN_Transmit(_IDCANBUS_MASS_SHIFTER, 8, CAN_TxMessage);
}

//--------Pistol--------//
void UCAN_Pistol_OLoop_Duty(float _duty, UCAN_Direction_of_Motor_TypeDef _direction)
{
	CAN_TxMessage[0] = 'O';
	CAN_TxMessage[1] = 'L';
	CAN_TxMessage[2] = _direction;
	UCAN_Convert_Float_to_Bytes(_duty, &CAN_TxMessage[3]);
	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);
	
	UCAN_Transmit(_IDCANBUS_PISTOL, 8, CAN_TxMessage);
}

void UCAN_SystemReady(void)
{
	CAN_TxMessage[0] = 'A';
	CAN_TxMessage[1] = 'R';
	CAN_TxMessage[2] = 'M';
	CAN_TxMessage[3] = '1';
	CAN_TxMessage[4] = 'R';
	CAN_TxMessage[5] = 'D';
	CAN_TxMessage[6] = 'Y';
	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);
	
	UCAN_Transmit(_IDCANBUS_ARM_1, 8, CAN_TxMessage);
}

void UCAN_SystemNotReady(void)
{
	CAN_TxMessage[0] = 'A';
	CAN_TxMessage[1] = 'R';
	CAN_TxMessage[2] = 'M';
	CAN_TxMessage[3] = '1';
	CAN_TxMessage[4] = 'N';
	CAN_TxMessage[5] = 'O';
	CAN_TxMessage[6] = 'T';
	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);
	
	UCAN_Transmit(_IDCANBUS_ARM_1, 8, CAN_TxMessage);
}

bool UCAN_IsSystemReady(void)
{
	return _system_ready;
}

void UCAN_PassSystemReady(FunctionalState NewState)
{
	_system_ready = (NewState == ENABLE)?true:false;
}

bool UCAN_IsNeedCheckSystem(void)
{
	return _need_system_ready;
}

void UCAN_Respond_ALLData(CAN_DataTypeDef *_can_data)
{
	//--------BMS24V40AH--------//
//	CAN_TxMessage[0] = ARM1_BMS24V40AH;
//	CAN_TxMessage[1] = STATUS_DATA;
//	CAN_TxMessage[2] = HOURS;
//	UCAN_Convert_Float_to_Bytes(_can_data->BMS40_Status.Hours, &CAN_TxMessage[3]);
//	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
//	UCAN_Transmit(_IDCANBUS_ARM_1, 8, CAN_TxMessage);
//	
//	CAN_TxMessage[0] = ARM1_BMS24V40AH;
//	CAN_TxMessage[1] = STATUS_DATA;
//	CAN_TxMessage[2] = MINUTES;
//	UCAN_Convert_Float_to_Bytes(_can_data->BMS40_Status.Minutes, &CAN_TxMessage[3]);
//	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
//	UCAN_Transmit(_IDCANBUS_ARM_1, 8, CAN_TxMessage);
//	
//	CAN_TxMessage[0] = ARM1_BMS24V40AH;
//	CAN_TxMessage[1] = STATUS_DATA;
//	CAN_TxMessage[2] = SECONDS;
//	UCAN_Convert_Float_to_Bytes(_can_data->BMS40_Status.Seconds, &CAN_TxMessage[3]);
//	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
//	UCAN_Transmit(_IDCANBUS_ARM_1, 8, CAN_TxMessage);
	
	CAN_TxMessage[0] = ARM1_PISTOL;
	CAN_TxMessage[1] = STATUS_DATA;
	CAN_TxMessage[2] = REQ_ENCODER;
	UCAN_Convert_Float_to_Bytes(_can_data->Pistol_Status.Encoder, &CAN_TxMessage[3]);
	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
	UCAN_Transmit(_IDCANBUS_ARM_1, 8, CAN_TxMessage);
	UDELAY_us(200);
	
	CAN_TxMessage[0] = ARM1_PISTOL;
	CAN_TxMessage[1] = STATUS_DATA;
	CAN_TxMessage[2] = REQ_PISTOL_LIMIT_SWITCH;
	CAN_TxMessage[3] = _can_data->Pistol_Status.LS_AtHead;
	CAN_TxMessage[4] = _can_data->Pistol_Status.LS_AtTail;
	CAN_TxMessage[5] = 0x00;
	CAN_TxMessage[6] = 0x00;
	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
	UCAN_Transmit(_IDCANBUS_ARM_1, 8, CAN_TxMessage);
	UDELAY_us(200);
	
	CAN_TxMessage[0] = ARM1_MASS_SHIFTER;
	CAN_TxMessage[1] = STATUS_DATA;
	CAN_TxMessage[2] = REQ_ENCODER;
	UCAN_Convert_Float_to_Bytes(_can_data->Mass_Status.Encoder, &CAN_TxMessage[3]);
	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
	UCAN_Transmit(_IDCANBUS_ARM_1, 8, CAN_TxMessage);
	UDELAY_us(200);
	
	CAN_TxMessage[0] = ARM1_MASS_SHIFTER;
	CAN_TxMessage[1] = STATUS_DATA;
	CAN_TxMessage[2] = REQ_MASS_LIMIT_SWITCH;
	CAN_TxMessage[3] = _can_data->Mass_Status.LS_AtHead;
	CAN_TxMessage[4] = _can_data->Mass_Status.LS_AtTail;
	CAN_TxMessage[5] = 0x00;
	CAN_TxMessage[6] = 0x00;
	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
	UCAN_Transmit(_IDCANBUS_ARM_1, 8, CAN_TxMessage);
	UDELAY_us(200);
	
	CAN_TxMessage[0] = ARM1_BMS24V40AH;
	CAN_TxMessage[1] = STATUS_DATA;
	CAN_TxMessage[2] = BATTERY_TOTAL;
	UCAN_Convert_Float_to_Bytes(_can_data->BMS40_Status.BatteryTotal, &CAN_TxMessage[3]);
	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
	UCAN_Transmit(_IDCANBUS_ARM_1, 8, CAN_TxMessage);
	UDELAY_us(200);
	
	CAN_TxMessage[0] = ARM1_BMS24V40AH;
	CAN_TxMessage[1] = STATUS_DATA;
	CAN_TxMessage[2] = BATTERY_CAPACITY;
	UCAN_Convert_Float_to_Bytes(_can_data->BMS40_Status.BatteryCapacity, &CAN_TxMessage[3]);
	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
	UCAN_Transmit(_IDCANBUS_ARM_1, 8, CAN_TxMessage);
	UDELAY_us(200);
	
	CAN_TxMessage[0] = ARM1_BMS24V40AH;
	CAN_TxMessage[1] = STATUS_DATA;
	CAN_TxMessage[2] = BATTERY_USED;
	UCAN_Convert_Float_to_Bytes(_can_data->BMS40_Status.BatteryUsed, &CAN_TxMessage[3]);
	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
	UCAN_Transmit(_IDCANBUS_ARM_1, 8, CAN_TxMessage);
	UDELAY_us(200);
	
	CAN_TxMessage[0] = ARM1_BMS24V40AH;
	CAN_TxMessage[1] = STATUS_DATA;
	CAN_TxMessage[2] = BATTERY_PERCENTAGE;
	UCAN_Convert_Float_to_Bytes(_can_data->BMS40_Status.BatteryPercentage, &CAN_TxMessage[3]);
	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
	UCAN_Transmit(_IDCANBUS_ARM_1, 8, CAN_TxMessage);
	UDELAY_us(200);
	
	CAN_TxMessage[0] = ARM1_BMS24V40AH;
	CAN_TxMessage[1] = STATUS_DATA;
	CAN_TxMessage[2] = BATTERY_CURRENT;
	UCAN_Convert_Float_to_Bytes(_can_data->BMS40_Status.BatteryCurrent, &CAN_TxMessage[3]);
	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
	UCAN_Transmit(_IDCANBUS_ARM_1, 8, CAN_TxMessage);
	UDELAY_us(200);
	
	CAN_TxMessage[0] = ARM1_BMS24V40AH;
	CAN_TxMessage[1] = STATUS_DATA;
	CAN_TxMessage[2] = BATTERY_VOLTAGE;
	UCAN_Convert_Float_to_Bytes(_can_data->BMS40_Status.BaterryVoltage, &CAN_TxMessage[3]);
	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
	UCAN_Transmit(_IDCANBUS_ARM_1, 8, CAN_TxMessage);
	UDELAY_us(200);
	
//	//--------BMS24V10AH--------//
//	CAN_TxMessage[0] = ARM1_BMS24V10AH;
//	CAN_TxMessage[1] = STATUS_DATA;
//	CAN_TxMessage[2] = HOURS;
//	UCAN_Convert_Float_to_Bytes(_can_data->BMS10_Status.Hours, &CAN_TxMessage[3]);
//	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
//	UCAN_Transmit(_IDCANBUS_ARM_1, 8, CAN_TxMessage);
//	
//	CAN_TxMessage[0] = ARM1_BMS24V10AH;
//	CAN_TxMessage[1] = STATUS_DATA;
//	CAN_TxMessage[2] = MINUTES;
//	UCAN_Convert_Float_to_Bytes(_can_data->BMS10_Status.Minutes, &CAN_TxMessage[3]);
//	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
//	UCAN_Transmit(_IDCANBUS_ARM_1, 8, CAN_TxMessage);
//	
//	CAN_TxMessage[0] = ARM1_BMS24V10AH;
//	CAN_TxMessage[1] = STATUS_DATA;
//	CAN_TxMessage[2] = SECONDS;
//	UCAN_Convert_Float_to_Bytes(_can_data->BMS10_Status.Seconds, &CAN_TxMessage[3]);
//	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
//	UCAN_Transmit(_IDCANBUS_ARM_1, 8, CAN_TxMessage);
//	
//	CAN_TxMessage[0] = ARM1_BMS24V10AH;
//	CAN_TxMessage[1] = STATUS_DATA;
//	CAN_TxMessage[2] = BATTERY_TOTAL;
//	UCAN_Convert_Float_to_Bytes(_can_data->BMS10_Status.BatteryTotal, &CAN_TxMessage[3]);
//	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
//	UCAN_Transmit(_IDCANBUS_ARM_1, 8, CAN_TxMessage);
//	
//	CAN_TxMessage[0] = ARM1_BMS24V10AH;
//	CAN_TxMessage[1] = STATUS_DATA;
//	CAN_TxMessage[2] = BATTERY_CAPACITY;
//	UCAN_Convert_Float_to_Bytes(_can_data->BMS10_Status.BatteryCapacity, &CAN_TxMessage[3]);
//	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
//	UCAN_Transmit(_IDCANBUS_ARM_1, 8, CAN_TxMessage);
//	
//	CAN_TxMessage[0] = ARM1_BMS24V10AH;
//	CAN_TxMessage[1] = STATUS_DATA;
//	CAN_TxMessage[2] = BATTERY_USED;
//	UCAN_Convert_Float_to_Bytes(_can_data->BMS10_Status.BatteryUsed, &CAN_TxMessage[3]);
//	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
//	UCAN_Transmit(_IDCANBUS_ARM_1, 8, CAN_TxMessage);
//	
//	CAN_TxMessage[0] = ARM1_BMS24V10AH;
//	CAN_TxMessage[1] = STATUS_DATA;
//	CAN_TxMessage[2] = BATTERY_PERCENTAGE;
//	UCAN_Convert_Float_to_Bytes(_can_data->BMS10_Status.BatteryPercentage, &CAN_TxMessage[3]);
//	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
//	UCAN_Transmit(_IDCANBUS_ARM_1, 8, CAN_TxMessage);
//	
//	CAN_TxMessage[0] = ARM1_BMS24V10AH;
//	CAN_TxMessage[1] = STATUS_DATA;
//	CAN_TxMessage[2] = BATTERY_CURRENT;
//	UCAN_Convert_Float_to_Bytes(_can_data->BMS10_Status.BatteryCurrent, &CAN_TxMessage[3]);
//	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
//	UCAN_Transmit(_IDCANBUS_ARM_1, 8, CAN_TxMessage);
//	
//	CAN_TxMessage[0] = ARM1_BMS24V10AH;
//	CAN_TxMessage[1] = STATUS_DATA;
//	CAN_TxMessage[2] = BATTERY_VOLTAGE;
//	UCAN_Convert_Float_to_Bytes(_can_data->BMS10_Status.BaterryVoltage, &CAN_TxMessage[3]);
//	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
//	UCAN_Transmit(_IDCANBUS_ARM_1, 8, CAN_TxMessage);
//	
	//--------ALTIMETER--------//
	CAN_TxMessage[0] = ARM1_ALTIMETER;
	CAN_TxMessage[1] = STATUS_DATA;
	CAN_TxMessage[2] = ALTIMETER_IN_METRES;
	UCAN_Convert_Float_to_Bytes(_can_data->ALTI_Status.ALTI_in_metres, &CAN_TxMessage[3]);
	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
	UCAN_Transmit(_IDCANBUS_ARM_1, 8, CAN_TxMessage);
	UDELAY_us(200);
	
	CAN_TxMessage[0] = ARM1_ALTIMETER;
	CAN_TxMessage[1] = STATUS_DATA;
	CAN_TxMessage[2] = ALTIMETER_IN_FATHOMS;
	UCAN_Convert_Float_to_Bytes(_can_data->ALTI_Status.ALTI_in_fathoms, &CAN_TxMessage[3]);
	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
	UCAN_Transmit(_IDCANBUS_ARM_1, 8, CAN_TxMessage);
	UDELAY_us(200);
	
	CAN_TxMessage[0] = ARM1_ALTIMETER;
	CAN_TxMessage[1] = STATUS_DATA;
	CAN_TxMessage[2] = ALTIMETER_IN_FEET;
	UCAN_Convert_Float_to_Bytes(_can_data->ALTI_Status.ALTI_in_feet, &CAN_TxMessage[3]);
	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
	UCAN_Transmit(_IDCANBUS_ARM_1, 8, CAN_TxMessage);
	UDELAY_us(200);
	
//	//--------LEAK SENSORS--------//
//	CAN_TxMessage[0] = ARM1_LEAK_SENSOR;
//	CAN_TxMessage[1] = STATUS_DATA;
//	CAN_TxMessage[2] = LEAK_POSITION;
//	UCAN_Convert_Float_to_Bytes(_can_data->Leak_Status, &CAN_TxMessage[3]);
//	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
//	UCAN_Transmit(_IDCANBUS_ARM_1, 8, CAN_TxMessage);
			
	//--------ACK--------//
	CAN_TxMessage[0] = 'R';
	CAN_TxMessage[1] = 'E';
	CAN_TxMessage[2] = 'Q';
	CAN_TxMessage[3] = 'A';
	CAN_TxMessage[4] = 'C';
	CAN_TxMessage[5] = 'K';
	CAN_TxMessage[6] = 0x00;
	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);
	UCAN_Transmit(_IDCANBUS_ARM_1,8,CAN_TxMessage);
}

bool UCAN_NeedRespondData(void)
{
	if(_can_respond_count >= num_frames_revc4req)
	{
		_need_respond_data = true;
		_can_respond_count = 0;
	}
	return _need_respond_data;
}

void UCAN_AllowRespondCheckSystem(FunctionalState NewState)
{
	_need_system_ready = (NewState == ENABLE)?true:false;
}

void UCAN_AllowRespondData(FunctionalState NewState)
{
	_need_respond_data = (NewState == ENABLE)?true:false;
}

/***********************************************************************************************************/
/*-------------------------------------- Peripherals Interrupt Handlers -----------------------------------*/
/***********************************************************************************************************/
void UCAN_CAN_IRQHandler(void)
{
	CAN_Receive(UCAN_CAN,CAN_FIFO0,&RxMessage);
	//--------Request ALL Data--------//
	if((RxMessage.StdId == _IDCANBUS_ARM_1)&&(RxMessage.IDE == CAN_ID_STD)&&(RxMessage.DLC == 8)&&(UCAN_Checksum(RxMessage.Data) == RxMessage.Data[7]))
	{
		if((RxMessage.Data[0] == 'A') && (RxMessage.Data[1] == 'R') 
				&& (RxMessage.Data[2] == 'M') && (RxMessage.Data[3] == '1')
				&& (RxMessage.Data[4] == 'C') && (RxMessage.Data[5] == 'H')
				&& (RxMessage.Data[6] == 'E'))
		{
			_need_system_ready = true;
		}
		
		switch(RxMessage.Data[1])
		{
			case READ_DATA:
			{
				if((RxMessage.Data[0] == ARM1_ALL_DATA) && (RxMessage.Data[6] == 0x0A))
				{
					_can_respond_count++;
				}
				break;
			}
			case WRITE_DATA:
			{
				if(RxMessage.Data[0] == ARM1_POWER_INT)
				{
					switch(RxMessage.Data[2])
					{
						case INT_24V40AH:
							UIO_Emergency24V40Ah(ENABLE);
							break;
						case INT_24V10AH:
							UIO_Emergency24V10Ah(ENABLE);
							break;
						default:
							break;						
					}
				}
				else if(RxMessage.Data[0] == ARM1_LIGHT)
				{
					if(RxMessage.Data[2] == LIGHT_ENABLE)
					{
						(RxMessage.Data[2] == 0x01)? UIO_Light_Cmd(ENABLE): UIO_Light_Cmd(DISABLE);
					}
				}
				break;
			}
			default:
				break;
		}
	}
}

