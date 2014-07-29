
#include "zce_predefine.h"
#include "zce_async_fw_base.h"
#include "zce_os_adapt_error.h"
#include "zce_trace_log_debug.h"
#include "zce_timer_queue_base.h"

//=============================================================================================

//TIME ID
const int ZCE_Async_Object::ASYNCOBJ_ACTION_ID[] = { 10001, 20001 };

//���캯��
ZCE_Async_Object::ZCE_Async_Object(ZCE_Async_ObjectMgr *async_mgr) :
    asyncobj_id_(ZCE_Async_ObjectMgr::INVALID_IDENTITY),
    async_mgr_(async_mgr),
    create_cmd_(ZCE_Async_ObjectMgr::INVALID_COMMAND),
    timeout_id_(ZCE_Timer_Queue::INVALID_TIMER_ID)
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
int ZCE_Async_Object::set_timeout(const ZCE_Time_Value &time_out)
{
    ZCE_Timer_Queue *timer_queue = async_mgr_->timer_queue();
    ZCE_Time_Value delay_time(time_out);
    //ע��ʹ�õ�TIME ID
    timeout_id_ = timer_queue->schedule_timer(async_mgr_,
                                              this,
                                              delay_time);

    if (ZCE_Timer_Queue::INVALID_TIMER_ID == timeout_id_)
    {
        return -1;
    }
    return 0;
}


//ȡ����ʱ�Ķ�ʱ��
void ZCE_Async_Object::cancel_timeout()
{
    if (ZCE_Timer_Queue::INVALID_TIMER_ID != timeout_id_)
    {
        ZCE_Timer_Queue *timer_queue = async_mgr_->timer_queue();
        timer_queue->cancel_timer(timeout_id_);
        timeout_id_ = ZCE_Timer_Queue::INVALID_TIMER_ID;
    }
}



//�첽����ʼ
void ZCE_Async_Object::on_start()
{
}

//Ŀǰ�������Ľ���������������ʱ��
void ZCE_Async_Object::on_end()
{
    cancel_timeout();
}


//=============================================================================================
//�첽���������

//�ڲ��ṹ
ZCE_Async_ObjectMgr::ASYNC_OBJECT_RECORD::ASYNC_OBJECT_RECORD():
    start_num_(0),
    active_num_(0),
    end_num_(0),
    force_end_num_(0),
    timeout_num_(0),
    run_consume_ms_(0)
{
}

ZCE_Async_ObjectMgr::ASYNC_OBJECT_RECORD::~ASYNC_OBJECT_RECORD()
{
}


ZCE_Async_ObjectMgr::ZCE_Async_ObjectMgr(ZCE_Timer_Queue *timer_queue) :
    ZCE_Timer_Handler(timer_queue),
    id_builder_(1),
    pool_init_size_(0),
    pool_extend_size_(0)
{
}

