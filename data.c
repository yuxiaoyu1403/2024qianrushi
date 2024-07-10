#include <rtthread.h>
#include "wenjian.h"

/* 邮箱控制块 */
static struct rt_mailbox mb;
/* 用于放邮件的内存池 */
static char mb_pool[128];

int mailbox_Init(void)
{
    rt_err_t result;
    /* 初始化一个mailbox */
    result = rt_mb_init(&mb,
                        "mbt",                      /* 名称是mbt */
                        &mb_pool[0],                /* 邮箱用到的内存池是mb_pool */
                        sizeof(mb_pool) / 4,        /* 邮箱中的邮件数目，因为一封邮件占4字节 */
                        RT_IPC_FLAG_FIFO);          /* 采用FIFO方式进行线程等待 */
    if (result != RT_EOK)
    {
        rt_kprintf("init mailbox failed.\n");
        return -1;
    }
    return 0;
}

char mb_str[32];
void send_mail(int a,int x, int y)
{
    /* 将x和y格式化为字符串，并存储在mb_str中 */
    snprintf(mb_str, 32, "num:%d , X: %d, Y: %d",a , x, y);
    /* 发送mb_str地址到邮箱中 */
    rt_mb_send(&mb, (rt_uint32_t)&mb_str);
//    rt_kprintf("eg\n");
    rt_thread_mdelay(100);
}



void recv_mail(void *parameter)
{
    char *str_ptr;
    int a, x, y;
    while (1)
    {
        /* 从邮箱中收取邮件 */
        if (rt_mb_recv(&mb, (rt_uint32_t *)&str_ptr, RT_WAITING_FOREVER) == RT_EOK)
        {
            if (sscanf(str_ptr, "num:%d , X: %d, Y: %d",&a, &x, &y) == 3)
            {
                sem1_send();
                lcd_show_string(10, 69, 32, "numbers:%d", a);
//                rt_kprintf("ok\n");
            }
//            /* 延时100ms */
            rt_thread_mdelay(100);
        }
    }
}
int data_sample(void)
{
    mailbox_Init();
    rt_thread_t data_thread = rt_thread_create("data_thread",recv_mail, RT_NULL,1024, 20, 5);
    if (data_thread != RT_NULL)
    {
            rt_thread_startup(data_thread);
    }

    return 0;
}
/* 导出到 msh 命令列表中 */
MSH_CMD_EXPORT(data_sample, recve data);
