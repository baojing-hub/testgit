#ifndef HAL_H
#define HAL_H
#include "stm32f10x.h"
#include "can.h"
#include "gpio.h"
#include "usart.h"
#include "rcc.h"
#include "ads8698.h"
#include "tim.h"



extern CanRxMsg g_RxMessage;
extern u8 g_CAN_RX_FLAG;
extern u8 g_SelfCanId;
extern u8 g_CAN_TX_FLAG;
extern u32 g_CAN_ERR_CNT;
extern u32 g_CAN1_ESR;


void Hal_Init(void);
#endif