ZCE_Async_ObjectMgr::~ZCE_Async_ObjectMgr()
{
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


//
void ZCE_Async_ObjectMgr::finish()
{
    RUNNING_ASYNOBJ_MAP::iterator run_iter = running_aysncobj_.begin();
    RUNNING_ASYNOBJ_MAP::iterator run_end = running_aysncobj_.end();
    for (; run_iter != run_end; ++run_iter)
    {
        ZCE_Async_Object *async_obj = run_iter->second;

        //ͳ��ǿ���˳�������
        ID_TO_REGASYNC_POOL_MAP::iterator iter_temp = aysncobj_pool_.find(async_obj->create_cmd_);
        if (iter_temp == aysncobj_pool_.end())
        {
            ZCE_ASSERT(false);
            continue;
        }
        ASYNC_OBJECT_RECORD &reg_async = iter_temp->second;
        ++reg_async.force_end_num_;

        async_obj->on_end();
        free_to_pool(async_obj);
    }

    //���ڴ�������������ȫ����������ø��ˣ���Ϊ���ͷ����ڴ棬��Inmoreliu�Ƕ��õ���һ�ٷ���
    ID_TO_REGASYNC_POOL_MAP::iterator pooliter = aysncobj_pool_.begin();
    ID_TO_REGASYNC_POOL_MAP::iterator poolenditer = aysncobj_pool_.end();

    for (; pooliter != poolenditer; ++pooliter)
    {
        unsigned int regframe_cmd = pooliter->first;
        ASYNC_OBJECT_RECORD &pool_reg = (pooliter->second);
        //��¼��Ϣ����
        ZCE_LOGMSG(RS_INFO, "[ZCELIB] Register command:%u size of pool:%u capacity of pool:%u.",
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
    return;
}

//ע��һ��Э�̣�����reg_cmd��Ӧ��
int ZCE_Async_ObjectMgr::register_asyncobj(unsigned int reg_cmd,
                                           ZCE_Async_Object *coroutine_base)
{

    //������ֵ�������������ù���
    ZCE_ASSERT(pool_init_size_ > 0 && pool_extend_size_ > 0);

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

    coroutine_base->create_cmd_ = reg_cmd;
    ref_rec.coroutine_pool_.initialize(pool_init_size_+1);
    ref_rec.coroutine_pool_.push_back(coroutine_base);
    for (size_t i = 0; i < pool_init_size_; i++)
    {
        ZCE_Async_Object *crtn = coroutine_base->clone(this);
        crtn->create_cmd_ = reg_cmd;
        crtn->initialize(reg_cmd);
        ref_rec.coroutine_pool_.push_back(crtn);
    }
    return 0;
}


//�ӳ����������һ��
int ZCE_Async_ObjectMgr::allocate_from_pool(unsigned int cmd,
                                            ASYNC_OBJECT_RECORD *&async_rec,
                                            ZCE_Async_Object *&crt_async)
{

    ID_TO_REGASYNC_POOL_MAP::iterator mapiter = aysncobj_pool_.find(cmd);
    if (mapiter == aysncobj_pool_.end())
    {
        return -1;
    }

    ASYNC_OBJECT_RECORD &reg_async = mapiter->second;

    //�������һ��
    if (reg_async.coroutine_pool_.size() == 1)
    {
        ZCE_LOGMSG(RS_INFO, "[ZCELIB] Before extend pool.");
        //ȡһ��ģ��
        ZCE_Async_Object *model_trans = NULL;
        reg_async.coroutine_pool_.pop_front(model_trans);

        size_t capacity_of_pool = reg_async.coroutine_pool_.capacity();
        reg_async.coroutine_pool_.resize(capacity_of_pool + pool_extend_size_);

        ZCE_LOGMSG(RS_INFO, "[ZCELIB] Coroutine pool Size=%u,  command %u, capacity = %u , resize =%u .",
                   reg_async.coroutine_pool_.size(),
                   cmd,
                   capacity_of_pool,
                   capacity_of_pool + pool_extend_size_);

        //��ģ�Ϳ�¡N-1��Trans
        for (size_t i = 0; i < pool_extend_size_; ++i)
        {
            ZCE_Async_Object *cloned_base = model_trans->clone(this);
            reg_async.coroutine_pool_.push_back(cloned_base);
        }

        //��ģ�ͷŵ���N��
        reg_async.coroutine_pool_.push_back(model_trans);
        ZCE_LOGMSG(RS_INFO, "[ZCELIB] After Extend trans.");
    }

    //ȡ��һ������
    reg_async.coroutine_pool_.pop_front(crt_async);
    async_rec = &reg_async;

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
    ZCE_LOGMSG(RS_DEBUG, "[framework] Return clone frame command %u,Pool size=%u .",
               free_crtn->create_cmd_,
               reg_record.coroutine_pool_.size());

    //
    reg_record.coroutine_pool_.push_back(free_crtn);
    return 0;
}

//�����첽����
int ZCE_Async_ObjectMgr::create_asyncobj(unsigned int cmd, unsigned int *id)
{
    int ret = 0;
    ZCE_Async_Object *crt_async = NULL;
    ASYNC_OBJECT_RECORD *async_rec = NULL;

    //�ӳ���������һ���첽����
    ret = allocate_from_pool(cmd, async_rec, crt_async);
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
    ++async_rec->start_num_;

    bool continue_run = false;
    crt_async->on_run(continue_run);

    //�������һ�¾��˳���,ֱ�ӽ�������
    if (continue_run == false)
    {
        ++async_rec->end_num_;
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

//����ĳ���Ѿ����е��첽����
int ZCE_Async_ObjectMgr::active_asyncobj(unsigned int id)
{
    int ret = 0;
    ZCE_Async_Object *async_obj = NULL;
    ret = find_running_asyncobj(id, async_obj);
    if (ret != 0)
    {
        return ret;
    }

    //���Ӽ�¼ͳ������
    ID_TO_REGASYNC_POOL_MAP::iterator mapiter = aysncobj_pool_.find(async_obj->create_cmd_);
    if (mapiter == aysncobj_pool_.end())
    {
        return -1;
    }
    ASYNC_OBJECT_RECORD &async_rec = mapiter->second;

    //����ͬʱȡ����ʱ��
    async_obj->cancel_timeout();

    bool continue_run = false;
    async_obj->on_run(continue_run);
    ++async_rec.active_num_;

    //��������������ˣ�
    if (continue_run == false)
    {
        ++async_rec.end_num_;
        async_obj->on_end();
        free_to_pool(async_obj);
    }


    return 0;
}

//��ʱ����
int ZCE_Async_ObjectMgr::handle_timeout(const ZCE_Time_Value &now_time,
                                        const void *act)
{
    ZCE_Async_Object *async_obj = (ZCE_Async_Object *)(act);
    //���Ӽ�¼ͳ������
    ID_TO_REGASYNC_POOL_MAP::iterator mapiter = aysncobj_pool_.find(async_obj->create_cmd_);
    if (mapiter == aysncobj_pool_.end())
    {
        return -1;
    }
    ASYNC_OBJECT_RECORD &async_rec = mapiter->second;

    //����ǰȡ����ʱ��
    async_obj->cancel_timeout();

    bool continue_run = false;
    async_obj->on_timeout(now_time, continue_run);
    ++async_rec.timeout_num_;

    //��������������ˣ�
    if (continue_run == false)
    {
        ++async_rec.end_num_;
        async_obj->on_end();
        free_to_pool(async_obj);
    }

    return 0;
}

//ͨ��ID��Ѱ��һ���������е��첽����
int ZCE_Async_ObjectMgr::find_running_asyncobj(unsigned int id,
                                               ZCE_Async_Object *&running_aysnc)
{
    running_aysnc = NULL;
    auto iter = running_aysncobj_.find(id);
    if (running_aysncobj_.end() == iter)
    {
        return -1;
    }
    running_aysnc = iter->second;
    return 0;
}


