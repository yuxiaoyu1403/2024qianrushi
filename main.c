#include "stm32f10x.h"                  // Device header
#include "Delay.h"
#include "OLED.h"
#include "Servo.h"
#include "Serial.h"

uint8_t RxData;

int main(void)
{
	OLED_Init();
	Servo_Init();
	Serial_Init();
	OLED_ShowString(2, 1, "Return:");
	
	while (1)
	{
		if (Serial_GetRxFlag() == 1)
		{
			RxData = Serial_GetRxData();
			if(RxData == 0x66)
			{	
				Servo2_SetAngle(120);
				Servo1_SetAngle(80);
				Delay_ms(500);
				Servo2_SetAngle(30);
				Servo1_SetAngle(130);
				Delay_ms(500);
				Servo2_SetAngle(120);
				Servo1_SetAngle(80);
				Serial_SendByte(RxData & 0x99);
				OLED_ShowHexNum(2, 8, RxData & 0x99, 2);
				OLED_ShowString(1, 1,"Execute Success!");	
			}
			else
			{
				Serial_SendByte(RxData | 0xFF);
				OLED_ShowHexNum(2, 8, RxData | 0xFF, 2);
				OLED_ShowString(1, 1,"Execute Failure!");
			}
		}
	}
}
