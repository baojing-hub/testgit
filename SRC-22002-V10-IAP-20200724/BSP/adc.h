/*******************************************************************************
* File Name: adc.h
*
* Description:
*  Contains the function prototypes and constants available to the adc
*  user module.
*
* Note:
*
*******************************************************************************/

#ifndef ADC_H
#define ADC_H
#include "stm32f10x.h"

void Adc_Config(void);
u16 Get_Adc(u8 ch);

#endif


/****END OF FILE****/
