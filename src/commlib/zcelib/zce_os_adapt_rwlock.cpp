
#include "zce_predefine.h"
#include "zce_os_adapt_predefine.h"
#include "zce_os_adapt_mutex.h"
#include "zce_os_adapt_condi.h"
#include "zce_os_adapt_time.h"
#include "zce_os_adapt_error.h"
#include "zce_os_adapt_rwlock.h"
#include "zce_trace_debugging.h"

//��д���Ķ���ĳ�ʼ��
int ZCE_LIB::pthread_rwlock_init(pthread_rwlock_t *rwlock,
                                 const pthread_rwlockattr_t *attr)
{
#if defined (ZCE_OS_WINDOWS)

    //�����������Ұ��ظ���ʼ�����Ƿ��ʼ���ĸ����ж�ɾ���ˣ��о�����������û��Ҫ

    //WIN SVR 2008�Ժ��������֧��
#if defined ZCE_SUPPORT_WINSVR2008 && ZCE_SUPPORT_WINSVR2008 == 1

    if (rwlock->use_win_slim_)
    {
        ::InitializeSRWLock(&(rwlock->rwlock_slim_));
        return 0;
    }

#endif
    //������ù��ֻ��ģ��

    int result = 0;

    if (attr)
    {
        rwlock->simulate_rw_.priority_to_write_ = attr->priority_to_write_;
    }
    else
    {
        rwlock->simulate_rw_.priority_to_write_ = false;
    }
    pthread_mutexattr_t mutex_attr;
    ZCE_LIB::pthread_mutexattr_init(&mutex_attr);
    mutex_attr.lock_shared_ = PTHREAD_PROCESS_PRIVATE;
    mutex_attr.lock_type_ = PTHREAD_MUTEX_RECURSIVE;

    //��ʼ������ͬ������

    //һЩ��������д�ı���
    if ( (result = ZCE_LIB::pthread_mutex_init(&rwlock->simulate_rw_.rw_mutex_, &mutex_attr)) != 0)
    {
        return EINVAL;
    }

    //�ȴ���ȡ������������ʼ��
    if ( (result = ZCE_LIB::pthread_cond_initex(&rwlock->simulate_rw_.rw_condreaders_,
                                                false )) != 0)
    {
        ZCE_LIB::pthread_mutex_destroy(&rwlock->simulate_rw_.rw_mutex_);
        return EINVAL;
    }

    //�ȴ�д�������������ʼ��
    if ( (result = ZCE_LIB::pthread_cond_initex(&rwlock->simulate_rw_.rw_condwriters_,
                                                false)) != 0)
    {
        ZCE_LIB::pthread_cond_destroy(&rwlock->simulate_rw_.rw_condreaders_);
        ZCE_LIB::pthread_mutex_destroy(&rwlock->simulate_rw_.rw_mutex_);
        return EINVAL;
    }

    rwlock->simulate_rw_.rw_nwaitreaders_ = 0;
    rwlock->simulate_rw_.rw_nwaitwriters_ = 0;
    rwlock->simulate_rw_.rw_refcount_ = 0;

    return 0;

#elif defined (ZCE_OS_LINUX)

    return ::pthread_rwlock_init(rwlock, attr);
#endif

}

//��ʼ����д������
int ZCE_LIB::pthread_rwlock_initex(pthread_rwlock_t *rwlock,
                                   bool  use_win_slim,
                                   bool  priority_to_write)
{
    int result = 0;
    pthread_rwlockattr_t attr;

#if defined ZCE_OS_WINDOWS
    attr.priority_to_write_ = priority_to_write;
    //���֧��2008���������������
#if defined ZCE_SUPPORT_WINSVR2008 && ZCE_SUPPORT_WINSVR2008 == 1
    rwlock->use_win_slim_ = use_win_slim;
#else
    rwlock->use_win_slim_ = false;
    ZCE_UNUSED_ARG(use_win_slim);
#endif

#elif defined ZCE_OS_LINUX

    ZCE_UNUSED_ARG(use_win_slim);
    ZCE_UNUSED_ARG(priority_to_write);

    //��ʵ�����룬������NULL����ʵ��һ��
    result = ::pthread_rwlockattr_init(&attr);

    if (result != 0)
    {
        return result;
    }

    result = ::pthread_rwlockattr_setpshared(&attr, PTHREAD_PROCESS_PRIVATE);
    if (result != 0)
    {
        return result;
    }

#endif

    result = ZCE_LIB::pthread_rwlock_init(rwlock, &attr);

    if (result != 0)
    {
        return result;
    }

    return 0;
}

