#ifndef SOARING_LIB_TRANSACTION_MANAGER_H_
#define SOARING_LIB_TRANSACTION_MANAGER_H_

#include "soar_zerg_frame_malloc.h"
#include "soar_zerg_mmappipe.h"

//�Ƿ���zengyu��˵�Ľ����е�IO�ӿڼ̳д���???
//�����г������������ʺ�
//�������������������һ����ܣ��Ҽ�����Ϊ��Ϊ�������ģ�Ͳ����޸�.����.....

//�򵥣����ǲ�Ҫ̫��.....
//�о��ҵ��뷨�ǱȽϼ򵥵�,���濴��û���¼�����,��ʵ������Reactor���й���,��ʵ�ڲ�ʵ�����ɶ��е�.

//����ʱ����.....

//class ITransIOProcess
//{
//    ReceiveMessage(Zerg_App_Frame *proc_frame) = 0;
//    SendMessage(const Zerg_App_Frame *proc_frame) = 0;
//};

/************************************************************************************
2008��12��26��,ʥ���ĺ���һ�죬��һ�ܼ�ֱ�ǻ���ڵ�,������������ĩ��
��Ϊһ���ʺ磬�����Ҫ������Ƭ��գ����������е�����
����������δ��룬Susantian�������һ����Ϣ����,����Ҫ��ܺ�����һ�����о�
�ṩһ�����ڲ��첽��������ķ���,
ԭ������һ�����еģ����ǣ����ǣ��һ���Ҫ˵˵��ʷ����δ���Ĺ������
�������ǵĴ�������ACE��Notify���ƵĶ��еģ�
�������������ģ��
RECV PIPE===============>ACE Notify �Ķ���==================>
                                                             �������������
SEND PIPE<===================================================

�����ڷ���ACE Notify������һ����������Ǹ�Ϊ�˲�Ҫ���С��������ACE���塷
ȡ��ACE_NOtify���ƺ��ģ��,
RECV PIPE==========================================>
                                                    �������������
SEND PIPE<==========================================

��������Ϊ��TASK�������Ǹ���һ���µ�ģʽ
���ڶ�TASK��������ģ��:
RECV PIPE =====================>          ==============> Send����(��������Send)
                                 ������                                  ��TASK����
SEND PIPE <=====================          <============= Recv����(��������Recv)

Ϊʲô����ϣ����PIPE�Ĵ������֮�⣬�ټ���һ���µĶ��д�����Ϣ��?
��Ϊ������еĴ�����ͬ������(�����������صķ�ʽ),�����˺ܶ�仯������һЩ�������Ʋ���ʹ��.
������ϣ��ʵ�ֵ��µ�ģ����
RECV PIPE==================================>
                                           �������������  <==============> ��Ϣ����
SEND PIPE<=================================

************************************************************************************/
class Zerg_MMAP_BusPipe;
class Transaction_Base;
class Zerg_App_Frame;

/******************************************************************************************
struct CREATE_TRANS_RECORD �����ͳ����Ϣ
******************************************************************************************/
//ע�������ĳ���
typedef ZCE_LIB::lordrings<Transaction_Base *>                     POOL_OF_REGISTERTRANS;

struct CREATE_TRANS_RECORD
{
public:
    //
    CREATE_TRANS_RECORD(unsigned int trans_cmd = 0);
    ~CREATE_TRANS_RECORD();

public:

    //ע�����������
    unsigned int                 trans_command_;
    //ע������ĳ���
    POOL_OF_REGISTERTRANS        crttrs_cmd_pool_;

    //-------------------------------------------------
    //�Ƿ��Զ������������
    bool                         if_auto_trans_lock_;
    //��������ID,��������������,����������Ҫһ�����,
    //�����������б���ռλһ���ط������ڼ���
    unsigned int                 trans_lock_cmd_;

    //--------------------------------------------------------------
    //������ͳ����Ϣ

    //���������������
    uint64_t                     create_trans_num_;

    //����ʱ����ȷ״̬����������
    uint64_t                     destroy_right_num_;
    //����ʱ��ʱ״̬������
    uint64_t                     destroy_timeout_num_;
    //����ʱ״̬�쳣����������
    uint64_t                     destroy_exception_num_;

    //�����������ʱ��
    uint64_t                     trans_consume_time_;

};

