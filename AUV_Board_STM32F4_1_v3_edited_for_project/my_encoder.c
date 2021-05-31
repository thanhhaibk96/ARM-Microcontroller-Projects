/* Includes ------------------------------------------------------------------*/
#include "my_encoder.h"

/* Public variables ----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private const/macros ------------------------------------------------------*/
/**
 * @defgroup Module Pin define
 * @{
 */
					/** 
					* @brief   Encoder [M]ass Shifter channel A Pin define 
					*/
					#define 	UENC_ENCM_A_PIN  									GPIO_Pin_4
					#define 	UENC_ENCM_A_PORT 									GPIOB
					#define 	UENC_ENCM_A_AF										GPIO_AF_TIM3
					#define 	UENC_ENCM_A_CLK										RCC_AHB1Periph_GPIOB
					#define 	UENC_ENCM_A_SOURCE								GPIO_PinSource4					
					#define		UENC_ENCM_A_CLK_Cmd		  					RCC_AHB1PeriphClockCmd


					/** 
					* @brief   Encoder [M]ass Shifter channel B Pin define 
					*/
					#define 	UENC_ENCM_B_PIN  									GPIO_Pin_5
					#define 	UENC_ENCM_B_PORT 									GPIOB
					#define 	UENC_ENCM_B_AF										GPIO_AF_TIM3
					#define 	UENC_ENCM_B_CLK										RCC_AHB1Periph_GPIOB
					#define 	UENC_ENCM_B_SOURCE								GPIO_PinSource5					
					#define		UENC_ENCM_B_CLK_Cmd		  					RCC_AHB1PeriphClockCmd
					
					
					/** 
					* @brief   Encoder [P]istol channel A Pin define 
					*/

					#define 	UENC_ENCP_A_PIN  									GPIO_Pin_6
					#define 	UENC_ENCP_A_PORT 									GPIOB
					#define 	UENC_ENCP_A_AF										GPIO_AF_TIM4
					#define 	UENC_ENCP_A_CLK										RCC_AHB1Periph_GPIOB
					#define 	UENC_ENCP_A_SOURCE								GPIO_PinSource6				
					#define		UENC_ENCP_A_CLK_Cmd		  					RCC_AHB1PeriphClockCmd

					/** 
					* @brief   Encoder [P]istol channel B Pin define 
					*/

					#define 	UENC_ENCP_B_PIN  									GPIO_Pin_7
					#define 	UENC_ENCP_B_PORT 									GPIOB
					#define 	UENC_ENCP_B_AF										GPIO_AF_TIM4
					#define 	UENC_ENCP_B_CLK										RCC_AHB1Periph_GPIOB
					#define 	UENC_ENCP_B_SOURCE								GPIO_PinSource7					
					#define		UENC_ENCP_B_CLK_Cmd		  					RCC_AHB1PeriphClockCmd
				
/**
 * @defgroup Encoder module define
 * @{
 */		
			/** 
			* @brief   Counter [M]ass Shifter Module define 
			*/
			#define 	UENC_ENCM_TIM  				      			TIM3
			#define 	UENC_ENCM_TIM_CLK					 	 			RCC_APB1Periph_TIM3	
			#define 	UENC_ENCM_TIM_CLK_Cmd    					RCC_APB1PeriphClockCmd
			//#define 	UENC_ENCM_TIM_IRQn    						xxx
			//#define 	UENC_ENCM_TIM_IRQHandler					xxx										
			#define 	UENC_ENCM_RESOLUTION							1024.0f	 
			#define 	UENC_ENCM_MODE										4.0f
			#define   UENC_ENCM_GEARBOX									43.0f
			#define 	UENC_ENCM_LSB											360.0f/(UENC_ENCM_RESOLUTION*UENC_ENCM_MODE)
			
					/** 
			* @brief   Counter [P]istol Module define 
			*/
			#define 	UENC_ENCP_TIM  				      			TIM4
			#define 	UENC_ENCP_TIM_CLK					 	 			RCC_APB1Periph_TIM4	
			#define 	UENC_ENCP_TIM_CLK_Cmd    					RCC_APB1PeriphClockCmd
			//#define 	UENC_ENCP_TIM_IRQn    						xxx
			//#define 	UENC_ENCP_TIM_IRQHandler					xxx			
			#define 	UENC_ENCP_RESOLUTION							500.0f
			#define 	UENC_ENCP_MODE										4.0f
			#define   UENC_ENCP_GEARBOX									81.0f
			#define 	UENC_ENCP_LSB											360.0f/(UENC_ENCM_RESOLUTION*UENC_ENCM_MODE)
			
			/** 
			* @brief   Timer Module define 
			*/
			#define 	UENC_TIM  				      			TIM7
			#define 	UENC_TIM_CLK					 	 			RCC_APB1Periph_TIM7	
			#define 	UENC_TIM_CLK_Cmd    					RCC_APB1PeriphClockCmd
			#define 	UENC_TIM_IRQn    							TIM7_IRQn				
			#define		UENC_TIM_PreemptionPriority		0x00
			#define		UENC_TIM_SubPriority					0x05		
			
			#define 	UENC_TIM_IRQHandler						TIM7_IRQHandler	
			
