
#include "zce_predefine.h"
#include "zce_trace_log_debug.h"
#include "zce_os_adapt_error.h"
#include "zce_os_adapt_coroutine.h"


#if defined ZCE_OS_WINDOWS

///Windows��Fiberʵ��(CreateFiber)����ָ���Ӧ�Ĳ���ֻ��һ����������Ҫ�ĵĺ���ֽ����WINAPI�ģ�
///����__stdcall�ģ�����Fiberû�з��ص�contextָ��,������һ��ת����
struct _FIBERS_TWOPARAFUN_ADAPT
{
    ///
    ucontext_t           *back_uctt_;
    ///����ָ��
    ZCE_COROUTINE_3PARA   fun_ptr_;
    //�����ĵ�1��������
    void                 *para1_;
    //�����ĵ�2������
    void                 *para2_;
    //�����ĵ�3������
    void                 *para3_;
};

//������ɺ�����������
VOID  WINAPI _fibers_adapt_fun (VOID *fun_para)
{
    
    _FIBERS_TWOPARAFUN_ADAPT *fun_adapt = (_FIBERS_TWOPARAFUN_ADAPT *)fun_para;
    ZCE_COROUTINE_3PARA twopara_fun = fun_adapt->fun_ptr_;
    void *para1 = fun_adapt->para1_;
    void *para2 = fun_adapt->para2_;
    void *para3 = fun_adapt->para3_;
    ucontext_t *back_ucct = fun_adapt->back_uctt_;
    //��������Ƕѷ���ģ�Ҫ������ͷ�
    delete fun_adapt;

    twopara_fun(para1,para2,para3);

    ::SwitchToFiber(*back_ucct);
}

#endif



//���ݷ�װ��makecontext���Ǳ�׼����������ʹ��2�������ĺ���ָ��
int ZCE_OS::makecontext(ucontext_t *uctt,
    size_t stack_size,
    ucontext_t *back_uc,
    ZCE_COROUTINE_3PARA fun_ptr,
    void *para1,
    void *para2,
    void *para3)
{
#if defined ZCE_OS_WINDOWS

    if (FALSE == ::IsThreadAFiber())
    {
        //FIBER_FLAG_FLOAT_SWITCH XP��֧�֣�
        ::ConvertThreadToFiberEx(NULL,
            FIBER_FLAG_FLOAT_SWITCH);
        if (NULL == uctt)
        {
            return -1;
        }
    }


    //ʹ������ṹ��ɺ�������
    struct _FIBERS_TWOPARAFUN_ADAPT *fibers_adapt = new _FIBERS_TWOPARAFUN_ADAPT();
    fibers_adapt->back_uctt_ = back_uc;
    fibers_adapt->fun_ptr_ = fun_ptr;
    fibers_adapt->para1_ = para1;
    fibers_adapt->para2_ = para2;
    fibers_adapt->para3_ = para3;
    
    //ע��FIBER_FLAG_FLOAT_SWITCH ��XP�ǲ���֧�ֵģ�
    *uctt = ::CreateFiberEx(stack_size,
        stack_size,
        FIBER_FLAG_FLOAT_SWITCH,
        _fibers_adapt_fun,
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
    uctt->uc_stack.uc_link = back_uc;
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
    printf("get %12p set %12p\n", get_uctt, set_uctt);
    if (ret == 0) 
    {
        ret = ZCE_OS::setcontext(set_uctt);
    }
    printf("get %12p set %12p\n", get_uctt, set_uctt);
    return ret;

#elif defined ZCE_OS_LINUX
    return ::swapcontext(get_uctt,set_uctt);
#endif 
}

