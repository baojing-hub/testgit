/*******************************************************************************
* File Name: ad7768.h
*
* Description:
*  Contains the function prototypes and constants available to the ad7768
*  user module.
*
* Note:
*
*******************************************************************************/

#ifndef AD7768_H
#define AD7768_H
#include "stm32f10x.h"
#include "rcc.h"
#include "spi.h"
#include "usart.h"

#define NULL 0

#define ADS8698A0_Reset_H      GPIO_SetBits(GPIOB,GPIO_Pin_5)
#define ADS8698A0_Reset_L      GPIO_ResetBits(GPIOB,GPIO_Pin_5)
#define ADS8698A1_Reset_H      GPIO_SetBits(GPIOA,GPIO_Pin_7)
#define ADS8698A1_Reset_L      GPIO_ResetBits(GPIOA,GPIO_Pin_7)
#define ADS8698A0_ALARM        GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_6)
#define ADS8698A1_ALARM        GPIO_ReadInputDataBit(GPIOD,GPIO_Pin_8)
#define ADS8698A0_CS_H 				 GPIO_SetBits(GPIOA,GPIO_Pin_4)
#define ADS8698A0_CS_L				 GPIO_ResetBits(GPIOA,GPIO_Pin_4)
#define ADS8698A1_CS_H 				 GPIO_SetBits(GPIOB,GPIO_Pin_12)
#define ADS8698A1_CS_L				 GPIO_ResetBits(GPIOB,GPIO_Pin_12)
	
#define AUTO_RST    0XA0//自动扫描模式
void ADS8698_GPIO_Init(void);
void ADS8698_Init(void);
void ADS8698_ConfigReg(void);
static void ADS8694_SendCmd(u8 cmd);
static void ADS8698_WriteReg(u8 addr, u8 data);	
void ADS8698_RadeData(void);
void Data_MaxmumValue(u32 *chbuff,int times);
extern u32 ADS8698_MaxValue[82][16];
#endif


/****END OF FILE****/
