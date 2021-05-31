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
/**
 * @}
 */
static bool _need_respond_data = false;
static bool _system_ready = false;

static int _IDCANBUS_ARM_1 = 0x121;
static int _IDCANBUS_ARM_2 = 0x122;
static int _IDCANBUS_MASS_SHIFTER = 0x123;
static int _IDCANBUS_PISTOL = 0x124;
static int _IDCANBUS_THRUSTER = 0x125;
static int _IDCANBUS_EPC = 0x126;

//--------Mass Shifter--------//
static bool Thruster_received_ActualSpeed_Done = false;
static bool Thruster_received_DutyCycle_Done = false;
static bool Thruster_received_DesiredSpeed_Done = false;
static bool Thruster_received_TempOnChip_Done = false;
static bool Thruster_received_TempOnLM35_Done = false;
static bool Thruster_received_iMotor_Done = false;

static float Thruster_value_ActualSpeed;
static float Thruster_value_DutyCycle;
static float Thruster_value_DesiredSpeed;
static float Thruster_value_TempOnChip;
static float Thruster_value_TempOnLM35;
static float Thruster_value_iMotor;

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
	CAN_FilterInitStruct.CAN_FilterIdHigh = _IDCANBUS_ARM_2 << 5;
	CAN_FilterInitStruct.CAN_FilterIdLow = _IDCANBUS_ARM_2 << 5;
	CAN_FilterInitStruct.CAN_FilterMaskIdHigh = _IDCANBUS_ARM_2 << 5;
	CAN_FilterInitStruct.CAN_FilterMaskIdLow = _IDCANBUS_ARM_2 << 5;
	CAN_FilterInitStruct.CAN_FilterFIFOAssignment = 0;
	CAN_FilterInitStruct.CAN_FilterActivation = ENABLE;
	CAN_FilterInit(&CAN_FilterInitStruct);

//	/* Enable FIFO 0 message pending Interrupt */
	CAN_ITConfig(UCAN_CAN, CAN_IT_FMP0, ENABLE);

	NVIC_InitStruct.NVIC_IRQChannel = CAN1_RX0_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
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
}

void UCAN_GetDataReceive(uint8_t* _data)
{
	_data = CAN_RxMessage;
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

//--------Thruster Shifter--------//
void UCAN_Thruster_StartCom(void)
{
	CAN_TxMessage[0] = 'C';
	CAN_TxMessage[1] = 'A';
	CAN_TxMessage[2] = 'N';
	CAN_TxMessage[3] = 'O';
	CAN_TxMessage[4] = 0x00;
	CAN_TxMessage[5] = 0x00;
	CAN_TxMessage[6] = 0x00;
	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);
	
	UCAN_Transmit(_IDCANBUS_THRUSTER, 8, CAN_TxMessage);
}

void UCAN_Thruster_CloseCom(void)
{
	CAN_TxMessage[0] = 'C';
	CAN_TxMessage[1] = 'A';
	CAN_TxMessage[2] = 'N';
	CAN_TxMessage[3] = 'C';
	CAN_TxMessage[4] = 0x00;
	CAN_TxMessage[5] = 0x00;
	CAN_TxMessage[6] = 0x00;
	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);
	
	UCAN_Transmit(_IDCANBUS_THRUSTER, 8, CAN_TxMessage);
}

void UCAN_Thruster_OLoop_Duty(float _duty, UCAN_Direction_of_Motor_TypeDef _direction)
{
	CAN_TxMessage[0] = 'O';
	CAN_TxMessage[1] = 'L';
	CAN_TxMessage[2] = _direction;
	UCAN_Convert_Float_to_Bytes(_duty, &CAN_TxMessage[3]);
	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);
	
	UCAN_Transmit(_IDCANBUS_THRUSTER, 8, CAN_TxMessage);
}

void UCAN_Thruster_PID_SetPoint(float _SP, UCAN_Direction_of_Motor_TypeDef _direction)
{
	CAN_TxMessage[0] = 'C';
	CAN_TxMessage[1] = 'P';
	CAN_TxMessage[2] = _direction;
	UCAN_Convert_Float_to_Bytes(_SP, &CAN_TxMessage[3]);
	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);
	
	UCAN_Transmit(_IDCANBUS_THRUSTER, 8, CAN_TxMessage);
}

