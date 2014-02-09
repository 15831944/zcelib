
#include "zce_predefine.h"
#include "zce_async_fw_coroutine.h"



//------------------------------------------------------------------------------------
ZCE_CRTNAsync_Coroutine::ZCE_CRTNAsync_Coroutine()
{
}

ZCE_CRTNAsync_Coroutine::~ZCE_CRTNAsync_Coroutine()
{
}


///ȡ��Э�̵ľ��
coroutine_t *ZCE_CRTNAsync_Coroutine::get_handle()
{
    return &handle_;
}


//�л���Main
void ZCE_CRTNAsync_Coroutine::switch_to_main()
{
    ZCE_OS::switch_to_main(&handle_);
}


///
void ZCE_CRTNAsync_Coroutine::set_state(ZCE_CRTNAsync_Coroutine::STATE_COROUTINE state)
{
    state_ = state;
}


//Э�̶�������к���
void ZCE_CRTNAsync_Coroutine::coroutine_do()
{
    //
    while (state_ != STATE_EXIT)
    {
        //
        coroutine_start();
        coroutine_run();
        coroutine_end();
        
        //
        switch_to_main();
    }
}

///static ����������Э�����к���������Э�̶�������к���
void ZCE_CRTNAsync_Coroutine::coroutine_do(ZCE_CRTNAsync_Coroutine *coroutine)
{
    coroutine->coroutine_do();
}


//------------------------------------------------------------------------------------

ZCE_CRTNAsync_Main::ZCE_CRTNAsync_Main()
{
}


ZCE_CRTNAsync_Main::~ZCE_CRTNAsync_Main()
{

}







