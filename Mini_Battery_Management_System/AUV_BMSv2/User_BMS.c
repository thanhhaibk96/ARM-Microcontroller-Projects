/* Includes ------------------------------------------------------------------*/
#include "User_BMS.h"
#include "stdbool.h"
#include "User_FlashData.h"

/**
 * @defgroup Module Pin define
 * @{
 */
		/** 
		* @brief   BMS Pin define 
		*/
			#define 	UBMS_CLK_AFIO  				  RCC_APB2Periph_AFIO	
			#define		UBMS_CLK_AFIO_Cmd		  	RCC_APB2PeriphClockCmd
		
			#define 	UBMS_ADCI_PIN  				   	GPIO_Pin_1			
			#define 	UBMS_ADCI_PORT 				  	GPIOB
			#define 	UBMS_ADC_CLK							RCC_APB2Periph_GPIOB				
			#define		UBMS_ADC_CLK_Cmd		  	 	RCC_APB2PeriphClockCmd
				
			#define		UBMS_ADC_ADC									ADC1
			#define 	UBMS_ADC_PCLK2_CLK						RCC_PCLK2_Div4
			#define 	UBMS_ADC_ADC_CLK							RCC_APB2Periph_ADC1
			#define		UBMS_ADC_ADC_CLK_Cmd		  		RCC_APB2PeriphClockCmd
			#define 	UBMS_ADC_ADC_IRQn							ADC1_2_IRQn
			#define		UBMS_ADC_ADC_PreemptionPriority		0x00
			#define		UBMS_ADC_ADC_SubPriority					0x01

			#define 	UBMS_ADCU_PIN  				   	GPIO_Pin_3
			#define 	UBMS_ADCU_PORT 				  	GPIOA
			
			#define   UBMS_DMA_Channel				DMA1_Channel1
			#define		UBMS_DMA_CLK_Cmd		  	RCC_AHBPeriphClockCmd
			#define 	UBMS_DMA_CLK						RCC_AHBPeriph_DMA1
			#define		UBMS_DMA_Channel_IRQn		DMA1_Channel1_IRQn
			#define		UBMS_DMA_PreemptionPriority		0x00
			#define		UBMS_DMA_SubPriority					0x01
			
			#define		UBMS_DMA_Stream_IRQHandler		DMA1_Channel1_IRQHandler			
			#define		UBMS_ADC_IRQHandler			ADC1_2_IRQHandler
			
			/** 
			* @brief   Timer Module define 
			*/
			#define 	UBMS_TIM  				      				TIM4
			#define 	UBMS_TIM_CLK					 	 				RCC_APB1Periph_TIM4	
			#define 	UBMS_TIM_CLK_Cmd    						RCC_APB1PeriphClockCmd
			#define 	UBMS_TIM_IRQn    								TIM4_IRQn				
			#define		UBMS_TIM_PreemptionPriority			0x00
			#define		UBMS_TIM_SubPriority						0x02		
			
			#define 	UBMS_TIM_IRQHandler							TIM4_IRQHandler				

			#define 	UBMS_LEDRED_PIN  				   			GPIO_Pin_0			
			#define 	UBMS_LEDRED_PORT 				  			GPIOB
			#define 	UBMS_LEDRED_CLK									RCC_APB2Periph_GPIOB						
			#define		UBMS_LEDRED_CLK_Cmd		  	 			RCC_APB2PeriphClockCmd	
			
			#define 	UBMS_LEDORANGE_PIN  				   	GPIO_Pin_7			
			#define 	UBMS_LEDORANGE_PORT 				  	GPIOA
			#define 	UBMS_LEDORANGE_CLK							RCC_APB2Periph_GPIOA						
			#define		UBMS_LEDORANGE_CLK_Cmd		  	 	RCC_APB2PeriphClockCmd	
			
			#define 	UBMS_LEDYELLOW_PIN  				   	GPIO_Pin_6			
			#define 	UBMS_LEDYELLOW_PORT 				  	GPIOA
			#define 	UBMS_LEDYELLOW_CLK							RCC_APB2Periph_GPIOA						
			#define		UBMS_LEDYELLOW_CLK_Cmd		  	 	RCC_APB2PeriphClockCmd	
			
			#define 	UBMS_LEDGREEN_PIN  				   		GPIO_Pin_5			
			#define 	UBMS_LEDGREEN_PORT 				  		GPIOA
			#define 	UBMS_LEDGREEN_CLK								RCC_APB2Periph_GPIOA						
			#define		UBMS_LEDGREEN_CLK_Cmd		  	 		RCC_APB2PeriphClockCmd	
			
			#define 	UBMS_LEDALL_PIN  				   			(GPIO_Pin_0|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7)
			
			
