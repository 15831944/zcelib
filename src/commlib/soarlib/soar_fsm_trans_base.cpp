/******************************************************************************************
Copyright           : 2000-2004, Fullsail Technology (Shenzhen) Company Limited.
FileName            : soar_transactbase.cpp
Author              : Sail(ZENGXING)//Author name here
Version             :
Date Of Creation    : 2006��3��29��
Description         :

Others              : Ϊ����TRANSACTION�Զ�ע��,�Զ�����,��TRANSACTION������MANAGER��ָ��
                      �����Ǿ�̬��,����뷨��ʵ�����˲�����Ϊ.�ò���?
Function List       :
    1.  ......
Modification History:
    1.Date  : 2010��3��22��
      Author  :Sailzeng
      Modification  :�����Ҳд��4����,�ܼ��4�꣬�����ף����ҽ�����Google�뿪�й��ĵ�3�죬���Σ�������
******************************************************************************************/

#include "soar_predefine.h"

#include "soar_zerg_frame.h"
#include "soar_zerg_svc_info.h"
#include "soar_stat_monitor.h"
#include "soar_stat_define.h"
#include "soar_fsm_trans_base.h"
#include "soar_fsm_trans_mgr.h"

//TIME ID
const int      Transaction_Base::TRANSACTION_TIME_ID[] = {1, 2};

//���캯��
Transaction_Base::Transaction_Base(ZCE_Timer_Queue *timer_queue,
                                   Transaction_Manager *pmngr,
                                   bool trans_locker)
    : ZCE_Timer_Handler(timer_queue)
    , trans_manager_(pmngr)
    , transaction_id_(0)
    , trans_locker_(trans_locker)
    , req_qq_uin_(0)
    , trans_command_(0)
    , trans_run_state_(INIT_RUN_STATE)
    , trans_phase_(INVALID_PHASE_ID)
    , req_snd_service_(SERVICES_ID::INVALID_SERVICES_TYPE, SERVICES_ID::INVALID_SERVICES_ID)
    , req_rcv_service_(SERVICES_ID::INVALID_SERVICES_TYPE, SERVICES_ID::INVALID_SERVICES_ID)
    , req_proxy_service_(SERVICES_ID::INVALID_SERVICES_TYPE, SERVICES_ID::INVALID_SERVICES_ID)
    , req_trans_id_(0)
    , req_session_id_(0)
    , req_game_app_id_(0)
    , req_ip_address_(0)
    , req_frame_option_(0)
    , tmp_recv_frame_(NULL)
    , trans_timeout_id_(-1)
    , active_auto_stop_(true)
    , trans_touchtimer_id_(-1)
    , wait_cmd_(CMD_INVALID_CMD)
    , trans_create_time_(0)
    , trace_log_pri_(RS_DEBUG)
    , process_errno_(0)
{
}

//��������
Transaction_Base::~Transaction_Base()
{
}

//�����ڴ�����
void Transaction_Base::re_init()
{
    transaction_id_ = 0;
    req_qq_uin_ = 0;
    trans_command_ = 0;
    trans_run_state_ = INIT_RUN_STATE;
    trans_phase_ = INVALID_PHASE_ID;
    req_snd_service_.set_svcid(SERVICES_ID::INVALID_SERVICES_TYPE, SERVICES_ID::INVALID_SERVICES_ID);
    req_rcv_service_.set_svcid(SERVICES_ID::INVALID_SERVICES_TYPE, SERVICES_ID::INVALID_SERVICES_ID),
                                   req_proxy_service_.set_svcid(SERVICES_ID::INVALID_SERVICES_TYPE, SERVICES_ID::INVALID_SERVICES_ID);
    req_trans_id_ = 0;
    req_session_id_ = 0;
    req_game_app_id_ = 0;
    req_ip_address_ = 0;
    req_frame_option_ = 0;
    tmp_recv_frame_ = NULL;
    trans_timeout_id_ = -1;
    active_auto_stop_ = true;
    trans_touchtimer_id_ = -1;
    wait_cmd_ = CMD_INVALID_CMD;
    trans_create_time_ = 0;
    trace_log_pri_ = RS_DEBUG;
    process_errno_ = 0;
}

