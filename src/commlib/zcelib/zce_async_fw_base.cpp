
#include "zce_predefine.h"
#include "zce_async_fw_base.h"
#include "zce_os_adapt_error.h"
#include "zce_trace_log_debug.h"
#include "zce_timer_queue_base.h"

//------------------------------------------------------------------------------------

//TIME ID
const int ZCE_Async_Object::ASYNCOBJ_TIME_ID[] = { 10001, 20001 };

//���캯��
ZCE_Async_Object::ZCE_Async_Object(ZCE_Async_ObjectMgr *async_mgr) :
    asyncobj_id_(ZCE_Async_ObjectMgr::INVALID_IDENTITY),
    async_mgr_(async_mgr),
    create_cmd_(ZCE_Async_ObjectMgr::INVALID_COMMAND),
    timeout_id_(ZCE_Timer_Queue::INVALID_TIMER_ID),
    touchtimer_id_(ZCE_Timer_Queue::INVALID_TIMER_ID)
{
}

//��������
ZCE_Async_Object::~ZCE_Async_Object()
{
}

//��ʼ���������ڹ��캯������ã�
int ZCE_Async_Object::initialize(unsigned int reg_cmd)
{
    create_cmd_ = reg_cmd;
    return 0;
}

//�������ٺ�����������ǰ�ĵ���
int ZCE_Async_Object::finish()
{
    return 0;
}


//���ó�ʱ��ʱ��
int ZCE_Async_Object::set_timeout_timer(int sec, int usec)
{
    ZCE_Timer_Queue* timer_queue = async_mgr_->get_timer_queue();
    ZCE_Time_Value delay_time(sec, usec);
    //ע��ʹ�õ�TIME ID
    timeout_id_ = timer_queue->schedule_timer(this,
        &ASYNCOBJ_TIME_ID[0],
        delay_time);

    if (ZCE_Timer_Queue::INVALID_TIMER_ID == timeout_id_)
    {
        return -1;
    }
    return 0;
}

//���ô�����ʱ��
int ZCE_Async_Object::set_timetouch_timer(int sec, int usec)
{
    ZCE_Timer_Queue* timer_queue = async_mgr_->get_timer_queue();
    ZCE_Time_Value delay_time(sec, usec);
    //ע��ʹ�õ�TIME ID
    touchtimer_id_ = timer_queue->schedule_timer(this,
        &ASYNCOBJ_TIME_ID[1],
        delay_time);

    if (ZCE_Timer_Queue::INVALID_TIMER_ID == touchtimer_id_)
    {
        return -1;
    }
    return 0;
}


void ZCE_Async_Object::cancel_timeout_timer()
{

}


void ZCE_Async_Object::cancel_touch_timer()
{

}


void ZCE_Async_Object::on_end()
{

}


//=============================================================================================
//�첽���������

ZCE_Async_ObjectMgr::ZCE_Async_ObjectMgr(ZCE_Timer_Queue *timer_queue) :
id_builder_(1),
timer_queue_(timer_queue)
{
}

ZCE_Async_ObjectMgr::~ZCE_Async_ObjectMgr()
{

    //���ڴ�������������ȫ����������ø��ˣ���Ϊ���ͷ����ڴ棬��Inmoreliu�Ƕ��õ���һ�ٷ���
    ID_TO_REGASYNC_POOL_MAP::iterator pooliter = aysncobj_pool_.begin();
    ID_TO_REGASYNC_POOL_MAP::iterator poolenditer = aysncobj_pool_.end();

    for (; pooliter != poolenditer; ++pooliter)
    {
        unsigned int regframe_cmd = pooliter->first;
        ASYNC_OBJECT_RECORD &pool_reg = (pooliter->second);
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
            ZCE_Async_Object *corout_base = NULL;
            pool_reg.coroutine_pool_.pop_front(corout_base);
            corout_base->finish();
            delete corout_base;
            corout_base = NULL;
        }
    }
}


//��ʼ����
int ZCE_Async_ObjectMgr::initialize(size_t crtn_type_num,
    size_t running_number)
{
    //�Բ���������
    if (crtn_type_num < DEFUALT_ASYNC_TYPE_NUM)
    {
        crtn_type_num = DEFUALT_ASYNC_TYPE_NUM;
    }
    if (running_number < DEFUALT_RUNNIG_ASYNC_SIZE)
    {
        running_number = DEFUALT_RUNNIG_ASYNC_SIZE;
    }

    aysncobj_pool_.rehash(crtn_type_num);
    running_aysncobj_.rehash(running_number);
    return 0;
}