//��д���Ķ��������
int ZCE_LIB::pthread_rwlock_destroy(pthread_rwlock_t *rwlock)
{
#if defined (ZCE_OS_WINDOWS)

    //WIN SVR 2008�Ժ��������֧��
#if defined ZCE_SUPPORT_WINSVR2008 && ZCE_SUPPORT_WINSVR2008 == 1

    if (rwlock->use_win_slim_)
    {
        //::ReleaseSRWLockShared(&(rwlock->rwlock_slim_));
        return 0;
    }

#endif

    //���еȴ��ģ���������
    if (rwlock->simulate_rw_.rw_refcount_ != 0
        || rwlock->simulate_rw_.rw_nwaitreaders_ != 0
        || rwlock->simulate_rw_.rw_nwaitwriters_ != 0)
    {
        return EBUSY;
    }

    ZCE_LIB::pthread_mutex_destroy(&rwlock->simulate_rw_.rw_mutex_);
    ZCE_LIB::pthread_cond_destroy(&rwlock->simulate_rw_.rw_condreaders_);
    ZCE_LIB::pthread_cond_destroy(&rwlock->simulate_rw_.rw_condwriters_);

    return 0;

#elif defined (ZCE_OS_LINUX)
    return ::pthread_rwlock_destroy(rwlock);
#endif
}

//��ö�ȡ����
int ZCE_LIB::pthread_rwlock_rdlock(pthread_rwlock_t *rwlock)
{

#if defined (ZCE_OS_WINDOWS)

#if defined ZCE_SUPPORT_WINSVR2008 && ZCE_SUPPORT_WINSVR2008 == 1

    //�����WIN�Դ��Ķ�д��
    if (rwlock->use_win_slim_)
    {
        ::AcquireSRWLockShared(&(rwlock->rwlock_slim_));
		rwlock->slim_mode_ = ZCE_SLIM_USE_SHARED_LOCK;
        return 0;
    }

#endif  //WIN SERVER 2008

    //���ϱ�����
    int  result = ZCE_LIB::pthread_mutex_lock(&rwlock->simulate_rw_.rw_mutex_);

    if ( 0 != result  )
    {
        return(result);
    }

    //PTHREAD WIN32��ʵ��������ط�û�������������ȴ���ԭ��λ�ã�������Ϊ������覴õģ�
    //���ܻᵼ�½����CPUѭ��

    //�ȴ���ö�д�������������д��������Ҫд������ڵȴ���ƫ��д����
    while (  (rwlock->simulate_rw_.rw_refcount_ < 0)
             || ( true == rwlock->simulate_rw_.priority_to_write_ && rwlock->simulate_rw_.rw_nwaitwriters_ > 0 ) )
    {
        rwlock->simulate_rw_.rw_nwaitreaders_++;
        //����wait������simulate_rw_.rw_mutex_�ᱻ�򿪣��������˻��������ʱ�����
        result = ZCE_LIB::pthread_cond_wait(&rwlock->simulate_rw_.rw_condreaders_,
                                            &(rwlock->simulate_rw_.rw_mutex_));
        rwlock->simulate_rw_.rw_nwaitreaders_--;

        if (result != 0)
        {
            break;
        }
    }

    //��õ��˶���������
    if (result == 0)
    {
        rwlock->simulate_rw_.rw_refcount_++;
    }

    ZCE_LIB::pthread_mutex_unlock(&rwlock->simulate_rw_.rw_mutex_);
    return (result);

#elif defined (ZCE_OS_LINUX)

    return ::pthread_rwlock_rdlock(rwlock);
#endif

}