//���պ�Ĵ���������Դ���ͷţ��ȵȣ�������֤�����������������ã���������������
void Transaction_Base::finish()
{

}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2006��3��29��
Function        : Transaction_Base::initialize_trans
Return          : int
Parameter List  :
  Param1: Zerg_App_Frame* proc_frame :��ʼ�����յ�FRAME����,
  Param2: unsigned int transid :����ID,
Description     : ��ʼ��,��������ڼ̳��������������,
Calls           :
Called By       : ����Frame��ʼ���õ��Է����͵���Ϣ
Other           : �������غ��������Ҳ������������,ע���ʱ������Ƿ����������,����֯���������
Modify Record   :
******************************************************************************************/
int Transaction_Base::initialize_trans(Zerg_App_Frame *proc_frame, unsigned int transid)
{
    transaction_id_ = transid;

    trans_command_ = proc_frame->frame_command_;

    //�õ����ͷ���������Ϣ,��δ���ֻ�������ʱ������.
    req_snd_service_ = proc_frame->send_service_;
    req_rcv_service_ = proc_frame->recv_service_;
    req_proxy_service_ = proc_frame->proxy_service_;

    req_trans_id_ = proc_frame->transaction_id_;
    req_session_id_ = proc_frame->backfill_trans_id_;
    req_game_app_id_ =  proc_frame->app_id_;

    req_qq_uin_ = proc_frame->frame_uid_;

    req_ip_address_ = proc_frame->send_ip_address_;
    req_frame_option_ = proc_frame->frame_option_;

    tmp_recv_frame_ = proc_frame;

    //����м��ѡ������־���𣬱�֤������־�õ����
    if (proc_frame->frame_option_ & Zerg_App_Frame::DESC_MONITOR_TRACK)
    {
        trace_log_pri_ = RS_INFO;
    }

    //������ʼ�������¼�
    int ret = process_trans_event();

    if (ret != 0)
    {
        return ret;
    }

    //�������ʱ�Ľ�������
    tmp_recv_frame_ = NULL;

    return 0;
}

//��ʱ����������Ĭ��ʵ�֣�������ڳ�ʱʱҪ�������ݣ����ҪҪ�����������
//Ĭ������Ĵ������˳�
Transaction_Base::TRANSACTION_PROCESS Transaction_Base::on_timeout()
{
    return EXIT_PROCESS_SUCC;
}

//�쳣����������Ĭ��ʵ�֣�������ڶ�ʱ����Ҫ�������ݣ����ҪҪ�����������
//Ĭ������Ĵ������˳�
Transaction_Base::TRANSACTION_PROCESS Transaction_Base::on_exception()
{
    return EXIT_PROCESS_SUCC;
}

//
int Transaction_Base::on_exit()
{
    return 0;
}

int Transaction_Base::receive_trans_msg(Zerg_App_Frame *proc_frame)
{

    //��������ָ��,ͬʱע���¼�
    tmp_recv_frame_ = proc_frame;

    int ret = process_trans_event();

    if (ret != 0)
    {
        return ret;
    }

    //�������ʱ�Ľ�������
    tmp_recv_frame_ = NULL;

    return 0;
}

