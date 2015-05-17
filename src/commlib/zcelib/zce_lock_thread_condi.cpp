﻿#include "zce_predefine.h"
#include "zce_os_adapt_error.h"
#include "zce_os_adapt_condi.h"
#include "zce_os_adapt_time.h"
#include "zce_trace_log_debug.h"
#include "zce_lock_thread_mutex.h"
#include "zce_lock_thread_condi.h"

//---------------------------------------------------------------------------------------
//为ZCE_Thread_Light_Mutex做的特化
//等待
template <>
void ZCE_Thread_Condition<ZCE_Thread_Light_Mutex>::wait (ZCE_Thread_Light_Mutex *external_mutex)
{
    int ret = ZCE_LIB::pthread_cond_wait(&lock_,
                                         external_mutex->get_lock());

    if (0 != ret)
    {
        ZCE_TRACE_FAIL_RETURN(RS_ERROR, "ZCE_LIB::pthread_cond_init", ret);
        return;
    }
}

//我根据ZCE_Thread_Light_Mutex，ZCE_Thread_Recursive_Mutex给了特化实现

//绝对时间超时的的等待，超时后解锁
template <>
bool ZCE_Thread_Condition<ZCE_Thread_Light_Mutex>::systime_wait(ZCE_Thread_Light_Mutex *external_mutex,
                                                                const ZCE_Time_Value &abs_time)
{
    int ret = ZCE_LIB::pthread_cond_timedwait(&lock_,
                                              external_mutex->get_lock(),
                                              abs_time);

    if (0 != ret)
    {
        ZCE_TRACE_FAIL_RETURN(RS_ERROR, "ZCE_LIB::pthread_cond_init", ret);
        return false;
    }

    return true;
}

//相对时间的超时锁定等待，超时后，解锁
template <>
bool ZCE_Thread_Condition<ZCE_Thread_Light_Mutex>::duration_wait(ZCE_Thread_Light_Mutex *external_mutex, const ZCE_Time_Value &relative_time)
{
    ZCE_Time_Value abs_time (ZCE_LIB::gettimeofday());
    abs_time += relative_time;
    return systime_wait(external_mutex, abs_time);
}

//---------------------------------------------------------------------------------------
//为ZCE_Thread_Light_Mutex做的特化
//等待
template <>
void ZCE_Thread_Condition<ZCE_Thread_Recursive_Mutex>::wait (ZCE_Thread_Recursive_Mutex *external_mutex)
{
    int ret = ZCE_LIB::pthread_cond_wait(&lock_,
                                         external_mutex->get_lock());

    if (0 != ret)
    {
        ZCE_TRACE_FAIL_RETURN(RS_ERROR, "ZCE_LIB::pthread_cond_init", ret);
        return;
    }
}

//我根据ZCE_Thread_Light_Mutex，ZCE_Thread_Recursive_Mutex给了特化实现

//绝对时间超时的的等待，超时后解锁
template <>
bool ZCE_Thread_Condition<ZCE_Thread_Recursive_Mutex>::systime_wait(ZCE_Thread_Recursive_Mutex *external_mutex,
                                                                    const ZCE_Time_Value &abs_time)
{
    int ret = ZCE_LIB::pthread_cond_timedwait(&lock_,
                                              external_mutex->get_lock(),
                                              abs_time);

    if (0 != ret)
    {
        ZCE_TRACE_FAIL_RETURN(RS_ERROR, "ZCE_LIB::pthread_cond_init", ret);
        return false;
    }

    return true;
}

//相对时间的超时锁定等待，超时后，解锁
template <>
bool ZCE_Thread_Condition<ZCE_Thread_Recursive_Mutex>::duration_wait(ZCE_Thread_Recursive_Mutex *external_mutex,
                                                                     const ZCE_Time_Value &relative_time)
{
    ZCE_Time_Value abs_time (ZCE_LIB::gettimeofday());
    abs_time += relative_time;
    return systime_wait(external_mutex, abs_time);
}