/******************************************************************************************
struct TRANS_LOCK_RECORD �����ļ�¼��Ԫ
******************************************************************************************/
struct SOARING_EXPORT TRANS_LOCK_RECORD
{
public:
    //Ҫ������QQUIN,
    unsigned int     lock_qq_uin_;
    //����ļ���ID���������һ�������Ӧһ����������ֱ��ʹ��������
    //����Ƕ�������һ����������������ռλһ�����Ȼ����Ǹ����������
    unsigned int     lock_trans_cmd_;

public:
    TRANS_LOCK_RECORD(unsigned int lock_qq_uin, unsigned int lock_trans_command);
    TRANS_LOCK_RECORD();
    ~TRANS_LOCK_RECORD();
};

//�õ�KEY��HASH����
struct SOARING_EXPORT HASH_OF_TRANS_LOCK
{
public:
    size_t operator()(const TRANS_LOCK_RECORD &lock_rec) const
    {
        return (size_t (lock_rec.lock_qq_uin_) + lock_rec.lock_trans_cmd_);
    }
};

//�ж���ȵĺ���
class SOARING_EXPORT EQUAL_OF_TRANS_LOCK
{
public:
    bool operator()(const TRANS_LOCK_RECORD &right, const TRANS_LOCK_RECORD &left) const
    {
        return (right.lock_qq_uin_ == left.lock_qq_uin_ && right.lock_trans_cmd_ == left.lock_trans_cmd_);
    }

};

/******************************************************************************************
class Transaction_Manager
******************************************************************************************/
class SOARING_EXPORT Transaction_Manager
{
    //������Ԫ
    friend class Transaction_Base;

protected:

    //һ��������FRAME����
    static const size_t MAX_ONCE_PROCESS_FRAME       = 1024;

    //����ÿ����չ���������
    static const size_t POOL_EXTEND_TRANSACTION_NUM  = 1024;

    //���յ���ֵ
    static const size_t RECYCLE_POOL_THRESHOLD_VALUE = 2048;

    static const size_t INIT_FRAME_MALLOC_NUMBER     = 2048;

    //QUEUE FRAME���е�ˮλ�꣬���ǵ�����MessageQueue�н���ŵ���ָ�룬
    //���ǳ���Ӧ��Ӧ���ǰ���APPFRAME��֡ͷ����ġ������������ĳ����Ѿ���С��
    static const size_t INNER_QUEUE_WATER_MARK = Zerg_App_Frame::LEN_OF_APPFRAME_HEAD * 102400;

protected:

    //�����ֶ�Ӧ���ӵĴ���
    typedef unordered_map<unsigned int , CREATE_TRANS_RECORD >        HASH_MAP_OF_POLLREGTRANS;
    //����ID��Ӧ����Ĺ�����
    typedef unordered_map<unsigned int , Transaction_Base * >         HASHMAP_OF_TRANSACTION;

    //ע������õĲ���ָ��,����ʵ���õ���ָ��
    typedef ZCE_Message_Queue_Deque<ZCE_NULL_SYNCH, Zerg_App_Frame *> INNER_FRAME_MESSAGE_QUEUE;
    //APPFRAME�ķ�����
    typedef AppFrame_Mallocor_Mgr<ZCE_Null_Mutex>                     INNER_APPFRAME_MALLOCOR;

    //�ڲ�����������
    typedef unordered_set<TRANS_LOCK_RECORD, HASH_OF_TRANS_LOCK, EQUAL_OF_TRANS_LOCK>  INNER_TRANS_LOCK_POOL;

protected:

    //ע���¼���ID�Ͷ�Ӧ�Ĵ���Handler.
    HASHMAP_OF_TRANSACTION      transc_map_;

    //
    HASH_MAP_OF_POLLREGTRANS    regtrans_pool_map_;

    //���ĳ���
    INNER_TRANS_LOCK_POOL       trans_lock_pool_;

    //�����¼�����
    size_t                      max_trans_;

    //����ID������
    unsigned int                trans_id_builder_;

    //�Լ���Services Info
    SERVICES_ID                 self_services_id_;

    //Ĭ��ʹ�õ�
    ZCE_Timer_Queue            *timer_queue_;

    //�����ڴ�Ĺܵ�
    Zerg_MMAP_BusPipe          *zerg_mmap_pipe_;