//���Ի�ȡ��ȡ��
int ZCE_LIB::pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock)
{

#if defined (ZCE_OS_WINDOWS)

#if defined ZCE_SUPPORT_WINSVR2008 && ZCE_SUPPORT_WINSVR2008 == 1

    //�����WIN�Դ��Ķ�д��
    if (rwlock->use_win_slim_)
    {
        BOOL bret = ::TryAcquireSRWLockShared(&(rwlock->rwlock_slim_));
        if (FALSE == bret )
        {
            return EBUSY;
        }
		rwlock->slim_mode_ = ZCE_SLIM_USE_SHARED_LOCK;
        return 0;
    }

#endif //WIN SERVER 2008

    int result = ZCE_LIB::pthread_mutex_lock(&rwlock->simulate_rw_.rw_mutex_);

    if ( result != 0)
    {
        return(result);
    }

    //���������д��������Ҫд������ڵȴ�,��ô�Ͳ���get��ȡ��
    if ((rwlock->simulate_rw_.rw_refcount_ < 0)
        || ( true == rwlock->simulate_rw_.priority_to_write_ && rwlock->simulate_rw_.rw_nwaitwriters_ > 0 ))
    {
        result = EBUSY;
    }
    else
    {
        rwlock->simulate_rw_.rw_refcount_++;
    }

    ZCE_LIB::pthread_mutex_unlock(&rwlock->simulate_rw_.rw_mutex_);
    return(result);

#elif defined (ZCE_OS_LINUX)
    return ::pthread_rwlock_tryrdlock(rwlock);
#endif
}

//��ȡ���ĳ�ʱ�������������UNP V2��û�и�����
int ZCE_LIB::pthread_rwlock_timedrdlock(pthread_rwlock_t *rwlock,
                                        const ::timespec *abs_timeout_spec)
{
#if defined (ZCE_OS_WINDOWS)

#if defined ZCE_SUPPORT_WINSVR2008 && ZCE_SUPPORT_WINSVR2008 == 1

    //�����WIN�Դ��Ķ�д��
    if (rwlock->use_win_slim_)
    {
        return EINVAL;
    }

#endif

    int result = ZCE_LIB::pthread_mutex_timedlock(&rwlock->simulate_rw_.rw_mutex_,
                                                  abs_timeout_spec);

    //���е����Ϊɶphtread�ĺܶຯ���þ���ʱ���ˣ�abs_timeout_spec
    if ( result != 0)
    {
        return(result);
    }

    //�ȴ���ö�д�������������д��������Ҫд������ڵȴ���ƫ��д����
    while ( (rwlock->simulate_rw_.rw_refcount_ < 0)
            || (true == rwlock->simulate_rw_.priority_to_write_ && rwlock->simulate_rw_.rw_nwaitwriters_ > 0) )
    {
        rwlock->simulate_rw_.rw_nwaitreaders_++;
        //����wait������simulate_rw_.rw_mutex_�ᱻ�򿪣��������˻��������ʱ�����
        result = ZCE_LIB::pthread_cond_timedwait(&rwlock->simulate_rw_.rw_condreaders_,
                                                 &(rwlock->simulate_rw_.rw_mutex_),
                                                 abs_timeout_spec);
        rwlock->simulate_rw_.rw_nwaitreaders_--;

        if (result != 0)
        {
            break;
        }
    }

    //��õ��˶���������
    if (result == 0)
    {
        rwlock->simulate_rw_.rw_refcount_++;
    }

    ZCE_LIB::pthread_mutex_unlock(&rwlock->simulate_rw_.rw_mutex_);
    return(result);

#elif defined (ZCE_OS_LINUX)
    return ::pthread_rwlock_timedrdlock(rwlock, abs_timeout_spec);
#endif
}

//�Ǳ�׼����ȡ���ĳ�ʱ������ʱ�����������timeval��
int ZCE_LIB::pthread_rwlock_timedrdlock(pthread_rwlock_t *rwlock,
                                        const timeval *abs_timeout_val)
{
    //���ʱ���Ǿ���ֵʱ�䣬Ҫ����Ϊ���ʱ��
    ::timespec abs_timeout_spec = ZCE_LIB::make_timespec(abs_timeout_val);
    return ZCE_LIB::pthread_rwlock_timedrdlock(rwlock, &abs_timeout_spec);
}

