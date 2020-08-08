/*******************************************************************************
* File Name: iic.c
*
* Description:
* 本文件提供iic驱动的配置函数(GPIO模拟方式)，请根据需要修改参数
* Note:
*
********************************************************************************/
#include "iic.h"
#define IO_PORT GPIOB
#define RCC_APB2Periph_GPIOx RCC_APB2Periph_GPIOB
#define PIN_SDA GPIO_Pin_11
#define PIN_SCL GPIO_Pin_10
/*************************************************************
函数名称:IIC_Config
函数原型:void IIC_Config(void)
功    能:
i2c初始化,使用GPIO模拟.
全局变量：无
参    数:无
返 回 值:无
*************************************************************/
void IIC_Config(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOx, ENABLE );
	GPIO_InitStructure.GPIO_Pin = PIN_SCL|PIN_SDA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IO_PORT, &GPIO_InitStructure);
	GPIO_SetBits(IO_PORT,PIN_SCL|PIN_SDA); 	//PB10,PB11 输出高
}
/*************************************************************
函数名称:SDA_OUT
函数原型:static void SDA_OUT(void)
功    能:
i2c的SDA配置为输出模式
全局变量：无
参    数:无
返 回 值:无
*************************************************************/
static void SDA_OUT(void)      
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = PIN_SDA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IO_PORT, &GPIO_InitStructure);
}

/*************************************************************
函数名称:SDA_IN
函数原型:static void  SDA_IN(void)
功    能:
i2c的SDA配置为输入模式
全局变量：无
参    数:无
返 回 值:无
*************************************************************/
static void  SDA_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = PIN_SDA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD ;   //下拉输出
	GPIO_Init(IO_PORT, &GPIO_InitStructure);
}

/*************************************************************
函数名称:IIC_Start
函数原型:void IIC_Start(void)
功    能:
产生IIC起始信号
全局变量：无
参    数:无
返 回 值:无
*************************************************************/
void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	GPIO_SetBits(IO_PORT,PIN_SDA);
	GPIO_SetBits(IO_PORT,PIN_SCL);
	Delay_Us(4);
 	GPIO_ResetBits(IO_PORT,PIN_SDA);//START:when CLK is high,DATA change form high to low
 	Delay_Us(4);
	GPIO_ResetBits(IO_PORT,PIN_SCL);//钳住I2C总线，准备发送或接收数据
}


/*************************************************************
函数名称:IIC_Stop
函数原型:void IIC_Stop(void)
功    能:
产生IIC停止信号
全局变量：无
参    数:无
返 回 值:无
*************************************************************/
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	GPIO_ResetBits(IO_PORT,PIN_SCL);
	GPIO_ResetBits(IO_PORT,PIN_SDA);//STOP:when CLK is high DATA change form low to high
	Delay_Us(4);
	GPIO_SetBits(IO_PORT,PIN_SCL);
	GPIO_SetBits(IO_PORT,PIN_SDA);//发送I2C总线结束信号
	Delay_Us(4);
}



/*************************************************************
函数名称:IIC_Wait_Ack
函数原型:u8 IIC_Wait_Ack(void)
功    能:
等待应答信号到来
全局变量：无
参    数:无
返 回 值:
1，接收应答失败
0，接收应答成功
*************************************************************/
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
	GPIO_SetBits(IO_PORT,PIN_SDA);
	Delay_Us(1);
	GPIO_SetBits(IO_PORT,PIN_SCL);
	Delay_Us(1);
	while(GPIO_ReadInputDataBit(IO_PORT,PIN_SDA))
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	GPIO_ResetBits(IO_PORT,PIN_SCL);//时钟输出0
	return 0;  
}

/*************************************************************
函数名称:IIC_Ack
函数原型:void IIC_Ack(void)
功    能:
产生ACK应答
全局变量：无
参    数:无
返 回 值:无
*************************************************************/
void IIC_Ack(void)
{
	GPIO_ResetBits(IO_PORT,PIN_SCL);
	SDA_OUT();
	GPIO_ResetBits(IO_PORT,PIN_SDA);
	Delay_Us(2);
	GPIO_SetBits(IO_PORT,PIN_SCL);
	Delay_Us(2);
	GPIO_ResetBits(IO_PORT,PIN_SCL);
}


/*************************************************************
函数名称:IIC_NAck
函数原型:void IIC_NAck(void)
功    能:
不产生ACK应答
全局变量：无
参    数:无
返 回 值:无
*************************************************************/
void IIC_NAck(void)
{
	GPIO_ResetBits(IO_PORT,PIN_SCL);
	SDA_OUT();
	GPIO_SetBits(IO_PORT,PIN_SDA);
	Delay_Us(2);
	GPIO_SetBits(IO_PORT,PIN_SCL);
	Delay_Us(2);
	GPIO_ResetBits(IO_PORT,PIN_SCL);
}					 				     


/*************************************************************
函数名称:IIC_Send_Byte
函数原型:void IIC_Send_Byte(u8 txd)
功    能:
IIC发送一个字节
全局变量：无
参    数:无
返 回 值:无
*************************************************************/
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    GPIO_ResetBits(IO_PORT,PIN_SCL);//拉低时钟开始数据传输
    for(t=0;t<8;t++)
    {              
        //IIC_SDA=(txd&0x80)>>7;
		if((txd&0x80)>>7)
			GPIO_SetBits(IO_PORT,PIN_SDA);
		else
			GPIO_ResetBits(IO_PORT,PIN_SDA);
		txd<<=1; 	  
		Delay_Us(2);   //对TEA5767这三个延时都是必须的
		GPIO_SetBits(IO_PORT,PIN_SCL);
		Delay_Us(2);
		GPIO_ResetBits(IO_PORT,PIN_SCL);
		Delay_Us(2);
    }	 
}



/*************************************************************
函数名称:IIC_Read_Byte
函数原型:u8 IIC_Read_Byte(unsigned char ack)
功    能:
IIC读1个字节
全局变量：无
参    数:unsigned char ack  ack=1时，发送ACK，ack=0，发送nACK
返 回 值:u8 receive，返回读取的字节
*************************************************************/
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
    for(i=0;i<8;i++ )
	{
        GPIO_ResetBits(IO_PORT,PIN_SCL);
        Delay_Us(2);
		GPIO_SetBits(IO_PORT,PIN_SCL);
        receive<<=1;
        if(GPIO_ReadInputDataBit(IO_PORT,PIN_SDA))receive++;
        Delay_Us(1);
    }					 
    if (!ack)
        IIC_NAck();//发送nACK
    else
        IIC_Ack(); //发送ACK   
    return receive;
}

/****END OF FILE****/
