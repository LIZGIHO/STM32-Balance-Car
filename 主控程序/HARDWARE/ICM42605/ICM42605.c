#include "i2c.h"
#include "ICM42605.h"
#include "stdio.h"

/**
 * @brief          ICM42605ʹ��ģ��IICͨѶ����
 *
 * @note           
 *
 * @date           2022-6-10 18:22
 *
 * @author         WxxGreat
 *
 * @version        1.0   
 */


static float accSensitivity;   		/* ���ٶȵ���С�ֱ��� */
static float gyroSensitivity;    	/* �����ǵ���С�ֱ��� */





/**
 * @brief          ��ԭʼICM42605��ֵת��Ϊ����������ļ��ٶ�(��λm/s^2)�ͽ��ٶ�(��λrad/s)
 *
 * @param          NULL
 *
 * @return         void
 *
 * @usage          ���øú���ǰ���ȶ�ȡԭʼֵ;���ݴ������Ĵ������õ�����������ֵ
 *
 * @date           2022-6-10 18:22
 *
 * @author         WxxGreat
 *
 * @version        1.0   
 */
void ICM42605_AD_trun(void)
{
	  float g = 9.7947f; //�Ϸ��������ٶ�
    float pi = 3.14159265359f;
		Get_ICM42605_accdata(&aacx,&aacy,&aacz);
	  Get_ICM42605_gyro(&gyrox,&gyroy,&gyroz);
    aacx =  aacx * (8.0f * g / 32768.0f);//ת����,��λm/s^2
    aacy =  aacy * (8.0f * g / 32768.0f);
    aacz =  aacz * (8.0f * g / 32768.0f);
    gyrox = gyrox * (2000.0f * pi / 180 / 32768.0f);//ת����,��λrad/s
    gyroy = gyroy * (2000.0f * pi / 180 / 32768.0f);
    gyroz = gyroz * (2000.0f * pi / 180 / 32768.0f);
//  temp_data = (icm4_temp / 132.48) + 25;//ת����,��λ���϶ȣ�To fix
}

//-------------------------------------------------------------------------------------------------------------------
//  ʹ�����IICͨ��
//-------------------------------------------------------------------------------------------------------------------

/**
 * @brief          �Լ�ICM42605
 *
 * @param          NULL
 *
 * @return         void
 *
 * @usage          ���øú���ǰ�����ȵ���ģ��IIC�ĳ�ʼ��
 *
 * @date           2022-6-10 18:22
 *
 * @author         WxxGreat
 *
 * @version        1.0   
 */
void ICM42605_self1_check(void)
{
    uint8_t dat=0;
    while(0x42 != dat)   //��ȡICM42605 ID, Ĭ��ID��0x42
    {
        dat = ICM42605_Read_Byte(ICM42605_WHO_AM_I);
        delay_ms(1);
    }
}

/* ���㵱ǰ���ٶȷֱ��� */
float bsp_Icm42605GetAres(uint8_t Ascale)
{
    switch(Ascale)
    {
    case AFS_2G:
        accSensitivity = 2000 / 32768.0f;
        break;
    case AFS_4G:
        accSensitivity = 4000 / 32768.0f;
        break;
    case AFS_8G:
        accSensitivity = 8000 / 32768.0f;
        break;
    case AFS_16G:
        accSensitivity = 16000 / 32768.0f;
        break;
    }

    return accSensitivity;
}

