/*******************************************************************************
* File Name: ADS8698.c
*
* Description:
* ���ļ��ṩADS8698����ģʽ�����ã��������Ҫ�޸Ĳ���
* Note:
*
********************************************************************************/
#include "ads8698.h"

extern u8 timeout_flag ;
u32 ADS8698_Value[16][5]={0};//����16��ͨ����ǰ���ǲɼ��Ĵ���
u32 ADS8698_MaxValue[82][16]={0};//����82�����ӣ�ÿ������16��ͨ��
//u32 *pBuffer;
u32  pBuffer[16];
u16 Readtimes=0;//1.4S�ڶ��˶��ٴ�
u32 Sum_buff=0;
u8 cup_number=0;
/****AD8698����ģʽ���GPIO������****/
void ADS8698_GPIO_Init()
{
  GPIO_InitTypeDef GPIO_InitStructure;

	/****��ʼ������оƬ��RESET����PB5��PB7,Ĭ���ǵ͵�ƽ****/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//ʹ��GPIOD��ʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5|GPIO_Pin_7;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/****��ʼ������оƬ��ALARM����PB6��PB8****/	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/****��ʼ������оƬ��CS����PB12****/	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	/****��ʼ������оƬ��CS����PA4****/	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//ʹ��GPIOD��ʱ��
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/****ADS8698��ʼ��****/
void ADS8698_Init()
{
	ADS8698_GPIO_Init();
	SPI1_Config();//����2.5M����Ƶ�ʣ�����оƬҪ���18M
	SPI2_Config();
	ADS8698A0_Reset_L;//ֻҪ����ʱ�����400ns���൱��POWER_DN����λ���еļĴ�����ֵ
  ADS8698A1_Reset_L;
	Delay_Us(1);
	ADS8698A0_Reset_H;
  ADS8698A1_Reset_H;
	Delay_Us(2);//��ʱ2Us�ȴ���صļĴ�����ֵ��λ
	ADS8698_ConfigReg();//������ؼĴ����Ĳ���
}

/****д����Ĵ���-д�����ֽڶ���һ���ֽڣ��ܹ���24��CLK****/
static void ADS8698_WriteReg(u8 addr, u8 data)
{
	//u8 echo_data;
	union {
		  struct {
				unsigned Data	: 8;//����λ
				_Bool WR	: 1;//дʹ��λ
				unsigned Addr	: 7;//��ַλ
			} Reg;
			uint8_t Bytes[2];
	} Write_data;
	Write_data.Reg.WR = 1;
	Write_data.Reg.Addr = addr;
	Write_data.Reg.Data = data;
	ADS8698A0_CS_L;
	SPI1_ReadWriteByte(Write_data.Bytes[1]);//д����Ĵ���-��һ��ADS8698
	SPI1_ReadWriteByte(Write_data.Bytes[0]);
	SPI1_ReadWriteByte(0xFF);
	//echo_data = SPI1_ReadWriteByte(0xFF);//�����������ݽ�����Ϊ���
	ADS8698A0_CS_H;//��������30ns
	ADS8698A1_CS_L;
	SPI2_ReadWriteByte(Write_data.Bytes[1]);//д����Ĵ���-�ڶ���ADS8698
	SPI2_ReadWriteByte(Write_data.Bytes[0]);
	SPI2_ReadWriteByte(0xFF);
	//echo_data = SPI1_ReadWriteByte(0xFF);//�����������ݽ�����Ϊ���
	ADS8698A1_CS_H;//��������30ns
  Delay_Us(1);
}
/****���Ĵ���ֵ-Ŀǰû���õ�
static u8 ADS8698_ReadReg(u8 addr)
{
	u8 read_data;
	ADS8698A0_CS_L;
	SPI1_ReadWriteByte(addr << 1);
	SPI1_ReadWriteByte(0xFF);//һ��ȷ���ļٶ���
	read_data = SPI1_ReadWriteByte(0xFF);
	ADS8698A0_CS_H;//��������30ns
  Delay_Us(1);
	return read_data;
}
****/
/****��������Ĵ���****/
static void ADS8694_SendCmd(u8 cmd)
{
	//���õ�һ��ADS8698����Ĵ���
	ADS8698A0_CS_L;
	SPI1_ReadWriteByte(cmd);
	SPI1_ReadWriteByte(0x00);
	SPI1_ReadWriteByte(0xFF);
	SPI1_ReadWriteByte(0xFF);
	SPI1_ReadWriteByte(0xFF);
	ADS8698A0_CS_H;//��������30ns
	//���õڶ���ADS8698����Ĵ���
	ADS8698A1_CS_L;
	SPI2_ReadWriteByte(cmd);
	SPI2_ReadWriteByte(0x00);
	SPI2_ReadWriteByte(0xFF);
	SPI2_ReadWriteByte(0xFF);
	SPI2_ReadWriteByte(0xFF);
	ADS8698A1_CS_H;//��������30ns
	Delay_Us(1);
}

/****оƬ��λ֮���оƬ������������****/
void ADS8698_ConfigReg()
{
	u8 i;
	for(i=0;i<8;i++)
	{
		ADS8698_WriteReg(0X05+i,0X05);//���ø���ͨ�������뷶Χ, 2.5 x VREF
	}
	ADS8698_WriteReg(0x01,0XFF);//����Ϊ�Զ�ɨ��ģʽ�����е�ͨ������
	ADS8694_SendCmd(AUTO_RST);
}

/****��ȡADS8698��ת������****/
void ADS8698_RadeData()
{
	u8 i;
//��ȡ��һ��оƬ������	
	for (i = 0; i < 8; i++)
	{
		u32 ADS8698A0_code = 0;
		ADS8698A0_CS_L;       
		SPI1_ReadWriteByte(0x00);//�����ϴε�ģʽ���Զ�ɨ�裬ͨ�������μ�1
		SPI1_ReadWriteByte(0x00);
		ADS8698A0_code |= SPI1_ReadWriteByte(0xFF) << 10;//��ȡ18λ����
		ADS8698A0_code |= SPI1_ReadWriteByte(0xFF) << 2;
		ADS8698A0_code |= SPI1_ReadWriteByte(0xFF);
		if(ADS8698A0_code>0X80000000)
		ADS8698A0_code=0Xffffffff-ADS8698A0_code+1;	
		pBuffer[i] = ADS8698A0_code;
		ADS8698A0_CS_H;//��������30ns
	}
	//��ȡ�ڶ���оƬ������	
	for (i = 0; i < 8; i++)
	{
		u32 ADS8698A1_code = 0;
		ADS8698A1_CS_L;       
		SPI1_ReadWriteByte(0x00);//�����ϴε�ģʽ���Զ�ɨ�裬ͨ�������μ�1
		SPI1_ReadWriteByte(0x00);
		ADS8698A1_code |= SPI2_ReadWriteByte(0xFF) << 10;//��ȡ18λ����
		ADS8698A1_code |= SPI2_ReadWriteByte(0xFF) << 2;
		ADS8698A1_code |= SPI2_ReadWriteByte(0xFF);
		if(ADS8698A1_code>0X80000000)
		ADS8698A1_code=0Xffffffff-ADS8698A1_code+1;	
		pBuffer[i+8] = ADS8698A1_code;
		ADS8698A1_CS_H;//��������30ns
	  Delay_Us(1);
    }
}
/****
//��һ��ʱ���ڲɼ�16��ͨ�����������ݣ������ڶ�̬�ռ���ʹ������ķ�ʽ������
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
	while(head==NULL)//����Ӹ�ֹͣ�ɼ����������
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


//����һ��ʱ���ڵĵ���ͨ����ǰ82�����ֵ
//ͨ�����㣬1.4s����Ҫ�ɼ���82�����ӣ������м����һ�����ӵļ�϶����ôһ�����ӵ�ʱ�䵥Ԫ��8ms������SPI��Ƶ�ʣ�8ms�ڴ�Լ�ܳ�64��ֵ
void Data_MaxmumValue(u32 *chbuff,int times)
{
	u8 i;
  u32 sum=0;
	u8 ch;
	//ʹ�ö�ά�������ʽ��ÿ���ֵ�����ۼӣ�Ȼ��ÿ������һ��ֵ��ÿ�αȽ��ۼ�ֵ��Ȼ�����һ�������е����ֵ	
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
	
	

 


















