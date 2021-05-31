/* Includes ------------------------------------------------------------------*/
#include "my_rcservo.h"

/* Public variables ----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private const/macros ------------------------------------------------------*/

/**
 * @defgroup Module Pin define
 * @{
 */
		/** 
		* @brief   RC Servo Pin define 
		*/
					/** 
		* @brief   COM Pin define 
		*/
			
			#define 	URC_PWM_PIN  				   	GPIO_Pin_2			
			#define 	URC_PWM_PORT 				  	GPIOA
			#define 	URC_PWM_SOURCE				 	GPIO_PinSource2	
			#define 	URC_PWM_CLK							RCC_AHB1Periph_GPIOA						
			#define		URC_PWM_CLK_Cmd		  	 	RCC_AHB1PeriphClockCmd			
			
			/** 
			* @brief   Timer Module define 
			*/
			#define 	URC_TIM  				      				TIM2
			#define 	URC_TIM_AF  				      		GPIO_AF_TIM2
			#define 	URC_TIM_CLK					 	 				RCC_APB1Periph_TIM2	
			#define 	URC_TIM_CLK_Cmd    						RCC_APB1PeriphClockCmd
			#define 	URC_TIM_IRQn    							TIM2_IRQn				
			#define		URC_TIM_PreemptionPriority		0x01
			#define		URC_TIM_SubPriority						0x05		
			
			#define 	URC_TIM_IRQHandler						TIM2_IRQHandler	


/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Exported function body ----------------------------------------------------*/
void URC_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStruct;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	/* GPIOA clock enable */
  URC_PWM_CLK_Cmd(URC_PWM_CLK, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = URC_PWM_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP ;
  GPIO_Init(URC_PWM_PORT, &GPIO_InitStructure); 

  /* Connect TIM2 pins to AF */  
  GPIO_PinAFConfig(URC_PWM_PORT, URC_PWM_SOURCE, URC_TIM_AF);
	
  /* Time base configuration 50Hz*/
	URC_TIM_CLK_Cmd(URC_TIM_CLK, ENABLE);	
	TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStruct.TIM_Period = 1000 - 1;
	TIM_TimeBaseStruct.TIM_Prescaler = 1680 - 1;
	TIM_TimeBaseInit(URC_TIM, &TIM_TimeBaseStruct);

  /* PWM1 Mode configuration: Channel3 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
  TIM_OCInitStructure.TIM_Pulse = 0;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;

  TIM_OC3Init(URC_TIM, &TIM_OCInitStructure);

  TIM_OC3PreloadConfig(URC_TIM, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(URC_TIM, ENABLE);

  /* TIM3 enable counter */
  TIM_Cmd(URC_TIM, ENABLE);
}

void URC_SetAngle(float _angle)
{
	uint32_t _pwm_ontime = (uint32_t)(((1.0f/90.0f)*_angle +(1.0f/2.0f))/0.02);
	TIM_SetCompare3(URC_TIM, _pwm_ontime);
}

/* Private functions body ----------------------------------------------------*/

/******************************************************************************/
/* Peripherals Interrupt Handlers --------------------------------------------*/
/******************************************************************************/

