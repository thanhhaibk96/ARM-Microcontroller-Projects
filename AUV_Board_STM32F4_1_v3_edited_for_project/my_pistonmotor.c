/* Includes ------------------------------------------------------------------*/
#include "my_pistonmotor.h"
#include <math.h>

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
			#define 	UPIS_LDIR_PIN  				   	GPIO_Pin_3			
			#define 	UPIS_LDIR_PORT 				  	GPIOC
			#define 	UPIS_LDIR_SOURCE				 	GPIO_PinSource3	
			#define 	UPIS_LDIR_CLK							RCC_AHB1Periph_GPIOC						
			#define		UPIS_LDIR_CLK_Cmd		  	 	RCC_AHB1PeriphClockCmd	
			
			#define 	UPIS_RDIR_PIN  				   	GPIO_Pin_0			
			#define 	UPIS_RDIR_PORT 				  	GPIOA
			#define 	UPIS_RDIR_SOURCE				 	GPIO_PinSource0	
			#define 	UPIS_RDIR_CLK							RCC_AHB1Periph_GPIOA						
			#define		UPIS_RDIR_CLK_Cmd		  	 	RCC_AHB1PeriphClockCmd	
			
			#define 	UPIS_PWM_PIN  				   	GPIO_Pin_1			
			#define 	UPIS_PWM_PORT 				  	GPIOA
			#define 	UPIS_PWM_SOURCE				 		GPIO_PinSource1	
			#define 	UPIS_PWM_CLK							RCC_AHB1Periph_GPIOA						
			#define		UPIS_PWM_CLK_Cmd		  	 	RCC_AHB1PeriphClockCmd					
			
			/** 
			* @brief   Timer Module define 
			*/
			#define 	UPIS_PWM_TIM  				      			TIM2
			#define 	UPIS_PWM_TIM_AF  				      		GPIO_AF_TIM2
			#define 	UPIS_PWM_TIM_CLK					 	 			RCC_APB1Periph_TIM2	
			#define 	UPIS_PWM_TIM_CLK_Cmd    					RCC_APB1PeriphClockCmd
			#define 	UPIS_PWM_TIM_IRQn    							TIM2_IRQn				
			#define		UPIS_PWM_TIM_PreemptionPriority		0x01
			#define		UPIS_PWM_TIM_SubPriority					0x05		
			#define		UPIS_PWM_DC_PERIOD								2000.0f
			#define   UPIS_PWM_DC_MAX										1800.0f
			#define		UPIS_PWM_DC_MIN										100.0f
			#define 	UPIS_PWM_TIM_IRQHandler						TIM2_IRQHandler	
			
			/** 
			* @brief   Encoder [P]istol channel A Pin define 
			*/

			#define 	UPIS_ENCP_A_PIN  									GPIO_Pin_6
			#define 	UPIS_ENCP_A_PORT 									GPIOB
			#define 	UPIS_ENCP_A_AF										GPIO_AF_TIM4
			#define 	UPIS_ENCP_A_CLK										RCC_AHB1Periph_GPIOB
			#define 	UPIS_ENCP_A_SOURCE								GPIO_PinSource6				
			#define		UPIS_ENCP_A_CLK_Cmd		  					RCC_AHB1PeriphClockCmd

			/** 
			* @brief   Encoder [P]istol channel B Pin define 
			*/

			#define 	UPIS_ENCP_B_PIN  									GPIO_Pin_7
			#define 	UPIS_ENCP_B_PORT 									GPIOB
			#define 	UPIS_ENCP_B_AF										GPIO_AF_TIM4
			#define 	UPIS_ENCP_B_CLK										RCC_AHB1Periph_GPIOB
			#define 	UPIS_ENCP_B_SOURCE								GPIO_PinSource7					
			#define		UPIS_ENCP_B_CLK_Cmd		  					RCC_AHB1PeriphClockCmd
		/** 
			* @brief   Counter [P]istol Module define 
			*/
			#define 	UPIS_ENCP_TIM  				      			TIM4
			#define 	UPIS_ENCP_TIM_CLK					 	 			RCC_APB1Periph_TIM4	
			#define 	UPIS_ENCP_TIM_CLK_Cmd    					RCC_APB1PeriphClockCmd	
			#define 	UPIS_ENCP_RESOLUTION							500.0f
			#define 	UPIS_ENCP_MODE										4.0f
			#define   UPIS_ENCP_GEARBOX									103.0f
			#define 	UPIS_ENCP_LSB											360.0f/(UPIS_ENCM_RESOLUTION*UPIS_ENCM_MODE)
			#define		UPIS_ENCP_TIM_PreemptionPriority	0x00
			#define		UPIS_ENCP_TIM_SubPriority					0x05		
			#define 	UPIS_ENCP_TIM_IRQn    						TIM4_IRQn
			
			#define 	UPIS_ENCP_TIM_IRQHandler					TIM4_IRQHandler	
			
			/** 
			* @brief   Timer Module define 
			*/
			#define 	UPIS_TIM  				      			TIM7
			#define 	UPIS_TIM_CLK					 	 			RCC_APB1Periph_TIM7	
			#define 	UPIS_TIM_CLK_Cmd    					RCC_APB1PeriphClockCmd
			#define 	UPIS_TIM_IRQn    							TIM7_IRQn				
			#define		UPIS_TIM_PreemptionPriority		0x00
			#define		UPIS_TIM_SubPriority					0x06		
			
			#define 	UPIS_TIM_IRQHandler						TIM7_IRQHandler	

