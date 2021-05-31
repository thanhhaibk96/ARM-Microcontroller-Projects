/* Includes ------------------------------------------------------------------*/
#include "my_pressure.h"

/* Public variables ----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/

/* Private const/macros ------------------------------------------------------*/

/**
 * @defgroup Module Pin define
 * @{
 */
		/** 
		* @brief   Pressure Pin define 
		*/
			#define   UPRES_ADC_NbrOfConversion					100	
			#define		UPRES_ADC_SampleTime							ADC_SampleTime_144Cycles
			#define 	UPRES_ADC													ADC1
			#define   UPRES_ADC_CLK											RCC_APB2Periph_ADC1
			#define		UPRES_ADC_CLK_Cmd									RCC_APB2PeriphClockCmd
			#define   UPRES_ADC_CHANNEL									ADC_Channel_3
			// Define Priority ADC Channel
			#define		UPRES_ADC_PRIO										1
			
			#define 	UPRES_PIN  				   							GPIO_Pin_3			
			#define 	UPRES_PORT 				  							GPIOA
			#define 	UPRES_CLK													RCC_AHB1Periph_GPIOA			
			#define 	UPRES_SOURCE				 							GPIO_PinSource3				
			#define		UPRES_CLK_Cmd		  	 							RCC_AHB1PeriphClockCmd				
			
			#define   UPRES_DMA_Channel									DMA_Channel_0
			#define   UPRES_DMA_Stream									DMA2_Stream0
			#define		UPRES_DMA_CLK_Cmd		  						RCC_AHB1PeriphClockCmd
			#define 	UPRES_DMA_CLK											RCC_AHB1Periph_DMA2
			#define		UPRES_DMA_Stream_IRQn							DMA2_Stream0_IRQn
			#define		UPRES_DMA_PreemptionPriority			0
			#define		UPRES_DMA_SubPriority							1
			
			#define		UPRES_DMA_Stream_IRQHandler				DMA2_Stream0_IRQHandler

/* Private variables ---------------------------------------------------------*/
static uint16_t ADC_DMAValue[UPRES_ADC_NbrOfConversion];
static float _pressure_value = 0.0f;
static float _pressure_value_LPF = 0.0f;
static float _vref_pressure = 5.0f;
static float _vref_adc			= 3.3f;
static float LPF_Beta				= 0.2;
static bool init_Status			= true;

/* Private function prototypes -----------------------------------------------*/

/* Exported function body ----------------------------------------------------*/
void UPRES_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	DMA_InitTypeDef DMA_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	ADC_InitTypeDef ADC_InitStruct;
	ADC_CommonInitTypeDef ADC_CommonInitStruct;
	
	UPRES_CLK_Cmd(UPRES_CLK,ENABLE);	
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = UPRES_PIN;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStruct.GPIO_Speed = GPIO_High_Speed;
	GPIO_Init(UPRES_PORT,&GPIO_InitStruct);
	
	/* ADC Common Init **********************************************************/
	UPRES_ADC_CLK_Cmd(UPRES_ADC_CLK, ENABLE);
	ADC_CommonInitStruct.ADC_Mode = ADC_Mode_Independent;
	ADC_CommonInitStruct.ADC_Prescaler = ADC_Prescaler_Div2;
	ADC_CommonInitStruct.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStruct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
	ADC_CommonInit(&ADC_CommonInitStruct);
	
	/* ADC1 Init ****************************************************************/
	ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;
	ADC_InitStruct.ADC_ScanConvMode = ENABLE; // Multiple Channels
	ADC_InitStruct.ADC_ContinuousConvMode = ENABLE; // Keep Sampling
	ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;
	ADC_InitStruct.ADC_NbrOfConversion = UPRES_ADC_NbrOfConversion;
	ADC_Init(UPRES_ADC, &ADC_InitStruct);
	
  /* DMA2 Stream0 channel0 configuration **************************************/
	UPRES_DMA_CLK_Cmd(UPRES_DMA_CLK,ENABLE);	
  DMA_InitStruct.DMA_Channel = UPRES_DMA_Channel;  
  DMA_InitStruct.DMA_PeripheralBaseAddr = (uint32_t)(&(ADC1 -> DR));
  DMA_InitStruct.DMA_Memory0BaseAddr = (uint32_t)&ADC_DMAValue;
  DMA_InitStruct.DMA_DIR = DMA_DIR_PeripheralToMemory;
  DMA_InitStruct.DMA_BufferSize = UPRES_ADC_NbrOfConversion;
  DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
  DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;
  DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;
  DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;
  DMA_InitStruct.DMA_Mode = DMA_Mode_Circular;
  DMA_InitStruct.DMA_Priority = DMA_Priority_High;
  DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Disable;         
  DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_HalfFull;
  DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;
  DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;
  DMA_Init(UPRES_DMA_Stream, &DMA_InitStruct);
	
	/* DMA Transfer complete interrupt configure */
	DMA_ITConfig(UPRES_DMA_Stream, DMA_IT_TC, ENABLE);

	NVIC_InitStruct.NVIC_IRQChannel = UPRES_DMA_Stream_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = UPRES_DMA_PreemptionPriority;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = UPRES_DMA_SubPriority;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);
	
	DMA_Cmd(UPRES_DMA_Stream, ENABLE);	
	/* ADC1 regular channel configuration **************************************/
  ADC_RegularChannelConfig(UPRES_ADC, UPRES_ADC_CHANNEL, UPRES_ADC_PRIO, UPRES_ADC_SampleTime);
	
 /* Enable DMA request after last transfer */
  ADC_DMARequestAfterLastTransferCmd(UPRES_ADC, ENABLE);

  /* Enable ADC1 DMA */
  ADC_DMACmd(UPRES_ADC, ENABLE);

  /* Enable ADC1 */
  ADC_Cmd(UPRES_ADC, ENABLE);
	
	ADC_SoftwareStartConv(UPRES_ADC);
}

float UPRES_GetData(void)
{
	return _pressure_value_LPF;
}

/* Private functions body ----------------------------------------------------*/

/******************************************************************************/
/* Peripherals Interrupt Handlers --------------------------------------------*/
/******************************************************************************/
void UPRES_DMA_Stream_IRQHandler(void)
{	
	uint32_t _sum_adc_value = 0;
	float _avr_adc_value = 0.0f;
	if(DMA_GetITStatus(UPRES_DMA_Stream, DMA_IT_TCIF0) != RESET)
	{	
		for(int i = 0; i < UPRES_ADC_NbrOfConversion; i++)
		{
			_sum_adc_value += (uint32_t)ADC_DMAValue[i];
		}
		
		_avr_adc_value = (float)_sum_adc_value / (float)UPRES_ADC_NbrOfConversion;
		
		_pressure_value = ((((_avr_adc_value * _vref_adc / 4095.0f) / _vref_pressure) - 0.1) / 1.6) * 10.0f; // Unit: bar
		
		if (init_Status == false)
		{
			_pressure_value_LPF = _pressure_value_LPF - (LPF_Beta * (_pressure_value_LPF - _pressure_value));
		}
		else 
		{
			_pressure_value_LPF = _pressure_value;
			init_Status = false;
		}		
		
		DMA_ClearITPendingBit(UPRES_DMA_Stream,DMA_IT_DMEIF0|DMA_IT_FEIF0|DMA_IT_HTIF0|DMA_IT_TCIF0|DMA_IT_TEIF0);
	}
}