void UCAN_Thruster_Fuzzy_SetPoint(float _SP, UCAN_Direction_of_Motor_TypeDef _direction)
{
	CAN_TxMessage[0] = 'C';
	CAN_TxMessage[1] = 'F';
	CAN_TxMessage[2] = _direction;
	UCAN_Convert_Float_to_Bytes(_SP, &CAN_TxMessage[3]);
	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);
	
	UCAN_Transmit(_IDCANBUS_THRUSTER, 8, CAN_TxMessage);
}

void UCAN_Thruster_PID_SetKp(float _Kp)
{
	CAN_TxMessage[0] = 'G';
	CAN_TxMessage[1] = 'K';
	CAN_TxMessage[2] = 'P';
	UCAN_Convert_Float_to_Bytes(_Kp, &CAN_TxMessage[3]);
	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);
	
	UCAN_Transmit(_IDCANBUS_THRUSTER, 8, CAN_TxMessage);
}

void UCAN_Thruster_PID_SetKi(float _Ki)
{
	CAN_TxMessage[0] = 'G';
	CAN_TxMessage[1] = 'K';
	CAN_TxMessage[2] = 'I';
	UCAN_Convert_Float_to_Bytes(_Ki, &CAN_TxMessage[3]);
	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);
	
	UCAN_Transmit(_IDCANBUS_THRUSTER, 8, CAN_TxMessage);
}

void UCAN_Thruster_PID_SetKd(float _Kd)
{
	CAN_TxMessage[0] = 'G';
	CAN_TxMessage[1] = 'K';
	CAN_TxMessage[2] = 'D';
	UCAN_Convert_Float_to_Bytes(_Kd, &CAN_TxMessage[3]);
	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);
	
	UCAN_Transmit(_IDCANBUS_THRUSTER, 8, CAN_TxMessage);
}

void UCAN_Thruster_Fuzzy_SetGe(float _Ge)
{
	CAN_TxMessage[0] = 'G';
	CAN_TxMessage[1] = 'G';
	CAN_TxMessage[2] = 'E';
	UCAN_Convert_Float_to_Bytes(_Ge, &CAN_TxMessage[3]);
	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);
	
	UCAN_Transmit(_IDCANBUS_THRUSTER, 8, CAN_TxMessage);
}

void UCAN_Thruster_Fuzzy_SetDe(float _De)
{
	CAN_TxMessage[0] = 'G';
	CAN_TxMessage[1] = 'D';
	CAN_TxMessage[2] = 'E';
	UCAN_Convert_Float_to_Bytes(_De, &CAN_TxMessage[3]);
	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);
	
	UCAN_Transmit(_IDCANBUS_THRUSTER, 8, CAN_TxMessage);
}

void UCAN_Thruster_Fuzzy_SetDu(float _Du)
{
	CAN_TxMessage[0] = 'G';
	CAN_TxMessage[1] = 'D';
	CAN_TxMessage[2] = 'U';
	UCAN_Convert_Float_to_Bytes(_Du, &CAN_TxMessage[3]);
	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);
	
	UCAN_Transmit(_IDCANBUS_THRUSTER, 8, CAN_TxMessage);
}

float UCAN_Thruster_Get_ActualSpeed(void)
{
	CAN_TxMessage[0] = 'R';
	CAN_TxMessage[1] = 'E';
	CAN_TxMessage[2] = 'Q';
	CAN_TxMessage[3] = 'R';
	CAN_TxMessage[4] = 'P';
	CAN_TxMessage[5] = 'V';
	CAN_TxMessage[6] = 0x0A;
	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);
	
	UCAN_Transmit(_IDCANBUS_THRUSTER, 8, CAN_TxMessage);
	
	Thruster_received_ActualSpeed_Done = false;
	while(!Thruster_received_ActualSpeed_Done);
	return Thruster_value_ActualSpeed;
}