/**
 * @}
 */
 
/* Public variables ----------------------------------------------------------*/
bool first_Status = true;
int RawData;
float SmoothData;
float LPF_Beta = 0.025;
/* Private types -------------------------------------------------------------*/
static const float SENSITIVITY = 100; // mV/A
static const float DEVIDE_REGISTER = 0.11108812577;

static uint16_t ADC_ConvertedValue[20];
uint16_t ADC_ConvertedValue_debug[20];
static UBMS_Status_Typedef UBMS_Status;

static float _total_time_convert = 0.0f; // Tconv = Tsamp + 12.5 cycles // 1 cycle = 1/18MHz
static float  vref = 3.2640f;
float tempa=0,tempcurrent=0,tempcurrentV=0;
static float _tempADC1 = 0.0f, _tempADC2 = 0.0f;

float tempadcvalue[4];
static uint8_t _config_status = 1, _toggle_leds = 0x00;
uint8_t _config_status_dis =0;

static float _initial_ACS712_Voltage = 0.0f;
static bool _calib_done = false;
static const float _calib_current_value = 200.0f; // mAh
static uint8_t _calib_count = 10;
static float _calib_ADCI = 0.0f;
/* Private const/macros ------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
void UBMS_ToggleLeds(uint16_t GPIO_Pin);
void UBMS_Calibration(void);

/* Exported function body ----------------------------------------------------*/

