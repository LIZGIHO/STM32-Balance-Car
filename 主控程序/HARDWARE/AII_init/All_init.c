#include "All_init.h"

void All_RCC_clock_init(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC, ENABLE); //ʹ��ABC�˿�ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1, ENABLE);												//ʹ�ܶ�ʱ��1��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);												//ʹ��USART1ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2 | RCC_APB1Periph_TIM3 | RCC_APB1Periph_TIM4, ENABLE);	//ʹ�ܶ�ʱ��234��ʱ��
}

void All_HardWare_init(void)
{
	All_RCC_clock_init();

	JTAG_Set(JTAG_SWD_DISABLE); //�ر�JTAG�ӿ�
	JTAG_Set(SWD_ENABLE);		    //��SWD�ӿ� �������������SWD�ӿڵ���
	delay_init();				        //��ʱ������ʼ��

	LED_Init();
	LCD_Init();
	MotorDriver_Init();
	uart_init(115200);

	Encoder_Init_TIM2(); // right
	Encoder_Init_TIM4(); // left

  Set_Motor_Speed( 0 , 0 );
	delay_ms(1000);
	/****************IMU��ʼ��*******************/
#if USE_MPU6050DMP == 1
	IMU_Init();			   // �����ǳ�ʼ��
	while (mpu_dmp_init()) // ������-dmp��ʼ��
	{
		LCD_printf(0, 0, "mpu dmp error:%d", mpu_dmp_init());
		delay_ms(1);
	}
#else
	IMU_Init(); // �����ǳ�ʼ��
#endif
	/*******************************************/


	TIM3_Int_Init(99, 7199); // 72M ��7200 ��100 = 10 ms

//	while (TIM3_Tick < 500)
//	{
//		LCD_printf(200, 90, "%d", TIM3_Tick / 10 * 2);
//		Protect_Check();
//	}
//	LCD_ShowChinese(0, 90, "������У׼�ɹ�", WHITE, BLACK, 32, 0);
//	while (TIM3_Tick < 650)
//	{
//		LCD_ShowChar(224, 90, '.', WHITE, BLACK, 32, 0);
//		Protect_Check();
//	}

	LCD_Fill(0, 90, LCD_W, 122, BLACK);
	PID_init();
}
