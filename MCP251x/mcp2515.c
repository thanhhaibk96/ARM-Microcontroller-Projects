/* Includes ------------------------------------------------------------------*/
#include "mcp2515.h"

/**
 * @defgroup Module Pin define
 * @{
 */
		/** 
		* @brief   SPI1 Pin define 
		*/
			#define 	UMCP_INT_PIN  				   		GPIO_Pin_3			
			#define 	UMCP_INT_PORT 				  		GPIOA
			#define 	UMCP_INT_CLK								RCC_AHB1Periph_GPIOA			
			#define 	UMCP_INT_SOURCE				 			GPIO_PinSource3				
			#define		UMCP_INT_CLK_Cmd		  	 		RCC_AHB1PeriphClockCmd				
			// EXTI
			#define 	UMCP_INT_EXTI_SOURCE			EXTI_PortSourceGPIOA
			#define		UMCP_INT_EXTI_PORT				EXTI_PinSource3
			#define		UMCP_INT_EXTI_Line				EXTI_Line3
			#define		UMCP_INT_EXTI_Trigger			EXTI_Trigger_Falling	
			#define		UMCP_INT_PreemptionPriority		0x00
			#define		UMCP_INT_SubPriority					0x03
			//Interrupt
			#define		UMCP_INT_IRQn						EXTI3_IRQn
			#define 	UMCP_INT_IRQHandler			EXTI3_IRQHandler
			
			#define 	UMCP_CS_PIN  				   		GPIO_Pin_4			
			#define 	UMCP_CS_PORT 				  		GPIOA
			#define 	UMCP_CS_CLK								RCC_AHB1Periph_GPIOA			
			#define 	UMCP_CS_SOURCE				 		GPIO_PinSource4				
			#define		UMCP_CS_CLK_Cmd		  	 		RCC_AHB1PeriphClockCmd			

			#define 	UMCP_SCK_PIN  				   	GPIO_Pin_5
			#define 	UMCP_SCK_PORT 				  	GPIOA
			#define 	UMCP_SCK_CLK							RCC_AHB1Periph_GPIOA
			#define 	UMCP_SCK_SOURCE				 		GPIO_PinSource5
			#define		UMCP_SCK_CLK_Cmd		  	 	RCC_AHB1PeriphClockCmd
			
			#define 	UMCP_MISO_PIN  				   	GPIO_Pin_6
			#define 	UMCP_MISO_PORT 				  	GPIOA
			#define 	UMCP_MISO_CLK							RCC_AHB1Periph_GPIOA
			#define 	UMCP_MISO_SOURCE				 	GPIO_PinSource6
			#define		UMCP_MISO_CLK_Cmd		  	 	RCC_AHB1PeriphClockCmd
			
			#define 	UMCP_MOSI_PIN  				   	GPIO_Pin_7
			#define 	UMCP_MOSI_PORT 				  	GPIOA
			#define 	UMCP_MOSI_CLK							RCC_AHB1Periph_GPIOA
			#define 	UMCP_MOSI_SOURCE				 	GPIO_PinSource7
			#define		UMCP_MOSI_CLK_Cmd		  	 	RCC_AHB1PeriphClockCmd
			
			#define		UMCP_SPI									SPI1
			#define 	UMCP_SPI_CLK							RCC_APB2Periph_SPI1
			#define		UMCP_SPI_CLK_Cmd		  		RCC_APB2PeriphClockCmd
			#define		UMCP_SPI_AF								GPIO_AF_SPI1
			#define 	UMCP_SPI_IRQn							SPI1_IRQn
			
			#define   UMCP_DMA_Channel					DMA_Channel_3
			#define   UMCP_DMA_Stream						DMA2_Stream2
			#define		UMCP_DMA_CLK_Cmd		  		RCC_AHB1PeriphClockCmd
			#define 	UMCP_DMA_CLK							RCC_AHB1Periph_DMA2
			#define		UMCP_DMA_Stream_IRQn			DMA2_Stream2_IRQn
			
			#define		UMCP_DMA_PreemptionPriority		0x00
			#define		UMCP_DMA_SubPriority					0x01
			
			#define		UMCP_DMA_Stream_IRQHandler		DMA2_Stream2_IRQHandler
			#define		UMCP_SPI_IRQHandler						SPI1_IRQHandler
			
			#define MCP2515_CS_HIGH()   GPIO_WriteBit(UMCP_CS_PORT, UMCP_CS_PIN, Bit_SET)
			#define MCP2515_CS_LOW()    GPIO_WriteBit(UMCP_CS_PORT, UMCP_CS_PIN, Bit_RESET)
			
// speed 16M

#define MCP_16MHz_1000kBPS_CFG1 (0x00)
#define MCP_16MHz_1000kBPS_CFG2 (0xD0)
#define MCP_16MHz_1000kBPS_CFG3 (0x82)

#define MCP_16MHz_500kBPS_CFG1 (0x00)
#define MCP_16MHz_500kBPS_CFG2 (0xF0)
#define MCP_16MHz_500kBPS_CFG3 (0x86)

#define MCP_16MHz_250kBPS_CFG1 (0x41)
#define MCP_16MHz_250kBPS_CFG2 (0xF1)
#define MCP_16MHz_250kBPS_CFG3 (0x85)

#define MCP_16MHz_200kBPS_CFG1 (0x01)
#define MCP_16MHz_200kBPS_CFG2 (0xFA)
#define MCP_16MHz_200kBPS_CFG3 (0x87)

#define MCP_16MHz_125kBPS_CFG1 (0x03)
#define MCP_16MHz_125kBPS_CFG2 (0xF0)
#define MCP_16MHz_125kBPS_CFG3 (0x86)

#define MCP_16MHz_100kBPS_CFG1 (0x03)
#define MCP_16MHz_100kBPS_CFG2 (0xFA)
#define MCP_16MHz_100kBPS_CFG3 (0x87)

#define MCP_16MHz_95kBPS_CFG1 (0x03)
#define MCP_16MHz_95kBPS_CFG2 (0xAD)
#define MCP_16MHz_95kBPS_CFG3 (0x07)

