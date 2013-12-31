
#include "zce_predefine.h"
#include "zce_os_adapt_semaphore.h"
#include "zce_os_adapt_time.h"
#include "zce_os_adapt_error.h"
#include "zce_trace_log_debug.h"
#include "zce_lock_process_semaphore.h"

//���캯��,
ZCE_Process_Semaphore::ZCE_Process_Semaphore(unsigned int init_value,
                                             const char *sem_name):
    lock_(NULL)
{

    ZCE_ASSERT(sem_name);

    int ret = 0;

    sema_name_[0] = '\0';
    sema_name_[sizeof(sema_name_) - 1] = '\0';

    //���������źŵ�,���ֿ��Կ�����unique_name�������

    strncpy(sema_name_, sem_name, PATH_MAX);

    lock_ = ZCE_OS::sem_open(sem_name, O_CREAT, ZCE_DEFAULT_FILE_PERMS, init_value);

    if (!lock_)
    {
        ret = -1;
        ZCE_TRACE_FAIL_RETURN(RS_ERROR, "ZCE_OS::sem_open fail.", ret);
    }

}

ZCE_Process_Semaphore::~ZCE_Process_Semaphore()
{
    //û�г�ʼ����
    if (!lock_)
    {
        return;
    }

    //������ֳ��Ȳ���0����ʾ������
    if ( '\0' != sema_name_[0] )
    {
        //�ͷţ��ر��źŵƶ���ɾ�����ֹ������ļ�
        ZCE_OS::sem_close(lock_);
        ZCE_OS::sem_unlink(sema_name_);
        lock_ = NULL;
    }
    else
    {
        ZCE_OS::sem_destroy(lock_);

        //sem_destroy�����ͷţ�
        delete lock_;
        lock_ = NULL;
    }

}

//����
void ZCE_Process_Semaphore::lock()
{
    //�źŵ�����
    int ret =  ZCE_OS::sem_wait (lock_);

    if (0 != ret)
    {
        ZCE_TRACE_FAIL_RETURN(RS_ERROR, "ZCE_OS::sem_wait", ret);
        return;
    }
}

//��������
bool ZCE_Process_Semaphore::try_lock()
{
    //�źŵ�����
    int ret =  ZCE_OS::sem_trywait (lock_);

    if (0 != ret)
    {
        return false;
    }

    return true;
}

//����,
void ZCE_Process_Semaphore::unlock()
{
    int ret = ZCE_OS::sem_post (lock_);

    if (0 != ret)
    {
        ZCE_TRACE_FAIL_RETURN(RS_ERROR, "ZCE_OS::sem_post", ret);
        return;
    }
}

//����ʱ�䳬ʱ�ĵ���������ʱ�����
bool ZCE_Process_Semaphore::systime_lock(const ZCE_Time_Value &abs_time)
{
    int ret = 0;
    ret = ZCE_OS::sem_timedwait(lock_, abs_time);

    if (0 != ret)
    {
        if (errno != ETIMEDOUT)
        {
            ZCE_TRACE_FAIL_RETURN(RS_ERROR, "ZCE_OS::sem_timedwait", ret);
        }

        return false;
    }

    return true;
}

//���ʱ��ĳ�ʱ��������ʱ�󣬽���
bool ZCE_Process_Semaphore::duration_lock(const ZCE_Time_Value &relative_time)
{
    timeval abs_time = ZCE_OS::gettimeofday();
    abs_time = ZCE_OS::timeval_add(abs_time, relative_time);
    return systime_lock(abs_time);
}