float UCAN_Thruster_Get_TempOnChip(void)
{
	CAN_TxMessage[0] = 'R';
	CAN_TxMessage[1] = 'E';
	CAN_TxMessage[2] = 'Q';
	CAN_TxMessage[3] = 'A';
	CAN_TxMessage[4] = 'O';
	CAN_TxMessage[5] = 'C';
	CAN_TxMessage[6] = 0x0A;
	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);
	
	UCAN_Transmit(_IDCANBUS_THRUSTER, 8, CAN_TxMessage);
	
	Thruster_received_TempOnChip_Done = false;
	while(!Thruster_received_TempOnChip_Done);
	return Thruster_value_TempOnChip;
}

float UCAN_Thruster_Get_TempOnLM35(void)
{
	CAN_TxMessage[0] = 'R';
	CAN_TxMessage[1] = 'E';
	CAN_TxMessage[2] = 'Q';
	CAN_TxMessage[3] = 'A';
	CAN_TxMessage[4] = 'T';
	CAN_TxMessage[5] = 'K';
	CAN_TxMessage[6] = 0x0A;
	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);
	
	UCAN_Transmit(_IDCANBUS_THRUSTER, 8, CAN_TxMessage);
	
	Thruster_received_TempOnLM35_Done = false;
	while(!Thruster_received_TempOnLM35_Done);
	return Thruster_value_TempOnLM35;
}

float UCAN_Thruster_Get_iMotor(void)
{
	CAN_TxMessage[0] = 'R';
	CAN_TxMessage[1] = 'E';
	CAN_TxMessage[2] = 'Q';
	CAN_TxMessage[3] = 'A';
	CAN_TxMessage[4] = 'I';
	CAN_TxMessage[5] = 'M';
	CAN_TxMessage[6] = 0x0A;
	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);
	
	UCAN_Transmit(_IDCANBUS_THRUSTER, 8, CAN_TxMessage);	
	
	Thruster_received_iMotor_Done = false;
	while(!Thruster_received_iMotor_Done);
	return Thruster_value_iMotor;
}

float UCAN_Thruster_Get_DutyCycle(void)
{
	CAN_TxMessage[0] = 'R';
	CAN_TxMessage[1] = 'E';
	CAN_TxMessage[2] = 'Q';
	CAN_TxMessage[3] = 'T';
	CAN_TxMessage[4] = 'D';
	CAN_TxMessage[5] = 'C';
	CAN_TxMessage[6] = 0x0A;
	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);
	
	UCAN_Transmit(_IDCANBUS_THRUSTER, 8, CAN_TxMessage);
	
	Thruster_received_DutyCycle_Done = false;
	while(!Thruster_received_DutyCycle_Done);
	return Thruster_value_DutyCycle;
}

float UCAN_Thruster_Get_DesiredSpeed(void)
{
	CAN_TxMessage[0] = 'R';
	CAN_TxMessage[1] = 'E';
	CAN_TxMessage[2] = 'Q';
	CAN_TxMessage[3] = 'R';
	CAN_TxMessage[4] = 'S';
	CAN_TxMessage[5] = 'P';
	CAN_TxMessage[6] = 0x0A;
	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);
	
	UCAN_Transmit(_IDCANBUS_THRUSTER, 8, CAN_TxMessage);
	
	Thruster_received_DesiredSpeed_Done = false;
	while(!Thruster_received_DesiredSpeed_Done);
	return Thruster_value_DesiredSpeed;
}

void UCAN_SystemReady(void)
{
	CAN_TxMessage[0] = 'A';
	CAN_TxMessage[1] = 'R';
	CAN_TxMessage[2] = 'M';
	CAN_TxMessage[3] = '2';
	CAN_TxMessage[4] = 'R';
	CAN_TxMessage[5] = 'D';
	CAN_TxMessage[6] = 'Y';
	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);
	
	UCAN_Transmit(_IDCANBUS_THRUSTER, 8, CAN_TxMessage);
}

bool UCAN_IsSystemReady(void)
{
	return _system_ready;
}

