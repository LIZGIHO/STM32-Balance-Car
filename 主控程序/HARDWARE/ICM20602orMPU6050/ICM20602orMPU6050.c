#include "ICM20602orMPU6050.h"
#include "sys.h"
#include "delay.h"
#include "usart.h"

//��ʼ��ICM20602 or MPU6050
//����ֵ:0,�ɹ�
//����,�������
uint8_t MPU_Init(void)
{
	uint8_t res;
	IIC_Init();								 //��ʼ��IIC����
	MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0X80); //��λMPU6050
	delay_ms(100);
	MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0X00); //����MPU6050
	MPU_Set_Gyro_Fsr(3);					 //�����Ǵ�����,��2000dps
	MPU_Set_Accel_Fsr(0);					 //���ٶȴ�����,��2g
	MPU_Set_Rate(1000);						 //���ò�����1000Hz
	MPU_Write_Byte(MPU_INT_EN_REG, 0X00);	 //�ر������ж�
	MPU_Write_Byte(MPU_USER_CTRL_REG, 0X00); // I2C��ģʽ�ر�
	MPU_Write_Byte(MPU_FIFO_EN_REG, 0X00);	 //�ر�FIFO
	MPU_Write_Byte(MPU_INTBP_CFG_REG, 0X80); // INT���ŵ͵�ƽ��Ч
	res = MPU_Read_Byte(MPU_DEVICE_ID_REG);
	if (res == MPU_ADDR || res == 0x12) //����ID��ȷ
	{
		MPU_Write_Byte(MPU_PWR_MGMT1_REG, 0X01); //����CLKSEL,PLL X��Ϊ�ο�
		MPU_Write_Byte(MPU_PWR_MGMT2_REG, 0X00); //���ٶ��������Ƕ�����
		MPU_Set_Rate(200);						 //���ò�����Ϊ100Hz
	}
	else  return 1;
	return 0;
}
//����CM20602 or MPU6050�����Ǵ����������̷�Χ
// fsr:0,��250dps;1,��500dps;2,��1000dps;3,��2000dps
//����ֵ:0,���óɹ�
//����,����ʧ��
uint8_t MPU_Set_Gyro_Fsr(uint8_t fsr)
{
	return MPU_Write_Byte(MPU_GYRO_CFG_REG, fsr << 3); //���������������̷�Χ
}



//����CM20602 or MPU6050���ٶȴ����������̷�Χ
// fsr:0,��2g;1,��4g;2,��8g;3,��16g
//����ֵ:0,���óɹ�
//����,����ʧ��
uint8_t MPU_Set_Accel_Fsr(uint8_t fsr)
{
	return MPU_Write_Byte(MPU_ACCEL_CFG_REG, fsr << 3); //���ü��ٶȴ����������̷�Χ
}



//����CM20602 or MPU6050�����ֵ�ͨ�˲���
// lpf:���ֵ�ͨ�˲�Ƶ��(Hz)
//����ֵ:0,���óɹ�
// ����,����ʧ��
uint8_t MPU_Set_LPF(u16 lpf)
{
	uint8_t data = 0;
	if (lpf >= 188)     data = 1;
	else if (lpf >= 98) data = 2;
	else if (lpf >= 42) data = 3;
	else if (lpf >= 20) data = 4;
	else if (lpf >= 10) data = 5;
	else                data = 6;
	return MPU_Write_Byte(MPU_CFG_REG, data); //�������ֵ�ͨ�˲���
}



//����CM20602 or MPU6050�Ĳ�����(�ٶ�Fs=1KHz)
// rate:4~1000(Hz)
//����ֵ:0,���óɹ�
//����,����ʧ��
uint8_t MPU_Set_Rate(u16 rate)
{
	uint8_t data;
	if (rate > 1000)  rate = 1000;
	if (rate < 4)     rate = 4;
	data = 1000 / rate - 1;
	data = MPU_Write_Byte(MPU_SAMPLE_RATE_REG, data); //�������ֵ�ͨ�˲���
	return MPU_Set_LPF(rate / 2);					  //�Զ�����LPFΪ�����ʵ�һ��
}

/**
 * @brief    ��ֵ�˲�����¶�ֵ
 * 
 * @param    ��
 * 
 * @return   �¶�ֵ
 * 
 * @note     ʹ��ģ��FIFO��10�β�����ƽ��ֵ
 * 
 * @example  MPU_Get_Temperature(&MPU6050_Temperature); ����MPU6050_Temperature��Ϊ��õ��¶ȡ�
 * 
 * @date     2022/6/21 16:15
 * 
 * @author   WxxGreat
 */
#define Temperature_FIFO_SIZE 10 //�������ݸ���
static short Temperature_FIFO[Temperature_FIFO_SIZE] = {0};
void MPU_Get_Temperature(float *MPU6050_Temperature)
{
	static unsigned char MPU6050_Temperature_Index;

	uint8_t buf[2];
	uint8_t i;
	MPU_Read_Len(MPU_ADDR, MPU_TEMP_OUTH_REG, 2, buf); //���¶ȼĴ�����8λ�͵�8λ

	Temperature_FIFO[MPU6050_Temperature_Index] = ((unsigned short int)buf[0] << 8) | buf[1]; //���������¶�ֵ�ŵ�FIFO������
	MPU6050_Temperature_Index = (MPU6050_Temperature_Index + 1) % Temperature_FIFO_SIZE;	  //������һ��ֵ����FIFO�����е�λ��

	for (i = 0; i < Temperature_FIFO_SIZE; i++)
		*MPU6050_Temperature += Temperature_FIFO[i]; //��FIFO�����е�ֵ�ۼ�

	*MPU6050_Temperature = *MPU6050_Temperature / Temperature_FIFO_SIZE / 340 + 36.53f;
}