//��ȡд��
int ZCE_LIB::pthread_rwlock_wrlock(pthread_rwlock_t *rwlock)
{
#if defined (ZCE_OS_WINDOWS)

#if defined ZCE_SUPPORT_WINSVR2008 && ZCE_SUPPORT_WINSVR2008 == 1

    //�����WIN�Դ��Ķ�д��
    if (rwlock->use_win_slim_)
    {
        ::AcquireSRWLockExclusive(&(rwlock->rwlock_slim_));
		rwlock->slim_mode_ = ZCE_SLIM_USE_EXCLUSIVE_LOCK;
        return 0;
    }

#endif

    int result = pthread_mutex_lock(&rwlock->simulate_rw_.rw_mutex_);
    if ( result != 0)
    {
        return(result);
    }

    //���������ʹ���������۶�д����Ҫ�ȴ��������ȡ���ȣ�������˻��ڵȴ�����Ҳ�ȴ�
    while ( (rwlock->simulate_rw_.rw_refcount_ != 0)
            || (false == rwlock->simulate_rw_.priority_to_write_ && rwlock->simulate_rw_.rw_nwaitreaders_ > 0) )
    {
        rwlock->simulate_rw_.rw_nwaitwriters_++;
        result = ZCE_LIB::pthread_cond_wait(&rwlock->simulate_rw_.rw_condwriters_,
                                            &(rwlock->simulate_rw_.rw_mutex_));
        rwlock->simulate_rw_.rw_nwaitwriters_--;

        if (result != 0)
        {
            break;
        }
    }

    //��ʶ����д�߻��
    if (result == 0)
    {
        rwlock->simulate_rw_.rw_refcount_ = -1;
    }

    ZCE_LIB::pthread_mutex_unlock(&rwlock->simulate_rw_.rw_mutex_);
    return(result);

#elif defined (ZCE_OS_LINUX)
    return ::pthread_rwlock_wrlock(rwlock);
#endif
}

//�����ܷ�ӵ��д������������ʽ
int ZCE_LIB::pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock)
{

#if defined (ZCE_OS_WINDOWS)

#if defined ZCE_SUPPORT_WINSVR2008 && ZCE_SUPPORT_WINSVR2008 == 1

    //�����WIN�Դ��Ķ�д��
    if (rwlock->use_win_slim_)
    {
        BOOL bret = ::TryAcquireSRWLockExclusive(&(rwlock->rwlock_slim_));
        if (FALSE == bret )
        {
            return EBUSY;
        }
		rwlock->slim_mode_ = ZCE_SLIM_USE_EXCLUSIVE_LOCK;
        return 0;
    }

#endif //WIN SERVER 2008

    int result = ZCE_LIB::pthread_mutex_lock(&rwlock->simulate_rw_.rw_mutex_);

    if ( result != 0)
    {
        return(result);
    }

    //����ж���д�ߴ��ڣ��Ͳ���ӵ��д��
    if ( (rwlock->simulate_rw_.rw_refcount_ != 0)
         || (false == rwlock->simulate_rw_.priority_to_write_ && rwlock->simulate_rw_.rw_nwaitreaders_ > 0) )
    {
        result = EBUSY;
    }
    //�����ӵ��д��
    else
    {
        rwlock->simulate_rw_.rw_refcount_ = -1;
    }

    ZCE_LIB::pthread_mutex_unlock(&rwlock->simulate_rw_.rw_mutex_);
    return(result);

#elif defined (ZCE_OS_LINUX)
    return ::pthread_rwlock_trywrlock(rwlock);
#endif
}

//��ȡд�������ҵȴ�����ʱΪֹ��
int ZCE_LIB::pthread_rwlock_timedwrlock(pthread_rwlock_t *rwlock,
                                        const ::timespec *abs_timeout_spec)
{
#if defined (ZCE_OS_WINDOWS)

#if defined ZCE_SUPPORT_WINSVR2008 && ZCE_SUPPORT_WINSVR2008 == 1

    //�����WIN�Դ��Ķ�д��
    if (rwlock->use_win_slim_)
    {
        return EINVAL;
    }

#endif

    int result = ZCE_LIB::pthread_mutex_timedlock(&rwlock->simulate_rw_.rw_mutex_, abs_timeout_spec);

    if ( result != 0)
    {
        return(result);
    }

    //���������ʹ���������۶�д����Ҫ�ȴ��������ȡ���ȣ�������˻��ڵȴ�����Ҳ�ȴ�
    while ((rwlock->simulate_rw_.rw_refcount_ != 0)
           || (false == rwlock->simulate_rw_.priority_to_write_ && rwlock->simulate_rw_.rw_nwaitreaders_ > 0) )
    {
        rwlock->simulate_rw_.rw_nwaitwriters_++;
        result = ZCE_LIB::pthread_cond_timedwait(&rwlock->simulate_rw_.rw_condwriters_,
                                                 &(rwlock->simulate_rw_.rw_mutex_),
                                                 abs_timeout_spec);
        rwlock->simulate_rw_.rw_nwaitwriters_--;

        if (result != 0)
        {
            break;
        }
    }

    if (result == 0)
    {
        rwlock->simulate_rw_.rw_refcount_ = -1;
    }

    ZCE_LIB::pthread_mutex_unlock(&rwlock->simulate_rw_.rw_mutex_);
    return(result);

#elif defined (ZCE_OS_LINUX)
    return ::pthread_rwlock_timedwrlock(rwlock, abs_timeout_spec);
#endif
}