    //ͳ��ʱ��
    const ZCE_Time_Value       *statistics_clock_;

    //���͵Ļ�����
    Zerg_App_Frame             *trans_send_buffer_;
    //�������ݻ�����
    Zerg_App_Frame             *trans_recv_buffer_;

    // fake���ݻ�����
    Zerg_App_Frame             *fake_recv_buffer_;

    //�ڲ�FRAME������
    INNER_APPFRAME_MALLOCOR    *inner_frame_malloc_;
    //�ڲ�FRAME�Ķ���
    INNER_FRAME_MESSAGE_QUEUE  *inner_message_queue_;

    //ͳ�Ʒ�����һЩ����
    //���������������¼
    uint64_t                    gen_trans_counter_;
    //һ�������ڲ�������������
    unsigned int                cycle_gentrans_counter_;

protected:
    //SingleTon��ָ��
    static Transaction_Manager *instance_;

public:

    //�������Ĺ��캯��
    Transaction_Manager();
    virtual ~Transaction_Manager();

protected:

    //�ӳ����м��������õ�һ��Trans�Ŀ�¡
    int get_clone_from_pool(unsigned int frame_cmd,
                            unsigned int qq_uin,
                            Transaction_Base *&ptxbase);
    //������м��������黹һ��Trans�Ŀ�¡
    int return_clone_to_pool(unsigned int frame_cmd, Transaction_Base *&ptxbase);

    //����һ���յ������
    int process_appframe( Zerg_App_Frame *ppetappframe, bool &crttx );

public:

    //�ӳ����м���ն��ڵ�����Ŀ�¡�������¡�����ռ�ù�����ڴ棬��װ���Ƶ�����ģ�ͣ�
    //��¡������Ӧ�ò���̫��
    void recycle_clone_from_pool();

    //����ܵ�������
    int process_pipe_frame(size_t &proc_frame, size_t &create_trans);
    //������Ϣ���е�����
    int process_queue_frame(size_t &proc_frame, size_t &create_trans);

    //��ĳһ���û���һ�������������м���
    int lock_qquin_trnas_cmd(unsigned int qq_uin,
                             unsigned int trnas_lock_id,
                             unsigned int trans_cmd);
    //��ĳһ���û���һ�������������м���
    void unlock_qquin_trans_cmd(unsigned int qq_uin,
                                unsigned int trnas_lock_id);

    //������Ļ���
    int register_trans_cmd(unsigned int cmd,
                           Transaction_Base *ptxbase,
                           bool if_lock_trans = false,
                           unsigned int lock_trans_cmd = 0);

    //ͨ������ID�õ���Ӧ������ָ��
    int get_handler_by_transid(unsigned int transid, unsigned int trans_cmd, Transaction_Base *&ptxbase);

    //����һ�����񣬱����Լ�Ҫ����ĳ�����ݸ�������������ʼ������.
    int create_self(Transaction_Base *ptxbase);

    //��ʼ��,סһ������Ĭ�ϲ���
    void initialize(size_t szregtrans,
                    size_t sztransmap,
                    const SERVICES_ID &selfsvr,
                    ZCE_Timer_Queue *time_reactor,
                    Zerg_MMAP_BusPipe *zerg_mmap_pipe,
                    unsigned int max_frame_len = Zerg_App_Frame::MAX_LEN_OF_APPFRAME,
                    bool init_inner_queue = false,
                    bool init_lock_pool = false);

    //�õ�һ��SvrInfo
    inline const SERVICES_ID *self_svc_info();

    //ȡ�ù������ĸ�������
    void get_manager_load_foctor(unsigned int &load_max,
                                 unsigned int &load_cur);

    void get_manager_load_foctor2(unsigned int &load_max,
                                  unsigned int &load_cur);

    //ע��TransID.
    int unregiester_trans_id(unsigned int transid,
                             unsigned int trans_cmd,
                             int run_state,
                             time_t trans_start);

    //��Transͳ����Ϣ���õ�һ����ǰʱ��
    void enable_trans_statistics (const ZCE_Time_Value *stat_clock);

    //DUMP���е�ͳ����Ϣ
    void dump_statistics_info() const;
    //DUMP���е�������Ϣ
    void dump_all_trans_info() const ;
    //DUMP���еĵ�Tans POOL ��Ϣ
    void dump_trans_pool_info() const;

