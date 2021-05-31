/* Includes ------------------------------------------------------------------*/
#include "User_GSM.h"
#include "stdbool.h"

/**
 * @defgroup Module Pin define
 * @{
 */
		/** 
		* @brief   COM Pin define 
		*/
			#define 	UGSM_CLK_AFIO  				  RCC_APB2Periph_AFIO	
			#define		UGSM_CLK_AFIO_Cmd		  	RCC_APB2PeriphClockCmd
			
			#define 	UGSM_TX_PIN  				   	GPIO_Pin_10			
			#define 	UGSM_TX_PORT 				  	GPIOB
			#define 	UGSM_TX_CLK							RCC_APB2Periph_GPIOB						
			#define		UGSM_TX_CLK_Cmd		  	 	RCC_APB2PeriphClockCmd			

			#define 	UGSM_RX_PIN  				   	GPIO_Pin_11
			#define 	UGSM_RX_PORT 				  	GPIOB
			#define 	UGSM_RX_CLK							RCC_APB2Periph_GPIOB
			#define		UGSM_RX_CLK_Cmd		  	 	RCC_APB2PeriphClockCmd
			
			#define		UGSM_USART							USART3
			#define 	UGSM_USART_CLK					RCC_APB1Periph_USART3
			#define		UGSM_USART_CLK_Cmd		  RCC_APB1PeriphClockCmd
			#define 	UGSM_USART_BAUDRATE			9600
			#define 	UGSM_USART_IRQn					USART3_IRQn
			#define		UGSM_USART_PreemptionPriority		0x00
			#define		UGSM_USART_SubPriority					0x04
			
			#define		UGSM_UART_IRQHandler		USART3_IRQHandler
			
/**
 * @}
 */
 
/* Public variables ----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private const/macros ------------------------------------------------------*/
static UGSM_FlagStatus _flag_check = _ERROR;
static int PING_DONE = 0;

/* Private variables ---------------------------------------------------------*/
uint8_t RxCounter; 
uint8_t NbrOfDataToRead;
uint8_t RxBuffer[10];
/* Private function prototypes -----------------------------------------------*/
uint8_t UGSM_Checksum(uint8_t *_data);
void UGSM_Transmit(char *szData);
void UGSM_ProcessData(char *szData);
/* Exported function body ----------------------------------------------------*/

/* Private functions body ----------------------------------------------------*/
void UGSM_Configure(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	UGSM_CLK_AFIO_Cmd(UGSM_CLK_AFIO, ENABLE);
	
	UGSM_TX_CLK_Cmd(UGSM_TX_CLK,ENABLE);	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = UGSM_TX_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(UGSM_TX_PORT,&GPIO_InitStruct);
	
	UGSM_RX_CLK_Cmd(UGSM_RX_CLK,ENABLE);	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
	GPIO_InitStruct.GPIO_Pin = UGSM_RX_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(UGSM_RX_PORT,&GPIO_InitStruct);
	
	UGSM_USART_CLK_Cmd(UGSM_USART_CLK,ENABLE);
	USART_InitStruct.USART_BaudRate = UGSM_USART_BAUDRATE;
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStruct.USART_Mode = USART_Mode_Rx|USART_Mode_Tx;
	USART_InitStruct.USART_Parity = USART_Parity_No;
	USART_InitStruct.USART_StopBits = USART_StopBits_1;
	USART_InitStruct.USART_WordLength = USART_WordLength_8b;
	USART_Init(UGSM_USART,&USART_InitStruct);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStruct.NVIC_IRQChannel = UGSM_USART_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = UGSM_USART_PreemptionPriority;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = UGSM_USART_SubPriority;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	USART_ITConfig(UGSM_USART, USART_IT_IDLE, DISABLE);
	USART_ITConfig(UGSM_USART, USART_IT_RXNE, DISABLE);
	
	USART_Cmd(UGSM_USART,ENABLE);
}

uint8_t UGSM_Checksum(uint8_t *_data)
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

