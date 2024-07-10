#include <rtthread.h>

static struct rt_semaphore static_sem1;

void sem1_init(void)
{
    rt_sem_init(&static_sem1, "sem", 0, RT_IPC_FLAG_PRIO);
}

void sem1_send(void)
{
    rt_sem_release(&static_sem1);
}

uint8_t sem1_recive()
{
    if ( rt_sem_take(&static_sem1, RT_WAITING_FOREVER) == RT_EOK)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