    //Dump ����DEBUG��Ϣ
    void dump_all_debug_info() const;

    //----------------------------------------------------------------------------------------------------------

    //��װ�յ�һ����Ϣ�����д���,�����ٵļ򻯰汾
    template< class T> int fake_receive_appframe(unsigned int cmd,
                                                 unsigned int qquin,
                                                 const SERVICES_ID &snd_svc,
                                                 const T &info,
                                                 unsigned int option = 0);

    //��װ�յ�һ����Ϣ�����д���,�����е�࣬������ʹ�õ�ʱ���ٽ���һ�η�װ
    template< class T> int fake_receive_appframe(unsigned int cmd,
                                                 unsigned int qquin,
                                                 unsigned int trans_id,
                                                 unsigned int backfill_trans_id,
                                                 const SERVICES_ID &proxy_svc,
                                                 const SERVICES_ID &snd_svc,
                                                 const T &info,
                                                 unsigned int app_id,
                                                 unsigned int option );

    //��װ�յ�һ����Ϣ(buffer)
    inline int fake_receive_appframe_buffer(unsigned int cmd,
        unsigned int qquin,
        unsigned int trans_id,
        unsigned int backfill_trans_id,
        const SERVICES_ID &proxy_svc,
        const SERVICES_ID &snd_svc,
        const char *send_buff,
        unsigned int buff_size,
        unsigned int app_id,
        unsigned int option);

    //----------------------------------------------------------------------------------------------------------
    //Postһ��FRAME���ݵ���Ϣ����,�򵥰汾��û������Ҫ������������
    template< class T> int mgr_postframe_to_msgqueue(unsigned int cmd,
                                                     unsigned int qquin,
                                                     const T &info,
                                                     unsigned int option = 0);

    //Postһ��FRAME���ݵ���Ϣ���У�����α��һЩ��Ϣ�������Ҳ�֪���ṩ�����Ƿ��Ǻ���,
    template< class T> int mgr_postframe_to_msgqueue(unsigned int cmd,
                                                     unsigned int qquin,
                                                     unsigned int trans_id,
                                                     unsigned int backfill_trans_id,
                                                     const SERVICES_ID &rcvsvc,
                                                     const SERVICES_ID &proxysvc,
                                                     const SERVICES_ID &sndsvc,
                                                     const T &info,
                                                     unsigned int app_id,
                                                     unsigned int option);

    //----------------------------------------------------------------------------------------------------------
    //����������һ�������һ��������,�ڲ�����
    template< class T> int mgr_sendmsg_to_service(unsigned int cmd,
                                                  unsigned int qquin,
                                                  unsigned int trans_id,
                                                  unsigned int backfill_trans_id,
                                                  const SERVICES_ID &rcvsvc,
                                                  const SERVICES_ID &proxysvc,
                                                  const SERVICES_ID &sndsvc,
                                                  const T &info,
                                                  unsigned int app_id,
                                                  unsigned int option);

    // ����buf��ĳ��service, buf�Ǵ�õİ�������ĳЩ��Ҫת��buf������
    int mgr_sendbuf_to_service(unsigned int cmd,
                               unsigned int qquin,
                               unsigned int trans_id,
                               unsigned int backfill_trans_id,
                               const SERVICES_ID &rcvsvc,
                               const SERVICES_ID &proxysvc,
                               const SERVICES_ID &sndsvc,
                               const unsigned char *buf,
                               size_t buf_len,
                               unsigned int app_id,
                               unsigned int option);

    //����һ�����ݵ�PIPE
    int push_back_sendpipe(Zerg_App_Frame *proc_frame);
protected:
    //����һ��Ϣͷ��һ��������,�ڲ�����
    int mgr_sendmsghead_to_service(unsigned int cmd,
                                   unsigned int qquin,
                                   const SERVICES_ID &rcvsvc,
                                   const SERVICES_ID &proxysvc,
                                   unsigned int backfill_trans_id = 0,
                                   unsigned int app_id = 0,
                                   unsigned int option = 0);

    //----------------------------------------------------------------------------------------------------------
protected:

    //����һ�����ݵ�QUEUE
    int mgr_postframe_to_msgqueue(Zerg_App_Frame *post_frame);

