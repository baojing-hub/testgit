/*******************************************************************************
* File Name: can.h
*
* Description:
*  Contains the function prototypes and constants available to the RCC
*  user module.
*
* Note:
*
*******************************************************************************/

#ifndef RCC_H_
#define RCC_H_
#include "stm32f10x_rcc.h"
extern RCC_ClocksTypeDef RCC_ClockFreq;
void RCC_Config(void);
void Delay_Ms(u16 nms);
void Delay_Us(u32 nus);
#endif /* RCC_H_ */




/****END OF FILE****/