/* Private functions body ----------------------------------------------------*/
void UBMS_Configure(void)
{
	UBMS_Status.BatteryUsed=0;
	GPIO_InitTypeDef GPIO_InitStruct;
	ADC_InitTypeDef ADC_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	DMA_InitTypeDef DMA_InitStruct;
	TIM_TimeBaseInitTypeDef  TIM_TimeBaseStruct;
	
	UBMS_CLK_AFIO_Cmd(UBMS_CLK_AFIO, ENABLE);
	
	UBMS_LEDRED_CLK_Cmd(UBMS_LEDRED_CLK,ENABLE);	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = UBMS_LEDRED_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(UBMS_LEDRED_PORT,&GPIO_InitStruct);
	
	UBMS_LEDORANGE_CLK_Cmd(UBMS_LEDORANGE_CLK,ENABLE);	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = UBMS_LEDORANGE_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(UBMS_LEDORANGE_PORT,&GPIO_InitStruct);
	
	UBMS_LEDYELLOW_CLK_Cmd(UBMS_LEDYELLOW_CLK,ENABLE);	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = UBMS_LEDYELLOW_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(UBMS_LEDYELLOW_PORT,&GPIO_InitStruct);
	
	UBMS_LEDGREEN_CLK_Cmd(UBMS_LEDGREEN_CLK,ENABLE);	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin = UBMS_LEDGREEN_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(UBMS_LEDGREEN_PORT,&GPIO_InitStruct);
	
	UBMS_ADC_CLK_Cmd(UBMS_ADC_CLK,ENABLE);	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStruct.GPIO_Pin = UBMS_ADCI_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(UBMS_ADCI_PORT,&GPIO_InitStruct);
	
	UBMS_ADC_CLK_Cmd(UBMS_ADC_CLK,ENABLE);	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AIN;
	GPIO_InitStruct.GPIO_Pin = UBMS_ADCU_PIN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(UBMS_ADCU_PORT,&GPIO_InitStruct);
	
	UBMS_TIM_CLK_Cmd(UBMS_TIM_CLK, ENABLE);	
	TIM_TimeBaseStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseStruct.TIM_Period = 72 - 1;
	TIM_TimeBaseStruct.TIM_Prescaler = 1000 - 1;
	TIM_TimeBaseInit(UBMS_TIM, &TIM_TimeBaseStruct);
	
	TIM_ITConfig(UBMS_TIM, TIM_IT_Update, ENABLE);
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStruct.NVIC_IRQChannel = UBMS_TIM_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = UBMS_TIM_PreemptionPriority;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = UBMS_TIM_SubPriority;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	
	UBMS_DMA_CLK_Cmd(UBMS_DMA_CLK,ENABLE);
	DMA_DeInit(UBMS_DMA_Channel);
	DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)(&(ADC1 -> DR));
	DMA_InitStruct.DMA_MemoryBaseAddr = (uint32_t)&ADC_ConvertedValue;
	DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStruct.DMA_BufferSize = 20;
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
	DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;
	DMA_InitStruct.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(UBMS_DMA_Channel,&DMA_InitStruct);	
	DMA_ITConfig(UBMS_DMA_Channel,DMA_IT_TC,ENABLE);

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	NVIC_InitStruct.NVIC_IRQChannel = UBMS_DMA_Channel_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = UBMS_DMA_PreemptionPriority;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = UBMS_DMA_SubPriority;
	NVIC_Init(&NVIC_InitStruct);
	DMA_Cmd(UBMS_DMA_Channel,ENABLE);	
	//-------Configure Current Measurement-------//
	RCC_ADCCLKConfig(UBMS_ADC_PCLK2_CLK);
	UBMS_ADC_ADC_CLK_Cmd(UBMS_ADC_ADC_CLK,ENABLE);	
	ADC_InitStruct.ADC_Mode = ADC_Mode_Independent;
	ADC_InitStruct.ADC_ScanConvMode = ENABLE;
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE;
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_NbrOfChannel = 2;
	ADC_Init(UBMS_ADC_ADC, &ADC_InitStruct);	
	ADC_RegularChannelConfig(UBMS_ADC_ADC,ADC_Channel_3,1,ADC_SampleTime_239Cycles5);
	ADC_RegularChannelConfig(UBMS_ADC_ADC,ADC_Channel_9,2,ADC_SampleTime_239Cycles5);
	
	ADC_Cmd(UBMS_ADC_ADC,ENABLE);	
	ADC_DMACmd(UBMS_ADC_ADC,ENABLE);
  /* Enable ADC1 reset calibration register */   
  ADC_ResetCalibration(UBMS_ADC_ADC);
  /* Check the end of ADC1 reset calibration register */
  while(ADC_GetResetCalibrationStatus(UBMS_ADC_ADC));
  /* Start ADC1 calibration */
  ADC_StartCalibration(UBMS_ADC_ADC);
  /* Check the end of ADC1 calibration */
  while(ADC_GetCalibrationStatus(UBMS_ADC_ADC));
	
	ADC_SoftwareStartConvCmd(UBMS_ADC_ADC, ENABLE);
	
	TIM_Cmd(UBMS_TIM, DISABLE);
	
}

void UBMS_Display(void)
{
	_config_status_dis =_config_status;
	if(_config_status == CONFIG_DONE)
	{
		if((UBMS_Status.BatteryPercentage <= 100.0f) && (UBMS_Status.BatteryPercentage >= 75.0f))
		{
			GPIO_SetBits(UBMS_LEDRED_PORT, UBMS_LEDRED_PIN);
			GPIO_SetBits(UBMS_LEDORANGE_PORT, UBMS_LEDORANGE_PIN);
			GPIO_SetBits(UBMS_LEDYELLOW_PORT, UBMS_LEDYELLOW_PIN);
			GPIO_SetBits(UBMS_LEDGREEN_PORT, UBMS_LEDGREEN_PIN);
		}
		else if((UBMS_Status.BatteryPercentage < 75.0f) && (UBMS_Status.BatteryPercentage >= 50.0f))
		{
			GPIO_SetBits(UBMS_LEDRED_PORT, UBMS_LEDRED_PIN);
			GPIO_SetBits(UBMS_LEDORANGE_PORT, UBMS_LEDORANGE_PIN);
			GPIO_SetBits(UBMS_LEDYELLOW_PORT, UBMS_LEDYELLOW_PIN);
			GPIO_ResetBits(UBMS_LEDGREEN_PORT, UBMS_LEDGREEN_PIN);
		}
		else if((UBMS_Status.BatteryPercentage < 50.0f) && (UBMS_Status.BatteryPercentage >= 25.0f))
		{
			GPIO_SetBits(UBMS_LEDRED_PORT, UBMS_LEDRED_PIN);
			GPIO_SetBits(UBMS_LEDORANGE_PORT, UBMS_LEDORANGE_PIN);
			GPIO_ResetBits(UBMS_LEDYELLOW_PORT, UBMS_LEDYELLOW_PIN);
			GPIO_ResetBits(UBMS_LEDGREEN_PORT, UBMS_LEDGREEN_PIN);
		}
		else if((UBMS_Status.BatteryPercentage < 25.0f) && (UBMS_Status.BatteryPercentage >= 10.0f))
		{
			GPIO_SetBits(UBMS_LEDRED_PORT, UBMS_LEDRED_PIN);
			GPIO_ResetBits(UBMS_LEDORANGE_PORT, UBMS_LEDORANGE_PIN);
			GPIO_ResetBits(UBMS_LEDYELLOW_PORT, UBMS_LEDYELLOW_PIN);
			GPIO_ResetBits(UBMS_LEDGREEN_PORT, UBMS_LEDGREEN_PIN);
		}
		else if((UBMS_Status.BatteryPercentage < 10.0f) && (UBMS_Status.BatteryPercentage >= 0.0f))
		{
			UBMS_ToggleLeds(UBMS_LEDRED_PIN);
		}
	}
	else
	{
		UBMS_ToggleLeds(UBMS_LEDALL_PIN);
	}
}