void UCAN_Respond_ALLData(CAN_DataTypeDef *_can_data)
{
//	//--------THRUSTER--------//
//	CAN_TxMessage[0] = ARM2_THRUSTER;
//	CAN_TxMessage[1] = STATUS_DATA;
//	CAN_TxMessage[2] = REQ_IMOTOR;
//	UCAN_Convert_Float_to_Bytes(_can_data->Thruster.iMotor, &CAN_TxMessage[3]);
//	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
//	UCAN_Transmit(_IDCANBUS_ARM_2, 8, CAN_TxMessage);
//	
//	CAN_TxMessage[0] = ARM2_THRUSTER;
//	CAN_TxMessage[1] = STATUS_DATA;
//	CAN_TxMessage[2] = REQ_TEMP_ON_CHIP;
//	UCAN_Convert_Float_to_Bytes(_can_data->Thruster.TempOnChip, &CAN_TxMessage[3]);
//	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
//	UCAN_Transmit(_IDCANBUS_ARM_2, 8, CAN_TxMessage);
//	
//	CAN_TxMessage[0] = ARM2_THRUSTER;
//	CAN_TxMessage[1] = STATUS_DATA;
//	CAN_TxMessage[2] = REQ_TEMP_ON_CHIP;
//	UCAN_Convert_Float_to_Bytes(_can_data->Thruster.TempOnLM35, &CAN_TxMessage[3]);
//	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
//	UCAN_Transmit(_IDCANBUS_ARM_2, 8, CAN_TxMessage);
//	
//	CAN_TxMessage[0] = ARM2_THRUSTER;
//	CAN_TxMessage[1] = STATUS_DATA;
//	CAN_TxMessage[2] = REQ_DUTY;
//	UCAN_Convert_Float_to_Bytes(_can_data->Thruster.DutyCycle, &CAN_TxMessage[3]);
//	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
//	UCAN_Transmit(_IDCANBUS_ARM_2, 8, CAN_TxMessage);
//	
//	CAN_TxMessage[0] = ARM2_THRUSTER;
//	CAN_TxMessage[1] = STATUS_DATA;
//	CAN_TxMessage[2] = REQ_DESIREDSPEED;
//	UCAN_Convert_Float_to_Bytes(_can_data->Thruster.DesiredSpeed, &CAN_TxMessage[3]);
//	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
//	UCAN_Transmit(_IDCANBUS_ARM_2, 8, CAN_TxMessage);
//	
//	CAN_TxMessage[0] = ARM2_THRUSTER;
//	CAN_TxMessage[1] = STATUS_DATA;
//	CAN_TxMessage[2] = REQ_AUTUALSPEED;
//	UCAN_Convert_Float_to_Bytes(_can_data->Thruster.ActualSpeed, &CAN_TxMessage[3]);
//	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
//	UCAN_Transmit(_IDCANBUS_ARM_2, 8, CAN_TxMessage);
//	
//	//--------BMS24V40AH--------//
//	CAN_TxMessage[0] = ARM2_BMS24V40AH;
//	CAN_TxMessage[1] = STATUS_DATA;
//	CAN_TxMessage[2] = HOURS;
//	UCAN_Convert_Float_to_Bytes(_can_data->BMS40_Status.Hours, &CAN_TxMessage[3]);
//	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
//	UCAN_Transmit(_IDCANBUS_ARM_2, 8, CAN_TxMessage);
//	
//	CAN_TxMessage[0] = ARM2_BMS24V40AH;
//	CAN_TxMessage[1] = STATUS_DATA;
//	CAN_TxMessage[2] = MINUTES;
//	UCAN_Convert_Float_to_Bytes(_can_data->BMS40_Status.Minutes, &CAN_TxMessage[3]);
//	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
//	UCAN_Transmit(_IDCANBUS_ARM_2, 8, CAN_TxMessage);
//	
//	CAN_TxMessage[0] = ARM2_BMS24V40AH;
//	CAN_TxMessage[1] = STATUS_DATA;
//	CAN_TxMessage[2] = SECONDS;
//	UCAN_Convert_Float_to_Bytes(_can_data->BMS40_Status.Seconds, &CAN_TxMessage[3]);
//	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
//	UCAN_Transmit(_IDCANBUS_ARM_2, 8, CAN_TxMessage);
//	
//	CAN_TxMessage[0] = ARM2_BMS24V40AH;
//	CAN_TxMessage[1] = STATUS_DATA;
//	CAN_TxMessage[2] = BATTERY_TOTAL;
//	UCAN_Convert_Float_to_Bytes(_can_data->BMS40_Status.BatteryTotal, &CAN_TxMessage[3]);
//	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
//	UCAN_Transmit(_IDCANBUS_ARM_2, 8, CAN_TxMessage);
//	
//	CAN_TxMessage[0] = ARM2_BMS24V40AH;
//	CAN_TxMessage[1] = STATUS_DATA;
//	CAN_TxMessage[2] = BATTERY_CAPACITY;
//	UCAN_Convert_Float_to_Bytes(_can_data->BMS40_Status.BatteryCapacity, &CAN_TxMessage[3]);
//	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
//	UCAN_Transmit(_IDCANBUS_ARM_2, 8, CAN_TxMessage);
//	
//	CAN_TxMessage[0] = ARM2_BMS24V40AH;
//	CAN_TxMessage[1] = STATUS_DATA;
//	CAN_TxMessage[2] = BATTERY_USED;
//	UCAN_Convert_Float_to_Bytes(_can_data->BMS40_Status.BatteryUsed, &CAN_TxMessage[3]);
//	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
//	UCAN_Transmit(_IDCANBUS_ARM_2, 8, CAN_TxMessage);
//	
//	CAN_TxMessage[0] = ARM2_BMS24V40AH;
//	CAN_TxMessage[1] = STATUS_DATA;
//	CAN_TxMessage[2] = BATTERY_PERCENTAGE;
//	UCAN_Convert_Float_to_Bytes(_can_data->BMS40_Status.BatteryPercentage, &CAN_TxMessage[3]);
//	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
//	UCAN_Transmit(_IDCANBUS_ARM_2, 8, CAN_TxMessage);
//	
//	CAN_TxMessage[0] = ARM2_BMS24V40AH;
//	CAN_TxMessage[1] = STATUS_DATA;
//	CAN_TxMessage[2] = BATTERY_CURRENT;
//	UCAN_Convert_Float_to_Bytes(_can_data->BMS40_Status.BatteryCurrent, &CAN_TxMessage[3]);
//	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
//	UCAN_Transmit(_IDCANBUS_ARM_2, 8, CAN_TxMessage);
//	
//	CAN_TxMessage[0] = ARM2_BMS24V40AH;
//	CAN_TxMessage[1] = STATUS_DATA;
//	CAN_TxMessage[2] = BATTERY_VOLTAGE;
//	UCAN_Convert_Float_to_Bytes(_can_data->BMS40_Status.BaterryVoltage, &CAN_TxMessage[3]);
//	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
//	UCAN_Transmit(_IDCANBUS_ARM_2, 8, CAN_TxMessage);
//	
//	//--------LEAK SENSORS--------//
//	CAN_TxMessage[0] = ARM2_LEAK_SENSOR;
//	CAN_TxMessage[1] = STATUS_DATA;
//	CAN_TxMessage[2] = LEAK_POSITION;
//	UCAN_Convert_Float_to_Bytes(_can_data->Leak_Status, &CAN_TxMessage[3]);
//	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
//	UCAN_Transmit(_IDCANBUS_ARM_2, 8, CAN_TxMessage);
//	
//	//--------MX28 RUDDER--------//
//	CAN_TxMessage[0] = ARM2_RUDDER;
//	CAN_TxMessage[1] = STATUS_DATA;
//	CAN_TxMessage[2] = MX28_PRESENT_POSITION;
//	UCAN_Convert_Float_to_Bytes(_can_data->MX28_Status.Position, &CAN_TxMessage[3]);
//	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
//	UCAN_Transmit(_IDCANBUS_ARM_2, 8, CAN_TxMessage);
//	
//	CAN_TxMessage[0] = ARM2_RUDDER;
//	CAN_TxMessage[1] = STATUS_DATA;
//	CAN_TxMessage[2] = MX28_PRESENT_SPEED;
//	UCAN_Convert_Float_to_Bytes(_can_data->MX28_Status.Speed, &CAN_TxMessage[3]);
//	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
//	UCAN_Transmit(_IDCANBUS_ARM_2, 8, CAN_TxMessage);
//	
//	CAN_TxMessage[0] = ARM2_RUDDER;
//	CAN_TxMessage[1] = STATUS_DATA;
//	CAN_TxMessage[2] = MX28_PRESENT_LOAD;
//	UCAN_Convert_Float_to_Bytes(_can_data->MX28_Status.Load, &CAN_TxMessage[3]);
//	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
//	UCAN_Transmit(_IDCANBUS_ARM_2, 8, CAN_TxMessage);
//	
//	CAN_TxMessage[0] = ARM2_RUDDER;
//	CAN_TxMessage[1] = STATUS_DATA;
//	CAN_TxMessage[2] = MX28_PRESENT_VOL;
//	UCAN_Convert_Float_to_Bytes(_can_data->MX28_Status.Voltage, &CAN_TxMessage[3]);
//	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
//	UCAN_Transmit(_IDCANBUS_ARM_2, 8, CAN_TxMessage);
//	
//	CAN_TxMessage[0] = ARM2_RUDDER;
//	CAN_TxMessage[1] = STATUS_DATA;
//	CAN_TxMessage[2] = MX28_PRESENT_TEMP;
//	UCAN_Convert_Float_to_Bytes(_can_data->MX28_Status.Temperature, &CAN_TxMessage[3]);
//	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
//	UCAN_Transmit(_IDCANBUS_ARM_2, 8, CAN_TxMessage);
	
		//--------KELLER PA3--------//
	CAN_TxMessage[0] = ARM2_PRESSURE;
	CAN_TxMessage[1] = STATUS_DATA;
	CAN_TxMessage[2] = DEPTH_DATA;
	UCAN_Convert_Float_to_Bytes(_can_data->KellerPA3_Status.Pressure, &CAN_TxMessage[3]);
	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
	UCAN_Transmit(_IDCANBUS_ARM_2, 8, CAN_TxMessage);
	UDELAY_us(10);
	
	CAN_TxMessage[0] = ARM2_PRESSURE;
	CAN_TxMessage[1] = STATUS_DATA;
	CAN_TxMessage[2] = TEMP_DATA;
	UCAN_Convert_Float_to_Bytes(_can_data->KellerPA3_Status.Temperature, &CAN_TxMessage[3]);
	CAN_TxMessage[7] = UCAN_Checksum(CAN_TxMessage);	
	UCAN_Transmit(_IDCANBUS_ARM_2, 8, CAN_TxMessage);
	
}

