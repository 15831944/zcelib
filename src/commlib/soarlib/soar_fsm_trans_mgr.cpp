#include "soar_predefine.h"
#include "soar_zerg_frame.h"
#include "soar_fsm_trans_base.h"
#include "soar_zerg_svc_info.h"
#include "soar_zerg_frame_malloc.h"
#include "soar_fsm_trans_mgr.h"

/******************************************************************************************
struct CREATE_TRANS_RECORD �����ͳ����Ϣ
******************************************************************************************/
CREATE_TRANS_RECORD::CREATE_TRANS_RECORD(unsigned int trans_cmd ):
    trans_command_(trans_cmd),
    if_auto_trans_lock_(false),
    trans_lock_cmd_(0),
    create_trans_num_(0),
    destroy_right_num_(0),
    destroy_timeout_num_(0),
    destroy_exception_num_(0),
    trans_consume_time_(0)
{
}
CREATE_TRANS_RECORD::~CREATE_TRANS_RECORD()
{
};

/******************************************************************************************
struct TRANS_LOCK_RECORD �����ļ�¼��Ԫ
******************************************************************************************/
TRANS_LOCK_RECORD::TRANS_LOCK_RECORD(unsigned int lock_qq_uin, unsigned int lock_trans_cmd):
    lock_qq_uin_(lock_qq_uin),
    lock_trans_cmd_(lock_trans_cmd)
{
}

TRANS_LOCK_RECORD::TRANS_LOCK_RECORD():
    lock_qq_uin_(0),
    lock_trans_cmd_(0)
{
}

TRANS_LOCK_RECORD::~TRANS_LOCK_RECORD()
{
}

/******************************************************************************************
class Transaction_Manager
******************************************************************************************/
Transaction_Manager *Transaction_Manager::instance_ = NULL;

Transaction_Manager::Transaction_Manager()
    : max_trans_(0)
    , trans_id_builder_(0)
    , self_svc_id_(0, 0)
    , timer_queue_(NULL)
    , zerg_mmap_pipe_(NULL)
    , statistics_clock_(NULL)
    , trans_send_buffer_ (NULL)
    , trans_recv_buffer_(NULL)
    , fake_recv_buffer_(NULL)
    , inner_frame_malloc_(NULL)
    , inner_message_queue_(NULL)
    , gen_trans_counter_(0)
    , cycle_gentrans_counter_(0)
{
}

//�������������������
Transaction_Manager::~Transaction_Manager()
{
    ZLOG_INFO("[framework] Transaction_Manager::~Transaction_Manager start.");

    //
    size_t trans_size  = transc_map_.size();
    ZLOG_INFO("[framework] close all transaction, transc_map_ size =%u", trans_size);

    //TMD��ɾ��������������������淭�˶��ٴδ��ˣ�BS��ļ�������
    for (size_t i = 0; i < trans_size; ++i)
    {
        //ÿ�ζ����ʼ��ɾ������һ�������Ǳ�����ġ�
        HASHMAP_OF_TRANSACTION::iterator mapiter = transc_map_.begin();
        Transaction_Base *pbase = mapiter->second;

        pbase->output_trans_info("[Application exit]:");
        //����handle_close�൱�ڽ��Լ���transc_map_ɾ��������ÿ�ζ��ɵ���һ��,
        pbase->handle_close();
    }

    //���ڴ�������������ȫ����������ø��ˣ���Ϊ���ͷ����ڴ棬��Inmoreliu�Ƕ��õ���һ�ٷ���
    HASHMAP_OF_POLLREGTRANS::iterator pooliter = regtrans_pool_map_.begin();
    HASHMAP_OF_POLLREGTRANS::iterator poolenditer = regtrans_pool_map_.end();

    for (; pooliter != poolenditer; ++pooliter)
    {
        unsigned int regframe_cmd = pooliter->first;
        POOL_OF_REGISTERTRANS &pool_reg_trans = (pooliter->second).crttrs_cmd_pool_;
        //��¼��Ϣ����
        ZLOG_INFO("[framework] Register command:%u size of pool:%u capacity of pool:%u.",
                  regframe_cmd,
                  pool_reg_trans.size(),
                  pool_reg_trans.capacity()
                 );

        //���������⣬
        if (pool_reg_trans.size() != pool_reg_trans.capacity())
        {
            ZLOG_ERROR("[framework] Plase notice!! size[%u] != capacity[%u] may be exist memory leak.",
                       pool_reg_trans.size(),
                       pool_reg_trans.capacity());
        }

        //�Ƿ����
        size_t pool_reg_trans_len = pool_reg_trans.size();

        for (size_t i = 0; i < pool_reg_trans_len; ++i)
        {
            Transaction_Base *ptsbase = NULL;
            pool_reg_trans.pop_front(ptsbase);

            delete ptsbase;
            ptsbase = NULL;
        }
    }

    //�����ڴ������
    if (inner_frame_malloc_)
    {
        delete inner_frame_malloc_ ;
        inner_frame_malloc_ = NULL;
    }

    //������Ϣ����
    if (inner_message_queue_)
    {
        delete inner_message_queue_;
        inner_message_queue_ = NULL;
    }
}