    //ע��TransID.
    int regiester_trans_id(unsigned int transid, unsigned int trans_cmd, Transaction_Base *ptxbase);

private:

public:
    //Ϊ��SingleTon��׼��
    //ʵ����ֵ
    static void instance(Transaction_Manager *);
    //���ʵ��
    static Transaction_Manager *instance();
    //���ʵ��
    static void clean_instance();

};

//��װ�յ�һ����Ϣ�����д���,�����ٵļ򻯰汾
template< class T>
int Transaction_Manager::fake_receive_appframe(unsigned int cmd,
                                               unsigned int qquin,
                                               const SERVICES_ID &snd_svc,
                                               const T &info,
                                               unsigned int option)
{

    return fake_receive_appframe(cmd,
                                 qquin,
                                 0,
                                 0,
                                 SERVICES_ID(0, 0),
                                 snd_svc,
                                 info,
                                 0,
                                 option);
}

//��װ�յ�һ����Ϣ�����д���,�����е�࣬������ʹ�õ�ʱ���ٽ���һ�η�װ
template< class T>
int Transaction_Manager::fake_receive_appframe(unsigned int cmd,
                                               unsigned int qquin,
                                               unsigned int trans_id,
                                               unsigned int backfill_trans_id,
                                               const SERVICES_ID &proxy_svc,
                                               const SERVICES_ID &snd_svc,
                                               const T &info,
                                               unsigned int app_id,
                                               unsigned int option )
{
    int ret = 0;

    Zerg_App_Frame *tmp_frame = reinterpret_cast<Zerg_App_Frame *>(fake_recv_buffer_);
    tmp_frame->init_framehead(Zerg_App_Frame::MAX_LEN_OF_APPFRAME, option, cmd);

    tmp_frame->frame_uin_ = qquin;
    tmp_frame->send_service_ = snd_svc;
    tmp_frame->recv_service_ = self_services_id_;
    tmp_frame->proxy_service_ = proxy_svc;

    tmp_frame->transaction_id_ = trans_id;
    tmp_frame->backfill_trans_id_ = backfill_trans_id;
    tmp_frame->app_id_ = app_id;

    ret = tmp_frame->appdata_encode(Zerg_App_Frame::MAX_LEN_OF_APPFRAME_DATA, info);

    if (ret != SOAR_RET::SOAR_RET_SUCC)
    {
        return ret;
    }

    //����һ���յ������
    bool crttx;
    ret = process_appframe(tmp_frame, crttx);

    if (ret != SOAR_RET::SOAR_RET_SUCC && ret != SOAR_RET::ERROR_TRANS_HAS_FINISHED)
    {
        return ret;
    }

    return SOAR_RET::SOAR_RET_SUCC;
}

// recv_svr������Լ����ͼ�װ�յ�һ������������fake
int Transaction_Manager::fake_receive_appframe_buffer(unsigned int cmd,
                                                      unsigned int qquin,
                                                      unsigned int trans_id,
                                                      unsigned int backfill_trans_id,
                                                      const SERVICES_ID &proxy_svc,
                                                      const SERVICES_ID &snd_svc,
                                                      const char *recv_buffer,
                                                      unsigned int buff_size,
                                                      unsigned int app_id,
                                                      unsigned int option)
{
    int ret = 0;

    Zerg_App_Frame *tmp_frame = reinterpret_cast<Zerg_App_Frame *>(fake_recv_buffer_);
    tmp_frame->init_framehead(Zerg_App_Frame::MAX_LEN_OF_APPFRAME, option, cmd);

    tmp_frame->frame_uin_ = qquin;
    tmp_frame->send_service_ = snd_svc;
    tmp_frame->recv_service_ = self_services_id_;
    tmp_frame->proxy_service_ = proxy_svc;

    tmp_frame->transaction_id_ = trans_id;
    tmp_frame->backfill_trans_id_ = backfill_trans_id;
    tmp_frame->app_id_ = app_id;

    tmp_frame->frame_length_ = (unsigned int)(buff_size + Zerg_App_Frame::LEN_OF_APPFRAME_HEAD);
    if(tmp_frame->frame_length_ > Zerg_App_Frame::MAX_LEN_OF_APPFRAME_DATA)
    {
        return SOAR_RET::ERROR_FRAME_DATA_IS_ERROR;
    }
    memcpy(tmp_frame->frame_appdata_, recv_buffer, buff_size);

    bool crttx = false;
    ret = process_appframe(tmp_frame, crttx);

    if (ret != SOAR_RET::SOAR_RET_SUCC && ret != SOAR_RET::ERROR_TRANS_HAS_FINISHED)
    {
        return ret;
    }

    return SOAR_RET::SOAR_RET_SUCC;
}

