/*******************************************************************************
* File Name: ADS8698.c
*
* Description:
* 本文件提供ADS8698引脚模式的配置，请根据需要修改参数
* Note:
*
********************************************************************************/
#include "ads8698.h"

extern u8 timeout_flag ;
u32 ADS8698_Value[16][5]={0};//定义16个通道，前面是采集的次数
u32 ADS8698_MaxValue[82][16]={0};//定义82个杯子，每个杯子16个通道
//u32 *pBuffer;
u32  pBuffer[16];
u16 Readtimes=0;//1.4S内读了多少次
u32 Sum_buff=0;
u8 cup_number=0;
/****AD8698引脚模式相关GPIO的配置****/
void ADS8698_GPIO_Init()
{
  GPIO_InitTypeDef GPIO_InitStructure;

	/****初始化两个芯片的RESET引脚PB5和PB7,默认是低电平****/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//使能GPIOD的时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/****初始化两个芯片的ALARM引脚PB6和PB8****/	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/****初始化两个芯片的CS引脚PB12****/	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/****初始化两个芯片的CS引脚PA4****/	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//使能GPIOD的时钟
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/****ADS8698初始化****/
void ADS8698_Init()
{
	ADS8698_GPIO_Init();
	SPI1_Config();//设置2.5M采样频率，低于芯片要求的18M
	SPI2_Config();
	ADS8698A0_Reset_L;//只要拉低时间大于400ns就相当于POWER_DN，复位所有的寄存器和值
  ADS8698A1_Reset_L;
	Delay_Us(1);
	ADS8698A0_Reset_H;
  ADS8698A1_Reset_H;
	Delay_Us(2);//延时2Us等待相关的寄存器和值复位
	ADS8698_ConfigReg();//配置相关寄存器的参数
}

/****写程序寄存区-写两个字节读回一个字节，总共是24个CLK****/
static void ADS8698_WriteReg(u8 addr, u8 data)
{
	//u8 echo_data;
	union {
		  struct {
				unsigned Data	: 8;//数据位
				_Bool WR	: 1;//写使能位
				unsigned Addr	: 7;//地址位
			} Reg;
			uint8_t Bytes[2];
	} Write_data;
	Write_data.Reg.WR = 1;
	Write_data.Reg.Addr = addr;
	Write_data.Reg.Data = data;
	ADS8698A0_CS_L;
	SPI1_ReadWriteByte(Write_data.Bytes[1]);//写程序寄存器-第一个ADS8698
	SPI1_ReadWriteByte(Write_data.Bytes[0]);
	SPI1_ReadWriteByte(0xFF);
	//echo_data = SPI1_ReadWriteByte(0xFF);//读回来的数据仅仅作为检查
	ADS8698A0_CS_H;//拉高至少30ns
	ADS8698A1_CS_L;
	SPI2_ReadWriteByte(Write_data.Bytes[1]);//写程序寄存器-第二个ADS8698
	SPI2_ReadWriteByte(Write_data.Bytes[0]);
	SPI2_ReadWriteByte(0xFF);
	//echo_data = SPI1_ReadWriteByte(0xFF);//读回来的数据仅仅作为检查
	ADS8698A1_CS_H;//拉高至少30ns
  Delay_Us(1);
}
/****读寄存器值-目前没有用到
static u8 ADS8698_ReadReg(u8 addr)
{
	u8 read_data;
	ADS8698A0_CS_L;
	SPI1_ReadWriteByte(addr << 1);
	SPI1_ReadWriteByte(0xFF);//一个确定的假动作
	read_data = SPI1_ReadWriteByte(0xFF);
	ADS8698A0_CS_H;//拉高至少30ns
  Delay_Us(1);
	return read_data;
}
****/
/****设置命令寄存器****/
static void ADS8694_SendCmd(u8 cmd)
{
	//设置第一个ADS8698命令寄存器
	ADS8698A0_CS_L;
	SPI1_ReadWriteByte(cmd);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0xFF);
	SPI1_ReadWriteByte(0xFF);
	SPI1_ReadWriteByte(0xFF);
	ADS8698A0_CS_H;//拉高至少30ns
	//设置第二个ADS8698命令寄存器
	ADS8698A1_CS_L;
	SPI2_ReadWriteByte(cmd);
	SPI2_ReadWriteByte(0x00);
	SPI2_ReadWriteByte(0xFF);
	SPI2_ReadWriteByte(0xFF);
	SPI2_ReadWriteByte(0xFF);
	ADS8698A1_CS_H;//拉高至少30ns
	Delay_Us(1);
}