/* Private variables ---------------------------------------------------------*/
static Encoder_StatusTypeDef Encoder_Status;

// -------Calculate accumulation for Mass Motor-------//
static int encm_mod_count = 0;
static int16_t encm_tim_cnt = 0, encm_tim_precnt = 0;
static const uint16_t encm_revolution_cnt = 5;

// -------Calculate accumulation for Pistol Motor-------//
static int encp_mod_count = 0;
static int16_t encp_tim_cnt = 0, encp_tim_precnt = 0;
static const uint16_t encp_revolution_cnt = 5;
/* Private function prototypes -----------------------------------------------*/

/* Exported function body ----------------------------------------------------*/
void UENC_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	//Configure for Mass Shifter Encoder
	UENC_ENCM_A_CLK_Cmd(UENC_ENCM_A_CLK,ENABLE);	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = UENC_ENCM_A_PIN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	GPIO_Init(UENC_ENCM_A_PORT,&GPIO_InitStruct);
	
	UENC_ENCM_B_CLK_Cmd(UENC_ENCM_B_CLK,ENABLE);	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = UENC_ENCM_B_PIN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	GPIO_Init(UENC_ENCM_B_PORT,&GPIO_InitStruct);
	
	GPIO_PinAFConfig(UENC_ENCM_A_PORT, UENC_ENCM_A_SOURCE, UENC_ENCM_A_AF);
	GPIO_PinAFConfig(UENC_ENCM_B_PORT, UENC_ENCM_B_SOURCE, UENC_ENCM_B_AF);
	
	UENC_ENCM_TIM_CLK_Cmd(UENC_ENCM_TIM_CLK, ENABLE);	
	TIM_TimeBaseStructInit(&TIM_TimeBaseStruct);	
	TIM_TimeBaseInit(UENC_ENCM_TIM, &TIM_TimeBaseStruct);			
	TIM_EncoderInterfaceConfig(UENC_ENCM_TIM, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_Cmd(UENC_ENCM_TIM, DISABLE);
	
	//Configure for Pistol Encoder
	UENC_ENCP_A_CLK_Cmd(UENC_ENCP_A_CLK,ENABLE);	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = UENC_ENCP_A_PIN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	GPIO_Init(UENC_ENCP_A_PORT,&GPIO_InitStruct);
	
	UENC_ENCP_B_CLK_Cmd(UENC_ENCP_B_CLK,ENABLE);	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = UENC_ENCP_B_PIN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	GPIO_Init(UENC_ENCP_B_PORT,&GPIO_InitStruct);
	
	GPIO_PinAFConfig(UENC_ENCP_A_PORT, UENC_ENCP_A_SOURCE, UENC_ENCP_A_AF);
	GPIO_PinAFConfig(UENC_ENCP_B_PORT, UENC_ENCP_B_SOURCE, UENC_ENCP_B_AF);
	
	UENC_ENCP_TIM_CLK_Cmd(UENC_ENCP_TIM_CLK, ENABLE);	
	TIM_TimeBaseStructInit(&TIM_TimeBaseStruct);	
	TIM_TimeBaseInit(UENC_ENCP_TIM, &TIM_TimeBaseStruct);			
	TIM_EncoderInterfaceConfig(UENC_ENCP_TIM, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
	TIM_Cmd(UENC_ENCP_TIM, DISABLE);
	
	UENC_TIM_CLK_Cmd(UENC_TIM_CLK, ENABLE);	
	TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStruct.TIM_Period = 840 - 1;
	TIM_TimeBaseStruct.TIM_Prescaler = 1000 - 1;
	TIM_TimeBaseInit(UENC_TIM, &TIM_TimeBaseStruct);
	
	TIM_ITConfig(UENC_TIM, TIM_IT_Update, DISABLE);
	
	NVIC_InitStruct.NVIC_IRQChannel = UENC_TIM_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = UENC_TIM_PreemptionPriority;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = UENC_TIM_SubPriority;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_Cmd(UENC_TIM, DISABLE);
}

void UENC_StartEncoder(void)
{
	TIM_Cmd(UENC_ENCM_TIM, ENABLE);
	TIM_Cmd(UENC_ENCP_TIM, ENABLE);
	TIM_ITConfig(UENC_TIM, TIM_IT_Update, ENABLE);
	TIM_Cmd(UENC_TIM, ENABLE);
}

void UENC_StopEncoder(void)
{
	TIM_Cmd(UENC_ENCM_TIM, DISABLE);
	TIM_Cmd(UENC_ENCP_TIM, DISABLE);
	TIM_SetCounter(UENC_ENCM_TIM,0);
	TIM_SetCounter(UENC_ENCP_TIM,0);
}

float UENC_GetNbrRoundMass(void)
{		
	return Encoder_Status.Mass_Encoder;
}

float UENC_GetNbrRoundPistol(void)
{
	return Encoder_Status.Pistol_Encoder;
}

/* Private functions body ----------------------------------------------------*/

/******************************************************************************/
/* Peripherals Interrupt Handlers --------------------------------------------*/
/******************************************************************************/
void UENC_TIM_IRQHandler(void)
{
	if(TIM_GetITStatus(UENC_TIM, TIM_IT_Update) != RESET)
	{
		//----------- Mass Shifter----------//
		encm_tim_cnt = ((int16_t)TIM_GetCounter(UENC_ENCM_TIM)) + encm_tim_precnt;
		if(encm_tim_cnt / ((uint16_t)UENC_ENCM_RESOLUTION*(uint16_t)UENC_ENCM_MODE*encm_revolution_cnt))
		{
			if(encm_tim_cnt < 0)
			{
				encm_mod_count--;
				encm_tim_precnt = ((uint16_t)UENC_ENCM_RESOLUTION*(uint16_t)UENC_ENCM_MODE*encm_revolution_cnt) + encm_tim_cnt;
			}
			else
			{
				encm_mod_count++;
				encm_tim_precnt = encm_tim_cnt - ((uint16_t)UENC_ENCM_RESOLUTION*(uint16_t)UENC_ENCM_MODE*encm_revolution_cnt);
			}
			TIM_SetCounter(UENC_ENCM_TIM, 0);
		}
		
		Encoder_Status.Mass_Encoder = ((float)encm_mod_count*5.0f + ((float)encm_tim_cnt)/(UENC_ENCM_RESOLUTION*UENC_ENCM_MODE))/UENC_ENCM_GEARBOX;
		
		//----------- Pistol----------//
		encp_tim_cnt = ((int16_t)TIM_GetCounter(UENC_ENCP_TIM)) + encp_tim_precnt;
		if(encp_tim_cnt / ((uint16_t)UENC_ENCP_RESOLUTION*(uint16_t)UENC_ENCP_MODE*encp_revolution_cnt))
		{
			if(encp_tim_cnt < 0)
			{
				encp_mod_count--;
				encp_tim_precnt = ((uint16_t)UENC_ENCP_RESOLUTION*(uint16_t)UENC_ENCP_MODE*encp_revolution_cnt) + encp_tim_cnt;
			}
			else
			{
				encp_mod_count++;
				encp_tim_precnt = encp_tim_cnt - ((uint16_t)UENC_ENCP_RESOLUTION*(uint16_t)UENC_ENCP_MODE*encp_revolution_cnt);
			}
			TIM_SetCounter(UENC_ENCP_TIM, 0);
		}
	
		Encoder_Status.Pistol_Encoder = ((float)encp_mod_count*5.0f + ((float)encp_tim_cnt)/(UENC_ENCP_RESOLUTION*UENC_ENCP_MODE))/UENC_ENCP_GEARBOX;

		TIM_ClearITPendingBit(UENC_TIM, TIM_IT_Update);
	}
}
