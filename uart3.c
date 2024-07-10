#include <rtthread.h>
#include "wenjian.h"

#define SAMPLE_UART_NAME       "uart3"      /* 串口设备名称 */
/* 用于接收消息的信号量 */
static struct rt_semaphore rx_sem;
static rt_device_t serial;

/* 接收数据回调函数 */
static rt_err_t uart_input(rt_device_t dev, rt_size_t size)
{
    /* 串口接收到数据后产生中断，调用此回调函数，然后发送接收信号量 */
    rt_sem_release(&rx_sem);

    return RT_EOK;
}

static void serial_thread_entry(void *parameter)
{
//    char ch;
    int num = 102;
    while (1)
    {
        if ( sem1_recive() == 1)
        {
            rt_thread_mdelay(2000);
            rt_device_write(serial, 0, &num, 1);
            rt_kprintf("send %d\n",num);
        }
    }
}

 int uart3_sample(void)
 {
     rt_err_t ret = RT_EOK;
     /* 查找串口设备 */
     serial = rt_device_find(SAMPLE_UART_NAME);
     if (!serial)
     {
         rt_kprintf("find %s failed!\n", SAMPLE_UART_NAME);
         return RT_ERROR;
     }

     /* 初始化信号量 */
     rt_sem_init(&rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);
     /* 以读写及中断接收方式打开串口设备 */
     rt_device_open(serial, RT_DEVICE_OFLAG_RDWR | RT_DEVICE_FLAG_INT_RX);
     /* 设置接收回调函数 */
     rt_device_set_rx_indicate(serial, uart_input);
     /* 创建 uart3_thread 线程 */
     rt_thread_t uart3_thread = rt_thread_create("UART3",
                                           serial_thread_entry,
                                           RT_NULL,
                                           1024,
                                           25, 10);
     /* 创建成功则启动线程 */
     if (uart3_thread != RT_NULL)
     {
         rt_thread_startup(uart3_thread);
     }
     else
     {
         ret = RT_ERROR;
     }

     return ret;
 }
 /* 导出到 msh 命令列表中 */
 MSH_CMD_EXPORT(uart3_sample, uart device sample);
