/* Includes ------------------------------------------------------------------*/
#include "User_FlashData.h"
#include "stdbool.h"

/**
 * @}
 */
#define	FLASH_PAGE_SIZE    ((uint16_t)0x400)

#define PAGE127_WRITE_START_ADDR   ((uint32_t)0x0801FC00)
#define PAGE127_WRITE_END_ADDR     ((uint32_t)0x0801FFFF)
/* Public variables ----------------------------------------------------------*/

/* Private types -------------------------------------------------------------*/
union
{
	uint32_t _array[6];
	UBMS_Status_Typedef _value;
}BMS_Data;

volatile FLASH_Status FLASHStatus = FLASH_COMPLETE;
uint32_t Address = 0x00;
volatile CheckStatus MemoryProgramStatus = PASSED;

/* Private const/macros ------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
void UFlash_ResetData(void);

/* Exported function body ----------------------------------------------------*/
void UFLASH_WriteFlash(UBMS_Status_Typedef* _ubms_status)
{
	FLASH_UnlockBank1();
	
	/* Clear All pending flags */
  FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);	

	FLASH_ErasePage(PAGE127_WRITE_START_ADDR);
	
//	BMS_Data._value.BatteryTotal = _ubms_status->BatteryTotal;
//	BMS_Data._value.BaterryVoltage = _ubms_status->BaterryVoltage;
//	BMS_Data._value.BatteryCapacity = _ubms_status->BatteryCapacity;
//	BMS_Data._value.BatteryCurrent = _ubms_status->BatteryCurrent;
//	BMS_Data._value.BatteryPercentage = _ubms_status->BatteryPercentage;
//	BMS_Data._value.BatteryUsed = _ubms_status->BatteryUsed;
	
	uint8_t i = 0;
	Address = PAGE127_WRITE_START_ADDR;
	
	for(i = 0; (i < 6) && (FLASHStatus == FLASH_COMPLETE); i++)
	{
		FLASHStatus = FLASH_ProgramWord(Address, BMS_Data._array[i]);
		Address+=4;
	}
	
	FLASH_LockBank1();
	
	Address = PAGE127_WRITE_START_ADDR;
	for(i = 0; (i < 6) && (MemoryProgramStatus != FAILED); i++)
	{
		if((*(__IO uint32_t*) Address) != BMS_Data._array[i])
    {
      MemoryProgramStatus = FAILED;
    }
    Address += 4;
	}
	
	UFlash_ResetData();	
}

void UFLASH_ReadFlash(UBMS_Status_Typedef* _ubms_status)
{
	UFlash_ResetData();
	
	uint8_t i = 0;
	Address = PAGE127_WRITE_START_ADDR;
	for(i = 0; (i < 6); i++)
	{
		BMS_Data._array[i] = *(__IO uint32_t*)(Address);
		Address+=4;
	}
//	_ubms_status->BaterryVoltage = BMS_Data._value.BaterryVoltage;
//	_ubms_status->BatteryCapacity = BMS_Data._value.BatteryCapacity;
//	_ubms_status->BatteryCurrent = BMS_Data._value.BatteryCurrent;
//	_ubms_status->BatteryPercentage = BMS_Data._value.BatteryPercentage;
//	_ubms_status->BatteryTotal = BMS_Data._value.BatteryTotal;
//	_ubms_status->BatteryUsed = BMS_Data._value.BatteryUsed;
	
}

void UFlash_ResetData(void)
{
//	BMS_Data._value.BaterryVoltage = 0;
//	BMS_Data._value.BatteryCapacity = 0;
//	BMS_Data._value.BatteryCurrent = 0;
//	BMS_Data._value.BatteryPercentage = 0;
//	BMS_Data._value.BatteryTotal = 0;
//	BMS_Data._value.BatteryUsed = 0;
}
/* Private functions body ----------------------------------------------------*/

/******************************************************************************/
/* Peripherals Interrupt Handlers --------------------------------------------*/
/******************************************************************************/
