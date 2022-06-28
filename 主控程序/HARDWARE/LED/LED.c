#include "LED.h"


//LED IO��ʼ��
void LED_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;

    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13;        //LED0-->PC.13 �˿�����
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;  //�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;  //IO���ٶ�Ϊ2MHz
    GPIO_Init(GPIOC, &GPIO_InitStructure);            //�����趨������ʼ��GPIOC.13
    GPIO_SetBits(GPIOC, GPIO_Pin_13);                 //PC.13 �����
}