/****芯片复位之后对芯片进行重新配置****/
void ADS8698_ConfigReg()
{
	u8 i;
	for(i=0;i<8;i++)
	{
		ADS8698_WriteReg(0X05+i,0X05);//设置各个通道的输入范围, 2.5 x VREF
	}
	ADS8698_WriteReg(0x01,0XFF);//设置为自动扫描模式，所有的通道都打开
	ADS8694_SendCmd(AUTO_RST);
}

/****读取ADS8698的转换数据****/
void ADS8698_RadeData()
{
	u8 i;
//读取第一个芯片的数据	
	for (i = 0; i < 8; i++)
	{
		u32 ADS8698A0_code = 0;
		ADS8698A0_CS_L;       
		SPI1_ReadWriteByte(0x00);//持续上次的模式：自动扫描，通道号依次加1
		SPI1_ReadWriteByte(0x00);
		ADS8698A0_code |= SPI1_ReadWriteByte(0xFF) << 10;//读取18位数据
		ADS8698A0_code |= SPI1_ReadWriteByte(0xFF) << 2;
		ADS8698A0_code |= SPI1_ReadWriteByte(0xFF);
		if(ADS8698A0_code>0X80000000)
		ADS8698A0_code=0Xffffffff-ADS8698A0_code+1;	
		pBuffer[i] = ADS8698A0_code;
		ADS8698A0_CS_H;//拉高至少30ns
	}
	//读取第二个芯片的数据	
	for (i = 0; i < 8; i++)
	{
		u32 ADS8698A1_code = 0;
		ADS8698A1_CS_L;       
		SPI1_ReadWriteByte(0x00);//持续上次的模式：自动扫描，通道号依次加1
		SPI1_ReadWriteByte(0x00);
		ADS8698A1_code |= SPI2_ReadWriteByte(0xFF) << 10;//读取18位数据
		ADS8698A1_code |= SPI2_ReadWriteByte(0xFF) << 2;
		ADS8698A1_code |= SPI2_ReadWriteByte(0xFF);
		if(ADS8698A1_code>0X80000000)
		ADS8698A1_code=0Xffffffff-ADS8698A1_code+1;	
		pBuffer[i+8] = ADS8698A1_code;
		ADS8698A1_CS_H;//拉高至少30ns
	  Delay_Us(1);
    }
}
/****
//在一定时间内采集16个通道的所有数据，保存在动态空间中使用链表的方式来进行
struct ADS8698_Data{
	u8  ch;
	u32 Data;
	struct ADS8698_Data *next;
};
struct ADS8698_Data *Data_collected(u32 *chbuff)
{
	u8 n,m=0;
	struct ADS8698_Data *head,*p1,*p2;
	n=0;
	p1=p2=(struct ADS8698_Data *)malloc(sizeof(struct ADS8698_Data));
	p1->ch=0;
	p1->Data=chbuff[0];
	head=NULL;
	while(head==NULL)//这个加个停止采集链表的条件
	{
		n=n+1;
		if(n==1)head=p1;
		else p2->next=p1;
		p2=p1;
		p1=(struct ADS8698_Data *)malloc(sizeof(struct ADS8698_Data));
		p1->ch=m+1;	
		p1->Data=chbuff[m+1];
		if(m==16)m=0;
	}
	p2->next=NULL;
	return (head);
}
******/


//计算一段时间内的单个通道的前82个最大值
//通过计算，1.4s内需要采集到82个杯子，加上中间空余一个杯子的间隙，那么一个杯子的时间单元是8ms，根绝SPI的频率，8ms内大约能出64个值
void Data_MaxmumValue(u32 *chbuff,int times)
{
	u8 i;
  u32 sum=0;
	u8 ch;
	//使用二维数组的形式，每五个值进行累加，然后每次右移一个值，每次比较累加值，然后输出一组数据中的最大值	
	for(ch=0;ch<16;ch++)
	{
		ADS8698_Value[ch][0] = chbuff[ch];
		for(i=0;i<times;i++)sum += ADS8698_Value[ch][i];
		for(i=times-1;i>0;i--) ADS8698_Value[ch][i]=ADS8698_Value[ch][i-1];
		if(sum < Sum_buff)
		Sum_buff=Sum_buff;
		else Sum_buff=sum;
		sum=0;
		if(timeout_flag==1)
		{	
			ADS8698_MaxValue[cup_number][ch]=Sum_buff/times;
			printf("The Number Cup:%d,channel VAlue:%d\n",cup_number,ADS8698_MaxValue[cup_number][ch]);
			cup_number=cup_number+1;
			Sum_buff=0;
			timeout_flag=0;
		}
	}

}
	
	

 


