void Transaction_Manager::initialize(size_t  szregtrans,
                                     size_t sztransmap,
                                     const SERVICES_ID &selfsvr,
                                     ZCE_Timer_Queue *timer_queue,
                                     Zerg_MMAP_BusPipe *zerg_mmap_pipe,
                                     unsigned int max_frame_len,
                                     bool init_inner_queue,
                                     bool init_lock_pool)
{

    ZCE_ASSERT(timer_queue != NULL);
    ZCE_ASSERT(zerg_mmap_pipe != NULL);

    regtrans_pool_map_.rehash(szregtrans + 128);
    transc_map_.rehash(sztransmap + 1024);
    max_trans_ = sztransmap;
    self_svc_id_ = selfsvr;
    timer_queue_ = timer_queue;
    zerg_mmap_pipe_ = zerg_mmap_pipe;

    trans_send_buffer_ = new (max_frame_len + 32) Zerg_App_Frame(CMD_INVALID_CMD, max_frame_len);
    trans_recv_buffer_ = new (max_frame_len + 32) Zerg_App_Frame(CMD_INVALID_CMD, max_frame_len);
    fake_recv_buffer_ = new (max_frame_len + 32) Zerg_App_Frame(CMD_INVALID_CMD, max_frame_len);

    //�����ȷҪ���ʼ���ڲ���QUEUE,
    if (init_inner_queue)
    {
        inner_frame_malloc_ = new INNER_APPFRAME_MALLOCOR();
        inner_frame_malloc_->initialize(INIT_FRAME_MALLOC_NUMBER);

        inner_message_queue_ = new INNER_FRAME_MESSAGE_QUEUE(INNER_QUEUE_WATER_MARK);
        //inner_message_queue_->open(,INNER_QUEUE_WATER_MARK);
    }

    //��ʼ������
    if (init_lock_pool)
    {
        //��������ߴ��һ���ʼ����������
        trans_lock_pool_.rehash(sztransmap / 2);
    }
}


