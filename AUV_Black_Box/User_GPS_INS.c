/* Includes ------------------------------------------------------------------*/
#include "User_GPS_INS.h"
#include "stdbool.h"

#define UNAVI_MAX_RECEIVEDDATA_LENGTH	53

/**
 * @defgroup Module Pin define
 * @{
 */
		/** 
		* @brief   COM Pin define 
		*/
			#define 	UNAVI_CLK_AFIO  				  RCC_APB2Periph_AFIO	
			#define		UNAVI_CLK_AFIO_Cmd		  	RCC_APB2PeriphClockCmd
			
			#define 	UNAVI_TX_PIN  				   	GPIO_Pin_9			
			#define 	UNAVI_TX_PORT 				  	GPIOA
			#define 	UNAVI_TX_CLK							RCC_APB2Periph_GPIOA				
			#define		UNAVI_TX_CLK_Cmd		  	 	RCC_APB2PeriphClockCmd			

			#define 	UNAVI_RX_PIN  				   	GPIO_Pin_10
			#define 	UNAVI_RX_PORT 				  	GPIOA
			#define 	UNAVI_RX_CLK							RCC_APB2Periph_GPIOA
			#define		UNAVI_RX_CLK_Cmd		  	 	RCC_APB2PeriphClockCmd
			
			#define		UNAVI_USART								USART1
			#define 	UNAVI_USART_CLK						RCC_APB2Periph_USART1
			#define		UNAVI_USART_CLK_Cmd		  	RCC_APB2PeriphClockCmd
			#define 	UNAVI_USART_BAUDRATE			9600
			//#define 	UNAVI_USART_BAUDRATE			460800
			#define 	UNAVI_USART_IRQn					USART1_IRQn
			#define		UNAVI_USART_PreemptionPriority		0x00
			#define		UNAVI_USART_SubPriority						0x05
			
			#define   UNAVI_DMA_Channel								DMA1_Channel5
			#define		UNAVI_DMA_CLK_Cmd		  					RCC_AHBPeriphClockCmd
			#define 	UNAVI_DMA_CLK										RCC_AHBPeriph_DMA1
			#define		UNAVI_DMA_Channel_IRQn					DMA1_Channel5_IRQn
			#define		UNAVI_DMA_PreemptionPriority		0x00
			#define		UNAVI_DMA_SubPriority						0x01
			
			#define		UNAVI_DMA_Channel_IRQHandler		DMA1_Channel5_IRQHandler			
			#define		UNAVI_UART_IRQHandler			USART1_IRQHandler
			
/**
 * @}
 */
 
/* Public variables ----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/
static uint8_t UNAVI_DMABuffer[UNAVI_MAX_RECEIVEDDATA_LENGTH], UNAVI_USARTBuffer[UNAVI_MAX_RECEIVEDDATA_LENGTH], count = 0;
static bool process_state = false, waiting_check_process = false;

//----------String Get Data----------//
static char my_com_start[] 	= "$Viam_Navi,EnableGetDataCom\r\n";
static char my_com_stop[] 	= "$Viam_Navi,DisableGetDataCom\r\n";
static char my_com_start_process[] 	= "$Viam_Navi,STARTProcess\r\n";
static char my_com_change_freq_100Hz[] = "$Viam_Navi,Change_Freq_100Hz\r\n";
static char my_com_change_freq_50Hz[] = "$Viam_Navi,Change_Freq_50Hz\r\n";
static char my_com_change_freq_25Hz[] = "$Viam_Navi,Change_Freq_25Hz\r\n";
static char my_com_change_freq_20Hz[] = "$Viam_Navi,Change_Freq_20Hz\r\n";
static char my_com_change_freq_10Hz[] = "$Viam_Navi,Change_Freq_10Hz\r\n";
static char my_com_change_freq_5Hz[] = "$Viam_Navi,Change_Freq_5Hz\r\n";
static char my_com_change_freq_4Hz[] = "$Viam_Navi,Change_Freq_4Hz\r\n";
static char my_com_change_freq_2Hz[] = "$Viam_Navi,Change_Freq_2Hz\r\n";
static char my_com_change_freq_1Hz[] = "$Viam_Navi,Change_Freq_1Hz\r\n";
static char my_com_stop_process[] 	= "$Viam_Navi,STOPProcess\r\n";
//----------Check Process----------//
static char my_com_check_process[] =  "$Viam_Navi,CheckProcess\r\n";
static char my_com_process_on[] =  "$Viam_Navi,Process_ON\r\n";
static char my_com_process_off[] =  "$Viam_Navi,Process_OFF\r\n";

union
{
	uint8_t _ByteData[UNAVI_MAX_RECEIVEDDATA_LENGTH];
	struct
	{
		uint8_t _header[10];
		UNAVI_RealOutputData_Typedef UNAVI_RealOutputData;
		uint8_t _tail[3];
	}_RealData;
}UNAVI_Data;

/* Private const/macros ------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
uint8_t UNAVI_Checksum(uint8_t *_data);
void UNAVI_ArrangeData(uint8_t *_data_in, uint8_t *_data_out);

/* Exported function body ----------------------------------------------------*/

