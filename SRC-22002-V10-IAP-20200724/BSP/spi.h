/*******************************************************************************
* File Name: spi.h
*
* Description:
*  Contains the function prototypes and constants available to the SPI
*  user module.
*
* Note:
*
*******************************************************************************/
#ifndef SPI_H
#define SPI_H
#include "stm32f10x.h"

void SPI1_Config(void);			 //��ʼ��SPI��
u8 SPI1_ReadWriteByte(u8 TxData);//SPI���߶�дһ���ֽ� 				  	    													  
void SPI2_Config(void);			 //��ʼ��SPI��
u8 SPI2_ReadWriteByte(u8 TxData);//SPI���߶�дһ���ֽ�
		 
#endif

