/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USER_GPS_INS_H
#define __USER_GPS_INS_H

#ifdef __cplusplus
 extern "C" {
#endif
	
	 
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"	 
#include <stdbool.h>
#include "User_Delay.h"
#include <string.h>
	 
/* Exported constants/macros -------------------------------------------------*/


 /**
 * @defgroup <Group name here>
 * @{
 */	
 
 /**
 * @}
 */	 
	 
 /**
 * @defgroup Data From GPS/INS
 * @{
 */	
		/** 
* @brief   EKF output data collection typedef 
*/	
	typedef struct
	{
		uint32_t	sample_count;
		float roll;				// Unit: degree
		float pitch;			// Unit: degree/sec
		float yaw;				// Unit: degree/sec
		float lat;				// Unit: mg
		float lon;				// Unit: mg
		float alt;				// Unit: mg
		float vN;					// Unit: mgauss
		float vE;					// Unit: mgauss
		float vD;					// Unit: mgauss
	} UNAVI_RealOutputData_Typedef;
	
/* Exported types ------------------------------------------------------------*/
	 
/* Exported function prototypes ----------------------------------------------*/
void UNAVI_Configure(void);
void UNAVI_COMConfig(FunctionalState NewState);
void UNAVI_ProcessConfig(FunctionalState NewState);
FlagStatus UNAVI_FlagProcess(void);
void UNAVI_Transmit(char *szData);
	
	/* Peripherals Interrupt prototypes ------------------------------------------*/


#ifdef __cplusplus
}
#endif

#endif 