/******************************************************************************************
Author          : Sail(ZENGXING)  Date Of Creation: 2009��7��11��
Function        : Transaction_Base::timer_timeout
Return          : int
Parameter List  :
  Param1: const ZCE_Time_Value& // time ʱ��
  Param2: const void* arg             ����
Description     :
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int Transaction_Base::timer_timeout(const ZCE_Time_Value & /*time*/, const void *arg)
{
    int ret = 0;
#if defined DEBUG || defined _DEBUG
    ZCE_LOGMSG(trace_log_pri_, "[framework] %s::Time Out touch off,Transprocess:%u,Transphase:%d.", typeid(*this).name(),
               transaction_id_,
               trans_phase_);
    output_trans_info("[Timeout]:");
#endif //DEBUG
    const int timeid = *(static_cast<const int *>(arg));

    //��ʱ����
    if (TRANSACTION_TIME_ID[0] == timeid)
    {
        trans_run_state_ = TIMEOUT_RUN_STATE;
    }

    //������������
    ret = process_trans_event();

    //���������Ѿ��������߷���ʲô���󣬵���handle_close
    if (ret != 0)
    {
        //ԭ����return -1 �ײ�����handle_close�����Ƿ����������õķ�ʽ������ָ��ȡ����ʱ��,
        //ֱ�ӵ���handle_close��׷������
        handle_close();
    }

    return 0;
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2006��11��11��
Function        : Transaction_Base::process_trans_event
Return          : int
Parameter List  : NULL
Description     : ���������¼�
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int Transaction_Base::process_trans_event()
{
    int ret = 0;
    TRANSACTION_PROCESS txprocess = EXIT_PROCESS_SUCC;

    //��ʼ��
    if (trans_run_state_ == INIT_RUN_STATE )
    {
        ZLOG_DEBUG("%s::on_init start ,requst trans id:%u .",
                   typeid(*this).name(),
                   req_trans_id_
                  );

        txprocess = on_init();

        ZLOG_DEBUG("%s::on_init end,new transaction id:%u,requst trans id:%u,trans process:%u,new trans phase:%d.",
                   typeid(*this).name(),
                   transaction_id_,
                   req_trans_id_,
                   txprocess,
                   trans_phase_);

        //������������״̬
        if (txprocess != EXIT_PROCESS_SUCC && txprocess != EXIT_PROCESS_FAIL)
        {
            trans_run_state_ = RIGHT_RUN_STATE;
        }
    }
    //�������е�״̬,
    else if (trans_run_state_ == RIGHT_RUN_STATE )
    {
        //ȡ��ԭ�еĳ�ʱ��ʱ��,
        if (trans_timeout_id_ != -1 && true == active_auto_stop_ )
        {
            timer_queue()->cancel_timer(trans_timeout_id_);
            trans_timeout_id_ = -1;
        }

        //��־���
        ZLOG_DEBUG("%s::on_active start,transaction id:%u,requst trans id:%u,trans phase:%d.",
                   typeid(*this).name(),
                   transaction_id_,
                   req_trans_id_,
                   trans_phase_);

        txprocess = on_active();
        ZLOG_DEBUG("%s::on_active end,transaction id:%u,requst trans id:%u,trans process:%u,new trans phase:%d,.",
                   typeid(*this).name(),
                   transaction_id_,
                   req_trans_id_,
                   txprocess,
                   trans_phase_);

    }
    //��ʱ��״̬
    else if (trans_run_state_ == TIMEOUT_RUN_STATE)
    {
        //ȡ��ԭ�еĳ�ʱ��ʱ��,
        if (trans_timeout_id_ != -1 && true == active_auto_stop_ )
        {
            timer_queue()->cancel_timer(trans_timeout_id_);
            trans_timeout_id_ = -1;
        }

        ZLOG_ERROR("%s::on_timeout start,transaction id:%u,requst trans id:%u,trans phase:%d.",
                   typeid(*this).name(),
                   transaction_id_,
                   req_trans_id_,
                   trans_phase_);
        txprocess = on_timeout();
        ZLOG_DEBUG("%s::on_timeout end,transaction id:%u,requst trans id:%u,trans process:%u,new trans phase:%d.",
                   typeid(*this).name(),
                   transaction_id_,
                   req_trans_id_,
                   txprocess,
                   trans_phase_);

        // ������ʱ��ͳ������
        Comm_Stat_Monitor::instance()->increase_once(COMM_STAT_TRANS_PROC_TIMEOUT,
                                                     req_game_app_id_,
                                                     trans_command_);

        // ���û���˳�, ���л���������״̬
        if (txprocess != EXIT_PROCESS_SUCC && txprocess != EXIT_PROCESS_FAIL)
        {
            trans_run_state_ = RIGHT_RUN_STATE;
        }

    }
    //�쳣״̬
    else if ( trans_run_state_ == EXCEPTION_RUN_STATE )
    {
        //ȡ��ԭ�еĳ�ʱ��ʱ��,
        if (trans_timeout_id_ != -1 && true == active_auto_stop_ )
        {
            timer_queue()->cancel_timer(trans_timeout_id_);
            trans_timeout_id_ = -1;
        }

        ZLOG_ERROR("%s::on_exception start,transaction id:%u,requst trans id:%u,trans phase:%d.", typeid(*this).name(),
                   transaction_id_,
                   req_trans_id_,
                   trans_phase_);
        txprocess = on_exception();
        ZLOG_ERROR("%s::on_exception end,transaction id:%u,requst trans id:%u,trans process:%u,new trans phase:%d.",
                   typeid(*this).name(),
                   transaction_id_,
                   req_trans_id_,
                   txprocess,
                   trans_phase_);
    }
    else
    {
        ZCE_ASSERT(false);
    }

    switch (txprocess)
    {
        case WAIT_PROCESS:
        {
            // transδ�����꣬�����ȴ�
            break;
        }

        case NEXT_PROCESS:
        {
            //����൱�ڵݹ����
            ret = process_trans_event();

            if (ret != 0)
            {
                return ret;
            }

            break;
        }

        case EXIT_PROCESS_SUCC:
        {
            // �ɹ��˳����޸ļ������
            Comm_Stat_Monitor::instance()->increase_once(COMM_STAT_TRANS_PROC_SUCC,
                                                         req_game_app_id_,
                                                         trans_command_);
            return SOAR_RET::ERROR_TRANS_HAS_FINISHED;
        }

        case EXIT_PROCESS_FAIL:
        {
            // ʧ���˳����޸ļ������
            Comm_Stat_Monitor::instance()->increase_once(COMM_STAT_TRANS_PROC_FAIL,
                                                         req_game_app_id_,
                                                         trans_command_);
            Comm_Stat_Monitor::instance()->increase_once(COMM_STAT_TRANS_PROC_ERRNO,
                                                         req_game_app_id_,
                                                         process_errno_);
            return SOAR_RET::ERROR_TRANS_HAS_FINISHED;
        }

        default:
        {
            // ��Ӧ�õ���������
            ZCE_ASSERT(0);
            return SOAR_RET::ERROR_TRANS_HAS_FINISHED;
        }
    }

    return 0;
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2006��4��22��
Function        : Transaction_Base::handle_close
Return          : int
Parameter List  :
Description     :
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int Transaction_Base::handle_close ()
{
    //�Ҳ�û��ע��READ_MASK�ĵ�ʱ�䣬Ӧ�ò���ȡ��

    if (trans_timeout_id_ != -1)
    {
        //�ر����к����Event Handler��صĶ�ʱ��
        timer_queue()->cancel_timer(trans_timeout_id_);
        trans_timeout_id_ = -1;
    }

    if (trans_touchtimer_id_ != -1)
    {
        timer_queue()->cancel_timer(trans_touchtimer_id_);
        trans_touchtimer_id_ = -1;
    }

    //ע�����Լ��ڹ�������ע��,����鷵��ֵ��������������־��¼
    trans_manager_->unregiester_trans_id(transaction_id_,
                                         trans_command_,
                                         trans_run_state_,
                                         trans_create_time_);

    return 0;
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2006��11��11��
Function        : Transaction_Base::set_timeout_timer
Return          : int
Parameter List  :
  Param1: int sec
  Param2: int suseconds
Description     : ���ó�ʱ��ʱ��
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int Transaction_Base::set_timeout_timer(int sec, int usec, bool active_auto_stop)
{
    //ȡ��ԭ���Ķ�ʱ��
    if (trans_timeout_id_ != -1)
    {
        timer_queue()->cancel_timer(trans_timeout_id_);
        trans_timeout_id_ = -1;
    }

    //��ʱ��.ʹ�ó�ʱID
    ZCE_Time_Value delay(sec, usec);
    trans_timeout_id_ = timer_queue()->schedule_timer (this,
                                                       &TRANSACTION_TIME_ID[0],
                                                       delay);

    active_auto_stop_ = active_auto_stop;

    return 0;
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2006��11��11��
Function        : Transaction_Base::set_timetouch_timer
Return          : int
Parameter List  :
  Param1: int sec
  Param2: int suseconds
Description     : ����ʱ�䴥���Ķ�ʱ��
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int Transaction_Base::set_timetouch_timer(int sec, int usec)
{
    //ȡ��ԭ���Ķ�ʱ��
    if (trans_touchtimer_id_ != -1)
    {
        timer_queue()->cancel_timer(trans_touchtimer_id_);
        trans_touchtimer_id_ = -1;
    }

    //��ʱ��.ʹ�ó�ʱID
    ZCE_Time_Value delay(sec, usec);
    trans_touchtimer_id_ = timer_queue()->schedule_timer (this, &TRANSACTION_TIME_ID[1], delay);

    return 0;
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2006��11��11��
Function        : Transaction_Base::close_request_service
Return          : int
Parameter List  : NULL
Description     : �ر�����ĵ�Service������
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int Transaction_Base::close_request_service() const
{

    ZLOG_INFO("[framework] close_request_service() at trans_command_=%u,trans_phase_=%d,qq_uin_=%u.",
              trans_command_,
              trans_phase_,
              req_qq_uin_);

    return trans_manager_->mgr_sendmsghead_to_service(INNER_RSP_CLOSE_SOCKET,
                                                      req_qq_uin_,
                                                      req_rcv_service_,
                                                      req_proxy_service_);
}

//ȡ��������ʱ��
void Transaction_Base::cancel_touch_timer()
{
    if (trans_touchtimer_id_ != -1)
    {
        timer_queue()->cancel_timer(trans_touchtimer_id_);
        trans_touchtimer_id_ = -1;
    }
}
//ȡ����ʱ��ʱ��
void Transaction_Base::cancel_timeout_timer()
{
    if (trans_timeout_id_ != -1)
    {
        timer_queue()->cancel_timer(trans_timeout_id_);
        trans_timeout_id_ = -1;
    }
}

//�����ܵ���FRAME�����ݺ�����
int Transaction_Base::check_receive_frame(const Zerg_App_Frame *recv_frame)
{
    //
    if (wait_cmd_ != CMD_INVALID_CMD && recv_frame->frame_command_ != wait_cmd_)
    {
        ZLOG_ERROR("[framework] check_receive_frame error,Transaction need Cmd error!Wait command[%u],Recieve command[%u] Transaction ID:[%u].",
                   wait_cmd_,
                   recv_frame->frame_command_,
                   recv_frame->transaction_id_
                  );
        return SOAR_RET::ERROR_TRANSACTION_NEED_CMD_ERROR;
    }

    //����Ĭ�Ϻ���
    set_wait_cmd();
    //
    return 0;
}

//�Ե�ǰ�û���һ����ID���м���
int Transaction_Base::lock_qquin_key(unsigned int one_key)
{
    return trans_manager_->lock_qquin_trnas_cmd(req_qq_uin_, one_key, trans_command_);
}
//�Ե�ǰ�û���һ����ID���н���
void Transaction_Base::unlock_qquin_key(unsigned int one_key)
{
    return trans_manager_->unlock_qquin_trans_cmd(req_qq_uin_, one_key);
}
//�Ե�ǰ�û��ģ���ǰ���������ֽ��м���
int Transaction_Base::lock_qquin_cmd()
{
    return trans_manager_->lock_qquin_trnas_cmd(req_qq_uin_, trans_command_, trans_command_);
}
//�Ե�ǰ�û��ģ���ǰ���������ֽ��н���
void Transaction_Base::unlock_qquin_cmd()
{
    return trans_manager_->unlock_qquin_trans_cmd(req_qq_uin_, trans_command_);
}

//DUMP���е��������Ϣ
void Transaction_Base::dump_transa_info(std::ostringstream &strstream) const
{
    strstream << "ID:" << transaction_id_ << " Uin:" << req_qq_uin_ << " Cmd:" << trans_command_ << " State:" << std::left << trans_run_state_ << " Phase:" << std::dec << trans_phase_ << " ";
    strstream << "ReqSndSvr:" << req_snd_service_.services_type_ << "|" << req_snd_service_.services_id_ \
              << " ReqRcvSvr:" << req_rcv_service_.services_type_ << "|" << req_rcv_service_.services_id_ \
              << " Reqproxy:" << req_proxy_service_.services_type_  << "|" << req_proxy_service_.services_id_ << " ";
    strstream << "ReqtransID:" << req_trans_id_ << " TimeoutID:" << trans_timeout_id_ << " TouchID:" << trans_touchtimer_id_ << " ";
    return;
}

//DUMP��������������Ϣ
void Transaction_Base::output_trans_info(const char *outstr ) const
{
    std::ostringstream strstream;
    dump_transa_info(strstream);
    ZLOG_DEBUG("[framework] %s:%s" , outstr, strstream.str().c_str());
}

int Transaction_Base::request_send_buf_to_peer(unsigned int cmd,
                                               unsigned int qquin,
                                               const SERVICES_ID &rcv_svc,
                                               const unsigned char *buf,
                                               unsigned int buf_len,
                                               unsigned int app_id,
                                               unsigned int option)
{
    SERVICES_ID proxy_svc(0, 0);
    return sendbuf_to_service(cmd,
                              qquin,
                              this->transaction_id_,
                              0,
                              rcv_svc,
                              proxy_svc,
                              *(trans_manager_->self_svc_info()),
                              buf,
                              buf_len,
                              app_id,
                              option);
}

int Transaction_Base::request_send_buf_to_proxy(unsigned int cmd,
                                                unsigned int qquin,
                                                const SERVICES_ID &proxy_svc,
                                                const SERVICES_ID &recv_svc,
                                                const unsigned char *buf,
                                                unsigned int buf_len,
                                                unsigned int app_id,
                                                unsigned int option)
{

    return sendbuf_to_service(cmd,
                              qquin,
                              this->transaction_id_,
                              0,
                              recv_svc,
                              proxy_svc,
                              *(trans_manager_->self_svc_info()),
                              buf,
                              buf_len,
                              app_id,
                              option);
}

int Transaction_Base::sendbuf_to_service(unsigned int cmd,
                                         unsigned int qquin,
                                         unsigned int transaction_id,
                                         unsigned int backfill_trans_id,
                                         const SERVICES_ID &rcv_svc,
                                         const SERVICES_ID &proxy_svc,
                                         const SERVICES_ID &snd_svc,
                                         const unsigned char *buf,
                                         size_t buf_len,
                                         unsigned int app_id,
                                         unsigned int option )
{
    // ������������Ҫ��Ҫ��أ�����Ĵ�����м��
    if (req_frame_option_ & Zerg_App_Frame::DESC_MONITOR_TRACK)
    {
        option |= Zerg_App_Frame::DESC_MONITOR_TRACK;
    }

    //���ù������ķ��ͺ���
    return trans_manager_->mgr_sendbuf_to_service(cmd,
                                                  qquin,
                                                  transaction_id,
                                                  backfill_trans_id,
                                                  rcv_svc,
                                                  proxy_svc,
                                                  snd_svc,
                                                  buf,
                                                  buf_len,
                                                  app_id,
                                                  option);
}

int Transaction_Base::response_buf_sendback(unsigned int cmd,
                                            unsigned int uin,
                                            const unsigned char *buf,
                                            unsigned int buf_len,
                                            unsigned int option )
{
    // �ذ�����Ҫ�Ӽ�ر�ǰɣ�

    //����UDP���صĴ��벿��
    if (req_frame_option_ & Zerg_App_Frame::DESC_UDP_FRAME)
    {
        option |= Zerg_App_Frame::DESC_UDP_FRAME;
    }

    //
    return sendbuf_to_service(cmd,
                              uin,
                              this->transaction_id_,
                              this->req_trans_id_,
                              this->req_snd_service_,
                              this->req_proxy_service_,
                              this->req_rcv_service_,
                              buf,
                              buf_len,
                              this->req_game_app_id_,
                              option);
}

