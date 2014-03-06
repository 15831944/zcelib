
#include "zce_predefine.h"
#include "zce_async_fw_coroutine.h"
#include "zce_os_adapt_error.h"
#include "zce_trace_log_debug.h"



//========================================================================================

ZCE_Async_Coroutine::ZCE_Async_Coroutine(ZCE_Async_CoroutineMgr *async_mgr) :
    ZCE_Async_Object(async_mgr),
    stack_size_(MIN_STACK_SIZE)
{
    //��ջ��СĬ��ѡ����С�ģ�
}

ZCE_Async_Coroutine::~ZCE_Async_Coroutine()
{
}


//��ʼ��Э�̵Ķ���
int ZCE_Async_Coroutine::initialize(unsigned int reg_cmd)
{
    ZCE_Async_Object::initialize(reg_cmd);
    int ret = 0;
    ret = ZCE_OS::make_coroutine(&handle_,
        stack_size_,
        true,
        (ZCE_COROUTINE_3PARA)ZCE_Async_Coroutine::static_do,
        (void *)this,
        NULL,
        NULL
        );
    if (ret != 0)
    {
        ZCE_TRACE_FAIL_RETURN(RS_ERROR, "ZCE_OS::make_coroutine return fail.", ret);
        return ret;
    }
    return 0;
}

//����Э�̶���
int ZCE_Async_Coroutine::finish()
{
    ZCE_Async_Object::finish();
    ZCE_OS::delete_coroutine(&handle_);
    return 0;
}

//����Э��
void ZCE_Async_Coroutine::on_run(bool &continue_run)
{
    continue_run = false;
    yeild_coroutine();
    
    //���ݵ��÷��صĺ�����¼��״ֵ̬�õ���ǰ��״̬
    if (coroutine_state_ == COROUTINE_CONTINUE)
    {
        continue_run = true;
    }
    else if (coroutine_state_ == COROUTINE_EXIT)
    {
        continue_run = false;
    }
    else
    {
        ZCE_ASSERT_ALL(false);
    }
}

//�л���Main��Э�̻����������
void ZCE_Async_Coroutine::yeild_main_continue()
{
    coroutine_state_ = COROUTINE_CONTINUE;
    ZCE_OS::yeild_main(&handle_);
}

//�л���Main,Э���˳�
void ZCE_Async_Coroutine::yeild_main_exit()
{
    coroutine_state_ = COROUTINE_EXIT;
    ZCE_OS::yeild_main(&handle_);
}


//�л���Э�̣�Ҳ�����л������Լ�����
void ZCE_Async_Coroutine::yeild_coroutine()
{
    ZCE_OS::yeild_coroutine(&handle_);
}



//Э�̶�������к���
void ZCE_Async_Coroutine::coroutine_do()
{
    //�����ҪЭ��
    for (;;)
    {
        //
        coroutine_run();
        
        //������Ϻ󣬷���������
        yeild_main_exit();
    }
}

///static ����������Э�����к���������Э�̶�������к���
void ZCE_Async_Coroutine::static_do(ZCE_Async_Coroutine *coroutine)
{
    coroutine->coroutine_do();
}


//------------------------------------------------------------------------------------

ZCE_Async_CoroutineMgr::ZCE_Async_CoroutineMgr(ZCE_Timer_Queue *timer_queue) :
ZCE_Async_ObjectMgr(timer_queue)
{
}

ZCE_Async_CoroutineMgr::~ZCE_Async_CoroutineMgr()
{
}



    int ret = 0;









