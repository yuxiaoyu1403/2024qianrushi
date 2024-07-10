#include <rtthread.h>
#include "wenjian.h"

#define PWM_DEV_NAME            "pwm1"  /* PWM设备名称 */
#define PWM_DEV_CHANNEL         4       /* PWM通道 */
#define THREAD_PRIORITY         25      /* 线程优先级 */
#define THREAD_STACK_SIZE       512     /* 线程栈大小 */
#define THREAD_TIMESLICE        5       /* 线程时间片大小 */

static rt_thread_t tid1 = RT_NULL;      /* 线程句柄 */
struct rt_device_pwm *pwm_dev;          /* PWM设备句柄 */
static rt_uint32_t period = 500000;     /* 周期为0.5ms，单位为纳秒ns */
static rt_uint32_t pulse = 0;           /* PWM脉冲宽度值的增减方向 */
//static rt_uint32_t dir = 1;

/* 线程 pwm_entry 的入口函数 */
static void pwm_entry(void *parameter)
{
    while (1)
    {
    rt_thread_mdelay(50);
//    if (dir)
//    {
//    pulse += 50000; /* 从0值开始每次增加5000ns */
////    rt_kprintf("pulse++");
//    }
//    else
//    {
//    pulse -= 5000; /* 从最大值开始每次减少5000ns */
////    rt_kprintf("pulse--");
//    }
//    if (pulse >= period)r
//    {
//    dir = 0;
//    }
//    if (0 == pulse)
//    {
//    dir = 1;
//    }
    pulse=period/5;
    /* 设置PWM周期和脉冲宽度 */
    rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL, period, pulse);
    }
//    /* step 3、如果不再使用该通道，可以关闭 PWM 通道的输出 */
//    rt_pwm_disable(pwm_dev, PWM_DEV_CHANNEL);
}

int pwm1_motor(void)
{
    /* step 1.1、查找 PWM 设备 */
    pwm_dev = (struct rt_device_pwm *)rt_device_find(PWM_DEV_NAME);
    if (pwm_dev == RT_NULL)
    {
        rt_kprintf("pwm sample run failed! can't find %s device!\n", PWM_DEV_NAME);
        return RT_ERROR;
    }

    /* step 1.2、设置 PWM 周期和脉冲宽度默认值 */
    rt_pwm_set(pwm_dev, PWM_DEV_CHANNEL, period, pulse);
    /* step 1.3、使能 PWM 设备的输出通道 */
    rt_pwm_enable(pwm_dev, PWM_DEV_CHANNEL);

    /* 创建线程，名称是 pwm1 ，入口是 pwm_entry*/
    tid1 = rt_thread_create("pwm1",
                             pwm_entry,
                             RT_NULL,
                             THREAD_STACK_SIZE,
                             THREAD_PRIORITY,
                             THREAD_TIMESLICE);

    /* 如果获得线程控制块，启动这个线程 */
    if (tid1 != RT_NULL)
        rt_thread_startup(tid1);

    return RT_EOK;
}
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(pwm1_motor, pwm sample);