#define MCP_16MHz_83k3BPS_CFG1 (0x03)
#define MCP_16MHz_83k3BPS_CFG2 (0xBE)
#define MCP_16MHz_83k3BPS_CFG3 (0x07)

#define MCP_16MHz_80kBPS_CFG1 (0x03)
#define MCP_16MHz_80kBPS_CFG2 (0xFF)
#define MCP_16MHz_80kBPS_CFG3 (0x87)

#define MCP_16MHz_50kBPS_CFG1 (0x07)
#define MCP_16MHz_50kBPS_CFG2 (0xFA)
#define MCP_16MHz_50kBPS_CFG3 (0x87)

#define MCP_16MHz_40kBPS_CFG1 (0x07)
#define MCP_16MHz_40kBPS_CFG2 (0xFF)
#define MCP_16MHz_40kBPS_CFG3 (0x87)

#define MCP_16MHz_33kBPS_CFG1 (0x09)
#define MCP_16MHz_33kBPS_CFG2 (0xBE)
#define MCP_16MHz_33kBPS_CFG3 (0x07)

#define MCP_16MHz_31k25BPS_CFG1 (0x0F)
#define MCP_16MHz_31k25BPS_CFG2 (0xF1)
#define MCP_16MHz_31k25BPS_CFG3 (0x85)

#define MCP_16MHz_25kBPS_CFG1 (0X0F)
#define MCP_16MHz_25kBPS_CFG2 (0XBA)
#define MCP_16MHz_25kBPS_CFG3 (0X07)

#define MCP_16MHz_20kBPS_CFG1 (0x0F)
#define MCP_16MHz_20kBPS_CFG2 (0xFF)
#define MCP_16MHz_20kBPS_CFG3 (0x87)

#define MCP_16MHz_10kBPS_CFG1 (0x1F)
#define MCP_16MHz_10kBPS_CFG2 (0xFF)
#define MCP_16MHz_10kBPS_CFG3 (0x87)

#define MCP_16MHz_5kBPS_CFG1 (0x3F)
#define MCP_16MHz_5kBPS_CFG2 (0xFF)
#define MCP_16MHz_5kBPS_CFG3 (0x87)

#define MCP_16MHz_666kBPS_CFG1 (0x00)
#define MCP_16MHz_666kBPS_CFG2 (0xA0)
#define MCP_16MHz_666kBPS_CFG3 (0x04)


// speed 8M

#define MCP_8MHz_1000kBPS_CFG1 (0x00)
#define MCP_8MHz_1000kBPS_CFG2 (0x80)
#define MCP_8MHz_1000kBPS_CFG3 (0x00)

#define MCP_8MHz_500kBPS_CFG1 (0x00)
#define MCP_8MHz_500kBPS_CFG2 (0x90)
#define MCP_8MHz_500kBPS_CFG3 (0x02)

#define MCP_8MHz_250kBPS_CFG1 (0x00)
#define MCP_8MHz_250kBPS_CFG2 (0xb1)
#define MCP_8MHz_250kBPS_CFG3 (0x05)

#define MCP_8MHz_200kBPS_CFG1 (0x00)
#define MCP_8MHz_200kBPS_CFG2 (0xb4)
#define MCP_8MHz_200kBPS_CFG3 (0x06)

#define MCP_8MHz_125kBPS_CFG1 (0x01)
#define MCP_8MHz_125kBPS_CFG2 (0xb1)
#define MCP_8MHz_125kBPS_CFG3 (0x05)

#define MCP_8MHz_100kBPS_CFG1 (0x01)
#define MCP_8MHz_100kBPS_CFG2 (0xb4)
#define MCP_8MHz_100kBPS_CFG3 (0x06)

#define MCP_8MHz_80kBPS_CFG1 (0x01)
#define MCP_8MHz_80kBPS_CFG2 (0xbf)
#define MCP_8MHz_80kBPS_CFG3 (0x07)

#define MCP_8MHz_50kBPS_CFG1 (0x03)
#define MCP_8MHz_50kBPS_CFG2 (0xb4)
#define MCP_8MHz_50kBPS_CFG3 (0x06)

#define MCP_8MHz_40kBPS_CFG1 (0x03)
#define MCP_8MHz_40kBPS_CFG2 (0xbf)
#define MCP_8MHz_40kBPS_CFG3 (0x07)

#define MCP_8MHz_31k25BPS_CFG1 (0x07)
#define MCP_8MHz_31k25BPS_CFG2 (0xa4)
#define MCP_8MHz_31k25BPS_CFG3 (0x04)

#define MCP_8MHz_20kBPS_CFG1 (0x07)
#define MCP_8MHz_20kBPS_CFG2 (0xbf)
#define MCP_8MHz_20kBPS_CFG3 (0x07)

#define MCP_8MHz_10kBPS_CFG1 (0x0f)
#define MCP_8MHz_10kBPS_CFG2 (0xbf)
#define MCP_8MHz_10kBPS_CFG3 (0x07)

#define MCP_8MHz_5kBPS_CFG1 (0x1f)
#define MCP_8MHz_5kBPS_CFG2 (0xbf)
#define MCP_8MHz_5kBPS_CFG3 (0x07)

/* Public variables ----------------------------------------------------------*/
static bool SPI_STATE_READY = false;
static uint32_t count_int = 0;

/* Private types -------------------------------------------------------------*/

/* Private const/macros ------------------------------------------------------*/		
static uint32_t convertReg2ExtendedCANid(uint8_t tempRXBn_EIDH, uint8_t tempRXBn_EIDL, uint8_t tempRXBn_SIDH, uint8_t tempRXBn_SIDL);
static uint32_t convertReg2StandardCANid(uint8_t tempRXBn_SIDH, uint8_t tempRXBn_SIDL) ;
static void convertCANid2Reg(uint32_t tempPassedInID, uint8_t canIdType, id_reg_t *passedIdReg);