/* ���㵱ǰ�����Ƿֱ��� */
float bsp_Icm42605GetGres(uint8_t Gscale)
{
    switch(Gscale)
    {
    case GFS_15_125DPS:
        gyroSensitivity = 15.125f / 32768.0f;
        break;
    case GFS_31_25DPS:
        gyroSensitivity = 31.25f / 32768.0f;
        break;
    case GFS_62_5DPS:
        gyroSensitivity = 62.5f / 32768.0f;
        break;
    case GFS_125DPS:
        gyroSensitivity = 125.0f / 32768.0f;
        break;
    case GFS_250DPS:
        gyroSensitivity = 250.0f / 32768.0f;
        break;
    case GFS_500DPS:
        gyroSensitivity = 500.0f / 32768.0f;
        break;
    case GFS_1000DPS:
        gyroSensitivity = 1000.0f / 32768.0f;
        break;
    case GFS_2000DPS:
        gyroSensitivity = 2000.0f / 32768.0f;
        break;
    }
	
	gyroSensitivity = gyroSensitivity*1000;
	
	
    return gyroSensitivity;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      ��ʼ��ICM40605
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:               ���øú���ǰ�����ȵ���ģ��IIC�ĳ�ʼ��
//-------------------------------------------------------------------------------------------------------------------
char ICM42605_init(void)
{
    uint8_t reg_val;

    IIC_Init();								 //��ʼ��IIC����
    delay_ms(10);  //�ϵ���ʱ

    
    ICM42605_self1_check();
	
    if(ICM42605_Read_Byte(ICM42605_WHO_AM_I) == 0x42)//�Լ�
		{   
			  //��������ԭ�������¼���
        //1 ICM42605���ˣ�������µ������ĸ��ʼ���
        //2 ���ߴ������û�нӺ�
        //3 ��������Ҫ����������裬������3.3V
	
				
			ICM42605_Write_Byte(ICM42605_REG_BANK_SEL, 0); 		/* ����bank 0����Ĵ��� */
			ICM42605_Write_Byte(ICM42605_REG_BANK_SEL, 0x01); 	/* ��λ������ */
			
			delay_ms(10);//���ٵȴ�1ms

				
			ICM42605_Write_Byte(ICM42605_REG_BANK_SEL, 1); 		/* ����bank 1����Ĵ��� */
			ICM42605_Write_Byte(ICM42605_INTF_CONFIG4, 0x02); 	/* ����Ϊ4��SPIͨ�� */

			ICM42605_Write_Byte(ICM42605_REG_BANK_SEL, 0); 		/* ����bank 0����Ĵ��� */
			ICM42605_Write_Byte(ICM42605_FIFO_CONFIG, 0x40); 	/* Stream-to-FIFO Mode(page61) */


			reg_val = ICM42605_Read_Byte(ICM42605_INT_SOURCE0);

			ICM42605_Write_Byte(ICM42605_INT_SOURCE0, 0x00);
			ICM42605_Write_Byte(ICM42605_FIFO_CONFIG2, 0x00); 
			ICM42605_Write_Byte(ICM42605_FIFO_CONFIG3, 0x02); 
			ICM42605_Write_Byte(ICM42605_INT_SOURCE0, reg_val);
			ICM42605_Write_Byte(ICM42605_FIFO_CONFIG1, 0x63); 	/* ʹ��FIFO */

			ICM42605_Write_Byte(ICM42605_REG_BANK_SEL, 0x00);
			ICM42605_Write_Byte(ICM42605_INT_CONFIG, 0x36);

			ICM42605_Write_Byte(ICM42605_REG_BANK_SEL, 0x00);
			reg_val = ICM42605_Read_Byte(ICM42605_INT_SOURCE0);
			reg_val |= (1 << 2);
			ICM42605_Write_Byte(ICM42605_INT_SOURCE0, reg_val);

			bsp_Icm42605GetAres(AFS_2G);						/* Ĭ��8g���� */
			ICM42605_Write_Byte(ICM42605_REG_BANK_SEL, 0x00);
			reg_val = ICM42605_Read_Byte(ICM42605_ACCEL_CONFIG0);
			reg_val |= (AFS_2G << 5);  
			reg_val |= (AODR_1000Hz);    
			ICM42605_Write_Byte(ICM42605_ACCEL_CONFIG0, reg_val);

			bsp_Icm42605GetGres(GFS_2000DPS);					/* Ĭ��1000dps���� */
			ICM42605_Write_Byte(ICM42605_REG_BANK_SEL, 0x00);
			reg_val = ICM42605_Read_Byte(ICM42605_GYRO_CONFIG0);	
			reg_val |= (GFS_2000DPS << 5);  
			reg_val |= (AODR_1000Hz);     
			ICM42605_Write_Byte(ICM42605_GYRO_CONFIG0, reg_val);

			ICM42605_Write_Byte(ICM42605_REG_BANK_SEL, 0x00);
			reg_val = ICM42605_Read_Byte(ICM42605_PWR_MGMT0); 	/* ��ȡPWR��MGMT0��ǰ�Ĵ�����ֵ(page72) */
			reg_val &= ~(1 << 5);		/* ʹ���¶Ȳ��� */
			reg_val |= ((3) << 2);		/* ����GYRO_MODE  0:�ر� 1:���� 2:Ԥ�� 3:������ */
			reg_val |= (3);				/* ����ACCEL_MODE 0:�ر� 1:�ر� 2:�͹��� 3:������ */
			ICM42605_Write_Byte(ICM42605_PWR_MGMT0, reg_val);
			delay_ms(1); 		/* ������PWR��MGMT0�Ĵ����� 200us�ڲ������κζ�д�Ĵ����Ĳ��� */
			return 0;
	}
		else return 1;
}

/**-------------------------------------------------------------------------------------------------------------------
  * @brief    ��ֵ�˲�����¶�ֵ
  * @param    *ICM42605_Temperature ��ʵ���¶�ֵ
  * @return   �¶�ֵ
  * @note     ʹ��ģ��FIFO��10�β�����ƽ��ֵ
  * @example  MPU_Get_Temperature(&MPU6050_Temperature); ����MPU6050_Temperature��Ϊ��õ��¶ȡ�
  * @date     2022/6/21 16:15
  * @Author   WxxGreat
  *-----------------------------------------------------------------------------------------------------------------*/
#ifndef Temperature_FIFO_SIZE  
#define Temperature_FIFO_SIZE 10  //�������ݸ���
#endif
static short ICM42605_Temperature_FIFO[Temperature_FIFO_SIZE] = {0};
void Get_ICM42605_Tempdata(float* ICM42605_Temperature)
{
  static unsigned char ICM42605_Temperature_Index;

	uint8_t buf[2];
	uint8_t i ;
	
	ICM42605_Read_Len(ICM42605_ADDRESS, ICM42605_TEMP_DATA1, 2, buf);

	ICM42605_Temperature_FIFO[ICM42605_Temperature_Index] =  ((unsigned short int)buf[0] << 8) | buf[1];//���������¶�ֵ�ŵ�FIFO������
	ICM42605_Temperature_Index = (ICM42605_Temperature_Index + 1) % Temperature_FIFO_SIZE;//������һ��ֵ����FIFO�����е�λ��
	
	for (i = 0; i < Temperature_FIFO_SIZE; i++)  *ICM42605_Temperature += ICM42605_Temperature_FIFO[i];//��FIFO�����е�ֵ�ۼ�

	*ICM42605_Temperature =   (*ICM42605_Temperature / Temperature_FIFO_SIZE / 132.48f) + 25;
}




//-------------------------------------------------------------------------------------------------------------------
//  @brief      ��ȡICM42605���ٶȼ�����
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:               ִ�иú�����ֱ�Ӳ鿴��Ӧ�ı�������
//-------------------------------------------------------------------------------------------------------------------
void Get_ICM42605_accdata(short *ax, short *ay, short *az)
{
    uint8_t dat[6];

	  ICM42605_Read_Len(ICM42605_ADDRESS, ICM42605_ACCEL_DATA_X1, 6, dat);
	
    *ax = (int16_t)(((int16_t)dat[0]<<8 | dat[1]));
    *ay = (int16_t)(((int16_t)dat[2]<<8 | dat[3]));
    *az = (int16_t)(((int16_t)dat[4]<<8 | dat[5]));
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      ��ȡICM42605����������
//  @param      NULL
//  @return     void
//  @since      v1.0
//  Sample usage:ִ�иú�����ֱ�Ӳ鿴��Ӧ�ı�������
//-------------------------------------------------------------------------------------------------------------------
void Get_ICM42605_gyro(short *gx, short *gy, short *gz)
{
    uint8_t dat[6];

	 ICM42605_Read_Len(ICM42605_ADDRESS, ICM42605_GYRO_DATA_X1, 6, dat);
	
    *gx = (int16_t)(((int16_t)dat[0]<<8 | dat[1]));
    *gy = (int16_t)(((int16_t)dat[2]<<8 | dat[3]));
    *gz = (int16_t)(((int16_t)dat[4]<<8 | dat[5]));
}



//IIC����д
//addr:������ַ
//reg:�Ĵ�����ַ
//len:д�볤��
//buf:������
//����ֵ:0,����
//����,�������
uint8_t ICM42605_Write_Len(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
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
//IIC������
//addr:������ַ
//reg:Ҫ��ȡ�ļĴ�����ַ
//len:Ҫ��ȡ�ĳ���
//buf:��ȡ�������ݴ洢��
//����ֵ:0,����
//����,�������
uint8_t ICM42605_Read_Len(uint8_t addr, uint8_t reg, uint8_t len, uint8_t *buf)
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
//IICдһ���ֽ�
//reg:�Ĵ�����ַ
//data:����
//����ֵ:0,����
//����,�������
uint8_t ICM42605_Write_Byte(uint8_t reg, uint8_t data)
{
	IIC_Start();
	IIC_Send_Byte((ICM42605_ADDRESS << 1) | 0); //����������ַ+д����
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
//IIC��һ���ֽ�
//reg:�Ĵ�����ַ
//����ֵ:����������
uint8_t ICM42605_Read_Byte(uint8_t reg)
{
	uint8_t res;
	IIC_Start();
	IIC_Send_Byte((ICM42605_ADDRESS << 1) | 0); //����������ַ+д����
	IIC_Wait_Ack();						//�ȴ�Ӧ��
	IIC_Send_Byte(reg);					//д�Ĵ�����ַ
	IIC_Wait_Ack();						//�ȴ�Ӧ��
	IIC_Start();
	IIC_Send_Byte((ICM42605_ADDRESS << 1) | 1); //����������ַ+������
	IIC_Wait_Ack();						//�ȴ�Ӧ��
	res = IIC_Read_Byte(0);				//��ȡ����,����nACK
	IIC_Stop();							//����һ��ֹͣ����
	return res;
}