/* Private functions body ----------------------------------------------------*/
void UNAVI_Configure(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	DMA_InitTypeDef DMA_InitStruct;
	
	UNAVI_CLK_AFIO_Cmd(UNAVI_CLK_AFIO, ENABLE);
	
	UNAVI_TX_CLK_Cmd(UNAVI_TX_CLK,ENABLE);	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = UNAVI_TX_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(UNAVI_TX_PORT,&GPIO_InitStruct);
	
	UNAVI_RX_CLK_Cmd(UNAVI_RX_CLK,ENABLE);	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStruct.GPIO_Pin = UNAVI_RX_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(UNAVI_RX_PORT,&GPIO_InitStruct);
	
	UNAVI_DMA_CLK_Cmd(UNAVI_DMA_CLK,ENABLE);
	
//	DMA_DeInit(DMA1_Channel5);
//	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)&USART1->DR;//0x40013804;
//	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)UNAVI_DMABuffer;
//	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
//	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
//	DMA_InitStruct.DMA_BufferSize = UNAVI_MAX_RECEIVEDDATA_LENGTH;

//	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
//	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
//	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
//	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
//	DMA_InitStruct.DMA_Priority = DMA_Priority_VeryHigh;
//	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
//	DMA_Init(UNAVI_DMA_Channel, &DMA_InitStruct);
//	
//	DMA_ITConfig(UNAVI_DMA_Channel,DMA_IT_TC,ENABLE);
//	
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
//	NVIC_InitStruct.NVIC_IRQChannel = UNAVI_DMA_Channel_IRQn;
//	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = UNAVI_DMA_PreemptionPriority;
//	NVIC_InitStruct.NVIC_IRQChannelSubPriority = UNAVI_DMA_SubPriority;
//	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
//	NVIC_Init(&NVIC_InitStruct);
	
	UNAVI_USART_CLK_Cmd(UNAVI_USART_CLK,ENABLE);
	USART_InitStruct.USART_BaudRate = UNAVI_USART_BAUDRATE;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(UNAVI_USART,&USART_InitStruct);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStruct.NVIC_IRQChannel = UNAVI_USART_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = UNAVI_USART_PreemptionPriority;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = UNAVI_USART_SubPriority;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	USART_ITConfig(UNAVI_USART, USART_IT_RXNE, ENABLE);
	
	USART_DMACmd(UNAVI_USART, USART_DMAReq_Rx, DISABLE); 

	DMA_Cmd(UNAVI_DMA_Channel, DISABLE);
	
	USART_Cmd(UNAVI_USART,ENABLE);
}

uint8_t UNAVI_Checksum(uint8_t *_data)
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

void UNAVI_Transmit(char *szData)
{
	char c = *szData;
	while(c)
	{
		while(USART_GetFlagStatus(UNAVI_USART,USART_FLAG_TXE) != SET);
		USART_SendData(UNAVI_USART,c);
		c = *(++szData);
	}
}

void UNAVI_ArrangeData(uint8_t *_data_in, uint8_t *_data_out)
{
	uint8_t _start = 0;
	for(int i = 0; i < UNAVI_MAX_RECEIVEDDATA_LENGTH; i++)
	{
		if((_data_in[i] == '$')&&(_data_in[i+1] == 'V')&&(_data_in[i+2] == 'i')&&(_data_in[i+3] == 'a')&&(_data_in[i+4] == 'm')) _start = i; 
	}
	for(int i = 0; i < UNAVI_MAX_RECEIVEDDATA_LENGTH; i++)
	{
		if((_start + i) < UNAVI_MAX_RECEIVEDDATA_LENGTH)
		{
			_data_out[i] = _data_in[_start + i];
		}
		else 
		{
			_data_out[i] = _data_in[i - (UNAVI_MAX_RECEIVEDDATA_LENGTH - _start)];
		}
	}	
}

void UNAVI_COMConfig(FunctionalState NewState)
{
	if(NewState)
	{
		UNAVI_Transmit(my_com_start);
	}
	else
	{
		UNAVI_Transmit(my_com_stop);
	}		
}

void UNAVI_ProcessConfig(FunctionalState NewState)
{
	if(NewState)
	{
		UNAVI_Transmit(my_com_start_process);
	}
	else
	{
		UNAVI_Transmit(my_com_stop_process);
	}		
}

FlagStatus UNAVI_FlagProcess(void)
{
	FlagStatus bitstatus = RESET;
	waiting_check_process = true;
	UNAVI_Transmit(my_com_check_process);
	while(waiting_check_process);
	bitstatus = process_state? SET:RESET;  
	return bitstatus;
}

void UNAVI_ProcessUSARTData(uint8_t *prxdata)
{
	if (strcmp((const char*)prxdata,my_com_process_on) == 0)
	{
		process_state = true;
		waiting_check_process = false;
	}
	else if (strcmp((const char*)prxdata,my_com_process_off) == 0)
	{
		process_state = false;
		waiting_check_process = false;
	}
}

/******************************************************************************/
/* Peripherals Interrupt Handlers --------------------------------------------*/
/******************************************************************************/
void UNAVI_DMA_Channel_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC5) != RESET)
  {
		UNAVI_ArrangeData(UNAVI_DMABuffer, UNAVI_Data._ByteData);
		DMA_ClearITPendingBit(DMA1_IT_GL5|DMA1_IT_TC5|DMA1_IT_HT5);
  }
}

void UNAVI_UART_IRQHandler(void)
{
	uint8_t _temp_data = 0;
	if(USART_GetITStatus(UNAVI_USART,USART_IT_RXNE) != RESET)
	{		
		_temp_data = USART_ReceiveData(UNAVI_USART);
		if((count == 0)&&(_temp_data == '$'))
		{
			UNAVI_USARTBuffer[count++] = _temp_data;
		}
		else
		{
			UNAVI_USARTBuffer[count++] = _temp_data;
		}
		
		if(_temp_data == '\n')
		{
			UNAVI_USARTBuffer[count] = 0;
			count = 0;
			UNAVI_ProcessUSARTData(UNAVI_USARTBuffer);
		}
	}
}