static void SPI_Tx(uint8_t data);
static void SPI_TxBuffer(uint8_t *buffer, uint8_t length);
static uint8_t SPI_Rx(void);
static void SPI_RxBuffer(uint8_t *buffer, uint8_t length);
/* Private variables ---------------------------------------------------------*/
ctrl_status_t ctrlStatus;
ctrl_error_status_t errorStatus;
id_reg_t idReg;

/* Private function prototypes -----------------------------------------------*/

/* Exported function body ----------------------------------------------------*/
/* MCP2515 ??? */
bool MCP2515_Initialize(void)
{
  MCP2515_CS_HIGH();    
  
  uint8_t loop = 10;
  
  do {
    /* SPI Ready ?? */
    if(SPI_STATE_READY)
      return true;
    
    loop--;
  } while(loop > 0); 
      
  return false;
}

/* MCP2515 ? ????? ?? */
bool MCP2515_SetConfigMode(void)
{
  /* CANCTRL Register Configuration ?? ?? */  
  MCP2515_WriteByte(MCP2515_CANCTRL, 0x80);
  
  uint8_t loop = 10;
  
  do {    
    /* ???? ?? */    
    if((MCP2515_ReadByte(MCP2515_CANSTAT) & 0xE0) == 0x80)
      return true;
    
    loop--;
  } while(loop > 0); 
  
  return false;
}

/* MCP2515 ? Normal??? ?? */
bool MCP2515_SetNormalMode(void)
{
  /* CANCTRL Register Normal ?? ?? */  
  MCP2515_WriteByte(MCP2515_CANCTRL, 0x00);
  
  uint8_t loop = 10;
  
  do {    
    /* ???? ?? */    
    if((MCP2515_ReadByte(MCP2515_CANSTAT) & 0xE0) == 0x00)
      return true;
    
    loop--;
  } while(loop > 0);
  
  return false;
}

/* MCP2515 ? Sleep ??? ?? */
bool MCP2515_SetSleepMode(void)
{
  /* CANCTRL Register Sleep ?? ?? */  
  MCP2515_WriteByte(MCP2515_CANCTRL, 0x20);
  
  uint8_t loop = 10;
  
  do {    
    /* ???? ?? */    
    if((MCP2515_ReadByte(MCP2515_CANSTAT) & 0xE0) == 0x20)
      return true;
    
    loop--;
  } while(loop > 0);
  
  return false;
}

void MCP2515_EnableInterrupt(MCP_InterruptDef _interrupt)
{
	
	MCP2515_WriteByte(MCP2515_CANINTE, _interrupt);
}

/* MCP2515 SPI-Reset */
void MCP2515_Reset(void)
{    
  MCP2515_CS_LOW();
      
  SPI_Tx(MCP2515_RESET);
      
  MCP2515_CS_HIGH();
}

/* 1??? ?? */
uint8_t MCP2515_ReadByte (uint8_t address)
{
  uint8_t retVal;
  
  MCP2515_CS_LOW();
  
  SPI_Tx(MCP2515_READ);
  SPI_Tx(address);
  retVal = SPI_Rx();
      
  MCP2515_CS_HIGH();
  
  return retVal;
}

/* Sequential Bytes ?? */
void MCP2515_ReadRxSequence(uint8_t instruction, uint8_t *data, uint8_t length)
{
  MCP2515_CS_LOW();
  
  SPI_Tx(instruction);        
  SPI_RxBuffer(data, length);
    
  MCP2515_CS_HIGH();
}

/* 1??? ?? */
void MCP2515_WriteByte(uint8_t address, uint8_t data)
{    
  MCP2515_CS_LOW(); 
	
  SPI_Tx(MCP2515_WRITE);
  SPI_Tx(address);
  SPI_Tx(data);  
    
  MCP2515_CS_HIGH();
}

/* Sequential Bytes ?? */
void MCP2515_WriteByteSequence(uint8_t startAddress, uint8_t endAddress, uint8_t *data)
{    
  MCP2515_CS_LOW();
  
  SPI_Tx(MCP2515_WRITE);
  SPI_Tx(startAddress);
  SPI_TxBuffer(data, (endAddress - startAddress + 1));
  
  MCP2515_CS_HIGH();
}

/* TxBuffer? Sequential Bytes ?? */
void MCP2515_LoadTxSequence(uint8_t instruction, uint8_t *idReg, uint8_t dlc, uint8_t *data)
{    
  MCP2515_CS_LOW();
  
  SPI_Tx(instruction);
  SPI_TxBuffer(idReg, 4);
  SPI_Tx(dlc);
  SPI_TxBuffer(data, dlc);
       
  MCP2515_CS_HIGH();
}

/* TxBuffer? 1 Bytes ?? */
void MCP2515_LoadTxBuffer(uint8_t instruction, uint8_t data)
{
  MCP2515_CS_LOW();
  
  SPI_Tx(instruction);
  SPI_Tx(data);
        
  MCP2515_CS_HIGH();
}

/* RTS ??? ??? TxBuffer ?? */
void MCP2515_RequestToSend(uint8_t instruction)
{
  MCP2515_CS_LOW();
  
  SPI_Tx(instruction);
      
  MCP2515_CS_HIGH();
}

/* MCP2515 Status ?? */
uint8_t MCP2515_ReadStatus(void)
{
  uint8_t retVal;
  
  MCP2515_CS_LOW();
  
  SPI_Tx(MCP2515_READ_STATUS);
  retVal = SPI_Rx();
        
  MCP2515_CS_HIGH();
  
  return retVal;
}

/* MCP2515 RxStatus ???? ?? */
uint8_t MCP2515_GetRxStatus(void)
{
  uint8_t retVal;
  
  MCP2515_CS_LOW();
  
  SPI_Tx(MCP2515_RX_STATUS);
  retVal = SPI_Rx();
        
  MCP2515_CS_HIGH();
  
  return retVal;
}

/* ???? ? ?? */
void MCP2515_BitModify(uint8_t address, uint8_t mask, uint8_t data)
{    
  MCP2515_CS_LOW();
  
  SPI_Tx(MCP2515_BIT_MOD);
  SPI_Tx(address);
  SPI_Tx(mask);
  SPI_Tx(data);
        
  MCP2515_CS_HIGH();
}

