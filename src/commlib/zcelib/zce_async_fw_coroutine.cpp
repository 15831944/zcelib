
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
void ZCE_CRTNAsync_Coroutine::yeild_main()
{
    ZCE_OS::yeild_main(&handle_);
}


///�л���Э�̣�Ҳ�����л������Լ�����
void ZCE_CRTNAsync_Coroutine::yeild_coroutine()
{
    ZCE_OS::yeild_coroutine(&handle_);
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
        yeild_main();
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
    ID_TO_REGCOR_POOL_MAP::iterator pooliter = reg_coroutine_.begin();
    ID_TO_REGCOR_POOL_MAP::iterator poolenditer = reg_coroutine_.end();

    for (; pooliter != poolenditer; ++pooliter)
    {
        unsigned int regframe_cmd = pooliter->first;
        COROUTINE_RECORD &pool_reg = (pooliter->second);
        //��¼��Ϣ����
        ZLOG_INFO("[ZCELIB] Register command:%u size of pool:%u capacity of pool:%u.",
            regframe_cmd,
            pool_reg.coroutine_pool_.size(),
            pool_reg.coroutine_pool_.capacity()
            );

        //���������⣬
        if (pool_reg.coroutine_pool_.size() != pool_reg.coroutine_pool_.capacity())
        {
            ZLOG_ERROR("[ZCELIB] Plase notice!! size[%u] != capacity[%u] may be exist memory leak.",
                pool_reg.coroutine_pool_.size(),
                pool_reg.coroutine_pool_.capacity());
        }

        //�Ƿ����
        size_t pool_reg_trans_len = pool_reg.coroutine_pool_.size();

        for (size_t i = 0; i < pool_reg_trans_len; ++i)
        {
            ZCE_CRTNAsync_Coroutine *corout_base = NULL;
            pool_reg.coroutine_pool_.pop_front(corout_base);

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

    reg_coroutine_.rehash(crtn_type_num);
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

    ID_TO_REGCOR_POOL_MAP::iterator iter_temp = reg_coroutine_.find(reg_cmd);
    if (iter_temp != reg_coroutine_.end())
    {
        return -1;
    }
    COROUTINE_RECORD record;
    reg_coroutine_[reg_cmd] = record;
    COROUTINE_RECORD &ref_rec = reg_coroutine_[reg_cmd];

    ref_rec.coroutine_pool_.push_back(coroutine_base);
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
        ref_rec.coroutine_pool_.push_back(crtn);
    }

    return 0;
}





///�ӳ����������һ��
int ZCE_CRTNAsync_Main::allocate_from_pool(unsigned int cmd, ZCE_CRTNAsync_Coroutine *&crt_crtn)
{

    ID_TO_REGCOR_POOL_MAP::iterator mapiter = reg_coroutine_.find(cmd);

    if (mapiter == reg_coroutine_.end())
    {
        return -1;
    }

    COROUTINE_RECORD &reg_crtn = reg_coroutine_[cmd];

    //�������һ��
    if (reg_crtn.coroutine_pool_.size() == 1)
    {
        ZLOG_INFO("[ZCELIB] Before extend pool.");
        //ȡһ��ģ��
        ZCE_CRTNAsync_Coroutine *model_trans = NULL;
        reg_crtn.coroutine_pool_.pop_front(model_trans);

        size_t capacity_of_pool = reg_crtn.coroutine_pool_.capacity();
        reg_crtn.coroutine_pool_.resize(capacity_of_pool + POOL_EXTEND_COROUTINE_NUM);

        ZLOG_INFO("[ZCELIB] Coroutine pool Size=%u,  command %u, capacity = %u , resize =%u .",
            reg_crtn.coroutine_pool_.size(),
            cmd,
            capacity_of_pool,
            capacity_of_pool + POOL_EXTEND_COROUTINE_NUM);

        //��ģ�Ϳ�¡N-1��Trans
        for (size_t i = 0; i < POOL_EXTEND_COROUTINE_NUM; ++i)
        {
            ZCE_CRTNAsync_Coroutine *cloned_base = model_trans->clone();
            reg_crtn.coroutine_pool_.push_back(cloned_base);
        }

        //��ģ�ͷŵ���N��
        reg_crtn.coroutine_pool_.push_back(model_trans);
        ZLOG_INFO("[ZCELIB] After Extend trans.");
    }

    //ȡ��һ������
    reg_crtn.coroutine_pool_.pop_front(crt_crtn);
    //��ʼ��Ѿ��
    crt_crtn->coroutine_start();

    return 0;
}

///
int ZCE_CRTNAsync_Main::free_to_pool(ZCE_CRTNAsync_Coroutine *free_crtn)
{

    ID_TO_REGCOR_POOL_MAP::iterator mapiter = reg_coroutine_.find(free_crtn->command_);

    if (mapiter == reg_coroutine_.end())
    {
        return -1;
    }


    //
    COROUTINE_RECORD &reg_record = mapiter->second;
    ZLOG_DEBUG("[framework] Return clone frame command %u,Pool size=%u .",
        free_crtn->command_,
        reg_record.coroutine_pool_.size());


    //������Դ�Ļ���
    free_crtn->coroutine_end();

    //
    reg_record.coroutine_pool_.push_back(free_crtn);
    return 0;
}




///����һ��Э��
int ZCE_CRTNAsync_Main::active_coroutine(unsigned int cmd, unsigned int *id)
{

    return 0;
}

///�л���ID��Ӧ���Ǹ��߳�
int ZCE_CRTNAsync_Main::yeild_coroutine(unsigned int id)
{

    return 0;
}