//�Ǳ�׼����ȡ���ĳ�ʱ������ʱ�����������timeval��
int ZCE_LIB::pthread_rwlock_timedwrlock(pthread_rwlock_t *rwlock,
                                        const timeval *abs_timeout_val)
{
    //���ʱ���Ǿ���ֵʱ�䣬Ҫ����Ϊ���ʱ��
    ::timespec abs_timeout_spec = ZCE_LIB::make_timespec(abs_timeout_val);
    return ZCE_LIB::pthread_rwlock_timedwrlock(rwlock, &abs_timeout_spec);
}

//�������������������Խ����ȡ������д������������Ҫ�ر�ָ��
int ZCE_LIB::pthread_rwlock_unlock(pthread_rwlock_t *rwlock)
{
#if defined (ZCE_OS_WINDOWS)

#if defined ZCE_SUPPORT_WINSVR2008 && ZCE_SUPPORT_WINSVR2008 == 1

    //�����WIN�Դ��Ķ�д��
    if (rwlock->use_win_slim_)
    {
        if (ZCE_SLIM_USE_SHARED_LOCK == rwlock->slim_mode_)
        {
            ::ReleaseSRWLockShared(&(rwlock->rwlock_slim_));
        }
        else
        {
            ::ReleaseSRWLockExclusive(&(rwlock->rwlock_slim_));
        }
        return 0;
    }

#endif

    //������
    int result = ZCE_LIB::pthread_mutex_lock(&rwlock->simulate_rw_.rw_mutex_);

    if ( result != 0)
    {
        return(result);
    }

    //����Ƕ���ռ��������
    if (rwlock->simulate_rw_.rw_refcount_ > 0)
    {
        rwlock->simulate_rw_.rw_refcount_--;
    }
    //��������ң�д�ߣ�ռ��������
    else if (rwlock->simulate_rw_.rw_refcount_ == -1)
    {
        rwlock->simulate_rw_.rw_refcount_ = 0;
    }
    //�����ϲ��ᵽ���
    else
    {
        //�������Ӧ���������д���ˣ�û�м���������������˽�������
    }

    //���ݶ����Ȼ���д�����ȣ����д���
    //���д����
    if (rwlock->simulate_rw_.priority_to_write_)
    {
        //�����ʱ����д����˵ȴ������ȸ����ҷ����ź�
        if (rwlock->simulate_rw_.rw_nwaitwriters_ > 0)
        {
            if (rwlock->simulate_rw_.rw_refcount_ == 0)
            {
                result = ZCE_LIB::pthread_cond_signal(&rwlock->simulate_rw_.rw_condwriters_);
            }
        }
        //�����ʱ���ж��ߵ��ڵȴ��������������㲥
        else if (rwlock->simulate_rw_.rw_nwaitreaders_ > 0)
        {
            result = ZCE_LIB::pthread_cond_broadcast(&rwlock->simulate_rw_.rw_condreaders_);
        }
    }
    //����Ƕ�ȡ����
    else
    {
        if (rwlock->simulate_rw_.rw_nwaitreaders_ > 0)
        {
            result = ZCE_LIB::pthread_cond_broadcast(&rwlock->simulate_rw_.rw_condreaders_);
        }
        //�����ʱ����д����˵ȴ������ȸ����ҷ����ź�
        else if (rwlock->simulate_rw_.rw_nwaitwriters_ > 0)
        {
            if (rwlock->simulate_rw_.rw_refcount_ == 0)
            {
                result = ZCE_LIB::pthread_cond_signal(&rwlock->simulate_rw_.rw_condwriters_);
            }
        }

        //�����ʱ���ж��ߵ��ڵȴ��������������㲥
    }

    ZCE_LIB::pthread_mutex_unlock(&rwlock->simulate_rw_.rw_mutex_);
    return(result);

#elif defined (ZCE_OS_LINUX)
    return ::pthread_rwlock_unlock(rwlock);
#endif

}