void UBMS_GetStatus(UBMS_Status_Typedef* _ubms_status)
{
	_ubms_status->BaterryVoltage = UBMS_Status.BaterryVoltage;
	_ubms_status->BatteryCapacity = UBMS_Status.BatteryCapacity;
	_ubms_status->BatteryCurrent = UBMS_Status.BatteryCurrent;
	_ubms_status->BatteryPercentage = UBMS_Status.BatteryPercentage;
	_ubms_status->BatteryTotal = UBMS_Status.BatteryTotal;
	_ubms_status->BatteryUsed = UBMS_Status.BatteryUsed;	
}

void UBMS_SetBatteryTotal(float _battery_total)
{
	UBMS_Status.BatteryTotal = _battery_total;
}

void UBMS_StartBMS(void)
{
	UBMS_SetBatteryTotal(40000);
//	ADC_SoftwareStartConvCmd(UBMS_ADC_ADC, ENABLE);
	TIM_Cmd(UBMS_TIM, ENABLE);
}

uint8_t UBMS_CheckConfiguration(void)
{
	return _config_status;
}

void UBMS_ToggleLeds(uint16_t GPIO_Pin)
{
	_toggle_leds ^= 0xFF;    //  phep dao bit
	if(GPIO_Pin == UBMS_LEDALL_PIN)
	{
		if(_toggle_leds)
		{
			GPIO_SetBits(UBMS_LEDRED_PORT, UBMS_LEDRED_PIN);
			GPIO_SetBits(UBMS_LEDORANGE_PORT, UBMS_LEDORANGE_PIN);
			GPIO_SetBits(UBMS_LEDYELLOW_PORT, UBMS_LEDYELLOW_PIN);
			GPIO_SetBits(UBMS_LEDGREEN_PORT, UBMS_LEDGREEN_PIN);
		}
		else
		{
			GPIO_ResetBits(UBMS_LEDRED_PORT, UBMS_LEDRED_PIN);
			GPIO_ResetBits(UBMS_LEDORANGE_PORT, UBMS_LEDORANGE_PIN);
			GPIO_ResetBits(UBMS_LEDYELLOW_PORT, UBMS_LEDYELLOW_PIN);
			GPIO_ResetBits(UBMS_LEDGREEN_PORT, UBMS_LEDGREEN_PIN);
		}
	}
	else if (GPIO_Pin == UBMS_LEDRED_PIN)
	{
		if(_toggle_leds)
		{
			GPIO_SetBits(UBMS_LEDRED_PORT, UBMS_LEDRED_PIN);
		}
		else
		{
			GPIO_ResetBits(UBMS_LEDRED_PORT, UBMS_LEDRED_PIN);
		}
	}
}

void UBMS_SetConfiguration(UBMS_ConfigStatus _set_config_status)
{
		_config_status = _set_config_status;
}

void UBMS_RealoadDataFromFlash(void)
{
	UFLASH_ReadFlash(&UBMS_Status);
}

