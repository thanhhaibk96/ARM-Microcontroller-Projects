/* Includes ------------------------------------------------------------------*/
#include "User_CAN.h"
#include "stdbool.h"

/**
 * @defgroup Module Pin define
 * @{
 */
		/** 
		* @brief   CAN Pin define 
		*/
			#define 	UCAN_CLK_AFIO  				  RCC_APB2Periph_AFIO	
			#define		UCAN_CLK_AFIO_Cmd		  	RCC_APB2PeriphClockCmd
			
			#define 	UCAN_TX_PIN  				   	GPIO_Pin_12			
			#define 	UCAN_TX_PORT 				  	GPIOA
			#define 	UCAN_TX_CLK							RCC_APB2Periph_GPIOA			
			#define 	UCAN_TX_SOURCE				 	GPIO_PinSource12				
			#define		UCAN_TX_CLK_Cmd		  	 	RCC_APB2PeriphClockCmd			

			#define 	UCAN_RX_PIN  				   	GPIO_Pin_11
			#define 	UCAN_RX_PORT 				  	GPIOA
			#define 	UCAN_RX_CLK							RCC_APB2Periph_GPIOA
			#define 	UCAN_RX_SOURCE				 	GPIO_PinSource11
			#define		UCAN_RX_CLK_Cmd		  	 	RCC_APB2PeriphClockCmd		

			#define		UCAN_CAN								CAN1
			#define 	UCAN_CAN_CLK						RCC_APB1Periph_CAN1
			#define		UCAN_CAN_CLK_Cmd		  	RCC_APB1PeriphClockCmd
			#define 	UCAN_CAN_IRQn						USB_LP_CAN1_RX0_IRQn
			#define		UCAN_CAN_PreemptionPriority		0x00
			#define		UCAN_CAN_SubPriority					0x02
			
			#define		UCAN_CAN_IRQHandler			USB_LP_CAN1_RX0_IRQHandler
/**
 * @}
 */
 
/* Public variables ----------------------------------------------------------*/
static int _IDCANBUS_ARM_1 = 0x121;
static int _IDCANBUS_ARM_2 = 0x122;
static int _IDCANBUS_MASS_SHIFTER = 0x123;
static int _IDCANBUS_PISTOL = 0x124;
static int _IDCANBUS_THRUSTER = 0x125;
static int _IDCANBUS_EPC = 0x126;
static int _IDCANBUS_BLACKBOX = 0x127;

/* Private types -------------------------------------------------------------*/
uint8_t CAN_RxMessage[8];
uint8_t CAN_TxData[8] = {0,0,0,0,0,0,0,0};

/* Private const/macros ------------------------------------------------------*/
#define CAN_BAUDRATE  1000      /* 1MBps   */
//#define CAN_BAUDRATE  500  /* 500kBps */
//#define CAN_BAUDRATE  250  /* 250kBps */
//#define CAN_BAUDRATE  125  /* 125kBps */
/* #define CAN_BAUDRATE  100*/  /* 100kBps */ 
//#define CAN_BAUDRATE  50   /* 50kBps  */ 
/* #define CAN_BAUDRATE  20*/   /* 20kBps  */ 
/* #define CAN_BAUDRATE  10*/   /* 10kBps  */ 

/* Private variables ---------------------------------------------------------*/
CanRxMsg RxMessage;
/* Private function prototypes -----------------------------------------------*/
uint8_t UCAN_Checksum(uint8_t *_data);

/* Exported function body ----------------------------------------------------*/