// Comment:
// Motor CW --> Increase Distance
// Motor CCW  --> Decrease Distance

/* Private variables ---------------------------------------------------------*/
// -------Calculate accumulation for Pistol Motor-------//
static int encp_mod_count = 0;
static int16_t encp_tim_cnt = 0, encp_tim_precnt = 0;
static float revolution = 0.0f; 
static const uint16_t encp_revolution_cnt = 10; // motor turn 10 rounds --> jump into timer interrupt
static const float visme_step = 5.0f; // 5mm/round 
static const float max_position = 170.0f; // mm
static const float min_position = 0.0f;
static float piston_position = 0.0f;
static uint8_t motor_direction = 0;
static bool update_PID = 0;

// 	PID implementation:
//    u[k] = -A1/A0*u[k-1] - A2/A0*u[k-2] + B0/A0*e[k] + B1/A0*e[k-1] + B2/A0*e[k-2]
//    B0 = Kp*(1 + N*Ts) + Ki*Ts*(1 + N*Ts) + Kd*N
//    B1 = -(Kp*(2 + N*Ts) + Ki*Ts + 2*Kd*N)
//    B2 = Kp + Kd*N
//    A0 = 1 + N*Ts
//    A1 = -(2 + N*Ts)
//    A2 = 1

static float Desired_Position;
static float Desired_Position_LPF;
static float LPF_Beta_DPosition;
static float Kp;
static float Ki;
static float Kd;
static float Deadband;
static float e0, e1, e2;
static float u0, u1, u2;
static float Ts; // Sample Time
static float N; // Coefficient of LPF filter for output signal
// Coefficient
static float B0;
static float B1;
static float B2;
static float A0; 
static float A1; 
static float A2;

/* Private function prototypes -----------------------------------------------*/
void UPIS_PIDCaculation(void);

