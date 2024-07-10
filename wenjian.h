/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2024-06-28     a2003       the first version
 */
#ifndef APPLICATIONS_WENJIAN_H_
#define APPLICATIONS_WENJIAN_H_

#include <stdio.h>
#include <string.h>
#include <drv_lcd.h>

//sem1
void sem1_init(void);
void sem1_send(void);
uint8_t sem1_recive();
//uart2
int uart2_sample(void);
//uart3
int uart3_sample(void);
//mail
void send_mail(int a,int x, int y);
int mailbox_Init(void);
int data_sample(void);
//motor
int pwm1_motor(void);
int pwm2_motor(void);
//key
int key1(void);

#endif /* APPLICATIONS_WENJIAN_H_ */