/* SPI Tx Wrapper ?? */
static void SPI_Tx(uint8_t data)
{
  SPI_I2S_SendData(UMCP_SPI, data);
	while(!SPI_I2S_GetFlagStatus(UMCP_SPI,SPI_FLAG_TXE)|| !SPI_I2S_GetFlagStatus(UMCP_SPI,SPI_FLAG_RXNE) || SPI_I2S_GetFlagStatus(UMCP_SPI,SPI_FLAG_BSY));
	(void)SPI_I2S_ReceiveData(UMCP_SPI);
}

/* SPI Tx Wrapper ?? */
static void SPI_TxBuffer(uint8_t *buffer, uint8_t length)
{
	uint8_t i = 0;
  for(i = 0; i < length; i++)
	{
		SPI_I2S_SendData(UMCP_SPI, buffer[i]);
		while(!SPI_I2S_GetFlagStatus(UMCP_SPI,SPI_FLAG_TXE)|| !SPI_I2S_GetFlagStatus(UMCP_SPI,SPI_FLAG_RXNE) || SPI_I2S_GetFlagStatus(UMCP_SPI,SPI_FLAG_BSY));
		(void)SPI_I2S_ReceiveData(UMCP_SPI);
	}	
}

/* SPI Rx Wrapper ?? */
static uint8_t SPI_Rx(void)
{
  uint8_t retVal;
	uint8_t dummybyte = 0x00;
	
	SPI_I2S_SendData(UMCP_SPI, dummybyte);
	while(!SPI_I2S_GetFlagStatus(UMCP_SPI,SPI_FLAG_TXE)|| !SPI_I2S_GetFlagStatus(UMCP_SPI,SPI_FLAG_RXNE) || SPI_I2S_GetFlagStatus(UMCP_SPI,SPI_FLAG_BSY)); 
	retVal = SPI_I2S_ReceiveData(UMCP_SPI);
  return retVal;
}

/* SPI Rx Wrapper ?? */
static void SPI_RxBuffer(uint8_t *buffer, uint8_t length)
{
  uint8_t i = 0;
	uint8_t dummybyte = 0x00;
  for(i = 0; i < length; i++)
	{
		SPI_I2S_SendData(UMCP_SPI, dummybyte);
		while(!SPI_I2S_GetFlagStatus(UMCP_SPI,SPI_FLAG_TXE)|| !SPI_I2S_GetFlagStatus(UMCP_SPI,SPI_FLAG_RXNE) || SPI_I2S_GetFlagStatus(UMCP_SPI,SPI_FLAG_BSY));
		buffer[i] = SPI_I2S_ReceiveData(UMCP_SPI);
	}	
}

/** CAN SPI APIs */ 

/* Sleep ?? ?? */
void CANSPI_Sleep(void)
{
  /* Clear CAN bus wakeup interrupt */
  MCP2515_BitModify(MCP2515_CANINTF, 0x40, 0x00);        
  
  /* Enable CAN bus activity wakeup */
  MCP2515_BitModify(MCP2515_CANINTE, 0x40, 0x40);        
  
  MCP2515_SetSleepMode();
}

