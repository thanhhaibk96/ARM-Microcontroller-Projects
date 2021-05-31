/* Includes ------------------------------------------------------------------*/
#include "User_RCServo.h"
#include "stdbool.h"

/**
 * @defgroup Module Pin define
 * @{
 */
		/** 
		* @brief   COM Pin define 
		*/
			#define 	URC_CLK_AFIO  				  RCC_APB2Periph_AFIO	
			#define		URC_CLK_AFIO_Cmd		  	RCC_APB2PeriphClockCmd
			
			#define 	URC_PWM_PIN  				   	GPIO_Pin_6			
			#define 	URC_PWM_PORT 				  	GPIOB
			#define 	URC_PWM_CLK							RCC_APB2Periph_GPIOB						
			#define		URC_PWM_CLK_Cmd		  	 	RCC_APB2PeriphClockCmd			
			
			#define		URC_TIM									TIM4
			#define 	URC_TIM_CLK							RCC_APB1Periph_TIM4
			#define		URC_TIM_CLK_Cmd		  		RCC_APB1PeriphClockCmd
			#define 	URC_TIM_IRQn						TIM4_IRQn
			
			#define		URC_TIM_IRQHandler			TIM4_IRQHandler
			
/**
 * @}
 */
 
/* Public variables ----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private const/macros ------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Exported function body ----------------------------------------------------*/

/* Private functions body ----------------------------------------------------*/
void URC_Configure(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStruct;
	TIM_OCInitTypeDef  TIM_OCInitStruct;
	
	URC_CLK_AFIO_Cmd(URC_CLK_AFIO, ENABLE);
	
	URC_PWM_CLK_Cmd(URC_PWM_CLK,ENABLE);	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStruct.GPIO_Pin = URC_PWM_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(URC_PWM_PORT,&GPIO_InitStruct);
	
	URC_TIM_CLK_Cmd(URC_TIM_CLK,ENABLE);
  /* Time base configuration */
  TIM_TimeBaseStruct.TIM_Period = 1000 - 1;
  TIM_TimeBaseStruct.TIM_Prescaler = 720 - 1 ;
  TIM_TimeBaseStruct.TIM_ClockDivision = 0;
  TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;

  TIM_TimeBaseInit(URC_TIM, &TIM_TimeBaseStruct);

  /* PWM1 Mode configuration: Channel1 */
  TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStruct.TIM_Pulse = 80; // Angle's RC Servo = 0
  TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_High;

  TIM_OC1Init(URC_TIM, &TIM_OCInitStruct);

  TIM_OC1PreloadConfig(URC_TIM, TIM_OCPreload_Enable);

  TIM_ARRPreloadConfig(URC_TIM, ENABLE);

  /* TIM4 enable counter */
  TIM_Cmd(URC_TIM, ENABLE);
}

void URC_SwitchON(void)
{
	TIM_SetCompare1(URC_TIM,40);
}

void URC_SwitchOFF(void)
{
	TIM_SetCompare1(URC_TIM,80);
}

/******************************************************************************/
/* Peripherals Interrupt Handlers --------------------------------------------*/
/******************************************************************************/
