#include "sys.h"

/**
 * @brief          ʹ��Mahony������̬�����ƽ�⳵
 *
 * @note           Forkde from my former project "MPU6050 DMP Demo For STM32F10x" 
 *
 * @date           2021-8-20 16:32
 *
 * @LastEditTime   2022-6-28 16:05
 *
 * @author         WxxGreat
 *
 * @version        1.0 ������ƽ��  
 */

int main(void)
{
	All_HardWare_init();

	while (1)
	{
		Protect_Check();
		LED_show_working();
		LCD_show_Brief_info();
	}
	
	
}
