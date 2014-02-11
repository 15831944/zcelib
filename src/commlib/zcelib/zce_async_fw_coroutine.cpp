
#include "zce_predefine.h"
#include "zce_async_fw_coroutine.h"
#include "zce_os_adapt_error.h"
#include "zce_trace_log_debug.h"



//------------------------------------------------------------------------------------
ZCE_CRTNAsync_Coroutine::ZCE_CRTNAsync_Coroutine():
    command_(0),
    coroutine_id_(0),
    state_()
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


///�л���Э�̣�Ҳ�����л������Լ�����
void ZCE_CRTNAsync_Coroutine::switch_to_coroutine()
{
    ZCE_OS::switch_to_coroutine(&handle_);
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
void ZCE_CRTNAsync_Coroutine::static_do(ZCE_CRTNAsync_Coroutine *coroutine)
{
    coroutine->coroutine_do();
}


//------------------------------------------------------------------------------------

ZCE_CRTNAsync_Main::ZCE_CRTNAsync_Main():
    corout_id_builder_(1)
{
}

ZCE_CRTNAsync_Main::~ZCE_CRTNAsync_Main()
{
    
    //���ڴ�������������ȫ����������ø��ˣ���Ϊ���ͷ����ڴ棬��Inmoreliu�Ƕ��õ���һ�ٷ���
    ID_TO_REGCOR_POOL_MAP::iterator pooliter = coroutine_pool_.begin();
    ID_TO_REGCOR_POOL_MAP::iterator poolenditer = coroutine_pool_.end();

    for (; pooliter != poolenditer; ++pooliter)
    {
        unsigned int regframe_cmd = pooliter->first;
        REG_COROUTINE_POOL &pool_reg = (pooliter->second);
        //��¼��Ϣ����
        ZLOG_INFO("[ZCELIB] Register command:%u size of pool:%u capacity of pool:%u.",
            regframe_cmd,
            pool_reg.size(),
            pool_reg.capacity()
            );

        //���������⣬
        if (pool_reg.size() != pool_reg.capacity())
        {
            ZLOG_ERROR("[ZCELIB] Plase notice!! size[%u] != capacity[%u] may be exist memory leak.",
                pool_reg.size(),
                pool_reg.capacity());
        }

        //�Ƿ����
        size_t pool_reg_trans_len = pool_reg.size();

        for (size_t i = 0; i < pool_reg_trans_len; ++i)
        {
            ZCE_CRTNAsync_Coroutine *corout_base = NULL;
            pool_reg.pop_front(corout_base);

            delete corout_base;
            corout_base = NULL;
        }
    }
}


//��ʼ����
int ZCE_CRTNAsync_Main::initialize(size_t crtn_type_num,
    size_t running_number)
{
    //�Բ���������
    if (crtn_type_num < DEFUALT_CRTN_TYPE_NUM)
    {
        crtn_type_num = DEFUALT_CRTN_TYPE_NUM;
    }
    if (running_number < DEFUALT_RUNNIG_CRTN_SIZE)
    {
        running_number = DEFUALT_RUNNIG_CRTN_SIZE;
    }

    coroutine_pool_.rehash(crtn_type_num);
    running_coroutine_.rehash(running_number);
    return 0;
}

//ע��һ��Э�̣�����reg_cmd��Ӧ��
int ZCE_CRTNAsync_Main::register_coroutine(unsigned int reg_cmd,
    ZCE_CRTNAsync_Coroutine* coroutine_base,
    size_t init_clone_num,
    size_t stack_size)
{
    int ret = 0;
    //�Բ���������
    if (init_clone_num < DEFUALT_INIT_POOL_SIZE)
    {
        init_clone_num = DEFUALT_INIT_POOL_SIZE;
    }

    ID_TO_REGCOR_POOL_MAP::iterator iter_temp = coroutine_pool_.find(reg_cmd);
    if (iter_temp != coroutine_pool_.end())
    {
        return -1;
    }
    REG_COROUTINE_POOL tmp_pool;
    coroutine_pool_[reg_cmd] = tmp_pool;
    REG_COROUTINE_POOL &pool = coroutine_pool_[reg_cmd];

    pool.push_back(coroutine_base);
    for (size_t i = 0; i < init_clone_num;i++)
    {
        ZCE_CRTNAsync_Coroutine *crtn = coroutine_base->clone();
        crtn->set_command(reg_cmd);
        ret = ZCE_OS::make_coroutine(crtn->get_handle(),
            stack_size,
            true,
            (ZCE_COROUTINE_3PARA)ZCE_CRTNAsync_Coroutine::static_do,
            (void *)crtn,
            NULL,
            NULL
            );
        if (ret != 0)
        {
            ZCE_TRACE_FAIL_RETURN(RS_ERROR, "ZCE_OS::make_coroutine return fail.", ret);
            return ret;
        }
        pool.push_back(crtn);
    }

    return 0;
}

/*
///����һ��Э��
int ZCE_CRTNAsync_Main::active_coroutine(unsigned int cmd, unsigned int *id)
{


}

*/



