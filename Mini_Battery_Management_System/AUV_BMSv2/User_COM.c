/* Includes ------------------------------------------------------------------*/
#include "User_COM.h"
#include "stdbool.h"

#define UCOM_MAX_RECEIVEDDATA_LENGTH 36
#define UCOM_MAX_TRANSMITDATA_LENGTH 36
/**
 * @defgroup Module Pin define
 * @{
 */
		/** 
		* @brief   COM Pin define 
		*/
			#define 	UCOM_CLK_AFIO  				  				RCC_APB2Periph_AFIO	
			#define		UCOM_CLK_AFIO_Cmd		  					RCC_APB2PeriphClockCmd
			
			#define 	UCOM_TX_PIN  				   					GPIO_Pin_9			
			#define 	UCOM_TX_PORT 				  					GPIOA
			#define 	UCOM_TX_CLK											RCC_APB2Periph_GPIOA				
			#define		UCOM_TX_CLK_Cmd		  	 					RCC_APB2PeriphClockCmd			

			#define 	UCOM_RX_PIN  				 				  	GPIO_Pin_10
			#define 	UCOM_RX_PORT 				  					GPIOA
			#define 	UCOM_RX_CLK											RCC_APB2Periph_GPIOA
			#define		UCOM_RX_CLK_Cmd		  	 					RCC_APB2PeriphClockCmd
			
			#define		UCOM_USART											USART1
			#define 	UCOM_USART_CLK									RCC_APB2Periph_USART1
			#define		UCOM_USART_CLK_Cmd		  				RCC_APB2PeriphClockCmd
			#define 	UCOM_USART_BAUDRATE							115200
			#define 	UCOM_USART_IRQn									USART1_IRQn
			#define		UCOM_USART_PreemptionPriority		0x00
			#define		UCOM_USART_SubPriority					0x03
			
			#define		UCOM_UART_IRQHandler						USART1_IRQHandler
			
/**
 * @}
 */

/* Public variables ----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/
static char UCOM_USARTBuffer[UCOM_MAX_RECEIVEDDATA_LENGTH];
			 char UCOM_USARTBuffer_dis[36], count = 0;
static uint8_t UCOM_TransmitDataBuffer[UCOM_MAX_TRANSMITDATA_LENGTH] = {'$', 'A', 'U', 'V', '2', '0', '0', '0',
																																			//	0, 0, 0,	// hour, minute, second
																																				0, 0, 0, 0,    0, 0, 0, 0,    0, 0, 0, 0, // BatteryTotal, BatteryCapacity, BatteryUsed, 
																																				0, 0, 0, 0,    0, 0, 0, 0,    0, 0, 0, 0, // BatteryPercentage, BatteryCurrent, BaterryVoltage
																																				0, 0,'\r', '\n'};

uint8_t buffer_dis[36];
uint8_t buffer_dis2[36];
uint8_t ComData_BatteryTotal[4];
uint8_t ComData_BatteryCapacity[4];
uint8_t ComData_BatteryUsed[4];
uint8_t ComData_BatteryPercentage[4];
uint8_t ComData_BatteryCurrent[4];
uint8_t ComData_BaterryVoltage[4];
																	
uint8_t ComData_BatteryTotal_Init[4];
uint32_t bt;			
int dem_xx;
int dem=0;																																				
/* Private const/macros ------------------------------------------------------*/
	
/* Private function prototypes -----------------------------------------------*/
uint8_t UCOM_Checksum(uint8_t *_data);
bool start_flag=false;
/* Exported function body ----------------------------------------------------*/
bool UCOM_Get_Flag(void)
{
	return start_flag;
}
static void UCOM_Convert_Float_to_Bytes(float _data_in, uint8_t* _data_out)
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

static float UCOM_Convert_Bytes_to_Float(uint8_t* _data_in)
{
	union
	{
		float _value;
		uint8_t _byte[4];
	}_part;
	
	_part._byte[0]=_data_in[0] ;
  _part._byte[1]=_data_in[1] ;
	_part._byte[2]=_data_in[2] ;
	_part._byte[3]=_data_in[3] ;
	
	return _part._value; 
}

/* Private functions body ----------------------------------------------------*/
void UCOM_Configure(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	UCOM_CLK_AFIO_Cmd(UCOM_CLK_AFIO, ENABLE);
	
	UCOM_TX_CLK_Cmd(UCOM_TX_CLK,ENABLE);	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = UCOM_TX_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(UCOM_TX_PORT,&GPIO_InitStruct);
	
	UCOM_RX_CLK_Cmd(UCOM_RX_CLK,ENABLE);	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_InitStruct.GPIO_Pin = UCOM_RX_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(UCOM_RX_PORT,&GPIO_InitStruct);
	
	UCOM_USART_CLK_Cmd(UCOM_USART_CLK,ENABLE);
	USART_InitStruct.USART_BaudRate = UCOM_USART_BAUDRATE;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(UCOM_USART,&USART_InitStruct);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStruct.NVIC_IRQChannel = UCOM_USART_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = UCOM_USART_PreemptionPriority;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = UCOM_USART_SubPriority;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	USART_ITConfig(UCOM_USART, USART_IT_RXNE, ENABLE);
		
	USART_Cmd(UCOM_USART,ENABLE);
	

}

uint8_t UCOM_Checksum(uint8_t *_data)
{
	uint8_t value = 0;

	//Calculate CheckSum (Byte)
	for (int i = 0; i < UCOM_MAX_TRANSMITDATA_LENGTH-4; i++)
	{
		value += _data[i];
	}
	value = ~value;
	value++;
	return (uint8_t)value;
}

