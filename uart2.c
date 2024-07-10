#include <rtthread.h>
#include <rtdevice.h>
#include "wenjian.h"

#define SAMPLE_UART_NAME    "uart2"

/* 用于接收消息的信号量 */
static struct rt_semaphore rx_sem;
static rt_device_t serial;

//static int uart_data_sample(void);

static void uart_parse_frame(uint8_t *buffer, uint8_t length)
{
    uint8_t sum = 0;
    for (uint8_t i = 0; i < (length - 1); i++)
        sum += *(buffer + i);
    if (!(sum == *(buffer + length - 1)))
        return;
    if (!(*(buffer) == 0xAA && *(buffer + 1) == 0x29))
        return;

    /* 参数读取ID 0x42 */
    if (buffer[2] == 0x42)
    {
        uint16_t flag = buffer[4];
        int16_t num = ((uint16_t)buffer[5] << 8) + buffer[6];
        int16_t blob_x = ((uint16_t)buffer[7] << 8) + buffer[8];
        int16_t blob_y = ((uint16_t)buffer[9] << 8) + buffer[10];
        uint16_t t_ms = buffer[11];
        rt_kprintf("flag:%d num:%d blob_x:%d blob_y:%d ms:%d\n", flag, num, blob_x, -blob_y, t_ms);
        if(flag == 2 && num < 8)
        {
            send_mail(num, blob_x, -blob_y);
        }
        else if(flag == 2 && num == 8)
        {
            lcd_show_string(10, 69, 32, "numbers:%d", num);
        }
    }
}

static int uart_receive_byte(uint8_t data)
{
    static uint8_t RxBuffer[50];
    static uint8_t _data_len = 0, _data_cnt = 0;
    static uint8_t state = 0;

    /* 帧头 */
    if (state == 0 && data == 0xAA)
    {
        _data_cnt = 0;
        _data_len = 0;

        state = 1;
        RxBuffer[0] = data;
    }
    /* 帧头 */
    else if (state == 1 && data == 0x29)
    {
        state = 2;
        RxBuffer[1] = data;
    }
    /* ID */
    else if (state == 2 && data < 0xF1)
    {
        state = 3;
        RxBuffer[2] = data;
    }
    /* 数据长度 */
    else if (state == 3 && data < 50)
    {
        state = 4;
        RxBuffer[3] = data;
        _data_len = data;
    }
    /* 和校验 */
    else if (state == 4)
    {
        RxBuffer[4 + _data_cnt++] = data;
        if (_data_cnt >= _data_len)
            state = 5;
    }
    /* 数据解析 */
    else if (state == 5)
    {
        state = 0;
        RxBuffer[4 + _data_cnt++] = data;

        uart_parse_frame(RxBuffer, _data_cnt + 4);
        return 1;
    }
    else
        state = 0;

    return 0;
}

/* 接收数据回调函数 */
static rt_err_t uart_rx_ind(rt_device_t dev, rt_size_t size)
{
    /* 串口接收到数据后产生中断，调用此回调函数，然后发送接收信号量 */
    if (size > 0)
    {
        rt_sem_release(&rx_sem);
    }

    return RT_EOK;
}

static char uart_sample_get_char(void)
{
    char ch;

    while (rt_device_read(serial, 0, &ch, 1) == 0)
    {
        rt_sem_control(&rx_sem, RT_IPC_CMD_RESET, RT_NULL);
        rt_sem_take(&rx_sem, RT_WAITING_FOREVER);
    }
    return ch;
}

/* 数据解析线程 */
static void data_parsing(void)
{
    while (1)
    {
        uart_receive_byte(uart_sample_get_char());
    }
}

int uart2_sample(void)
{
    rt_err_t ret = RT_EOK;
    struct serial_configure config = RT_SERIAL_CONFIG_DEFAULT;  /* 初始化配置参数 */

    /* 查找系统中的串口设备 */
    serial = rt_device_find(SAMPLE_UART_NAME);
    if (!serial)
    {
        rt_kprintf("find %s failed!\n", SAMPLE_UART_NAME);
        return RT_ERROR;
    }

    /* 修改串口配置参数 */
    config.baud_rate = 115200;          //修改波特率为 115200
    config.data_bits = DATA_BITS_8;     //数据位 8
    config.stop_bits = STOP_BITS_1;     //停止位 1
    config.bufsz     = 512;             //修改缓冲区
    config.parity    = PARITY_NONE;     //无奇偶校验位

    /* 控制串口设备。通过控制接口传入命令控制字，与控制参数 */
    rt_device_control(serial, RT_DEVICE_CTRL_CONFIG, &config);

    /* 初始化信号量 */
    rt_sem_init(&rx_sem, "rx_sem", 0, RT_IPC_FLAG_FIFO);
    /* 以中断接收及轮询发送模式打开串口设备 */
    rt_device_open(serial, RT_DEVICE_FLAG_INT_RX);
    /* 设置接收回调函数 */
    rt_device_set_rx_indicate(serial, uart_rx_ind);

    /* 创建 serial 线程 */
    rt_thread_t thread = rt_thread_create("UART2", (void (*)(void *parameter))data_parsing, RT_NULL, 1024, 25, 10);
    /* 创建成功则启动线程 */
    if (thread != RT_NULL)
    {
        rt_thread_startup(thread);
    }
    else
    {
        ret = RT_ERROR;
    }

    return ret;
}
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(uart2_sample, uart device sample);
