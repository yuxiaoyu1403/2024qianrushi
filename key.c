#include <rtthread.h>
#include "wenjian.h"
#include <board.h>

/* 配置 KEY 输入引脚  */
#define PIN_KEY0        GET_PIN(C, 0)     // PC0:  KEY0         --> KEY LEFT
#define PIN_KEY1        GET_PIN(C, 1)      // PC1 :  KEY1         --> KEY DOWN
#define PIN_KEY2        GET_PIN(C, 4)      // PC4 :  KEY2         --> KEY RIGHT
#define PIN_KEY3        GET_PIN(C, 5)     // PC5:  WK_UP        --> KEY UP

#define led1  GET_PIN(E, 11)
#define led2  GET_PIN(D, 8)
#define led3  GET_PIN(E, 12)
#define led4  GET_PIN(D, 9)
#define motor1  GET_PIN(A, 6)
#define motor2  GET_PIN(E, 3)
#define motor3  GET_PIN(F, 6)
#define motor4  GET_PIN(F, 5)

#define THREAD_PRIORITY         25
#define THREAD_STACK_SIZE       512
#define THREAD_TIMESLICE        5

static rt_thread_t tid1 = RT_NULL;

static void key_Init(void)
{
    /* 设置按键引脚为输入模式 */
    rt_pin_mode(PIN_KEY0, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(PIN_KEY1, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(PIN_KEY2, PIN_MODE_INPUT_PULLUP);
    rt_pin_mode(PIN_KEY3, PIN_MODE_INPUT_PULLUP);

    rt_pin_mode(led1, PIN_MODE_OUTPUT);
    rt_pin_write(led1,PIN_HIGH);
    rt_pin_mode(led2, PIN_MODE_OUTPUT);
    rt_pin_write(led2,PIN_HIGH);
    rt_pin_mode(led3, PIN_MODE_OUTPUT);
    rt_pin_write(led3,PIN_HIGH);
    rt_pin_mode(led4, PIN_MODE_OUTPUT);
    rt_pin_write(led4,PIN_HIGH);

    rt_pin_mode(motor1, PIN_MODE_OUTPUT);
    rt_pin_write(motor1,PIN_LOW);
    rt_pin_mode(motor2, PIN_MODE_OUTPUT);
    rt_pin_write(motor2,PIN_LOW);

    rt_pin_mode(motor3, PIN_MODE_OUTPUT);
    rt_pin_write(motor3,PIN_LOW);
    rt_pin_mode(motor4, PIN_MODE_OUTPUT);
    rt_pin_write(motor4,PIN_LOW);
}

static uint8_t button_read()
{
    if (rt_pin_read(PIN_KEY0) == PIN_LOW)
    {
        return 1;
    }
    if (rt_pin_read(PIN_KEY1) == PIN_LOW)
    {
        return 2;
    }
    if (rt_pin_read(PIN_KEY2) == PIN_LOW)
    {
        return 3;
    }
    if (rt_pin_read(PIN_KEY3) == PIN_LOW)
    {
        return 4;
    }
    return 0;
}

/* key1的入口函数 */
static void key_read()
{
    uint32_t key = button_read();
    switch (key)
    {
    case 0:
        break;
    case 1:
        rt_kprintf("KEY0 pressed!");
        break;
    case 2:
        rt_kprintf("KEY1 pressed!");
        rt_pin_write(motor1,PIN_HIGH);
        rt_pin_write(motor3,PIN_HIGH);
        break;
    case 3:
        rt_kprintf("KEY2 pressed!");
        rt_pin_write(motor1,PIN_LOW);
        rt_pin_write(motor3,PIN_LOW);
        break;
    case 4:
        rt_kprintf("KEY3 pressed!");
        break;
    }
}

/* 线程1的入口函数 */
static void key_select(void *parameter)
{
    key_Init();
    while (1)
    {
        key_read();
        rt_thread_mdelay(30);
    }
}


int key1(void)
{
    key_Init();

    /* 创建线程1，名称是key，入口是key1*/
    tid1 = rt_thread_create("key",
                            key_select, RT_NULL,
                            THREAD_STACK_SIZE,
                            THREAD_PRIORITY, THREAD_TIMESLICE);

    /* 如果获得线程控制块，启动这个线程 */
    if (tid1 != RT_NULL)
        rt_thread_startup(tid1);
    return RT_EOK;
}

/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(key1, select key);

