#ifndef __IOI2C_H
#define __IOI2C_H
#include "sys.h"

/****************SDA��SCL������**************************/
#define IIC_SDA_GPIO_Pin           GPIO_Pin_10
#define IIC_SCL_GPIO_Pin           GPIO_Pin_11
#define IIC_GPIOx                  GPIOB
#define IIC_RCC_APB2Periph_GPIOx   RCC_APB2Periph_GPIOB

#define IIC_SCL                    PBout(11) //SCL
#define IIC_SDA                    PBout(10) //SDA	 
#define READ_SDA                   PBin (10) //����SDA 
/********************************************************/



/****************������������F103 ��  F407 ��GPIO�����иı�****************************/
void SDA_IN(void);  //IO��������
void SDA_OUT(void);
void IIC_Init(void);        //��ʼ��IIC��IO��		
/*************************************************************************************/




//IIC���в�������
	 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(uint8_t txd);			//IIC����һ���ֽ�
uint8_t IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
uint8_t IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�

void IIC_Write_One_Byte(uint8_t daddr,uint8_t addr,uint8_t data);
uint8_t IIC_Read_One_Byte(uint8_t daddr,uint8_t addr);	  


#endif

//------------------End of File----------------------------