//ע�������Լ���Ӧ�����������
int Transaction_Manager::register_trans_cmd(unsigned int cmd,
                                            Transaction_Base *ptxbase,
                                            bool if_auto_lock_trans,
                                            unsigned int lock_trans_cmd)
{
    //����Ƿ����ظ���ע����������
#if defined DEBUG || defined _DEBUG
    HASHMAP_OF_POLLREGTRANS::iterator mapiter = regtrans_pool_map_.find(cmd);

    if (mapiter != regtrans_pool_map_.end())
    {
        ZLOG_ERROR("[framework] Find Repeat Command ID:%u From MAP.", cmd);
        delete ptxbase;
        return SOAR_RET::ERROR_FIND_REPEAT_CMD_ID;
    }

#endif

    CREATE_TRANS_RECORD ctr_trans_rec;
    ctr_trans_rec.trans_command_ = cmd;
    ctr_trans_rec.if_auto_trans_lock_ = if_auto_lock_trans;
    ctr_trans_rec.trans_lock_cmd_ = lock_trans_cmd;
    regtrans_pool_map_[cmd] = ctr_trans_rec;

    //��ʼ��
    regtrans_pool_map_[cmd].crttrs_cmd_pool_.initialize(POOL_EXTEND_TRANSACTION_NUM);

    for (size_t i = 0; i < POOL_EXTEND_TRANSACTION_NUM; ++i)
    {
        Transaction_Base *tmp_txbase = ptxbase->create_self(timer_queue_, this);
        regtrans_pool_map_[cmd].crttrs_cmd_pool_.push_back(tmp_txbase);
    }

    //����ط�Υ����˭���룬˭ɾ����ԭ�򣬲��ã�����
    delete ptxbase;

    return 0;
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2006��4��3��
Function        : Transaction_Manager::regiester_trans_id
Return          : int ==0 ��ʾ�ɹ�,����ʧ��,�������������ظ�������ID
Parameter List  :
  Param1: unsigned int transid    ����ID
  Param2: Transaction_Base* ptxbase �����Ӧ�����Handler
Description     : ע��һ������ID,�Լ�����ID��Ӧ�����Handler
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int Transaction_Manager::regiester_trans_id(unsigned int transid,
                                            unsigned int trans_cmd,
                                            Transaction_Base *ptxbase)
{
    //����Ƿ����ظ�������ID,����transid���ۼӵģ����Ի�����������¶�
#if defined DEBUG || defined _DEBUG
    HASHMAP_OF_TRANSACTION::iterator mapiter = transc_map_.find(transid);

    if (mapiter != transc_map_.end())
    {
        ZLOG_ERROR("[framework] Find Repeat Transaction ID:%u From MAP.", transid);
        return SOAR_RET::ERROR_FIND_REPEAT_TRANSACTION_ID;
    }

#endif

    //
    if (statistics_clock_)
    {
        ptxbase->trans_create_time_ = statistics_clock_->sec();
        ++(regtrans_pool_map_[trans_cmd].create_trans_num_);
    }

    transc_map_[transid] = ptxbase;

    return 0;
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2006��4��3��
Function        : Transaction_Manager::unregiester_trans_id
Return          : int ��ʾ�ɹ�,����ʧ��,�������������ظ�������ID
Parameter List  :
  Param1: unsigned int transid ����ID
Description     : ע��TransID.
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int Transaction_Manager::unregiester_trans_id(unsigned int transid,
                                              unsigned int trans_cmd,
                                              int run_state,
                                              time_t trans_start)
{
    //����Ƿ������ҵ���Ӧ���¼�ID

    HASHMAP_OF_TRANSACTION::iterator mapiter = transc_map_.find(transid);

    if (mapiter == transc_map_.end())
    {
        ZLOG_ERROR("[framework] unregiester_trans_id,Can't Find Transaction ID:%u From MAP.", transid);
        return SOAR_RET::ERROR_CANNOT_FIND_TRANSACTION_ID;
    }

    //���Ҫ����ͳ��
    if (statistics_clock_)
    {
        switch (run_state)
        {
            case Transaction_Base::INIT_RUN_STATE:
            case Transaction_Base::RIGHT_RUN_STATE:
                ++(regtrans_pool_map_[trans_cmd].destroy_right_num_);
                break;

            case Transaction_Base::TIMEOUT_RUN_STATE:
                ++(regtrans_pool_map_[trans_cmd].destroy_timeout_num_) ;
                break;

            case Transaction_Base::EXCEPTION_RUN_STATE:
            default:
                ++(regtrans_pool_map_[trans_cmd].destroy_exception_num_);
                break;
        }

        //ͳ�ƺķѵ�ʱ��
        (regtrans_pool_map_[trans_cmd].trans_consume_time_) += (statistics_clock_->sec() - trans_start);
    }

    //��erase֮ǰ����ָ��
    Transaction_Base *rt_tsbase = mapiter->second;

    transc_map_.erase(transid);

    //���س���
    return_clone_to_pool(trans_cmd, rt_tsbase);

    return 0;
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2008��1��9��
Function        : Transaction_Manager::process_pipe_frame
Return          : int
Parameter List  :
  Param1: size_t& proc_frame
  Param2: size_t& create_trans
Description     :
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int Transaction_Manager::process_pipe_frame(size_t &proc_frame, size_t &create_trans)
{
    int ret = 0;
    create_trans = 0;

    Zerg_App_Frame *tmp_frame = reinterpret_cast<Zerg_App_Frame *>(trans_recv_buffer_);

    for (proc_frame = 0; zerg_mmap_pipe_->is_empty_bus(Zerg_MMAP_BusPipe::RECV_PIPE_ID) == false && proc_frame < MAX_ONCE_PROCESS_FRAME ;  ++proc_frame)
    {
        //
        ret = zerg_mmap_pipe_->pop_front_recvpipe(tmp_frame);

        if (ret !=  0)
        {
            return 0;
        }

        DEBUGDUMP_FRAME_HEAD(tmp_frame, "FROM RECV PIPE FRAME", RS_DEBUG);

        //�Ƿ񴴽�һ������
        bool bcrtcx = false;
        //tmp_frame���û���
        ret = process_appframe(tmp_frame, bcrtcx);

        //
        if (ret !=  0)
        {
            continue;
        }

        //������һ������
        if ( true == bcrtcx )
        {
            ++create_trans;
        }
    }

    //
    return 0;
}

//�����ݷ��뷢�͹ܵ�
int Transaction_Manager::push_back_sendpipe(Zerg_App_Frame *proc_frame)
{
    //Zerg_MMAP_BusPipe�����ȳ�ʼ��....
    return zerg_mmap_pipe_->push_back_sendpipe(proc_frame);
}

//
/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2008��1��9��
Function        : Transaction_Manager::get_clone_from_pool
Return          : int
Parameter List  :
  Param1: unsigned int frame_cmd     ��¡�����������
  Param2: unsigned int qq_uin,       ʹ�õ��û���QQ����
  Param3: Transaction_Base*& crt_trans ȡ�ߵ�����Ŀ�¡
Description     : �ӳ����м��������õ�һ��Trans�Ŀ�¡
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int Transaction_Manager::get_clone_from_pool(unsigned int frame_cmd,
                                             unsigned int qq_uin,
                                             Transaction_Base *&crt_trans)
{
    int ret = 0;

    HASHMAP_OF_POLLREGTRANS::iterator mapiter = regtrans_pool_map_.find(frame_cmd);

    if (mapiter == regtrans_pool_map_.end())
    {
        //�����һ��rsp�����Ҳ����������� ����һ����ż����rsp�������֣�������req��������
        //Ϊʲô������������־�أ���Ϊ��ЩREQ�м���Ϊ�����Ӧ��,�ں������ͳһ����־���

        return SOAR_RET::ERROR_TRANS_NO_CLONE_COMMAND;
    }

    CREATE_TRANS_RECORD &reg_ctr_trans = regtrans_pool_map_[frame_cmd];

    //����������Ҫ���м���
    if (reg_ctr_trans.if_auto_trans_lock_)
    {
        //���м���,
        ret = lock_qquin_trnas_cmd(qq_uin,
                                   reg_ctr_trans.trans_lock_cmd_,
                                   frame_cmd);

        if (0 != ret )
        {
            return ret;
        }
    }

    ZLOG_DEBUG("Get clone frame command [%u],QQUin[%u],Pool size=[%u].",
               frame_cmd,
               qq_uin,
               reg_ctr_trans.crttrs_cmd_pool_.size());

    //�������һ��
    if (reg_ctr_trans.crttrs_cmd_pool_.size() == 1)
    {
        ZLOG_INFO("[framework] Before extend trans.");
        //ȡһ��ģ��
        Transaction_Base *model_trans = NULL;
        reg_ctr_trans.crttrs_cmd_pool_.pop_front(model_trans);

        size_t capacity_of_pool = reg_ctr_trans.crttrs_cmd_pool_.capacity();
        reg_ctr_trans.crttrs_cmd_pool_.resize(capacity_of_pool + POOL_EXTEND_TRANSACTION_NUM);

        ZLOG_INFO("[framework] Pool Size=%u,  command %u, capacity = %u , resize =%u .",
                  reg_ctr_trans.crttrs_cmd_pool_.size(),
                  frame_cmd,
                  capacity_of_pool,
                  capacity_of_pool + POOL_EXTEND_TRANSACTION_NUM);

        //��ģ�Ϳ�¡N-1��Trans
        for (size_t i = 0; i < POOL_EXTEND_TRANSACTION_NUM; ++i)
        {
            Transaction_Base *cloned_txbase = model_trans->create_self(timer_queue_, this);
            reg_ctr_trans.crttrs_cmd_pool_.push_back(cloned_txbase);
        }

        //��ģ�ͷŵ���N��
        reg_ctr_trans.crttrs_cmd_pool_.push_back(model_trans);
        ZLOG_INFO("[framework] After Extend trans.");
    }

    //ȡ��һ������
    reg_ctr_trans.crttrs_cmd_pool_.pop_front(crt_trans);
    //��ʼ��Ѿ��
    crt_trans->re_init();

    return 0;
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2008��1��21��
Function        : Transaction_Manager::return_clone_to_pool
Return          : int
Parameter List  :
  Param1: unsigned int frame_cmd     ��¡�����������
  Param2: Transaction_Base*& rt_txbase �黹�Ŀ�¡������
Description     : ������м��������黹һ��Trans�Ŀ�¡
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int Transaction_Manager::return_clone_to_pool(unsigned int frame_cmd, Transaction_Base *&rt_txbase)
{
#if defined DEBUG || defined _DEBUG
    HASHMAP_OF_POLLREGTRANS::iterator mapiter = regtrans_pool_map_.find(frame_cmd);

    if (mapiter == regtrans_pool_map_.end())
    {
        return SOAR_RET::ERROR_TRANS_NO_CLONE_COMMAND;
    }

#endif //defined DEBUG || defined _DEBUG
    //
    CREATE_TRANS_RECORD &reg_ctr_trans  = regtrans_pool_map_[frame_cmd];
    ZLOG_DEBUG("[framework] Return clone frame command %u,Pool size=%u .",
               frame_cmd,
               reg_ctr_trans.crttrs_cmd_pool_.size());

    //���ԭ��Ҫ����������ھͽ���
    if (reg_ctr_trans.if_auto_trans_lock_)
    {
        unlock_qquin_trans_cmd(rt_txbase->req_qq_uin_,
                               reg_ctr_trans.trans_lock_cmd_);
    }

    //������Դ�Ļ���
    rt_txbase->finish();

    //
    reg_ctr_trans.crttrs_cmd_pool_.push_back(rt_txbase);
    return 0;
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2008��1��21��
Function        : Transaction_Manager::recycle_clone_from_pool
Return          : void
Parameter List  : NULL
Description     : �ӳ����м���ն��ڵ�����Ŀ�¡�������¡�����ռ�ù�����ڴ棬��װ���Ƶ�����ģ�ͣ�
Calls           :
Called By       :
Other           : ��¡������Ӧ�ò���̫��
Modify Record   :
******************************************************************************************/
void Transaction_Manager::recycle_clone_from_pool()
{
    //
    HASHMAP_OF_POLLREGTRANS::iterator iter_tmp = regtrans_pool_map_.begin();
    HASHMAP_OF_POLLREGTRANS::iterator iter_end = regtrans_pool_map_.end();
    //��Ϊ�ȽϹؼ�������RS_INFO
    ZLOG_INFO("[framework] Recycle trans,transaction manager are processing pool number [%d] . ",
              regtrans_pool_map_.size());

    for (unsigned int i = 1; iter_tmp != iter_end ; ++iter_tmp, ++i)
    {
        unsigned int frame_command = iter_tmp->first;
        POOL_OF_REGISTERTRANS &pool_regtrans = (iter_tmp->second).crttrs_cmd_pool_;
        ZLOG_INFO("[framework] %u.Pool porcess command:%u,capacity:%u,size:%u,use:%u.",
                  i,
                  frame_command,
                  pool_regtrans.capacity(),
                  pool_regtrans.size(),
                  pool_regtrans.capacity() - pool_regtrans.size()
                 );

        //������ӵĿռ� > RECYCLE_POOL_THRESHOLD_VALUE ��������ʹ�õ������¡����
        //��������ӣ����ղ������ݣ������˷�
        const size_t USE_TRANS_THRESHOLD = RECYCLE_POOL_THRESHOLD_VALUE / 8;

        if (pool_regtrans.capacity() - pool_regtrans.size() < USE_TRANS_THRESHOLD &&
            pool_regtrans.size() > RECYCLE_POOL_THRESHOLD_VALUE )
        {
            const size_t RECYCLE_TRANS_NUM = RECYCLE_POOL_THRESHOLD_VALUE / 2;
            size_t pool_capacity = pool_regtrans.capacity();

            //ѭ��ȡ��һ������ɾ����
            for (size_t i = 0; i < RECYCLE_TRANS_NUM; ++i)
            {
                Transaction_Base *recycle_txbase = NULL;
                pool_regtrans.pop_front(recycle_txbase);
                delete recycle_txbase;
            }

            //�������ӵ�����
            pool_regtrans.resize(pool_capacity - RECYCLE_TRANS_NUM);
        }
    }

    //
    return ;
}

/******************************************************************************************
Author          : Sail(ZENGXING)  Date Of Creation: 2009��3��16��
Function        : Transaction_Manager::lock_qquin_trnas_cmd
Return          : int
Parameter List  :
  Param1: unsigned int qq_uin        QQUIN
  Param2: unsigned int trnas_lock_id ������ID,���Ժ���������ͬ�����߲�ͬ
  Param3: unsigned int frame_cmd     ������������������־���
Description     : ��ĳһ���û���һ�������������м���
Calls           :
Called By       : ����������˼�Ǳ�֤һ��ʱ�̣�ֻ��һ������������,������������
Other           :
Modify Record   :
******************************************************************************************/
int Transaction_Manager::lock_qquin_trnas_cmd(unsigned int qq_uin,
                                              unsigned int trnas_lock_id,
                                              unsigned int frame_cmd)
{
    TRANS_LOCK_RECORD lock_rec(qq_uin, trnas_lock_id);
    std::pair <INNER_TRANS_LOCK_POOL::iterator, bool> iter_tmp = trans_lock_pool_.insert(lock_rec);

    //����Ѿ���һ�����ˣ���ô����ʧ��
    if (false == iter_tmp.second )
    {
        ZLOG_ERROR("[framework] [LOCK]Oh!Transaction lock fail.QQUin[%u] trans lock id[%u] trans cmd[%u].",
                   qq_uin,
                   trnas_lock_id,
                   frame_cmd);
        return -1;
    }

    return 0;
}

//��ĳһ���û���һ�������������м���
void Transaction_Manager::unlock_qquin_trans_cmd(unsigned int qq_uin, unsigned int lock_trnas_id)
{
    TRANS_LOCK_RECORD lock_rec(qq_uin, lock_trnas_id);
    trans_lock_pool_.erase(lock_rec);
    return;
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2006��4��22��
Function        : Transaction_Manager::process_appframe
Return          : int
Parameter List  :
  Param1: const Zerg_App_Frame* ppetappframe ����������֡���ݣ�ppetappframe֡������������process_appframe��������
  Param3: bool& bcrttx                �Ƿ񴴽�����
Description     :
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int Transaction_Manager::process_appframe( Zerg_App_Frame *ppetappframe, bool &bcrttx)
{
    bcrttx = false;
    int ret = 0;

    //����Ǹ��������ӡ����
    if (ppetappframe->frame_option_ & Zerg_App_Frame::DESC_MONITOR_TRACK)
    {
        Zerg_App_Frame::dumpoutput_framehead(ppetappframe, "[TRACK MONITOR][TRANS PROCESS]", RS_INFO);
    }

    Transaction_Base *crt_trans = NULL;
    ret = get_clone_from_pool(ppetappframe->frame_command_,
                              ppetappframe->frame_uid_,
                              crt_trans);

    //��һ���������������
    if (ret == 0)
    {

        //����0����Ϊ�����Transaction IDĬ��Ϊ0,���������кô�,���׷��ִ���.
        if (++trans_id_builder_ == 0)
        {
            ++trans_id_builder_;
        }

        ret = regiester_trans_id(trans_id_builder_, ppetappframe->frame_command_, crt_trans);

        //������һ������
        if (ret != 0)
        {
            //
            return_clone_to_pool(ppetappframe->frame_command_, crt_trans);
            return ret;
        }

        ret = crt_trans->initialize_trans(ppetappframe, trans_id_builder_);

        //ע����������õ���handle_close
        if (ret != 0)
        {
            crt_trans->handle_close();
            return ret;
        }

        bcrttx = true;

        //ͳ�Ƽ�����
        ++gen_trans_counter_;
        ++cycle_gentrans_counter_;

        ZLOG_DEBUG("Create Trascation ,Command:%u Transaction ID:%u .",
                   ppetappframe->frame_command_, trans_id_builder_);
    }
    else
    {
        //��������IDѰ������,�ĳ��û��������ID��ѯ
        Transaction_Base *run_tans = NULL;
        ret = get_handler_by_transid(ppetappframe->backfill_trans_id_,
                                     ppetappframe->frame_command_,
                                     run_tans);

        if (ret != 0 )
        {
            return ret;
        }

        ZLOG_DEBUG("Find raw Transaction ID: %u. ", ppetappframe->backfill_trans_id_);

        //����ܵ���FRAME�����ݺ���Ϣ
        ret = run_tans->check_receive_frame(ppetappframe);

        if (ret != 0 )
        {
            return ret;
        }

        //�յ�һ�����ݣ����д���
        ret = run_tans->receive_trans_msg(ppetappframe);

        if (ret != 0 )
        {
            run_tans->handle_close();
            return ret;
        }
    }

    return 0;
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2006��4��3��
Function        : Transaction_Manager::get_handler_by_transid
Return          : int
Parameter List  :
  Param1: unsigned int transid     ����ID
  Param1: unsigned int trans_cmd   ����inmore��Ҫ��������һ��CMD�������޷�����ʱ��Ӧ
  Param2: Transaction_Base*& ptxbase ���ص�Handlerָ��
Description     : ��������IDѰ������
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int Transaction_Manager::get_handler_by_transid(unsigned int transid, unsigned int trans_cmd, Transaction_Base *&ptxbase)
{
    //��������IDѰ������
    HASHMAP_OF_TRANSACTION::iterator mapiter = transc_map_.find(transid);

    if (mapiter == transc_map_.end())
    {
        ZLOG_INFO("[framework] get_handler_by_transid,Can't Find Transaction ID:%u,Command:%u From MAP.",
                  transid,
                  trans_cmd);
        return SOAR_RET::ERROR_CANNOT_FIND_TRANSACTION_ID;
    }

    ptxbase = mapiter->second;

    return 0;
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2007��11��14��
Function        : Transaction_Manager::dump_all_trans_info
Return          : void
Parameter List  : NULL
Description     : Dump�õ�ʵ��
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
void Transaction_Manager::dump_all_trans_info() const
{
    //
    HASHMAP_OF_TRANSACTION::const_iterator iter_tmp = transc_map_.begin();
    HASHMAP_OF_TRANSACTION::const_iterator iter_end = transc_map_.end();
    //��Ϊ�ȽϹؼ�������RS_INFO
    ZLOG_INFO("[framework] Transaction Manager are processing [%d] transactions. ", transc_map_.size());

    for (unsigned int i = 1; iter_tmp != iter_end ; ++iter_tmp, ++i)
    {
        Transaction_Base *pbase = iter_tmp->second;
        ZLOG_INFO("[framework] %u.Transaction ID:%u,Request UIN:%u,Command:%u,State:%u.",
                  i,
                  (pbase)->req_qq_uin_,
                  (pbase)->req_qq_uin_,
                  (pbase)->trans_command_,
                  (pbase)->trans_phase_,
                  (pbase)->transaction_id_ );
    }

    //
    return ;
}

void Transaction_Manager::dump_trans_pool_info() const
{
    //
    HASHMAP_OF_POLLREGTRANS::const_iterator iter_tmp = regtrans_pool_map_.begin();
    HASHMAP_OF_POLLREGTRANS::const_iterator iter_end = regtrans_pool_map_.end();
    //��Ϊ�ȽϹؼ�������RS_INFO
    ZLOG_INFO("[framework] Transaction Manager are processing pool number [%d] . ", regtrans_pool_map_.size());

    for (unsigned int i = 1; iter_tmp != iter_end ; ++iter_tmp, ++i)
    {
        unsigned int frame_command = iter_tmp->first;
        const POOL_OF_REGISTERTRANS &pool_regtrans = (iter_tmp->second).crttrs_cmd_pool_;
        ZLOG_INFO("[framework] %u.Pool porcess command:%u,capacity:%u,size:%u.",
                  i,
                  frame_command,
                  pool_regtrans.capacity(),
                  pool_regtrans.size()
                 );
    }

    //
    return ;
}

//DUMP���е�ͳ����Ϣ
void Transaction_Manager::dump_statistics_info() const
{
    //
    HASHMAP_OF_POLLREGTRANS::const_iterator iter_tmp = regtrans_pool_map_.begin();
    HASHMAP_OF_POLLREGTRANS::const_iterator iter_end = regtrans_pool_map_.end();

    ZLOG_INFO("[framework] [TRANS INFO] All generate transaction counter [%llu] ,previous cycle generate transaction number[%llu].",
              gen_trans_counter_,
              cycle_gentrans_counter_);

    //��Ϊ�ȽϹؼ�������RS_INFO
    ZLOG_INFO("[framework] Transaction Manager are processing [%d] transactions. ", transc_map_.size());

    for (unsigned int i = 1; iter_tmp != iter_end ; ++iter_tmp, ++i)
    {
        ZLOG_INFO("[framework] [TRANS INFO]%u.Transaction command ID [%u],create [%llu], destroy right[%llu], destroy timeout[%llu],destroy exception[%llu],consume seconds[%llu]",
                  i,
                  (iter_tmp->second).trans_command_,
                  (iter_tmp->second).create_trans_num_,
                  (iter_tmp->second).destroy_right_num_,
                  (iter_tmp->second).destroy_timeout_num_,
                  (iter_tmp->second).destroy_exception_num_,
                  (iter_tmp->second).trans_consume_time_
                 );
    }
}

void Transaction_Manager::dump_all_debug_info() const
{
    dump_trans_pool_info();
    dump_statistics_info();
    dump_all_trans_info();
}

//����������һ��Ϣͷ��һ��������,_��ʾ����һ���ڲ����������ṩ���������ʿʹ��
int Transaction_Manager::mgr_sendmsghead_to_service(unsigned int cmd,
                                                    unsigned int qquin,
                                                    const SERVICES_ID &rcvsvc,
                                                    const SERVICES_ID &proxysvc,
                                                    unsigned int backfill_trans_id,
                                                    unsigned int app_id,
                                                    unsigned int option)
{
    //
    Zerg_App_Frame *rsp_msg = reinterpret_cast<Zerg_App_Frame *>(trans_send_buffer_);
    rsp_msg->init_framehead(Zerg_App_Frame::MAX_LEN_OF_APPFRAME);

    rsp_msg->frame_length_ = Zerg_App_Frame::LEN_OF_APPFRAME_HEAD;
    rsp_msg->frame_command_ = cmd;
    rsp_msg->frame_uid_ = qquin;

    rsp_msg->transaction_id_ = 0;
    rsp_msg->recv_service_ = rcvsvc;
    rsp_msg->proxy_service_ = proxysvc;
    rsp_msg->send_service_ =  this->self_svc_id_;
    rsp_msg->frame_option_ = option;

    //��������ID
    rsp_msg->backfill_trans_id_ = backfill_trans_id;
    rsp_msg->app_id_ = app_id;

    return push_back_sendpipe(rsp_msg);
}

//������ͳ��
void Transaction_Manager::enable_trans_statistics (const ZCE_Time_Value *stat_clock)
{
    statistics_clock_ = stat_clock;
}

int Transaction_Manager::mgr_postframe_to_msgqueue(Zerg_App_Frame *post_frame)
{
    int ret = 0;
    Zerg_App_Frame *tmp_frame = NULL;

    //����Ǵӳ����м�ȡ����FRAME����ʲô������

    inner_frame_malloc_->clone_appframe(post_frame, tmp_frame);

    //�����ϲ��õȴ��κ�ʱ��
    ret = inner_message_queue_->enqueue(tmp_frame);

    //����ֵС��0��ʾʧ��
    if (ret < 0)
    {
        ZLOG_DEBUG("Post message to send queue fail.ret =%d"
                   "Send queue message_count:%u message_bytes:%u. ",
                   ret,
                   inner_message_queue_->size(),
                   inner_message_queue_->size() * sizeof(Zerg_App_Frame *));
        //�������Ժ󻹻�ȥ
        inner_frame_malloc_->free_appframe(tmp_frame);

        return SOAR_RET::ERROR_NOTIFY_SEND_QUEUE_ENQUEUE_FAIL;
    }

    return 0;
}

//����ӽ��ն���ȡ����FRAME
int Transaction_Manager::process_queue_frame(size_t &proc_frame, size_t &create_trans)
{
    int ret = 0;
    create_trans = 0;

    //�������
    for (proc_frame = 0; inner_message_queue_->empty() == false && proc_frame < MAX_ONCE_PROCESS_FRAME ;  ++proc_frame)
    {

        Zerg_App_Frame *tmp_frame = NULL;
        //
        ret = inner_message_queue_->dequeue(tmp_frame);

        //���С��0��ʾ���󣬵�����ط�Ӧ����һ��������Ϊ���滹��һ���ж�
        if (ret < 0)
        {
            ZLOG_ERROR("[framework] Recv queue dequeue fail ,ret=%u,", ret);
            return 0;
        }

        DEBUGDUMP_FRAME_HEAD(tmp_frame, "FROM RECV QUEUE FRAME:", RS_DEBUG);

        //�Ƿ񴴽�һ������
        bool bcrtcx = false;

        //tmp_frame  ���ϻ���
        ret = process_appframe(tmp_frame, bcrtcx);
        //�ͷ��ڴ�
        inner_frame_malloc_->free_appframe(tmp_frame);

        //
        if (ret !=  0)
        {
            continue;
        }

        //������һ������
        if ( true == bcrtcx )
        {
            ++create_trans;
        }
    }

    //
    return 0;
}

//�õ��������ĸ��ز���
void Transaction_Manager::get_manager_load_foctor(unsigned int &load_max, unsigned int &load_cur)
{
    load_max = static_cast<unsigned int>(max_trans_);
    load_cur = static_cast<unsigned int>( transc_map_.size());

    //���������������1
    if (load_cur == 0)
    {
        load_cur = 1;
    }
}

//�õ��������ĸ��ز���
//��һЩ��������û�н׶��Ե�����������ĺ��������ر����룬
void Transaction_Manager::get_manager_load_foctor2(unsigned int &load_max, unsigned int &load_cur)
{
    const unsigned int ONE_CYCLE_GENERATE_TRANS = 30000;

    //�õ����ص�����
    load_max = ONE_CYCLE_GENERATE_TRANS;

    if (cycle_gentrans_counter_ > ONE_CYCLE_GENERATE_TRANS)
    {
        load_cur = ONE_CYCLE_GENERATE_TRANS;
    }
    else
    {
        load_cur = cycle_gentrans_counter_;
    }

    //���ڼ���������
    cycle_gentrans_counter_ = 0;

    //���������������1
    if (load_cur == 0)
    {
        load_cur = 1;
    }
}

//�õ�ʵ��
Transaction_Manager *Transaction_Manager::instance()
{
    if (instance_ == NULL)
    {
        instance_ = new Transaction_Manager();
    }

    return instance_;
}

//ʵ����ֵ
void Transaction_Manager::instance(Transaction_Manager *pinstatnce)
{
    clean_instance();
    instance_ = pinstatnce;
    return;
}

//���ʵ��
void Transaction_Manager::clean_instance()
{
    if (instance_)
    {
        delete instance_;
        instance_ = NULL;
    }

    return;
}

//ֱ�ӷ���һ��buffer to services��
int Transaction_Manager::mgr_sendbuf_to_service(unsigned int cmd,
                                                unsigned int qquin,
                                                unsigned int trans_id,
                                                unsigned int backfill_trans_id,
                                                const SERVICES_ID &rcvsvc,
                                                const SERVICES_ID &proxysvc,
                                                const SERVICES_ID &sndsvc,
                                                const unsigned char *buf,
                                                size_t buf_len,
                                                unsigned int app_id,
                                                unsigned int option )
{
    return zerg_mmap_pipe_->pipe_sendbuf_to_service(cmd,
                                                    qquin,
                                                    trans_id,
                                                    backfill_trans_id,
                                                    rcvsvc,
                                                    proxysvc,
                                                    sndsvc,
                                                    buf,
                                                    buf_len,
                                                    app_id,
                                                    option);
}