void UCOM_Transmit(uint8_t *szData)
{
	for(int index= 0 ; index < 36; index ++)
	{
		while(USART_GetFlagStatus(UCOM_USART,USART_FLAG_TXE) != SET);
		USART_SendData(UCOM_USART,*(szData+index));
		dem++;
	}
}

void UCOM_RequiredConfigBaterry(void)
{	
	UCOM_TransmitDataBuffer[8]  = 'C';
	UCOM_TransmitDataBuffer[9]  = 'O';
	UCOM_TransmitDataBuffer[10] = 'N';
	UCOM_TransmitDataBuffer[11] = 'F';
	UCOM_TransmitDataBuffer[12] = 'I';
	UCOM_TransmitDataBuffer[13] = 'G';
	UCOM_TransmitDataBuffer[35] = UCOM_Checksum(UCOM_TransmitDataBuffer);
	for(int nm = 0 ; nm < 38; nm ++)
	{
			buffer_dis2[nm]=UCOM_TransmitDataBuffer[nm];
	}	
	UCOM_Transmit(UCOM_TransmitDataBuffer);
}

void UCOM_SendStatusBaterry(URTC_Time_Typedef* _urtc_time, UBMS_Status_Typedef* _ubms_status)
{
//	UCOM_TransmitDataBuffer[8] =_urtc_time->hours;
//	UCOM_TransmitDataBuffer[9] =_urtc_time->minutes;
//	UCOM_TransmitDataBuffer[10] =_urtc_time->seconds;
	
	UCOM_Convert_Float_to_Bytes(_ubms_status->BatteryTotal, ComData_BatteryTotal);
	UCOM_Convert_Float_to_Bytes(_ubms_status->BatteryCapacity,ComData_BatteryCapacity);
	UCOM_Convert_Float_to_Bytes(_ubms_status->BatteryUsed,ComData_BatteryUsed);
	UCOM_Convert_Float_to_Bytes(_ubms_status->BatteryPercentage,ComData_BatteryPercentage);
	UCOM_Convert_Float_to_Bytes(_ubms_status->BatteryCurrent,ComData_BatteryCurrent);
	UCOM_Convert_Float_to_Bytes(_ubms_status->BaterryVoltage,ComData_BaterryVoltage);

	for(int ComIndex = 0 ; ComIndex < 4 ; ComIndex ++)
	{
		  UCOM_TransmitDataBuffer[ComIndex + 8] = ComData_BatteryTotal[ComIndex];
		  UCOM_TransmitDataBuffer[ComIndex + 12] = ComData_BatteryCapacity[ComIndex];
		  UCOM_TransmitDataBuffer[ComIndex + 16] = ComData_BatteryUsed[ComIndex];
		  UCOM_TransmitDataBuffer[ComIndex + 20] = ComData_BatteryPercentage[ComIndex];
		  UCOM_TransmitDataBuffer[ComIndex + 24] = ComData_BatteryCurrent[ComIndex];
		  UCOM_TransmitDataBuffer[ComIndex + 28] = ComData_BaterryVoltage[ComIndex];
	}
		
	UCOM_TransmitDataBuffer[32] = UCOM_Checksum(UCOM_TransmitDataBuffer);
	UCOM_Transmit(UCOM_TransmitDataBuffer);
	
//	for(int nm =0 ; nm < 34; nm ++)
//	{
//		buffer_dis[nm]=UCOM_TransmitDataBuffer[nm];
//	}
//	for(int i = 0; i < 36; i++)
//	{
//		UCOM_TransmitDataBuffer[i] = i;
//	}
//	UCOM_Transmit(UCOM_TransmitDataBuffer);
}

void UCOM_ProcessUSARTData(char *prxdata)
{
	if((prxdata[0] == '$') && (prxdata[1] == 'A') && (prxdata[2] == 'U') && (prxdata[3] == 'V') 
		&& (prxdata[4] == '2') && (prxdata[5] == '0') && (prxdata[6] == '0') && (prxdata[7] == '0'))
	{
		if((prxdata[8] == 'S') && (prxdata[9] == 'T') && (prxdata[10] == 'A') && (prxdata[11] == 'R') && (prxdata[12] == 'T'))
		{
			start_flag = true;
		}
			
		if((prxdata[8] == 'S') && (prxdata[9] == 'T') && (prxdata[10] == 'O') && (prxdata[11] == 'P'))
		{
			start_flag = false;
		}
	}
}

/******************************************************************************/
/* Peripherals Interrupt Handlers --------------------------------------------*/
/******************************************************************************/
void UCOM_UART_IRQHandler(void)
{
	if(USART_GetITStatus(UCOM_USART, USART_IT_RXNE) != RESET)
  {	
		UCOM_USARTBuffer[count] = USART_ReceiveData(UCOM_USART);
		if ((UCOM_USARTBuffer[count]=='\n')&&(UCOM_USARTBuffer[count-1]=='\r')) 
		{
			count = 0;
			UCOM_ProcessUSARTData(UCOM_USARTBuffer);
		
			for(int k = 0 ; k < 15; k++)
			{
				UCOM_USARTBuffer[k]=0;
			}
		}
		else if(UCOM_USARTBuffer[0] =='$') 
		{
			count++;
		}
	}
	USART_ClearITPendingBit(UCOM_USART, USART_IT_RXNE);
	
}


