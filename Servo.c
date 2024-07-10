#include "stm32f10x.h"                  // Device header
#include "PWM.h"

void Servo_Init(void)
{
	PWM2_CH2_Init();
	PWM2_CH3_Init();
}

void Servo1_SetAngle(float Angle)
{
	PWM_SetCompare2(Angle / 180 * 2000 + 500);
}

void Servo2_SetAngle(float Angle)
{
	PWM_SetCompare3(Angle / 180 * 2000 + 500);
}