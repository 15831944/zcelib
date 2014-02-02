
#include "zce_predefine.h"
#include "zce_trace_log_debug.h"
#include "zce_os_adapt_error.h"
#include "zce_os_adapt_coroutine.h"


#if defined ZCE_OS_WINDOWS


//��ΪCreateFiber����Ҫ�ĵĺ���ֽ����WINAPI�ģ�����__stdcall�ģ�������һ��ת����
struct _FIBERS_FUN_ADAPT_STRUCT
{
    ///����ָ��
    ZCE_COROUTINE_FUN_PTR   fun_ptr_;
    //�����ĵ�һ�����������
    void                   *left_para_;
    //�����ĵ�һ���������Ҳ�
    void                   *right_para_;
};

//������ɺ�����������
VOID  WINAPI _fibers_adapt_fun (VOID *fun_para)
{
    _FIBERS_FUN_ADAPT_STRUCT *fun_adapt = (_FIBERS_FUN_ADAPT_STRUCT *)fun_para;
    fun_adapt->fun_ptr_(fun_adapt->left_para_, fun_adapt->right_para_);
}

#endif



//�Ǳ�׼��������һ�������Ƕ�ջ��С���ڲ����Լ�����һ�������ݣ��������ջ�����Ա������deletecontext
int ZCE_OS::makecontext(ucontext_t * uctt,
    size_t slack_size, 
    ZCE_COROUTINE_FUN_PTR fun_ptr,
    void *left_para, 
    void *right_para)
{
#if defined ZCE_OS_WINDOWS

    //ʹ������ṹ��ɺ�������
    struct _FIBERS_FUN_ADAPT_STRUCT fibers_adapt;
    fibers_adapt.fun_ptr_ = fun_ptr;
    fibers_adapt.left_para_ = left_para;
    fibers_adapt.right_para_ = right_para;

    //ע��FIBER_FLAG_FLOAT_SWITCH ��XP�ǲ���֧�ֵģ�
    *uctt = ::CreateFiberEx(slack_size,
        slack_size,
        FIBER_FLAG_FLOAT_SWITCH,
        _fibers_adapt_fun,
        &fibers_adapt);

    if (NULL == uctt)
    {
        return -1;
    }
    return 0;
#elif defined ZCE_OS_LINUX
    //ֻʹ��һ��������������ʹ�ñ�Σ�Windwos��֧��
    const int ONLY_TWO_ARG_COUNT = 2;

    uctt->uc_stack.ss_sp = new char [slack_size];
    uctt->uc_stack.ss_size = slack_size;

    ::makecontext(uctt, 
        (void(*)(void)) fun_ptr,
        ONLY_TWO_ARG_COUNT,
        left_para,
        right_para);
    return 0;
#endif
}

//�Ǳ�׼������
void ZCE_OS::deletecontext(ucontext_t *uctt)
{
#if defined ZCE_OS_WINDOWS
    return ::DeleteFiber(*uctt);
#elif defined ZCE_OS_LINUX

    //�ͷ�ZCE_OS::makecontext����Ŀռ�
    delete [] (char *) uctt->uc_stack.ss_sp ;
    uctt->uc_stack.ss_sp = NULL;
    uctt->uc_stack.ss_size = 0;
#endif
}


int ZCE_OS::getcontext(ucontext_t *uctt)
{
#if defined ZCE_OS_WINDOWS

    //����Ѿ����˳��ˣ��õ���ǰ�˳̵ı�ʶ
    if (TRUE == ::IsThreadAFiber() )
    {
        *uctt = ::GetCurrentFiber();
        if (NULL == uctt)
        {
            return -1;
        }
        return 0;
    }
    //�����ǰ�������˳̣�����ת����ͬʱҲ����ǰ���˳̱�ʶ
    else
    {
        //FIBER_FLAG_FLOAT_SWITCH XP��֧�֣�
        *uctt = ::ConvertThreadToFiberEx(NULL,
            FIBER_FLAG_FLOAT_SWITCH);
        if (NULL == uctt)
        {
            return -1;
        }
        return 0;
    }
   

#elif defined ZCE_OS_LINUX
    return ::getcontext(uctt);
#endif 
}


//����context
int ZCE_OS::setcontext(const ucontext_t *uctt)
{
#if defined ZCE_OS_WINDOWS
    ::SwitchToFiber(*uctt);
    return 0;
#elif defined ZCE_OS_LINUX
    return ::setcontext(uctt);
#endif
}


//����context,���浱ǰ�ģ����ó��µ�
int ZCE_OS::swapcontext(ucontext_t *get_uctt, const ucontext_t *set_uctt)
{
#if defined ZCE_OS_WINDOWS
    int ret = 0;

    if ((get_uctt == NULL) || (set_uctt == NULL)) 
    {
        errno = EINVAL;
        return -1;
    }

    ret = ZCE_OS::getcontext(get_uctt);
    if (ret == 0) 
    {
        ret = ZCE_OS::setcontext(set_uctt);
    }
    return ret;

#elif defined ZCE_OS_LINUX
    return ::swapcontext(get_uctt,set_uctt);
#endif 
}