bool UCAN_NeedRespondData(void)
{
	return _need_respond_data;
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
	
	if((RxMessage.StdId == _IDCANBUS_ARM_2)&&(RxMessage.IDE == CAN_ID_STD)&&(RxMessage.DLC == 8)&&(UCAN_Checksum(RxMessage.Data) == RxMessage.Data[7]))
	{
		
		if((RxMessage.Data[0] == 'A') && (RxMessage.Data[1] == 'R') 
				&& (RxMessage.Data[2] == 'M') && (RxMessage.Data[3] == '2')
				&& (RxMessage.Data[4] == 'C') && (RxMessage.Data[5] == 'H')
				&& (RxMessage.Data[6] == 'E'))
		{
			_system_ready = true;
			UCAN_SystemReady();
		}
		
		if((RxMessage.Data[0] == 'R') && (RxMessage.Data[1] == 'E') 
				&& (RxMessage.Data[2] == 'Q') && (RxMessage.Data[3] == 'A')
				&& (RxMessage.Data[4] == 'L') && (RxMessage.Data[5] == 'L')
				&& (RxMessage.Data[6] == 0x0A))
		{
			_need_respond_data = true;
		}
		
		switch(RxMessage.Data[1])
		{
			case WRITE_DATA:
			{
				if((RxMessage.Data[0] == ARM2_POWER_INT) && (RxMessage.Data[2] == INT_24V40AH))
				{
					UIO_Emergency24V40Ah(ENABLE);
				}
				break;
			}				
			default:
				break;
		}
	}
}	