/******************************************************************************/
/* Peripherals Interrupt Handlers --------------------------------------------*/
/******************************************************************************/
void UBMS_DMA_Stream_IRQHandler(void)
{
	if(DMA_GetITStatus(DMA1_IT_TC1) != RESET)
  {		
//		for(int xx = 0 ; xx< 20 ; xx++)
//		{
//			ADC_ConvertedValue_debug[xx]=ADC_ConvertedValue[xx];
//		}
		
		if(_calib_done == false)
		{
			_tempADC2 = (ADC_ConvertedValue[1]+ADC_ConvertedValue[3]+ADC_ConvertedValue[5]+ADC_ConvertedValue[7]+ADC_ConvertedValue[9]+ADC_ConvertedValue[11]+ADC_ConvertedValue[13]+ADC_ConvertedValue[15]+ADC_ConvertedValue[17]+ADC_ConvertedValue[19]);
			_calib_ADCI += _tempADC2/10;// ADCI
			_calib_count--;
			if(_calib_count == 0) 
			{
				_calib_ADCI = _calib_ADCI/10;
				_initial_ACS712_Voltage = _calib_current_value * SENSITIVITY/1000000.0 + _calib_ADCI*vref/4095;
				_calib_done = true;
				UBMS_StartBMS();
			}
		}
		else
		{
			_tempADC1 = (ADC_ConvertedValue[0]+ADC_ConvertedValue[2]+ADC_ConvertedValue[4]+ADC_ConvertedValue[6]+ADC_ConvertedValue[8]+ADC_ConvertedValue[10]+ADC_ConvertedValue[12]+ADC_ConvertedValue[14]+ADC_ConvertedValue[16]+ADC_ConvertedValue[18]);
			_tempADC2 = (ADC_ConvertedValue[1]+ADC_ConvertedValue[3]+ADC_ConvertedValue[5]+ADC_ConvertedValue[7]+ADC_ConvertedValue[9]+ADC_ConvertedValue[11]+ADC_ConvertedValue[13]+ADC_ConvertedValue[15]+ADC_ConvertedValue[17]+ADC_ConvertedValue[19]);
		
			_tempADC1=_tempADC1/10; // ADCU
			_tempADC2=_tempADC2/10;// ADCI
			tempadcvalue[0]=_tempADC1;
			tempadcvalue[1]=_tempADC2;
		}
		
		DMA_ClearITPendingBit(DMA1_IT_GL1|DMA1_IT_TC1|DMA1_IT_HT1);
  }
}

void UBMS_TIM_IRQHandler(void)
{
	if(TIM_GetITStatus(UBMS_TIM, TIM_IT_Update) != RESET)
	{
		_total_time_convert = 1000.0f + ((239.5 + 12.5)*20)/18;//don vi  us  // 1000us + ((sample_cycles + 12.5)*20 )/18MHz   
		RawData = (_initial_ACS712_Voltage - _tempADC2*vref/4095.0)*1000000/SENSITIVITY;
//		RawData = _tempADC2;
		if (first_Status==false)
		{
			SmoothData = SmoothData - (LPF_Beta * (SmoothData - RawData));
			UBMS_Status.BatteryCurrent=SmoothData;
		}
		else 
		{
			SmoothData=RawData;
			first_Status=false;
		}
		tempcurrentV=_tempADC2*vref/4095.0;
		UBMS_Status.BaterryVoltage= _tempADC1*vref/4095.0/DEVIDE_REGISTER;// trong SCH VPS = 3.7* 8 = 29.6V, doc ADC thi chuyen ve 3.3V qua mach chia ap       => Vin = Vol*(398.484848+50)/50;// cai chia ap phai coi lai cho that chinh xac, theo SCH la VR/(VR+R3), neu nhu su dung kahc 26.9V thi chia ap lai

		UBMS_Status.BatteryUsed = (UBMS_Status.BatteryCurrent*_total_time_convert)/1000000.0f/3600.0f + UBMS_Status.BatteryUsed; //A.h

		UBMS_Status.BatteryCapacity = UBMS_Status.BatteryTotal - UBMS_Status.BatteryUsed;

		UBMS_Status.BatteryPercentage = (UBMS_Status.BatteryTotal-UBMS_Status.BatteryUsed )*100/UBMS_Status.BatteryTotal; // %
//		UFLASH_WriteFlash(&UBMS_Status);
		UBMS_Display();
			
		TIM_ClearITPendingBit(UBMS_TIM, TIM_IT_Update);
	}
}