//
inline const SERVICES_ID *Transaction_Manager::self_svc_info()
{
    return &self_services_id_;
}

//����������һ�������һ��������,�ڲ�����
template< class T>
int Transaction_Manager::mgr_sendmsg_to_service(unsigned int cmd,
                                                unsigned int qquin,
                                                unsigned int trans_id,
                                                unsigned int backfill_trans_id,
                                                const SERVICES_ID &rcvsvc,
                                                const SERVICES_ID &proxysvc,
                                                const SERVICES_ID &sndsvc,
                                                const T &info,
                                                unsigned int app_id,
                                                unsigned int option)
{
    //[ע��]һ������ط���recv��send�������ߵ�˳���Ƿ���
    return zerg_mmap_pipe_->pipe_sendmsg_to_service(cmd,
                                                    qquin,
                                                    trans_id,
                                                    backfill_trans_id,
                                                    rcvsvc,
                                                    proxysvc,
                                                    sndsvc,
                                                    info,
                                                    app_id,
                                                    option);
}

//Postһ��FRAME���ݵ���Ϣ���У�����α��һЩ��Ϣ�������Ҳ�֪���ṩ�����Ƿ��Ǻ���
template< class T>
int Transaction_Manager::mgr_postframe_to_msgqueue(
    unsigned int cmd,
    unsigned int qquin,
    unsigned int trans_id,
    unsigned int backfill_trans_id,
    const SERVICES_ID &rcvsvc,
    const SERVICES_ID &proxysvc,
    const SERVICES_ID &sndsvc,
    const T &info,
    unsigned int app_id,
    unsigned int option)
{
    Zerg_App_Frame *rsp_msg = reinterpret_cast<Zerg_App_Frame *>(trans_send_buffer_);
    rsp_msg->init_framehead(Zerg_App_Frame::MAX_LEN_OF_APPFRAME, option, cmd);

    rsp_msg->frame_uin_ = qquin;
    rsp_msg->transaction_id_ = trans_id;
    rsp_msg->recv_service_ = rcvsvc;
    rsp_msg->proxy_service_ = proxysvc;
    rsp_msg->send_service_ = sndsvc;

    //��д�Լ�transaction_id_,��ʵ���Լ�������ID,������������ҵ��Լ�
    rsp_msg->backfill_trans_id_ = backfill_trans_id;
    rsp_msg->app_id_ = app_id;

    //�������͵�MSG Block
    int ret = rsp_msg->appdata_encode(Zerg_App_Frame::MAX_LEN_OF_APPFRAME_DATA, info);

    if (ret != SOAR_RET::SOAR_RET_SUCC )
    {
        return SOAR_RET::ERROR_APPFRAME_BUFFER_SHORT;
    }

    //�������������ռ����ѭ��
    ret = mgr_postframe_to_msgqueue(rsp_msg);
    DEBUGDUMP_FRAME_HEAD(rsp_msg, "TO MESSAGE QUEUE FRAME", RS_DEBUG);

    if (ret != SOAR_RET::SOAR_RET_SUCC)
    {
        ZLOG_ERROR("[framework] mgr_postframe_to_msgqueue but fail.Send queue is full or task process too slow to process request.");
        return ret;
    }

    return SOAR_RET::SOAR_RET_SUCC;
}

//Postһ��FRAME���ݵ���Ϣ����
template< class T>
int Transaction_Manager::mgr_postframe_to_msgqueue(
    unsigned int cmd,
    unsigned int qquin,
    const T &info,
    unsigned int option)
{
    SERVICES_ID rcvsvc = self_services_id_;
    SERVICES_ID proxysvc(0, 0);
    SERVICES_ID sndsvc = self_services_id_;
    return mgr_postframe_to_msgqueue(
               cmd,
               qquin,
               0,
               0,
               rcvsvc,
               proxysvc,
               sndsvc,
               info,
               0,
               option);
}

#endif //SOARING_LIB_TRANSACTION_MANAGER_H_