/* CAN ?? ???  */
bool CANSPI_Initialize(CAN_BaudrateDef speedset, MCP_OSCDef clockset)
{
	SPI_InitTypeDef SPI_InitStruct;
	GPIO_InitTypeDef GPIO_InitStruct;
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	//LIMIT SWITCH 3
	UMCP_INT_CLK_Cmd(UMCP_INT_CLK,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG,ENABLE);	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = UMCP_INT_PIN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(UMCP_INT_PORT,&GPIO_InitStruct);
	
	SYSCFG_EXTILineConfig(UMCP_INT_EXTI_SOURCE,UMCP_INT_EXTI_PORT);
	
	EXTI_InitStruct.EXTI_Line = UMCP_INT_EXTI_Line;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = UMCP_INT_EXTI_Trigger;
	EXTI_Init(&EXTI_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel = UMCP_INT_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = UMCP_INT_PreemptionPriority;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = UMCP_INT_SubPriority;
	NVIC_Init(&NVIC_InitStruct);
	
	//Set up SPI port A (Structure in STM32f4 User_manual page 36)
	UMCP_SCK_CLK_Cmd(UMCP_SCK_CLK, ENABLE);
	GPIO_InitStruct.GPIO_Pin = UMCP_SCK_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;															//GPIO Alternate function Mode
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(UMCP_SCK_PORT, &GPIO_InitStruct);
	
	UMCP_MISO_CLK_Cmd(UMCP_MISO_CLK, ENABLE);
	GPIO_InitStruct.GPIO_Pin = UMCP_MISO_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;															//GPIO Alternate function Mode
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(UMCP_MISO_PORT, &GPIO_InitStruct);
	
	UMCP_MOSI_CLK_Cmd(UMCP_MOSI_CLK, ENABLE);
	GPIO_InitStruct.GPIO_Pin = UMCP_MOSI_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;															//GPIO Alternate function Mode
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(UMCP_MOSI_PORT, &GPIO_InitStruct);	
	
	GPIO_PinAFConfig(UMCP_SCK_PORT, UMCP_SCK_SOURCE, GPIO_AF_SPI1);										//Set alternate port for mode GPIO_Mode_AF
	GPIO_PinAFConfig(UMCP_MISO_PORT, UMCP_MISO_SOURCE, GPIO_AF_SPI1);
	GPIO_PinAFConfig(UMCP_MOSI_PORT, UMCP_MOSI_SOURCE, GPIO_AF_SPI1);
	
	//The SS(slave select) is PA4 (Structure in STM32f4 User_manual page 36)
	UMCP_CS_CLK_Cmd(UMCP_CS_CLK, ENABLE);
	GPIO_InitStruct.GPIO_Pin = UMCP_CS_PIN;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(UMCP_CS_PORT, &GPIO_InitStruct);

	GPIO_SetBits(UMCP_CS_PORT, UMCP_CS_PIN);
	/* SPI_MASTER configuration ------------------------------------------------*/
	UMCP_SPI_CLK_Cmd(UMCP_SPI_CLK, ENABLE);
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;																
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;															
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft|SPI_NSSInternalSoft_Set;				
	SPI_InitStruct.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8;	
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_Init(UMCP_SPI, &SPI_InitStruct);
	
	SPI_Cmd(UMCP_SPI, ENABLE);
	
	UDELAY_ms(1000);
	
	SPI_STATE_READY = true;
	
  RXF0 RXF0reg;
  RXF1 RXF1reg;
  RXF2 RXF2reg;
  RXF3 RXF3reg;
  RXF4 RXF4reg;
  RXF5 RXF5reg;
  RXM0 RXM0reg;
  RXM1 RXM1reg;
      
  /* Rx Mask values ??? */
  RXM0reg.RXM0SIDH = 0x00;
  RXM0reg.RXM0SIDL = 0x00;
  RXM0reg.RXM0EID8 = 0x00;
  RXM0reg.RXM0EID0 = 0x00;
  
  RXM1reg.RXM1SIDH = 0x00;
  RXM1reg.RXM1SIDL = 0x00;
  RXM1reg.RXM1EID8 = 0x00;
  RXM1reg.RXM1EID0 = 0x00;
  
  /* Rx Filter values ??? */
  RXF0reg.RXF0SIDH = 0x00;      
  RXF0reg.RXF0SIDL = 0x00;      //Starndard Filter
  RXF0reg.RXF0EID8 = 0x00;
  RXF0reg.RXF0EID0 = 0x00;
  
  RXF1reg.RXF1SIDH = 0x00;
  RXF1reg.RXF1SIDL = 0x08;      //Exntended Filter
  RXF1reg.RXF1EID8 = 0x00;
  RXF1reg.RXF1EID0 = 0x00;
  
  RXF2reg.RXF2SIDH = 0x00;
  RXF2reg.RXF2SIDL = 0x00;
  RXF2reg.RXF2EID8 = 0x00;
  RXF2reg.RXF2EID0 = 0x00;
  
  RXF3reg.RXF3SIDH = 0x00;
  RXF3reg.RXF3SIDL = 0x00;
  RXF3reg.RXF3EID8 = 0x00;
  RXF3reg.RXF3EID0 = 0x00;
  
  RXF4reg.RXF4SIDH = 0x00;
  RXF4reg.RXF4SIDL = 0x00;
  RXF4reg.RXF4EID8 = 0x00;
  RXF4reg.RXF4EID0 = 0x00;
  
  RXF5reg.RXF5SIDH = 0x00;
  RXF5reg.RXF5SIDL = 0x08;
  RXF5reg.RXF5EID8 = 0x00;
  RXF5reg.RXF5EID0 = 0x00;
  
  /* MCP2515 ???, SPI ?? ?? ?? */
  if(!MCP2515_Initialize())
    return false;
    
  /* Configuration ??? ?? */
  if(!MCP2515_SetConfigMode())
    return false;
  
  /* Filter & Mask ? ?? */
  MCP2515_WriteByteSequence(MCP2515_RXM0SIDH, MCP2515_RXM0EID0, &(RXM0reg.RXM0SIDH));
  MCP2515_WriteByteSequence(MCP2515_RXM1SIDH, MCP2515_RXM1EID0, &(RXM1reg.RXM1SIDH));
  MCP2515_WriteByteSequence(MCP2515_RXF0SIDH, MCP2515_RXF0EID0, &(RXF0reg.RXF0SIDH));
  MCP2515_WriteByteSequence(MCP2515_RXF1SIDH, MCP2515_RXF1EID0, &(RXF1reg.RXF1SIDH));
  MCP2515_WriteByteSequence(MCP2515_RXF2SIDH, MCP2515_RXF2EID0, &(RXF2reg.RXF2SIDH));
  MCP2515_WriteByteSequence(MCP2515_RXF3SIDH, MCP2515_RXF3EID0, &(RXF3reg.RXF3SIDH));
  MCP2515_WriteByteSequence(MCP2515_RXF4SIDH, MCP2515_RXF4EID0, &(RXF4reg.RXF4SIDH));
  MCP2515_WriteByteSequence(MCP2515_RXF5SIDH, MCP2515_RXF5EID0, &(RXF5reg.RXF5SIDH));
  
  /* Accept All (Standard + Extended) */
  MCP2515_WriteByte(MCP2515_RXB0CTRL, 0x04);    //Enable BUKT, Accept Filter 0
  MCP2515_WriteByte(MCP2515_RXB1CTRL, 0x01);    //Accept Filter 1
      
  /* 
  * tq = 2 * (brp(0) + 1) / 8000000 = 0.25us
  * tbit = (SYNC_SEG(1 fixed) + PROP_SEG + PS1 + PS2)
  * tbit = 1tq + 5tq + 6tq + 4tq = 16tq
  * 16tq = 2us = 250kbps
  */
	uint8_t cfg1, cfg2, cfg3;
  switch (clockset) {		
		case (MCP_16MHz) :
				switch (speedset) {
						case (CAN_5KBPS):
								cfg1 = MCP_16MHz_5kBPS_CFG1;
								cfg2 = MCP_16MHz_5kBPS_CFG2;
								cfg3 = MCP_16MHz_5kBPS_CFG3;
								break;

						case (CAN_10KBPS):
								cfg1 = MCP_16MHz_10kBPS_CFG1;
								cfg2 = MCP_16MHz_10kBPS_CFG2;
								cfg3 = MCP_16MHz_10kBPS_CFG3;
								break;

						case (CAN_20KBPS):
								cfg1 = MCP_16MHz_20kBPS_CFG1;
								cfg2 = MCP_16MHz_20kBPS_CFG2;
								cfg3 = MCP_16MHz_20kBPS_CFG3;
								break;

						case (CAN_25KBPS):
								cfg1 = MCP_16MHz_25kBPS_CFG1;
								cfg2 = MCP_16MHz_25kBPS_CFG2;
								cfg3 = MCP_16MHz_25kBPS_CFG3;
								break;

						case (CAN_31K25BPS):
								cfg1 = MCP_16MHz_31k25BPS_CFG1;
								cfg2 = MCP_16MHz_31k25BPS_CFG2;
								cfg3 = MCP_16MHz_31k25BPS_CFG3;
								break;

						case (CAN_33KBPS):
								cfg1 = MCP_16MHz_33kBPS_CFG1;
								cfg2 = MCP_16MHz_33kBPS_CFG2;
								cfg3 = MCP_16MHz_33kBPS_CFG3;
								break;

						case (CAN_40KBPS):
								cfg1 = MCP_16MHz_40kBPS_CFG1;
								cfg2 = MCP_16MHz_40kBPS_CFG2;
								cfg3 = MCP_16MHz_40kBPS_CFG3;
								break;

						case (CAN_50KBPS):
								cfg1 = MCP_16MHz_50kBPS_CFG1;
								cfg2 = MCP_16MHz_50kBPS_CFG2;
								cfg3 = MCP_16MHz_50kBPS_CFG3;
								break;

						case (CAN_80KBPS):
								cfg1 = MCP_16MHz_80kBPS_CFG1;
								cfg2 = MCP_16MHz_80kBPS_CFG2;
								cfg3 = MCP_16MHz_80kBPS_CFG3;
								break;

						case (CAN_83K3BPS):
								cfg1 = MCP_16MHz_83k3BPS_CFG1;
								cfg2 = MCP_16MHz_83k3BPS_CFG2;
								cfg3 = MCP_16MHz_83k3BPS_CFG3;
								break;

						case (CAN_95KBPS):
								cfg1 = MCP_16MHz_95kBPS_CFG1;
								cfg2 = MCP_16MHz_95kBPS_CFG2;
								cfg3 = MCP_16MHz_95kBPS_CFG3;
								break;

						case (CAN_100KBPS):
								cfg1 = MCP_16MHz_100kBPS_CFG1;
								cfg2 = MCP_16MHz_100kBPS_CFG2;
								cfg3 = MCP_16MHz_100kBPS_CFG3;
								break;

						case (CAN_125KBPS):
								cfg1 = MCP_16MHz_125kBPS_CFG1;
								cfg2 = MCP_16MHz_125kBPS_CFG2;
								cfg3 = MCP_16MHz_125kBPS_CFG3;
								break;

						case (CAN_200KBPS):
								cfg1 = MCP_16MHz_200kBPS_CFG1;
								cfg2 = MCP_16MHz_200kBPS_CFG2;
								cfg3 = MCP_16MHz_200kBPS_CFG3;
								break;

						case (CAN_250KBPS):
								cfg1 = MCP_16MHz_250kBPS_CFG1;
								cfg2 = MCP_16MHz_250kBPS_CFG2;
								cfg3 = MCP_16MHz_250kBPS_CFG3;
								break;

						case (CAN_500KBPS):
								cfg1 = MCP_16MHz_500kBPS_CFG1;
								cfg2 = MCP_16MHz_500kBPS_CFG2;
								cfg3 = MCP_16MHz_500kBPS_CFG3;
								break;

						case (CAN_666KBPS):
								cfg1 = MCP_16MHz_666kBPS_CFG1;
								cfg2 = MCP_16MHz_666kBPS_CFG2;
								cfg3 = MCP_16MHz_666kBPS_CFG3;
								break;

						case (CAN_1000KBPS):
								cfg1 = MCP_16MHz_1000kBPS_CFG1;
								cfg2 = MCP_16MHz_1000kBPS_CFG2;
								cfg3 = MCP_16MHz_1000kBPS_CFG3;
								break;

						default:
								break;
				}
				break;

		case (MCP_8MHz) :
				switch (speedset) {
						case (CAN_5KBPS) :
								cfg1 = MCP_8MHz_5kBPS_CFG1;
								cfg2 = MCP_8MHz_5kBPS_CFG2;
								cfg3 = MCP_8MHz_5kBPS_CFG3;
								break;

						case (CAN_10KBPS) :
								cfg1 = MCP_8MHz_10kBPS_CFG1;
								cfg2 = MCP_8MHz_10kBPS_CFG2;
								cfg3 = MCP_8MHz_10kBPS_CFG3;
								break;

						case (CAN_20KBPS) :
								cfg1 = MCP_8MHz_20kBPS_CFG1;
								cfg2 = MCP_8MHz_20kBPS_CFG2;
								cfg3 = MCP_8MHz_20kBPS_CFG3;
								break;

						case (CAN_31K25BPS) :
								cfg1 = MCP_8MHz_31k25BPS_CFG1;
								cfg2 = MCP_8MHz_31k25BPS_CFG2;
								cfg3 = MCP_8MHz_31k25BPS_CFG3;
								break;

						case (CAN_40KBPS) :
								cfg1 = MCP_8MHz_40kBPS_CFG1;
								cfg2 = MCP_8MHz_40kBPS_CFG2;
								cfg3 = MCP_8MHz_40kBPS_CFG3;
								break;

						case (CAN_50KBPS) :
								cfg1 = MCP_8MHz_50kBPS_CFG1;
								cfg2 = MCP_8MHz_50kBPS_CFG2;
								cfg3 = MCP_8MHz_50kBPS_CFG3;
								break;

						case (CAN_80KBPS) :
								cfg1 = MCP_8MHz_80kBPS_CFG1;
								cfg2 = MCP_8MHz_80kBPS_CFG2;
								cfg3 = MCP_8MHz_80kBPS_CFG3;
								break;

						case (CAN_100KBPS) :
								cfg1 = MCP_8MHz_100kBPS_CFG1;
								cfg2 = MCP_8MHz_100kBPS_CFG2;
								cfg3 = MCP_8MHz_100kBPS_CFG3;
								break;

						case (CAN_125KBPS) :
								cfg1 = MCP_8MHz_125kBPS_CFG1;
								cfg2 = MCP_8MHz_125kBPS_CFG2;
								cfg3 = MCP_8MHz_125kBPS_CFG3;
								break;

						case (CAN_200KBPS) :
								cfg1 = MCP_8MHz_200kBPS_CFG1;
								cfg2 = MCP_8MHz_200kBPS_CFG2;
								cfg3 = MCP_8MHz_200kBPS_CFG3;
								break;

						case (CAN_250KBPS) :
								cfg1 = MCP_8MHz_250kBPS_CFG1;
								cfg2 = MCP_8MHz_250kBPS_CFG2;
								cfg3 = MCP_8MHz_250kBPS_CFG3;
								break;

						case (CAN_500KBPS) :
								cfg1 = MCP_8MHz_500kBPS_CFG1;
								cfg2 = MCP_8MHz_500kBPS_CFG2;
								cfg3 = MCP_8MHz_500kBPS_CFG3;
								break;

						case (CAN_1000KBPS) :
								cfg1 = MCP_8MHz_1000kBPS_CFG1;
								cfg2 = MCP_8MHz_1000kBPS_CFG2;
								cfg3 = MCP_8MHz_1000kBPS_CFG3;
								break;

						default:
								break;
				}
				break;

		default:
				break;
    }
  /* 00(SJW 1tq) 000000 */  
  MCP2515_WriteByte(MCP2515_CNF1, cfg1);
  
  /* 1 1 100(5tq) 101(6tq) */  
  MCP2515_WriteByte(MCP2515_CNF2, cfg2);
  
  /* 1 0 000 011(4tq) */  
  MCP2515_WriteByte(MCP2515_CNF3, cfg3);
  
	MCP2515_EnableInterrupt(RX0IE | RX1IE | WAKIE);
  /* Normal ??? ?? */
  if(!MCP2515_SetNormalMode())
    return false;	
	
	//Check READY MCP2515
	uCAN_MSG _tmp_rxMessage;
	while((MCP2515_ReadByte(MCP2515_CANSTAT)&0x0E) != 0x00)
	{
		CANSPI_Receive(&_tmp_rxMessage);
	}
	
  return true;
}

/* CAN ??? ?? */
uint8_t CANSPI_Transmit(uCAN_MSG *tempCanMsg) 
{
  uint8_t returnValue = 0;
  
  idReg.tempSIDH = 0;
  idReg.tempSIDL = 0;
  idReg.tempEID8 = 0;
  idReg.tempEID0 = 0;
  
  ctrlStatus.ctrl_status = MCP2515_ReadStatus();
  
  /* ?? Transmission ? Pending ?? ?? ??? ??? ????. */
  if (ctrlStatus.TXB0REQ != 1)
  {
    /* ID Type? ?? ?? */
    convertCANid2Reg(tempCanMsg->frame.id, tempCanMsg->frame.idType, &idReg);
    
    /* Tx Buffer? ??? ??? Loading */
    MCP2515_LoadTxSequence(MCP2515_LOAD_TXB0SIDH, &(idReg.tempSIDH), tempCanMsg->frame.dlc, &(tempCanMsg->frame.data0));
    
    /* Tx Buffer? ??? ???? */
    MCP2515_RequestToSend(MCP2515_RTS_TX0);
    
    returnValue = 1;
  }
  else if (ctrlStatus.TXB1REQ != 1)
  {
    convertCANid2Reg(tempCanMsg->frame.id, tempCanMsg->frame.idType, &idReg);
    
    MCP2515_LoadTxSequence(MCP2515_LOAD_TXB1SIDH, &(idReg.tempSIDH), tempCanMsg->frame.dlc, &(tempCanMsg->frame.data0));
    MCP2515_RequestToSend(MCP2515_RTS_TX1);
    
    returnValue = 1;
  }
  else if (ctrlStatus.TXB2REQ != 1)
  {
    convertCANid2Reg(tempCanMsg->frame.id, tempCanMsg->frame.idType, &idReg);
    
    MCP2515_LoadTxSequence(MCP2515_LOAD_TXB2SIDH, &(idReg.tempSIDH), tempCanMsg->frame.dlc, &(tempCanMsg->frame.data0));
    MCP2515_RequestToSend(MCP2515_RTS_TX2);
    
    returnValue = 1;
  }
  
  return (returnValue);
}

/* CAN ??? ?? */
uint8_t CANSPI_Receive(uCAN_MSG *tempCanMsg) 
{
  uint8_t returnValue = 0;
  rx_reg_t rxReg;
  ctrl_rx_status_t rxStatus;
  
  rxStatus.ctrl_rx_status = MCP2515_GetRxStatus();
  
  /* ??? ??? ???? ??? ?? */
  if (rxStatus.rxBuffer != 0)
  {
    /* ?? ??? ???? ??? ?? ? ?? */
    if ((rxStatus.rxBuffer == MSG_IN_RXB0)|(rxStatus.rxBuffer == MSG_IN_BOTH_BUFFERS))
    {
      MCP2515_ReadRxSequence(MCP2515_READ_RXB0SIDH, rxReg.rx_reg_array, sizeof(rxReg.rx_reg_array));
    }
    else if (rxStatus.rxBuffer == MSG_IN_RXB1)
    {
      MCP2515_ReadRxSequence(MCP2515_READ_RXB1SIDH, rxReg.rx_reg_array, sizeof(rxReg.rx_reg_array));
    }
    
    /* Extended ?? */
    if (rxStatus.msgType == dEXTENDED_CAN_MSG_ID_2_0B)
    {
      tempCanMsg->frame.idType = (uint8_t) dEXTENDED_CAN_MSG_ID_2_0B;
      tempCanMsg->frame.id = convertReg2ExtendedCANid(rxReg.RXBnEID8, rxReg.RXBnEID0, rxReg.RXBnSIDH, rxReg.RXBnSIDL);
    } 
    else 
    {
      /* Standard ?? */
      tempCanMsg->frame.idType = (uint8_t) dSTANDARD_CAN_MSG_ID_2_0B;
      tempCanMsg->frame.id = convertReg2StandardCANid(rxReg.RXBnSIDH, rxReg.RXBnSIDL);
    }
    
    tempCanMsg->frame.dlc   = rxReg.RXBnDLC;
    tempCanMsg->frame.data0 = rxReg.RXBnD0;
    tempCanMsg->frame.data1 = rxReg.RXBnD1;
    tempCanMsg->frame.data2 = rxReg.RXBnD2;
    tempCanMsg->frame.data3 = rxReg.RXBnD3;
    tempCanMsg->frame.data4 = rxReg.RXBnD4;
    tempCanMsg->frame.data5 = rxReg.RXBnD5;
    tempCanMsg->frame.data6 = rxReg.RXBnD6;
    tempCanMsg->frame.data7 = rxReg.RXBnD7;
    
    returnValue = 1;
  }
  
  return (returnValue);
}

/* ?? ??? ???? ??? ?? */
uint8_t CANSPI_messagesInBuffer(void)
{
  uint8_t messageCount = 0;
  
  ctrlStatus.ctrl_status = MCP2515_ReadStatus();
  
  if(ctrlStatus.RX0IF != 0)
  {
    messageCount++;
  }
  
  if(ctrlStatus.RX1IF != 0)
  {
    messageCount++;
  }
  
  return (messageCount);
}

/* CAN BUS ? Offline ?? ?? */
uint8_t CANSPI_isBussOff(void)
{
  uint8_t returnValue = 0;
  
  errorStatus.error_flag_reg = MCP2515_ReadByte(MCP2515_EFLG);
  
  if(errorStatus.TXBO == 1)
  {
    returnValue = 1;
  }
  
  return (returnValue);
}

/* Rx Passive Error ???? ?? */
uint8_t CANSPI_isRxErrorPassive(void)
{
  uint8_t returnValue = 0;
  
  errorStatus.error_flag_reg = MCP2515_ReadByte(MCP2515_EFLG);
  
  if(errorStatus.RXEP == 1)
  {
    returnValue = 1;
  }
  
  return (returnValue);
}

/* Tx Passive Error ???? ?? */
uint8_t CANSPI_isTxErrorPassive(void)
{
  uint8_t returnValue = 0;
  
  errorStatus.error_flag_reg = MCP2515_ReadByte(MCP2515_EFLG);
  
  if(errorStatus.TXEP == 1)
  {
    returnValue = 1;
  }
  
  return (returnValue);
}

/* Register ???? Extended ID ???? ???? ?? ?? */
static uint32_t convertReg2ExtendedCANid(uint8_t tempRXBn_EIDH, uint8_t tempRXBn_EIDL, uint8_t tempRXBn_SIDH, uint8_t tempRXBn_SIDL) 
{
  uint32_t returnValue = 0;
  uint32_t ConvertedID = 0;
  uint8_t CAN_standardLo_ID_lo2bits;
  uint8_t CAN_standardLo_ID_hi3bits;
  
  CAN_standardLo_ID_lo2bits = (tempRXBn_SIDL & 0x03);
  CAN_standardLo_ID_hi3bits = (tempRXBn_SIDL >> 5);
  ConvertedID = (tempRXBn_SIDH << 3);
  ConvertedID = ConvertedID + CAN_standardLo_ID_hi3bits;
  ConvertedID = (ConvertedID << 2);
  ConvertedID = ConvertedID + CAN_standardLo_ID_lo2bits;
  ConvertedID = (ConvertedID << 8);
  ConvertedID = ConvertedID + tempRXBn_EIDH;
  ConvertedID = (ConvertedID << 8);
  ConvertedID = ConvertedID + tempRXBn_EIDL;
  returnValue = ConvertedID;    
  return (returnValue);
}

/* Register ???? Standard ID ???? ???? ?? ?? */
static uint32_t convertReg2StandardCANid(uint8_t tempRXBn_SIDH, uint8_t tempRXBn_SIDL) 
{
  uint32_t returnValue = 0;
  uint32_t ConvertedID;
  
  ConvertedID = (tempRXBn_SIDH << 3);
  ConvertedID = ConvertedID + (tempRXBn_SIDL >> 5);
  returnValue = ConvertedID;
  
  return (returnValue);
}

/* CAN ID? Register? ???? ?? ?? ?? */
static void convertCANid2Reg(uint32_t tempPassedInID, uint8_t canIdType, id_reg_t *passedIdReg) 
{
  uint8_t wipSIDL = 0;
  
  if (canIdType == dEXTENDED_CAN_MSG_ID_2_0B) 
  {
    //EID0
    passedIdReg->tempEID0 = 0xFF & tempPassedInID;
    tempPassedInID = tempPassedInID >> 8;
    
    //EID8
    passedIdReg->tempEID8 = 0xFF & tempPassedInID;
    tempPassedInID = tempPassedInID >> 8;
    
    //SIDL
    wipSIDL = 0x03 & tempPassedInID;
    tempPassedInID = tempPassedInID << 3;
    wipSIDL = (0xE0 & tempPassedInID) + wipSIDL;
    wipSIDL = wipSIDL + 0x08;
    passedIdReg->tempSIDL = 0xEB & wipSIDL;
    
    //SIDH
    tempPassedInID = tempPassedInID >> 8;
    passedIdReg->tempSIDH = 0xFF & tempPassedInID;
  } 
  else
  {
    passedIdReg->tempEID8 = 0;
    passedIdReg->tempEID0 = 0;
    tempPassedInID = tempPassedInID << 5;
    passedIdReg->tempSIDL = 0xFF & tempPassedInID;
    tempPassedInID = tempPassedInID >> 8;
    passedIdReg->tempSIDH = 0xFF & tempPassedInID;
  }
}

/* Private functions body ----------------------------------------------------*/

/******************************************************************************/
/* Peripherals Interrupt Handlers --------------------------------------------*/
/******************************************************************************/
uCAN_MSG rxMessage;
void UMCP_INT_IRQHandler(void)
{
	if(EXTI_GetITStatus(UMCP_INT_EXTI_Line) != RESET)
	{
		count_int++;
		CANSPI_Receive(&rxMessage);
	}
	EXTI_ClearITPendingBit(UMCP_INT_EXTI_Line);
}