//�õ�������ֵ(ԭʼֵ)
// gx,gy,gz:������x,y,z���ԭʼ����(������)
//����ֵ:0,�ɹ�
//����,�������
void MPU_Get_Gyroscope(short *gx, short *gy, short *gz)
{
	uint8_t buf[6];
  MPU_Read_Len(MPU_ADDR, MPU_GYRO_XOUTH_REG, 6, buf);

	*gx = ((u16)buf[0] << 8) | buf[1];
	*gy = ((u16)buf[2] << 8) | buf[3];
	*gz = ((u16)buf[4] << 8) | buf[5];
}


//�õ����ٶ�ֵ(ԭʼֵ)
// gx,gy,gz:������x,y,z���ԭʼ����(������)
//����ֵ:0,�ɹ�
//����,�������
void MPU_Get_Accelerometer(short *ax, short *ay, short *az)
{
	uint8_t buf[6];
	MPU_Read_Len(MPU_ADDR, MPU_ACCEL_XOUTH_REG, 6, buf);

	*ax = ((u16)buf[0] << 8) | buf[1];
	*ay = ((u16)buf[2] << 8) | buf[3];
	*az = ((u16)buf[4] << 8) | buf[5];
}


// IIC����д
// addr:������ַ
// reg:�Ĵ�����ַ
// len:д�볤��
// buf:������
//����ֵ:0,����
//����,�������
uint8_t MPU_Write_Len(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
	uint8_t i;
	IIC_Start();
	IIC_Send_Byte((addr << 1) | 0); //����������ַ+д����
	if (IIC_Wait_Ack())				//�ȴ�Ӧ��
	{
		IIC_Stop();
		return 1;
	}
	IIC_Send_Byte(reg); //д�Ĵ�����ַ
	IIC_Wait_Ack();		//�ȴ�Ӧ��
	for (i = 0; i < len; i++)
	{
		IIC_Send_Byte(buf[i]); //��������
		if (IIC_Wait_Ack())	   //�ȴ�ACK
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_Stop();
	return 0;
}


// IIC������
// addr:������ַ
// reg:Ҫ��ȡ�ļĴ�����ַ
// len:Ҫ��ȡ�ĳ���
// buf:��ȡ�������ݴ洢��
//����ֵ:0,����
//����,�������
uint8_t MPU_Read_Len(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
{
	IIC_Start();
	IIC_Send_Byte((addr << 1) | 0); //����������ַ+д����
	if (IIC_Wait_Ack())				//�ȴ�Ӧ��
	{
		IIC_Stop();
		return 1;
	}
	IIC_Send_Byte(reg); //д�Ĵ�����ַ
	IIC_Wait_Ack();		//�ȴ�Ӧ��
	IIC_Start();
	IIC_Send_Byte((addr << 1) | 1); //����������ַ+������
	IIC_Wait_Ack();					//�ȴ�Ӧ��
	while (len)
	{
		if (len == 1)
			*buf = IIC_Read_Byte(0); //������,����nACK
		else
			*buf = IIC_Read_Byte(1); //������,����ACK
		len--;
		buf++;
	}
	IIC_Stop(); //����һ��ֹͣ����
	return 0;
}


// IICдһ���ֽ�
// reg:�Ĵ�����ַ
// data:����
//����ֵ:0,����
//����,�������
uint8_t MPU_Write_Byte(uint8_t reg, uint8_t data)
{
	IIC_Start();
	IIC_Send_Byte((MPU_ADDR << 1) | 0); //����������ַ+д����
	if (IIC_Wait_Ack())					//�ȴ�Ӧ��
	{
		IIC_Stop();
		return 1;
	}
	IIC_Send_Byte(reg);	 //д�Ĵ�����ַ
	IIC_Wait_Ack();		 //�ȴ�Ӧ��
	IIC_Send_Byte(data); //��������
	if (IIC_Wait_Ack())	 //�ȴ�ACK
	{
		IIC_Stop();
		return 1;
	}
	IIC_Stop();
	return 0;
}


// IIC��һ���ֽ�
// reg:�Ĵ�����ַ
//����ֵ:����������
uint8_t MPU_Read_Byte(uint8_t reg)
{
	uint8_t res;
	IIC_Start();
	IIC_Send_Byte((MPU_ADDR << 1) | 0); //����������ַ+д����
	IIC_Wait_Ack();						//�ȴ�Ӧ��
	IIC_Send_Byte(reg);					//д�Ĵ�����ַ
	IIC_Wait_Ack();						//�ȴ�Ӧ��
	IIC_Start();
	IIC_Send_Byte((MPU_ADDR << 1) | 1); //����������ַ+������
	IIC_Wait_Ack();						//�ȴ�Ӧ��
	res = IIC_Read_Byte(0);				//��ȡ����,����nACK
	IIC_Stop();							//����һ��ֹͣ����
	return res;
}
