/*******************************************************************************
* File Name: iic.c
*
* Description:
* ���ļ��ṩiic���������ú���(GPIOģ�ⷽʽ)���������Ҫ�޸Ĳ���
* Note:
*
********************************************************************************/
#include "iic.h"
#define IO_PORT GPIOB
#define RCC_APB2Periph_GPIOx RCC_APB2Periph_GPIOB
#define PIN_SDA GPIO_Pin_11
#define PIN_SCL GPIO_Pin_10
/*************************************************************
��������:IIC_Config
����ԭ��:void IIC_Config(void)
��    ��:
i2c��ʼ��,ʹ��GPIOģ��.
ȫ�ֱ�������
��    ��:��
�� �� ֵ:��
*************************************************************/
void IIC_Config(void)
{					     
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_APB2PeriphClockCmd(	RCC_APB2Periph_GPIOx, ENABLE );
	GPIO_InitStructure.GPIO_Pin = PIN_SCL|PIN_SDA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IO_PORT, &GPIO_InitStructure);
	GPIO_SetBits(IO_PORT,PIN_SCL|PIN_SDA); 	//PB10,PB11 �����
}
/*************************************************************
��������:SDA_OUT
����ԭ��:static void SDA_OUT(void)
��    ��:
i2c��SDA����Ϊ���ģʽ
ȫ�ֱ�������
��    ��:��
�� �� ֵ:��
*************************************************************/
static void SDA_OUT(void)      
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = PIN_SDA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(IO_PORT, &GPIO_InitStructure);
}

/*************************************************************
��������:SDA_IN
����ԭ��:static void  SDA_IN(void)
��    ��:
i2c��SDA����Ϊ����ģʽ
ȫ�ֱ�������
��    ��:��
�� �� ֵ:��
*************************************************************/
static void  SDA_IN(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = PIN_SDA;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPD ;   //�������
	GPIO_Init(IO_PORT, &GPIO_InitStructure);
}

/*************************************************************
��������:IIC_Start
����ԭ��:void IIC_Start(void)
��    ��:
����IIC��ʼ�ź�
ȫ�ֱ�������
��    ��:��
�� �� ֵ:��
*************************************************************/
void IIC_Start(void)
{
	SDA_OUT();     //sda�����
	GPIO_SetBits(IO_PORT,PIN_SDA);
	GPIO_SetBits(IO_PORT,PIN_SCL);
	Delay_Us(4);
 	GPIO_ResetBits(IO_PORT,PIN_SDA);//START:when CLK is high,DATA change form high to low
 	Delay_Us(4);
	GPIO_ResetBits(IO_PORT,PIN_SCL);//ǯסI2C���ߣ�׼�����ͻ��������
}


/*************************************************************
��������:IIC_Stop
����ԭ��:void IIC_Stop(void)
��    ��:
����IICֹͣ�ź�
ȫ�ֱ�������
��    ��:��
�� �� ֵ:��
*************************************************************/
void IIC_Stop(void)
{
	SDA_OUT();//sda�����
	GPIO_ResetBits(IO_PORT,PIN_SCL);
	GPIO_ResetBits(IO_PORT,PIN_SDA);//STOP:when CLK is high DATA change form low to high
	Delay_Us(4);
	GPIO_SetBits(IO_PORT,PIN_SCL);
	GPIO_SetBits(IO_PORT,PIN_SDA);//����I2C���߽����ź�
	Delay_Us(4);
}



/*************************************************************
��������:IIC_Wait_Ack
����ԭ��:u8 IIC_Wait_Ack(void)
��    ��:
�ȴ�Ӧ���źŵ���
ȫ�ֱ�������
��    ��:��
�� �� ֵ:
1������Ӧ��ʧ��
0������Ӧ��ɹ�
*************************************************************/
u8 IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	SDA_IN();      //SDA����Ϊ����  
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
	GPIO_ResetBits(IO_PORT,PIN_SCL);//ʱ�����0
	return 0;  
}

/*************************************************************
��������:IIC_Ack
����ԭ��:void IIC_Ack(void)
��    ��:
����ACKӦ��
ȫ�ֱ�������
��    ��:��
�� �� ֵ:��
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
��������:IIC_NAck
����ԭ��:void IIC_NAck(void)
��    ��:
������ACKӦ��
ȫ�ֱ�������
��    ��:��
�� �� ֵ:��
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
��������:IIC_Send_Byte
����ԭ��:void IIC_Send_Byte(u8 txd)
��    ��:
IIC����һ���ֽ�
ȫ�ֱ�������
��    ��:��
�� �� ֵ:��
*************************************************************/
void IIC_Send_Byte(u8 txd)
{                        
    u8 t;   
	SDA_OUT(); 	    
    GPIO_ResetBits(IO_PORT,PIN_SCL);//����ʱ�ӿ�ʼ���ݴ���
    for(t=0;t<8;t++)
    {              
        //IIC_SDA=(txd&0x80)>>7;
		if((txd&0x80)>>7)
			GPIO_SetBits(IO_PORT,PIN_SDA);
		else
			GPIO_ResetBits(IO_PORT,PIN_SDA);
		txd<<=1; 	  
		Delay_Us(2);   //��TEA5767��������ʱ���Ǳ����
		GPIO_SetBits(IO_PORT,PIN_SCL);
		Delay_Us(2);
		GPIO_ResetBits(IO_PORT,PIN_SCL);
		Delay_Us(2);
    }	 
}



/*************************************************************
��������:IIC_Read_Byte
����ԭ��:u8 IIC_Read_Byte(unsigned char ack)
��    ��:
IIC��1���ֽ�
ȫ�ֱ�������
��    ��:unsigned char ack  ack=1ʱ������ACK��ack=0������nACK
�� �� ֵ:u8 receive�����ض�ȡ���ֽ�
*************************************************************/
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
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
        IIC_NAck();//����nACK
    else
        IIC_Ack(); //����ACK   
    return receive;
}

/****END OF FILE****/