//ע��һ��Э�̣�����reg_cmd��Ӧ��
int ZCE_Async_ObjectMgr::register_asyncobj(unsigned int reg_cmd,
    ZCE_Async_Object* coroutine_base,
    size_t init_clone_num)
{
    //�Բ���������
    if (init_clone_num < DEFUALT_INIT_POOL_SIZE)
    {
        init_clone_num = DEFUALT_INIT_POOL_SIZE;
    }

    //
    if (ZCE_Async_ObjectMgr::INVALID_COMMAND == reg_cmd)
    {
        return -1;
    }

    //һ��������CMDֻ��ע��һ��
    ID_TO_REGASYNC_POOL_MAP::iterator iter_temp = aysncobj_pool_.find(reg_cmd);
    if (iter_temp != aysncobj_pool_.end())
    {
        return -1;
    }
    ASYNC_OBJECT_RECORD record;
    aysncobj_pool_[reg_cmd] = record;
    ASYNC_OBJECT_RECORD &ref_rec = aysncobj_pool_[reg_cmd];

    ref_rec.coroutine_pool_.push_back(coroutine_base);
    for (size_t i = 0; i < init_clone_num; i++)
    {
        ZCE_Async_Object *crtn = coroutine_base->clone(this);
        crtn->initialize(reg_cmd);
        ref_rec.coroutine_pool_.push_back(crtn);
    }

    return 0;
}

///�ӳ����������һ��
int ZCE_Async_ObjectMgr::allocate_from_pool(unsigned int cmd, ZCE_Async_Object *&crt_async)
{

    ID_TO_REGASYNC_POOL_MAP::iterator mapiter = aysncobj_pool_.find(cmd);
    if (mapiter == aysncobj_pool_.end())
    {
        return -1;
    }

    ASYNC_OBJECT_RECORD &reg_async = aysncobj_pool_[cmd];

    //�������һ��
    if (reg_async.coroutine_pool_.size() == 1)
    {
        ZLOG_INFO("[ZCELIB] Before extend pool.");
        //ȡһ��ģ��
        ZCE_Async_Object *model_trans = NULL;
        reg_async.coroutine_pool_.pop_front(model_trans);

        size_t capacity_of_pool = reg_async.coroutine_pool_.capacity();
        reg_async.coroutine_pool_.resize(capacity_of_pool + POOL_EXTEND_ASYNC_NUM);

        ZLOG_INFO("[ZCELIB] Coroutine pool Size=%u,  command %u, capacity = %u , resize =%u .",
            reg_async.coroutine_pool_.size(),
            cmd,
            capacity_of_pool,
            capacity_of_pool + POOL_EXTEND_ASYNC_NUM);

        //��ģ�Ϳ�¡N-1��Trans
        for (size_t i = 0; i < POOL_EXTEND_ASYNC_NUM; ++i)
        {
            ZCE_Async_Object *cloned_base = model_trans->clone(this);
            reg_async.coroutine_pool_.push_back(cloned_base);
        }

        //��ģ�ͷŵ���N��
        reg_async.coroutine_pool_.push_back(model_trans);
        ZLOG_INFO("[ZCELIB] After Extend trans.");
    }

    //ȡ��һ������
    reg_async.coroutine_pool_.pop_front(crt_async);


    return 0;
}

///�黹���������棬�ͷ�һ���첽���󵽳�������
int ZCE_Async_ObjectMgr::free_to_pool(ZCE_Async_Object *free_crtn)
{

    ID_TO_REGASYNC_POOL_MAP::iterator mapiter = aysncobj_pool_.find(free_crtn->create_cmd_);

    if (mapiter == aysncobj_pool_.end())
    {
        return -1;
    }


    //
    ASYNC_OBJECT_RECORD &reg_record = mapiter->second;
    ZLOG_DEBUG("[framework] Return clone frame command %u,Pool size=%u .",
        free_crtn->create_cmd_,
        reg_record.coroutine_pool_.size());

    //
    reg_record.coroutine_pool_.push_back(free_crtn);
    return 0;
}


int ZCE_Async_ObjectMgr::create_asyncobj(unsigned int cmd, unsigned int *id)
{
    int ret = 0;
    ZCE_Async_Object *crt_async = NULL;
    
    //�ӳ���������һ���첽����
    ret = allocate_from_pool(cmd, crt_async);
    if (ret != 0)
    {
        return ret;
    }
    ++id_builder_;
    if (id_builder_ == INVALID_IDENTITY)
    {
        ++id_builder_;
    }
    *id = id_builder_;
    crt_async->asyncobj_id_ = id_builder_;
    
    //����Ѿ��
    crt_async->on_start();

    bool continue_run = false;
    crt_async->on_run(continue_run);
    
    //�������һ�¾��˳���,ֱ�ӽ�������
    if (continue_run == false)
    {
        crt_async->on_end();
        free_to_pool(crt_async);
    }
    else
    {
        //��һ����auto���о���ˬ������
        auto iter = running_aysncobj_.insert(std::make_pair(id_builder_, crt_async));
        //���û�в���ɹ�,������û�п��ܳ�����������������ܳ�����ô���첽����
        if (iter.second == false)
        {
            ZCE_ASSERT_ALL(false);
            return -1;
        }
    }

    return 0;
}


//ͨ��ID��Ѱ��һ���������е��첽����
int ZCE_Async_ObjectMgr::find_running_asyncobj(unsigned int id, ZCE_Async_Object *&running_aysnc)
{
    running_aysnc = NULL;
    auto iter =  running_aysncobj_.find(id);
    if (running_aysncobj_.end()== iter)
    {
        return -1;
    }
    running_aysnc = iter->second;
    return 0;
}

//
inline ZCE_Timer_Queue * ZCE_Async_ObjectMgr::get_timer_queue()
{
    return timer_queue_;
}