void UGSM_Transmit(char *szData)
{
	char c = *szData;
	while(c)
	{
		while(USART_GetFlagStatus(UGSM_USART,USART_FLAG_TXE) != SET);
		USART_SendData(UGSM_USART,c);
		c = *(++szData);
	}
}

void UGSM_InitGSM(int _baudrate)
{
	char _strtemp[20];
	
	UGSM_Transmit("ATE1\r\n");                     
  UDELAY_ms(2000);
  
	sprintf(_strtemp, "AT+IPR=%d\r\n", _baudrate);
	
	UGSM_Transmit(_strtemp);              // Dat toc do truyen nhan du lieu 9600 bps
  UDELAY_ms(2000);
  UGSM_Transmit("AT+CMGF=1\r\n");                // Chon che do TEXT Mode
  UDELAY_ms(2000);
  UGSM_Transmit("AT+CLIP=1\r\n");                // Hien thi thong tin nguoi goi den
  UDELAY_ms(2000);
  UGSM_Transmit("AT+CNMI=2,2\r\n");              // Hien thi truc tiep noi dung tin nhan
  UDELAY_ms(2000);
	
	USART_Cmd(UGSM_USART,DISABLE);
	USART_ITConfig(UGSM_USART, USART_IT_IDLE, ENABLE);
	USART_ITConfig(UGSM_USART, USART_IT_RXNE, ENABLE);
	USART_Cmd(UGSM_USART,ENABLE);
}

void UGSM_MakeSMS(char *_number, char *szData)
{
	char temp_str_head[] = "AT+CMGS=\"";
	char temp_str_tail[] = "\"\r";
	char temp_str[300] = "";
	char ctrl_Z = 0x1A;
	strcpy(temp_str, temp_str_head);
	strcat(temp_str, _number);
	strcat(temp_str, temp_str_tail);
	UGSM_Transmit(temp_str);     															// Lenh gui tin nhan
  UDELAY_ms(5000);                                       // Cho ky tu '>' phan hoi ve 
  
	UGSM_Transmit(szData);                             // Gui noi dung
  while(USART_GetFlagStatus(UGSM_USART,USART_FLAG_TXE) != SET);
	USART_SendData(UGSM_USART,(char)ctrl_Z);                            // Gui Ctrl+Z hay 26 de ket thuc noi dung tin nhan va gui tin di
  UDELAY_ms(5000);                                       // delay 5s
}

UGSM_FlagStatus UGSM_Ping(void)
{
	char _strtemp[10];
	uint8_t i = 0;
	int time_out = 20000;
	UGSM_FlagStatus _flag_check;
	UGSM_Transmit("AT\r\n");
	while((PING_DONE != 1)||(time_out--));
	if(time_out != 0) _flag_check = _OK;
	else _flag_check = _ERROR;
	return _flag_check;
}

void UGSM_MakeCall(char *_number)
{
	char temp_str_head[] = "ATD";
	char temp_str_tail[] = ";\r\n";
	char temp_str[300] = "";
	
	strcpy(temp_str, temp_str_head);
	strcat(temp_str, _number);
	strcat(temp_str, temp_str_tail);
	UGSM_Transmit(temp_str);     															        
}

/******************************************************************************/
/* Peripherals Interrupt Handlers --------------------------------------------*/
/******************************************************************************/
void UGSM_UART_IRQHandler(void)
{
	if(USART_GetITStatus(UGSM_USART, USART_IT_RXNE) != RESET)
  {
    /* Read one byte from the receive data register */
    RxBuffer[RxCounter++] = USART_ReceiveData(UGSM_USART);
  }
	if(USART_GetITStatus(UGSM_USART, USART_IT_IDLE) != RESET)
  {
		RxCounter = 0;
		if(strcmp((const char*)RxBuffer, "AT\r\r\nOK\r\n") == 0) 
		{
			PING_DONE = 1;
			USART_Cmd(UGSM_USART,DISABLE);
			USART_ITConfig(UGSM_USART, USART_IT_IDLE, DISABLE);
			USART_ITConfig(UGSM_USART, USART_IT_RXNE, DISABLE);
			USART_Cmd(UGSM_USART,ENABLE);
		}
  }
}