/* Private functions body ----------------------------------------------------*/
void UCAN_Configure(void)
{
	CAN_InitTypeDef CAN_InitStruct;
	CAN_FilterInitTypeDef CAN_FilterInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	GPIO_InitTypeDef GPIO_InitStruct;
	uint16_t NVIC_Priority_Number;
	
	UCAN_CLK_AFIO_Cmd(UCAN_CLK_AFIO, ENABLE);
	UCAN_RX_CLK_Cmd(UCAN_RX_CLK,ENABLE);
	// Configure CANRX
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IPU;
	GPIO_InitStruct.GPIO_Pin = UCAN_RX_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(UCAN_RX_PORT,&GPIO_InitStruct);
	
	// Configure CANTX
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = UCAN_TX_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(UCAN_TX_PORT,&GPIO_InitStruct);
	
	UCAN_CAN_CLK_Cmd(UCAN_CAN_CLK,ENABLE);
	// CAN_CLK = 32MHz
	CAN_DeInit(UCAN_CAN);
		
	CAN_StructInit(&CAN_InitStruct);
	CAN_InitStruct.CAN_TTCM = DISABLE;
  CAN_InitStruct.CAN_ABOM = DISABLE;
  CAN_InitStruct.CAN_AWUM = DISABLE;
  CAN_InitStruct.CAN_NART = DISABLE;
  CAN_InitStruct.CAN_RFLM = DISABLE;
  CAN_InitStruct.CAN_TXFP = ENABLE;
  CAN_InitStruct.CAN_Mode = CAN_Mode_Normal;
  CAN_InitStruct.CAN_SJW = CAN_SJW_1tq;  
  CAN_InitStruct.CAN_BS1 = CAN_BS1_3tq;
  CAN_InitStruct.CAN_BS2 = CAN_BS2_2tq;
	
	// BaudRate = CAN_CLK/(CAN_Prescaler*(1 + CAN_BS1 + CAN_BS2))
	#if CAN_BAUDRATE == 1000 /* 1MBps */
  CAN_InitStruct.CAN_Prescaler = 6;
	#elif CAN_BAUDRATE == 500 /* 500KBps */
  CAN_InitStruct.CAN_Prescaler = 12;
	#elif CAN_BAUDRATE == 250 /* 250KBps */
  CAN_InitStruct.CAN_Prescaler = 24;
	#elif CAN_BAUDRATE == 125 /* 125KBps */
  CAN_InitStruct.CAN_Prescaler = 48;
	#elif  CAN_BAUDRATE == 100 /* 100KBps */
  CAN_InitStruct.CAN_Prescaler = 60;
	#elif  CAN_BAUDRATE == 50 /* 50KBps */
  CAN_InitStruct.CAN_Prescaler = 120;
	#elif  CAN_BAUDRATE == 20 /* 20KBps */
  CAN_InitStruct.CAN_Prescaler = 300;
	#elif  CAN_BAUDRATE == 10 /* 10KBps */
  CAN_InitStruct.CAN_Prescaler = 600;
	#else
			#error "Please select first the CAN Baudrate in Private defines in User_CAN.c "
	#endif  /* CAN_BAUDRATE == 1000 */
	
	CAN_Init(UCAN_CAN,&CAN_InitStruct);
	
	CAN_FilterInitStruct.CAN_FilterNumber = 1;
  CAN_FilterInitStruct.CAN_FilterMode = CAN_FilterMode_IdMask;
  CAN_FilterInitStruct.CAN_FilterScale = CAN_FilterScale_16bit;
  CAN_FilterInitStruct.CAN_FilterIdHigh = 0x0000;
  CAN_FilterInitStruct.CAN_FilterIdLow = 0x0000;
  CAN_FilterInitStruct.CAN_FilterMaskIdHigh = 0x0000;
  CAN_FilterInitStruct.CAN_FilterMaskIdLow = 0x0000;
  CAN_FilterInitStruct.CAN_FilterFIFOAssignment = 0;
  CAN_FilterInitStruct.CAN_FilterActivation = ENABLE;
  CAN_FilterInit(&CAN_FilterInitStruct);
	
	NVIC_Priority_Number = NVIC_EncodePriority(0,0,0);
	NVIC_SetPriority(UCAN_CAN_IRQn, NVIC_Priority_Number); 
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	NVIC_InitStruct.NVIC_IRQChannel = UCAN_CAN_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = UCAN_CAN_PreemptionPriority;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = UCAN_CAN_SubPriority;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	CAN_ITConfig(UCAN_CAN,CAN_IT_FMP0,ENABLE);
}

void UCAN_Transmit(int _IDstd, uint8_t _data[])
{
	CanTxMsg _TxMessage;
	uint8_t mailbox;
	uint8_t 	status;
	int _length = 8;
	
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

/******************************************************************************/
/* Peripherals Interrupt Handlers --------------------------------------------*/
/******************************************************************************/

void UCAN_CAN_IRQHandler(void)
{
	CAN_Receive(UCAN_CAN,CAN_FIFO0,&RxMessage);
	UCAN_Transmit(0x200, RxMessage.Data);
}
