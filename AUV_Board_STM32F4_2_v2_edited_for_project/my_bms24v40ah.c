/* Includes ------------------------------------------------------------------*/
#include "my_bms24v40ah.h"

#define UBMS40_BUFFER_LENGTH			38
/* Public variables ----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/
static uint8_t UBMS40_DMABuffer[UBMS40_BUFFER_LENGTH], UBMS40_DataBuffer[UBMS40_BUFFER_LENGTH];

/* Private const/macros ------------------------------------------------------*/

/**
 * @defgroup Module Pin define
 * @{
 */
		/** 
		* @brief   BMS40 Pin define 
		*/
			#define 	UBMS40_TX_PIN  				   	GPIO_Pin_10			
			#define 	UBMS40_TX_PORT 				  	GPIOC
			#define 	UBMS40_TX_CLK							RCC_AHB1Periph_GPIOC			
			#define 	UBMS40_TX_SOURCE				 	GPIO_PinSource10				
			#define		UBMS40_TX_CLK_Cmd		  	 	RCC_AHB1PeriphClockCmd			

			#define 	UBMS40_RX_PIN  				   	GPIO_Pin_11
			#define 	UBMS40_RX_PORT 				  	GPIOC
			#define 	UBMS40_RX_CLK							RCC_AHB1Periph_GPIOC
			#define 	UBMS40_RX_SOURCE				 	GPIO_PinSource11
			#define		UBMS40_RX_CLK_Cmd		  	 	RCC_AHB1PeriphClockCmd
			
			#define		UBMS40_USART							UART4
			#define 	UBMS40_USART_CLK					RCC_APB1Periph_UART4
			#define		UBMS40_USART_CLK_Cmd		  RCC_APB1PeriphClockCmd
			#define 	UBMS40_USART_BAUDRATE			115200
			#define		UBMS40_USART_AF						GPIO_AF_UART4
			#define 	UBMS40_USART_IRQn					UART4_IRQn
			
			#define   UBMS40_DMA_Channel				DMA_Channel_4
			#define   UBMS40_DMA_Stream					DMA1_Stream2
			#define		UBMS40_DMA_CLK_Cmd		  	RCC_AHB1PeriphClockCmd
			#define 	UBMS40_DMA_CLK						RCC_AHB1Periph_DMA1
			#define		UBMS40_DMA_Stream_IRQn		DMA1_Stream2_IRQn
			
			#define		UBMS40_DMA_Stream_IRQHandler		DMA1_Stream2_IRQHandler
			#define		UBMS40_USART_IRQHandler					UART4_IRQHandler
			

/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Exported function body ----------------------------------------------------*/
void UBMS40_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	DMA_InitTypeDef DMA_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	UBMS40_TX_CLK_Cmd(UBMS40_TX_CLK,ENABLE);	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = UBMS40_TX_PIN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	GPIO_Init(UBMS40_TX_PORT,&GPIO_InitStruct);
	
	UBMS40_RX_CLK_Cmd(UBMS40_RX_CLK,ENABLE);	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = UBMS40_RX_PIN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	GPIO_Init(UBMS40_RX_PORT,&GPIO_InitStruct);
	
	GPIO_PinAFConfig(UBMS40_TX_PORT,UBMS40_TX_SOURCE,UBMS40_USART_AF);
	GPIO_PinAFConfig(UBMS40_RX_PORT,UBMS40_RX_SOURCE,UBMS40_USART_AF);
	
	UBMS40_USART_CLK_Cmd(UBMS40_USART_CLK,ENABLE);
	USART_InitStruct.USART_BaudRate = UBMS40_USART_BAUDRATE;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(UBMS40_USART,&USART_InitStruct);
	
	USART_DMACmd(UBMS40_USART,USART_DMAReq_Rx,ENABLE);
	
	UBMS40_DMA_CLK_Cmd(UBMS40_DMA_CLK,ENABLE);	
	DMA_InitStruct.DMA_Channel = UBMS40_DMA_Channel;
	DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t)(&(UBMS40_DMABuffer));
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)(&(UBMS40_USART->DR));
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralToMemory;
	DMA_InitStruct.DMA_BufferSize = UBMS40_BUFFER_LENGTH;																	
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;
	DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Disable;
	DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
	DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;
	DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;

	DMA_Init(UBMS40_DMA_Stream, &DMA_InitStruct);

	DMA_Cmd(UBMS40_DMA_Stream, ENABLE);	

	/* DMA Transfer complete interrupt configure */
	DMA_ITConfig(UBMS40_DMA_Stream, DMA_IT_TC, ENABLE);

	NVIC_InitStruct.NVIC_IRQChannel = UBMS40_DMA_Stream_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 0x00;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0x01;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	USART_Cmd(UBMS40_USART,ENABLE);

}

void UBMS40_Transmit(char *szData)
{
	char c = *szData;
	while(c)
	{
		while(USART_GetFlagStatus(UBMS40_USART,USART_FLAG_TXE) != SET);
		USART_SendData(UBMS40_USART,c);
		c = *(++szData);
	}
}

/* Private functions body ----------------------------------------------------*/

/******************************************************************************/
/* Peripherals Interrupt Handlers --------------------------------------------*/
/******************************************************************************/
void UBMS40_DMA_Stream_IRQHandler(void)
{	
	if(DMA_GetITStatus(UBMS40_DMA_Stream, DMA_IT_TCIF2) != RESET)
	{	
		
		DMA_ClearITPendingBit(UBMS40_DMA_Stream,DMA_IT_DMEIF2|DMA_IT_FEIF2|DMA_IT_HTIF2|DMA_IT_TCIF2|DMA_IT_TEIF2);
	}
}

