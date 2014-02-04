
#include "zce_predefine.h"
#include "zce_trace_log_debug.h"
#include "zce_os_adapt_error.h"
#include "zce_os_adapt_coroutine.h"

#if defined ZCE_OS_WINDOWS


//������ɺ�����������
VOID  WINAPI _fibers_adapt_nonparafun(VOID *fun_para)
{
    ZCE_COROUTINE_NONPARA fun_ptr = (ZCE_COROUTINE_NONPARA) fun_para;
    fun_ptr();
}
#endif


//���ݷ�װ��makecontext���Ǳ�׼����������ʹ��2�������ĺ���ָ��
int ZCE_OS::makecontext(ucontext_t * uctt,
    size_t stack_size,
    ZCE_COROUTINE_NONPARA nopara_fun)
{
#if defined ZCE_OS_WINDOWS

    
    //ע��FIBER_FLAG_FLOAT_SWITCH ��XP�ǲ���֧�ֵģ�
    //��ΪCreateFiber����Ҫ�ĵĺ���ֽ����WINAPI�ģ�����__stdcall�ģ�������һ��ת����
    *uctt = ::CreateFiberEx(stack_size,
        stack_size,
        FIBER_FLAG_FLOAT_SWITCH,
        _fibers_adapt_nonparafun,
        nopara_fun);

    if (NULL == uctt)
    {
        return -1;
    }
    return 0;
#elif defined ZCE_OS_LINUX

    //
    int ret = ::getcontext(uctt);
    if (0 != ret)
    {
        return ret;
    }

    //ʹ��0������
    const int ONLY_NON_ARG_COUNT = 0;

    uctt->uc_stack.ss_sp = new char[stack_size];
    uctt->uc_stack.ss_size = stack_size;

    ::makecontext(uctt,
        (void(*)(void)) nopara_fun,
        ONLY_NON_ARG_COUNT);
    return 0;
#endif
}



#if defined ZCE_OS_WINDOWS


//��ΪCreateFiber����Ҫ�ĵĺ���ֽ����WINAPI�ģ�����__stdcall�ģ�������һ��ת����
struct _FIBERS_TWOPARAFUN_ADAPT
{
    ///����ָ��
    ZCE_COROUTINE_TWOPARA   fun_ptr_;
    //�����ĵ�һ�����������
    void                   *left_para_;
    //�����ĵ�һ���������Ҳ�
    void                   *right_para_;
};

//������ɺ�����������
VOID  WINAPI _fibers_adapt_twoparafun (VOID *fun_para)
{
    
    _FIBERS_TWOPARAFUN_ADAPT *fun_adapt = (_FIBERS_TWOPARAFUN_ADAPT *)fun_para;
    ZCE_COROUTINE_TWOPARA twopara_fun = fun_adapt->fun_ptr_;
    void *left_para = fun_adapt->left_para_;
    void *right_para = fun_adapt->right_para_;
    
    //��������Ƕѷ���ģ�Ҫ������ͷ�
    delete fun_adapt;

    twopara_fun(left_para,right_para);


}

#endif



//���ݷ�װ��makecontext���Ǳ�׼����������ʹ��2�������ĺ���ָ��
int ZCE_OS::makecontext(ucontext_t * uctt,
    size_t stack_size, 
    ZCE_COROUTINE_TWOPARA fun_ptr,
    void *left_para, 
    void *right_para)
{
#if defined ZCE_OS_WINDOWS

    //ʹ������ṹ��ɺ�������
    struct _FIBERS_TWOPARAFUN_ADAPT *fibers_adapt = new _FIBERS_TWOPARAFUN_ADAPT();
    fibers_adapt->fun_ptr_ = fun_ptr;
    fibers_adapt->left_para_ = left_para;
    fibers_adapt->right_para_ = right_para;

    //ע��FIBER_FLAG_FLOAT_SWITCH ��XP�ǲ���֧�ֵģ�
    *uctt = ::CreateFiberEx(stack_size,
        stack_size,
        FIBER_FLAG_FLOAT_SWITCH,
        _fibers_adapt_twoparafun,
        fibers_adapt);

    if (NULL == uctt)
    {
        return -1;
    }
    return 0;
#elif defined ZCE_OS_LINUX
    
    //
    int ret = ::getcontext(uctt);
    if (0 != ret)
    {
        return ret;
    }

    //ֻʹ��һ��������������ʹ�ñ�Σ�Windwos��֧��
    const int ONLY_TWO_ARG_COUNT = 2;

    uctt->uc_stack.ss_sp = new char [stack_size];
    uctt->uc_stack.ss_size = stack_size;

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

    //�����ǰ�������˳̣�����ת����ͬʱҲ����ǰ���˳̱�ʶ
    if (FALSE == ::IsThreadAFiber() )
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
    //����Ѿ����˳��ˣ��õ���ǰ�˳̵ı�ʶ
    else
    {
        *uctt = ::GetCurrentFiber();
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

