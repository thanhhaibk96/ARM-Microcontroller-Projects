/* Includes ------------------------------------------------------------------*/
#include "User_XBEE.h"
#include "stdbool.h"

/**
 * @defgroup Module Pin define
 * @{
 */
		/** 
		* @brief   COM Pin define 
		*/
			#define 	UXBEE_CLK_AFIO  				  RCC_APB2Periph_AFIO	
			#define		UXBEE_CLK_AFIO_Cmd		  	RCC_APB2PeriphClockCmd
			
			#define 	UXBEE_TX_PIN  				   	GPIO_Pin_2			
			#define 	UXBEE_TX_PORT 				  	GPIOA
			#define 	UXBEE_TX_CLK							RCC_APB2Periph_GPIOA				
			#define		UXBEE_TX_CLK_Cmd		  	 	RCC_APB2PeriphClockCmd			

			#define 	UXBEE_RX_PIN  				   	GPIO_Pin_3
			#define 	UXBEE_RX_PORT 				  	GPIOA
			#define 	UXBEE_RX_CLK							RCC_APB2Periph_GPIOA
			#define		UXBEE_RX_CLK_Cmd		  	 	RCC_APB2PeriphClockCmd
			
			#define		UXBEE_USART								USART2
			#define 	UXBEE_USART_CLK						RCC_APB1Periph_USART2
			#define		UXBEE_USART_CLK_Cmd		  	RCC_APB1PeriphClockCmd
			#define 	UXBEE_USART_BAUDRATE			115200
			#define 	UXBEE_USART_IRQn					USART2_IRQn
			#define		UXBEE_USART_PreemptionPriority		0x00
			#define		UXBEE_USART_SubPriority					0x03
			
			#define		UXBEE_UART_IRQHandler			USART2_IRQHandler
			
/**
 * @}
 */
 
/* Public variables ----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private const/macros ------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
uint8_t UXBEE_Checksum(uint8_t *_data);
void UXBEE_Println(const char *szData);
void UXBEE_Print(const char *szData);

/* Exported function body ----------------------------------------------------*/

/* Private functions body ----------------------------------------------------*/
void UXBEE_Configure(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	UXBEE_CLK_AFIO_Cmd(UXBEE_CLK_AFIO, ENABLE);
	
	UXBEE_TX_CLK_Cmd(UXBEE_TX_CLK,ENABLE);	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = UXBEE_TX_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(UXBEE_TX_PORT,&GPIO_InitStruct);
	
	UXBEE_RX_CLK_Cmd(UXBEE_RX_CLK,ENABLE);	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStruct.GPIO_Pin = UXBEE_RX_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(UXBEE_RX_PORT,&GPIO_InitStruct);
	
	UXBEE_USART_CLK_Cmd(UXBEE_USART_CLK,ENABLE);
	USART_InitStruct.USART_BaudRate = UXBEE_USART_BAUDRATE;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(UXBEE_USART,&USART_InitStruct);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStruct.NVIC_IRQChannel = UXBEE_USART_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = UXBEE_USART_PreemptionPriority;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = UXBEE_USART_SubPriority;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	USART_ITConfig(UXBEE_USART, USART_IT_RXNE, ENABLE);
		
	USART_Cmd(UXBEE_USART,ENABLE);
}

uint8_t UXBEE_Checksum(uint8_t *_data)
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

void UXBEE_Transmit(char *szData)
{
	char c = *szData;
	while(c)
	{
		while(USART_GetFlagStatus(UXBEE_USART,USART_FLAG_TXE) != SET);
		USART_SendData(UXBEE_USART,c);
		c = *(++szData);
	}
}

/******************************************************************************/
/* Peripherals Interrupt Handlers --------------------------------------------*/
/******************************************************************************/
void UXBEE_UART_IRQHandler(void)
{
	if(USART_GetITStatus(UXBEE_USART, USART_IT_RXNE) != RESET)
  {
		
		USART_ClearITPendingBit(UXBEE_USART,USART_IT_RXNE);
  }
}