/* Exported function body ----------------------------------------------------*/
void UPIS_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStruct;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	TIM_ICInitTypeDef TIM_ICInitStructure;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	/* DIRECTION enable */
  UPIS_RDIR_CLK_Cmd(UPIS_RDIR_CLK, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = UPIS_RDIR_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(UPIS_RDIR_PORT, &GPIO_InitStructure); 
	
	UPIS_LDIR_CLK_Cmd(UPIS_LDIR_CLK, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = UPIS_LDIR_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_DOWN;
  GPIO_Init(UPIS_LDIR_PORT, &GPIO_InitStructure); 
	
	/* GPIOA clock enable */
  UPIS_PWM_CLK_Cmd(UPIS_PWM_CLK, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = UPIS_PWM_PIN;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL ;
  GPIO_Init(UPIS_PWM_PORT, &GPIO_InitStructure); 

  /* Connect TIM2 pins to AF */  
  GPIO_PinAFConfig(UPIS_PWM_PORT, UPIS_PWM_SOURCE, UPIS_PWM_TIM_AF);
	
  /* Time base configuration 2KHz*/
	UPIS_PWM_TIM_CLK_Cmd(UPIS_PWM_TIM_CLK, ENABLE);	
	TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStruct.TIM_Period =  (uint32_t)UPIS_PWM_DC_PERIOD - 1;
	TIM_TimeBaseStruct.TIM_Prescaler = 21 - 1;
	TIM_TimeBaseInit(UPIS_PWM_TIM, &TIM_TimeBaseStruct);

  /* PWM1 Mode configuration: Channel2 */
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
  TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
	TIM_OCInitStructure.TIM_OCIdleState = TIM_OCIdleState_Reset;
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
  TIM_OCInitStructure.TIM_Pulse = 0;  
	
	TIM_OC2Init(UPIS_PWM_TIM, &TIM_OCInitStructure);

  TIM_OC2PreloadConfig(UPIS_PWM_TIM, TIM_OCPreload_Enable);
	TIM_ARRPreloadConfig(UPIS_PWM_TIM, ENABLE);
	
	TIM_CtrlPWMOutputs(UPIS_PWM_TIM,ENABLE);
  /* TIM2 enable counter */
  TIM_Cmd(UPIS_PWM_TIM, ENABLE);
	
	/////////////////////////Configure for Pistol Encoder
	UPIS_ENCP_A_CLK_Cmd(UPIS_ENCP_A_CLK,ENABLE);	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = UPIS_ENCP_A_PIN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
	GPIO_Init(UPIS_ENCP_A_PORT,&GPIO_InitStructure);
	
	UPIS_ENCP_B_CLK_Cmd(UPIS_ENCP_B_CLK,ENABLE);	
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Pin = UPIS_ENCP_B_PIN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Speed = GPIO_High_Speed;
	GPIO_Init(UPIS_ENCP_B_PORT,&GPIO_InitStructure);
	
	GPIO_PinAFConfig(UPIS_ENCP_A_PORT, UPIS_ENCP_A_SOURCE, UPIS_ENCP_A_AF);
	GPIO_PinAFConfig(UPIS_ENCP_B_PORT, UPIS_ENCP_B_SOURCE, UPIS_ENCP_B_AF);
	
	UPIS_ENCP_TIM_CLK_Cmd(UPIS_ENCP_TIM_CLK, ENABLE);	
	TIM_TimeBaseStructInit(&TIM_TimeBaseStruct);
//	TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
//	TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
//	TIM_TimeBaseStruct.TIM_Period = ((uint16_t)UPIS_ENCP_RESOLUTION*(uint16_t)UPIS_ENCP_MODE*encp_revolution_cnt) - 1;
//	TIM_TimeBaseStruct.TIM_Period = 1000 - 1;
//	TIM_TimeBaseStruct.TIM_Prescaler = 1 - 1;
//	TIM_TimeBaseStruct.TIM_RepetitionCounter = 0;
	TIM_TimeBaseInit(UPIS_ENCP_TIM, &TIM_TimeBaseStruct);		
	
  TIM_EncoderInterfaceConfig(UPIS_ENCP_TIM, TIM_EncoderMode_TI12, TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);	//Mode x4;
	
	TIM_ICStructInit(&TIM_ICInitStructure);
	TIM_ICInitStructure.TIM_Channel = TIM_Channel_1|TIM_Channel_2;
	TIM_ICInitStructure.TIM_ICFilter = 6;
	TIM_ICInitStructure.TIM_ICPrescaler = TIM_ICPSC_DIV1;
	TIM_ICInitStructure.TIM_ICSelection = TIM_ICSelection_DirectTI;		
	TIM_ICInit(UPIS_ENCP_TIM, &TIM_ICInitStructure);

	TIM_Cmd(UPIS_ENCP_TIM, DISABLE);
	
	UPIS_TIM_CLK_Cmd(UPIS_TIM_CLK, ENABLE);	
	TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStruct.TIM_Period = 840 - 1;
	TIM_TimeBaseStruct.TIM_Prescaler = 1000 - 1;
	TIM_TimeBaseInit(UPIS_TIM, &TIM_TimeBaseStruct);
	
	TIM_ITConfig(UPIS_TIM, TIM_IT_Update, DISABLE);
	
	NVIC_InitStruct.NVIC_IRQChannel = UPIS_TIM_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = UPIS_TIM_PreemptionPriority;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = UPIS_TIM_SubPriority;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	TIM_Cmd(UPIS_TIM, DISABLE);
	
	// Init PID Parameter
	Desired_Position = 0.0f;
	Desired_Position_LPF = 0.0f;
	LPF_Beta_DPosition = 0.05f;
	Kp = 10.0;
	Ki = 0;
	Kd = 0.1;	
	Ts = 0.01; // 0.01s
	N = 20.0;
	e0 = 0.0;
	e1 = 0.0;
	e2 = 0.0;
	u0 = 0.0;
	u1 = 0.0;
	u2 = 0.0;
	Deadband = 0.2; // 0.2 mm
	
	B0 = 0.0f;
	B1 = 0.0f;
	B2 = 0.0f;
	A0 = 0.0f;
	A1 = 0.0f;
	A2 = 1.0f;
}

void UPIS_SetDuty(float _duty, UPIS_Direction _direction)
{
	if(_direction == PISTON_CW)
	{
		motor_direction = PISTON_CW;
		GPIO_SetBits(UPIS_RDIR_PORT, UPIS_RDIR_PIN);
		GPIO_ResetBits(UPIS_LDIR_PORT, UPIS_LDIR_PIN);
		TIM_CCxCmd(UPIS_PWM_TIM,TIM_Channel_2,TIM_CCx_Enable);
	}
	else if(_direction == PISTON_CCW)
	{
		motor_direction = PISTON_CCW;
		GPIO_ResetBits(UPIS_RDIR_PORT, UPIS_RDIR_PIN);
		GPIO_SetBits(UPIS_LDIR_PORT, UPIS_LDIR_PIN);
		TIM_CCxCmd(UPIS_PWM_TIM,TIM_Channel_2,TIM_CCx_Enable);
	}
	
	uint16_t Duty = (uint16_t)(_duty*(float)UPIS_PWM_DC_MAX/100.0);
	if((Duty >= UPIS_PWM_DC_MIN)&&(Duty <= UPIS_PWM_DC_MAX))
	{
		TIM_SetCompare2(UPIS_PWM_TIM, Duty);
	}
	else if(Duty > UPIS_PWM_DC_MAX)
	{
		TIM_SetCompare2(UPIS_PWM_TIM, UPIS_PWM_DC_MAX);
	}
	else if(Duty < UPIS_PWM_DC_MIN)
	{
		TIM_CCxCmd(UPIS_PWM_TIM,TIM_Channel_2,TIM_CCx_Disable);
		TIM_SetCompare2(UPIS_PWM_TIM, 0);
		GPIO_ResetBits(UPIS_RDIR_PORT, UPIS_RDIR_PIN);
		GPIO_ResetBits(UPIS_LDIR_PORT, UPIS_LDIR_PIN);
	}
}

void UPIS_StartEncoder(void)
{
	TIM_SetCounter(UPIS_ENCP_TIM,0);
	TIM_ITConfig(UPIS_TIM, TIM_IT_Update, ENABLE);
	TIM_Cmd(UPIS_ENCP_TIM, ENABLE);
	TIM_Cmd(UPIS_TIM, ENABLE);
}

void UPIS_StopEncoder(void)
{
	TIM_ITConfig(UPIS_TIM, TIM_IT_Update, DISABLE);
	TIM_Cmd(UPIS_ENCP_TIM, DISABLE);
	TIM_Cmd(UPIS_TIM, DISABLE);
	TIM_SetCounter(UPIS_ENCP_TIM,0);
	TIM_SetCounter(UPIS_TIM,0);
	encp_tim_cnt = 0;
	encp_tim_precnt = 0;
	encp_mod_count = 0;
	revolution = 0.0f;
	piston_position = 0.0f;
}

float UPIS_GetCurrentPosition(void)
{
	return piston_position;
}

/* Private functions body ----------------------------------------------------*/
void UPIS_SetGain(float kp, float ki, float kd)
{
	Kp = kp;
	Ki = ki;
	Kd = kd;
}

void UPIS_SetDesiredPosition(float position)
{
	Desired_Position = position;
}

float UPIS_GetLPFDesiredPosition(void)
{
	return Desired_Position_LPF;
}

void UPIS_PIDCaculation(void)
{
	if(Desired_Position_LPF < Desired_Position)
	{
		Desired_Position_LPF += LPF_Beta_DPosition;
		if(Desired_Position_LPF > Desired_Position) Desired_Position_LPF = Desired_Position;
	}
	else if (Desired_Position_LPF > Desired_Position)
	{
		Desired_Position_LPF -= LPF_Beta_DPosition;
		if(Desired_Position_LPF < Desired_Position) Desired_Position_LPF = Desired_Position;
	}	
	
//	if(fabs(Desired_Position_LPF - Desired_Position) >= 0.5)
//		Desired_Position_LPF = Desired_Position_LPF - (LPF_Beta_DPosition*(Desired_Position_LPF - Desired_Position));
//	else
//		Desired_Position_LPF = Desired_Position;
	
	B0 = Kp*(1+N*Ts) + Ki*Ts*(1+N*Ts) + Kd*N;
	B1 = -(Kp*(2+N*Ts) + Ki*Ts + 2*Kd*N);
	B2 = Kp+ Kd*N;
	A0 = 1 + N*Ts;
	A1 = -(2 + N*Ts);
	A2 = 1;
	
	e2 = e1;
	e1 = e0;
	u2 = u1;
	u1 = u0;
	e0 = fabs(Desired_Position_LPF - piston_position);
	
	if(fabs(e0) > Deadband)
	{
		u0 = -A1*u1/A0 - A2*u2/A0 + B0*e0/A0 +  B1*e1/A0 + B2*e2/A0;
		if(u0 > 100.0) u0 = 100.0;
		else if(u0 < 0.0) u0 = 0.0;
	}
	else
	{
		u0 = 0.0f;
	}
	
	if(Desired_Position_LPF >= piston_position)
	{
		UPIS_SetDuty(u0, PISTON_CW);
	}
	else
	{
		UPIS_SetDuty(u0, PISTON_CCW);
	}
}

bool UPIS_GetFlagUpdatePID(void)
{
	return update_PID;
}

void UPIS_ResetFlagUpdatePID(void)
{
	update_PID = false;
}

/******************************************************************************/
/* Peripherals Interrupt Handlers --------------------------------------------*/
//void UPIS_ENCP_TIM_IRQHandler(void)
//{
//	if(TIM_GetITStatus(UPIS_ENCP_TIM, TIM_IT_Update) != RESET)
//	{
//		if(motor_direction == PISTON_CW) revolution++;
//		else if(motor_direction == PISTON_CCW) revolution--;
//		TIM_ClearITPendingBit(UPIS_ENCP_TIM, TIM_IT_Update);
//	}
//}

void UPIS_TIM_IRQHandler(void)
{
	if(TIM_GetITStatus(UPIS_TIM, TIM_IT_Update) != RESET)
	{		
		//----------- Pistol----------//
		encp_tim_cnt = ((int16_t)TIM_GetCounter(UPIS_ENCP_TIM)) + encp_tim_precnt;
		if(encp_tim_cnt / ((uint16_t)UPIS_ENCP_RESOLUTION*(uint16_t)UPIS_ENCP_MODE*encp_revolution_cnt))
		{
			if(encp_tim_cnt < 0)
			{
				encp_mod_count--;
				encp_tim_precnt = ((uint16_t)UPIS_ENCP_RESOLUTION*(uint16_t)UPIS_ENCP_MODE*encp_revolution_cnt) + encp_tim_cnt;
			}
			else
			{
				encp_mod_count++;
				encp_tim_precnt = encp_tim_cnt - ((uint16_t)UPIS_ENCP_RESOLUTION*(uint16_t)UPIS_ENCP_MODE*encp_revolution_cnt);
			}
			TIM_SetCounter(UPIS_ENCP_TIM, 0);
		}
		revolution = ((float)encp_mod_count*encp_revolution_cnt + ((float)encp_tim_cnt)/(UPIS_ENCP_RESOLUTION*UPIS_ENCP_MODE))/UPIS_ENCP_GEARBOX;
		piston_position = revolution*visme_step;
		
		update_PID = true;
		
		TIM_ClearITPendingBit(UPIS_TIM, TIM_IT_Update);
	}
}