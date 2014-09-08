
#include "zerg_predefine.h"
#include "zerg_tcp_ctrl_handler.h"
#include "zerg_comm_manager.h"
#include "zerg_app_timer.h"
#include "zerg_stat_define.h"

/****************************************************************************************************
class  TCP_Svc_Handler
****************************************************************************************************/
//CONNECT��ȴ����ݵĳ�ʱʱ��
unsigned int   TCP_Svc_Handler::accepted_timeout_ = 3;
//�������ݵĳ�ʱʱ��
unsigned int   TCP_Svc_Handler::receive_timeout_ = 5;

//TIME ID
const int      TCP_Svc_Handler::TCPCTRL_TIME_ID[] = { 1, 2 };


//
Active_SvcHandle_Set TCP_Svc_Handler::svr_peer_info_set_;



//���û��ʹ��Singlton�ķ�ʽ��ԭ�����£�
//1.�ٶȵ�һ��˼��
//2.ԭ��û����instance

//
ZBuffer_Storage  *TCP_Svc_Handler::zbuffer_storage_ = NULL;
//ͨ�Ź�����
Zerg_Comm_Manager *TCP_Svc_Handler::zerg_comm_mgr_ = NULL;
//
Comm_Stat_Monitor *TCP_Svc_Handler::server_status_ = NULL;

//�Լ��Ƿ��Ǵ���
bool           TCP_Svc_Handler::if_proxy_ = false;

//
size_t         TCP_Svc_Handler::num_accept_peer_ = 0;
//
size_t         TCP_Svc_Handler::num_connect_peer_ = 0;


//�����Խ��ܵĽ�������
size_t         TCP_Svc_Handler::max_accept_svr_ = 0;
//�����Խ��ܵ���������
size_t         TCP_Svc_Handler::max_connect_svr_ = 0;

//�����澯��ֵ
size_t         TCP_Svc_Handler::accpet_threshold_warn_ = 0;
//�Ѿ������澯��ֵ�Ĵ���
size_t         TCP_Svc_Handler::threshold_warn_number_ = 0;

//
Zerg_Auto_Connector TCP_Svc_Handler::zerg_auto_connect_;


//svc handler�ĳ���
TCP_Svc_Handler::POOL_OF_TCP_HANDLER TCP_Svc_Handler::pool_of_acpthdl_;
//svc handler�ĳ���
TCP_Svc_Handler::POOL_OF_TCP_HANDLER TCP_Svc_Handler::pool_of_cnthdl_;

//���ͻ����������frame���������ö�ȡ
size_t         TCP_Svc_Handler::snd_buf_size_ = 0;

//�������ӵķ��Ͷ��г���
size_t  TCP_Svc_Handler::connect_send_deque_size_ = 0;

unsigned int  TCP_Svc_Handler::handler_id_builder_ = 0;


//���캯��
TCP_Svc_Handler::TCP_Svc_Handler(TCP_Svc_Handler::HANDLER_MODE hdl_mode) :
    ZCE_Event_Handler(ZCE_Reactor::instance()),
    ZCE_Timer_Handler(ZCE_Timer_Queue::instance()),
    handler_mode_(hdl_mode),
    my_svc_id_(0, 0),
    peer_svr_id_(0, 0),
    rcv_buffer_(NULL),
    recieve_counter_(0),
    send_counter_(0),
    recieve_bytes_(0),
    send_bytes_(0),
    peer_status_(PEER_STATUS_NOACTIVE),
    timeout_time_id_(-1),
    receive_times_(0),
    if_force_close_(false),
    start_live_time_(0)
{

    if (HANDLER_MODE_CONNECT == hdl_mode)
    {
        snd_buffer_deque_.initialize(connect_send_deque_size_);
    }
    else if (HANDLER_MODE_ACCEPTED == hdl_mode)
    {
        snd_buffer_deque_.initialize(snd_buf_size_);
    }
    else
    {
        ZCE_ASSERT(false);
    }
}


//����Accept�Ķ˿ڵĴ���Event Handle��ʼ������.
void TCP_Svc_Handler::init_tcpsvr_handler(const SERVICES_ID &my_svcinfo,
                                          const ZCE_Socket_Stream &sockstream,
                                          const ZCE_Sockaddr_In     &socketaddr)
{
    handler_mode_ = HANDLER_MODE_ACCEPTED;
    my_svc_id_ = my_svcinfo;
    peer_svr_id_.set_svcid(0, 0);
    rcv_buffer_ = NULL;
    recieve_counter_ = 0;
    send_counter_ = 0;
    recieve_bytes_ = 0;
    send_bytes_ = 0;
    socket_peer_ = sockstream;
    peer_address_ = socketaddr;
    peer_status_ = PEER_STATUS_JUST_ACCEPT;
    timeout_time_id_ = -1;
    receive_times_ = 0;
    if_force_close_ = false;
    start_live_time_ = 0;


    ////����Socket ΪO_NONBLOCK
    int ret = socket_peer_.sock_enable(O_NONBLOCK);


    ZLOG_INFO("[zergsvr] Accept peer socket IP Address:[%s|%u] Success. Set O_NONBLOCK ret =%d.",
              peer_address_.get_host_addr(),
              peer_address_.get_port_number(),
              ret);

    //�����Ӽ�������������¹رյ�ʱ����--�ˡ�
    ++num_accept_peer_;

    //��������������,REACTOR�Լ���ʵ�п���,��������Ҫ����ACCEPT��Ҫ����CONNECT.
    //����ֻ��,ͷXX��, �������������ʵ������������,�������ᷢ��,
    if (num_accept_peer_ <= max_accept_svr_)
    {
        //��������Ƿ��и澯��ֵ
        if (num_accept_peer_ > accpet_threshold_warn_)
        {
            const size_t WARNNING_TIMES = 5;

            if ((threshold_warn_number_ % (WARNNING_TIMES)) == 0)
            {
                ZLOG_ALERT("[zergsvr] Great than threshold_warn_number_ Reject! num_accept_peer_:%u,threshold_warn_number_:%u,accpet_threshold_warn_:%u,max_accept_svr_:%u .",
                           num_accept_peer_,
                           threshold_warn_number_,
                           accpet_threshold_warn_,
                           max_accept_svr_);
            }

            //��¼�澯�ܴ���������
            ++threshold_warn_number_;
        }

        //ע���д�¼�
        ret = reactor()->register_handler(this,
                                          ZCE_Event_Handler::READ_MASK | ZCE_Event_Handler::WRITE_MASK);

        //
        if (ret != 0)
        {
            ZLOG_ERROR("[zergsvr] Register accept [%s|%u] handler fail! ret =%u  errno=%u|%s .",
                       peer_address_.get_host_addr(),
                       peer_address_.get_port_number(),
                       ret,
                       ZCE_LIB::last_error(),
                       strerror(ZCE_LIB::last_error()));

            handle_close();
            return;
        }

        reactor()->cancel_wakeup(this, ZCE_Event_Handler::WRITE_MASK);

        //ͳ��
        server_status_->set_by_statid(ZERG_ACCEPT_PEER_NUMBER, 0, 0, static_cast<int>(num_accept_peer_));
        server_status_->increase_by_statid(ZERG_ACCEPT_PEER_COUNTER, 0, 0, 1);
    }
    //Ҫ���Լ��һ��,
    else
    {
        ZLOG_ERROR("[zergsvr] Peer [%s|%u] great than max_accept_svr_ Reject! num_accept_peer_:%u,max_accept_svr_:%u .",
                   peer_address_.get_host_addr(),
                   peer_address_.get_port_number(),
                   num_accept_peer_,
                   max_accept_svr_);
        handle_close();
        return;
    }

    //��������˳�ʱ����,N������յ�һ����

    ZCE_Time_Value delay(0, 0);
    ZCE_Time_Value interval(0, 0);

    //
    (accepted_timeout_ > 0) ? delay.sec(accepted_timeout_) : delay.sec(STAT_TIMER_INTERVAL_SEC);
    (receive_timeout_ > 0) ? interval.sec(receive_timeout_) : interval.sec(STAT_TIMER_INTERVAL_SEC);

    timeout_time_id_ = timer_queue()->schedule_timer(this, &TCPCTRL_TIME_ID[0], delay, interval);


    //����
    int keep_alive = 1;
    socklen_t opvallen = sizeof(int);
    socket_peer_.setsockopt(SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<void *>(&keep_alive), opvallen);

    //����ط����¹�һ��BUG�����ǿͻ����ղ������ݣ��������ѡ����ʺ������������ʹ�á��Ǻǡ�
    //���ѡ���Ǳ�֤�����رյ�ʱ�򣬲��õȴ������ݷ��͸��Է�,
    //��ζ����������Σ���Ҳ�ر������Σ��һ��ǹ�����һЩϸ����û��Ū���ס�
    //struct linger sock_linger = {1, 30};
    //sock_linger.l_onoff = 1;
    //sock_linger.l_linger = 30;
    //opvallen = sizeof(linger);
    //socket_peer_.set_option(SOL_SOCKET,SO_LINGER,reinterpret_cast<void *>(&sock_linger),opvallen);

#if defined _DEBUG || defined DEBUG
    socklen_t sndbuflen, rcvbuflen;
    opvallen = sizeof(socklen_t);
    socket_peer_.getsockopt(SOL_SOCKET, SO_RCVBUF, reinterpret_cast<void *>(&rcvbuflen), &opvallen);
    socket_peer_.getsockopt(SOL_SOCKET, SO_SNDBUF, reinterpret_cast<void *>(&sndbuflen), &opvallen);
    ZLOG_DEBUG("[zergsvr] Accept peer SO_RCVBUF:%u SO_SNDBUF %u.", rcvbuflen, sndbuflen);

#endif
}



//����CONNET���ӳ�ȥ��HANDLER����ӦEvent Handle�ĳ�ʼ��.
void TCP_Svc_Handler::init_tcpsvr_handler(const SERVICES_ID &my_svcinfo,
                                          const SERVICES_ID &peer_svrinfo,
                                          const ZCE_Socket_Stream &sockstream,
                                          const ZCE_Sockaddr_In     &socketaddr)
{
    handler_mode_ = HANDLER_MODE_CONNECT;
    my_svc_id_ = my_svcinfo;
    peer_svr_id_ = peer_svrinfo;
    rcv_buffer_ = NULL;
    recieve_counter_ = 0;
    send_counter_ = 0;
    recieve_bytes_ = 0;
    send_bytes_ = 0;
    socket_peer_ = sockstream;
    peer_address_ = socketaddr;
    peer_status_ = PEER_STATUS_NOACTIVE;
    timeout_time_id_ = -1;
    receive_times_ = 0;
    if_force_close_ = false;
    start_live_time_ = 0;

    //����Socket ΪACE_NONBLOCK
    int ret = socket_peer_.sock_enable(O_NONBLOCK);

    ZLOG_INFO("[zergsvr] Connect peer socket Services ID[%u|%u] IP Address:[%s|%u] Success. Set O_NONBLOCK ret =%d.",
              peer_svr_id_.services_type_,
              peer_svr_id_.services_id_,
              peer_address_.get_host_addr(),
              peer_address_.get_port_number(),
              ret);

    snd_buffer_deque_.initialize(connect_send_deque_size_);

    //ע�ᵽ
    ret = reactor()->register_handler(this, ZCE_Event_Handler::CONNECT_MASK);

    //�Ҽ���û�м���register_handlerʧ��,
    if (ret != 0)
    {
        ZLOG_ERROR("[zergsvr] Register services [%u|%u] IP[%s|%u]  connect handler fail! ret =%d  errno=%d|%s .",
                   peer_svr_id_.services_type_,
                   peer_svr_id_.services_id_,
                   peer_address_.get_host_addr(),
                   peer_address_.get_port_number(),
                   ret,
                   ZCE_LIB::last_error(),
                   strerror(ZCE_LIB::last_error()));
        handle_close();
        return;
    }

    //�������Ӵ����MAP
    ret = svr_peer_info_set_.add_services_peerinfo(peer_svr_id_, this);

    //�������ɱ�ǲ���Σ����һ��
    if (ret != 0)
    {
        handle_close();
        return;
    }

    ++num_connect_peer_;

    ZCE_Time_Value delay(STAT_TIMER_INTERVAL_SEC, 0);
    ZCE_Time_Value interval(STAT_TIMER_INTERVAL_SEC, 0);

    timeout_time_id_ = timer_queue()->schedule_timer(this, &TCPCTRL_TIME_ID[0], delay, interval);

    //ͳ��
    server_status_->set_by_statid(ZERG_CONNECT_PEER_NUMBER, 0, 0, num_connect_peer_);
    server_status_->increase_by_statid(ZERG_CONNECT_PEER_COUNTER, 0, 0, 1);

    //SO_RCVBUF��SO_SNDBUF������UNPv1�Ľ��⣬Ӧ����connect֮ǰ���ã���Ȼ�ҵĲ���֤���������������Ҳ���ԡ�

    int keep_alive = 1;
    socklen_t opvallen = sizeof(int);
    socket_peer_.setsockopt(SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<void *>(&keep_alive), opvallen);

    //Win32��û�����ѡ��
#ifndef ZCE_OS_WINDOWS
    //����DELAY�����������
    int NODELAY = 1;
    opvallen = sizeof(int);
    socket_peer_.setsockopt(SOL_TCP, TCP_NODELAY, reinterpret_cast<void *>(&NODELAY), opvallen);
#endif

#if defined _DEBUG || defined DEBUG
    socklen_t sndbuflen = 0, rcvbuflen = 0;
    opvallen = sizeof(socklen_t);
    socket_peer_.getsockopt(SOL_SOCKET, SO_RCVBUF, reinterpret_cast<void *>(&rcvbuflen), &opvallen);
    socket_peer_.getsockopt(SOL_SOCKET, SO_SNDBUF, reinterpret_cast<void *>(&sndbuflen), &opvallen);
    ZLOG_DEBUG("[zergsvr] Set Connect Peer SO_RCVBUF:%u SO_SNDBUF %u.", rcvbuflen, sndbuflen);
#endif

}

TCP_Svc_Handler::~TCP_Svc_Handler()
{
}


/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2006��3��22��
Function        : TCP_Svc_Handler::get_tcpctrl_conf
Return          : int
Parameter List  :
Param1: const char* cfgfilename �����ļ�����
Description     : �������ļ���ȡ������Ϣ
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int TCP_Svc_Handler::get_config(const Zerg_Server_Config *config)
{
    int ret = 0;

    //unsigned int tmp_uint = 0 ;
    //��CONNECT���յ����ݵ�ʱ��
    accepted_timeout_ = config->zerg_cfg_data_.accepted_timeout_;


    //RECEIVEһ�����ݵĳ�ʱʱ��,Ϊ0��ʾ������
    receive_timeout_ = config->zerg_cfg_data_.receive_timeout_;


    //�Ƿ���һ���������,����ķ�����Ϊ����ͨ��������һ��.
    if_proxy_ = config->zerg_cfg_data_.is_proxy_;


    //���������ҵķ���������
    max_accept_svr_ = config->zerg_cfg_data_.max_accept_svr_;


    //�����澯��ֵ
    accpet_threshold_warn_ = static_cast<size_t> (max_accept_svr_ * 0.8);
    ZLOG_INFO("[zergsvr] Max accept svr number :%u,accept warn threshold number:%u. ",
              max_accept_svr_,
              accpet_threshold_warn_);


    //���ͻ����������frame��
    snd_buf_size_ = config->zerg_cfg_data_.acpt_send_deque_size_;
    TESTCONFIG((ret == 0), "COMMCFG|ACCEPTSNDBUFSIZE key error.");

    //�������ӵķ��Ͷ��г���
    connect_send_deque_size_ = config->zerg_cfg_data_.cnnt_send_deque_size_;
    ZLOG_INFO("[zergsvr] conncet send deque size :%d", connect_send_deque_size_);

    //�õ����ӵ�SERVER������
    ret = zerg_auto_connect_.get_config(config);

    if (ret != 0)
    {
        return ret;
    }

    //
    return 0;
}



/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2006��3��22��
Function        : TCP_Svc_Handler::init_all_static_data
Return          : int
Parameter List  : NULL
Description     : �����ò�����ʼ��
Calls           :
Called By       :
Other           : һЩ�������������ȡ,�������Ĳ�����Ҫ����������
Modify Record   :
******************************************************************************************/
int TCP_Svc_Handler::init_all_static_data()
{
    //
    //int ret = 0;
    //
    zerg_comm_mgr_ = Zerg_Comm_Manager::instance();
    //�Լ��ķ��������,������,APPID
    //
    zbuffer_storage_ = ZBuffer_Storage::instance();

    //��������ͳ�Ʋ���ʵ��
    server_status_ = Comm_Stat_Monitor::instance();


    //���Ҫ�������������Զ����ӷ��������,����16��
    max_connect_svr_ = zerg_auto_connect_.numsvr_connect() + 16;

    ZLOG_INFO("[zergsvr] MaxAcceptSvr:%u MaxConnectSvr:%u.", max_accept_svr_, max_connect_svr_);

    //ΪCONNECT��HDLԤ�ȷ����ڴ棬��Ϊһ������
    ZLOG_INFO("[zergsvr] Connet Hanlder:size of TCP_Svc_Handler [%u],one connect handler have deqeue length [%u],number of connect handler [%u]."
              "About need  memory [%u] bytes.",
              sizeof(TCP_Svc_Handler),
              MAX_OF_CONNECT_PEER_SEND_DEQUE,
              max_connect_svr_,
              (max_connect_svr_ * (sizeof(TCP_Svc_Handler) + MAX_OF_CONNECT_PEER_SEND_DEQUE * sizeof(size_t)))
             );
    pool_of_cnthdl_.initialize(max_connect_svr_);

    for (size_t i = 0; i < max_connect_svr_; ++i)
    {
        TCP_Svc_Handler *p_handler = new TCP_Svc_Handler(HANDLER_MODE_CONNECT);
        pool_of_cnthdl_.push_back(p_handler);
    }

    //ΪACCEPT��HDLԤ�ȷ����ڴ棬��Ϊһ������
    ZLOG_INFO("[zergsvr] Accept Hanlder:size of TCP_Svc_Handler [%u],one accept handler have deqeue length [%u],number of accept handler [%u]."
              "About need  memory [%u] bytes.",
              sizeof(TCP_Svc_Handler),
              snd_buf_size_,
              max_accept_svr_,
              (max_accept_svr_ * (sizeof(TCP_Svc_Handler) + snd_buf_size_ * sizeof(size_t)))
             );
    pool_of_acpthdl_.initialize(max_accept_svr_);

    for (size_t i = 0; i < max_accept_svr_; ++i)
    {
        TCP_Svc_Handler *p_handler = new TCP_Svc_Handler(HANDLER_MODE_ACCEPTED);
        pool_of_acpthdl_.push_back(p_handler);
    }

    //Ҳ��������չһЩ�Ƚ�Ч�ʺá�
    svr_peer_info_set_.init_services_peerinfo(max_accept_svr_ + max_connect_svr_ + 1024);

    //�������е�SERVER
    size_t szsucc = 0, szfail = 0, szvalid = 0;
    zerg_auto_connect_.reconnect_allserver(szvalid, szsucc, szfail);

    return 0;
}

//ȡ�þ��
ZCE_HANDLE TCP_Svc_Handler::get_handle(void) const
{
    return (ZCE_HANDLE)socket_peer_.get_handle();
}

//���һ�����͵�handle
unsigned int TCP_Svc_Handler::get_handle_id()
{
#if defined ZCE_OS_WINDOWS

    ++handler_id_builder_;

    if (handler_id_builder_ == 0)
    {
        ++handler_id_builder_;
    }

    return  handler_id_builder_;

#elif defined ZCE_OS_LINUX
    return get_handle();
#endif
}

/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2005��11��27��
Function        : TCP_Svc_Handler::handle_input
Return          : int
Parameter List  :
Description     : ��ȡ,�������¼�����������
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int TCP_Svc_Handler::handle_input()
{
    //��ȡ����
    size_t szrecv;

    int ret = read_data_from_peer(szrecv);

    ZCE_LOGMSG_DBG(RS_DEBUG, "Read event ,svcinfo[%u|%u] IP[%s|%u], handle input event triggered. ret:%d,szrecv:%u.",
                   peer_svr_id_.services_type_,
                   peer_svr_id_.services_id_,
                   peer_address_.get_host_addr(),
                   peer_address_.get_port_number(),
                   ret,
                   szrecv);

    //����κδ��󶼹ر�,
    if (ret != 0)
    {
        return -1;
    }

    //�����ݷ�����յĹܵ�,������صĴ���Ӧ�ö���Ԥ����Ĵ���,����ܵ��Ĵ���Ӧ�ò�������
    ret = push_frame_to_comm_mgr();

    //����κδ��󶼹ر�,
    if (ret != 0)
    {
        return -1;
    }


    return 0;
}


/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2005��11��27��
Function        : TCP_Svc_Handler::handle_output
Return          : int
Parameter List  :
Param1: ZCE_HANDLE
Description     : ACE��ȡ,�������¼�����������
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int TCP_Svc_Handler::handle_output()
{

    //���NON BLOCK Connect�ɹ�,Ҳ�����handle_output
    if (PEER_STATUS_NOACTIVE == peer_status_)
    {
        //�������Ӻ������
        process_connect_register();

        return 0;
    }

    int ret = 0;
    ret = write_all_data_to_peer();

    if (0 != ret)
    {
        //
        //Ϊʲô�Ҳ����������,��return -1,��Ϊ��������ر�Socket,handle_input��������,������ظ�����
        //������жϵȴ���,������Լ�����.
        //����Ϊɶ�ָĳ���return -1,�ӿ촦��?���������ˡ�Ӧ��дע��ѽ��
        return -1;
    }

    return 0;
}



/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2005��12��20��
Function        : TCP_Svc_Handler::timer_timeout
Return          : int
Parameter List  :
Param1: const ZCE_Time_Value& * time  ʱ��,
Param2: const void* arg               Ψһ��ʾ����
Description     : ��ʱ����
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int TCP_Svc_Handler::timer_timeout(const ZCE_Time_Value &now_time, const void *arg)
{
    const int timeid = *(static_cast<const int *>(arg));

    //������N�룬���߽�����M��
    if (TCPCTRL_TIME_ID[0] == timeid)
    {
        //������ܵ����ݣ���ôʲôҲ����
        if (receive_times_ > 0)
        {
            receive_times_ = 0;
        }
        //���û���յ�����,��¥��ɱ
        else
        {
            //����Ǽ����Ķ˿ڣ���������Ӧ�ĳ�ʱ�ж�
            if (HANDLER_MODE_ACCEPTED == handler_mode_ &&
                ((0 == start_live_time_ && 0 < accepted_timeout_) || 
                (0 < start_live_time_ && 0 < receive_timeout_)))
            {

                ZLOG_ERROR("[zergsvr] Connect or receive expire event,peer services [%u|%u] IP[%s|%u] want to close handle. live time %lu. recieve times=%u.",
                           peer_svr_id_.services_type_,
                           peer_svr_id_.services_id_,
                           peer_address_.get_host_addr(),
                           peer_address_.get_port_number(),
                           now_time.sec() - start_live_time_,
                           receive_times_);

                //�����ֱ�ӵ���handle_close
                handle_close();
                return 0;
            }
        }

        //��һ��ʹ�õ���accepted_timeout_
        if (0 == start_live_time_)
        {
            start_live_time_ = now_time.sec();
        }

        //��ӡһ�¸����˿ڵ�������Ϣ
        ZLOG_DEBUG("[zergsvr] Connect or receive expire event,peer services [%u|%u] IP[%s|%u] live time %lu. recieve times=%u.",
                   peer_svr_id_.services_type_,
                   peer_svr_id_.services_id_,
                   peer_address_.get_host_addr(),
                   peer_address_.get_port_number(),
                   now_time.sec() - start_live_time_,
                   receive_times_);

        //����ͳ���������Ƶ��Ӱ����������,���Է��붨ʱ����,��Ȼ��о�����̫׼ȷ,������������
        server_status_->increase_by_statid(ZERG_RECV_SUCC_COUNTER, 0, 0, recieve_counter_);
        server_status_->increase_by_statid(ZERG_SEND_SUCC_COUNTER, 0, 0, send_counter_);
        server_status_->increase_by_statid(ZERG_SEND_BYTES_COUNTER, 0, 0, send_bytes_);
        server_status_->increase_by_statid(ZERG_RECV_BYTES_COUNTER, 0, 0, recieve_bytes_);

        recieve_counter_ = 0;
        recieve_bytes_ = 0;
        send_counter_ = 0;
        send_bytes_ = 0;
    }
    else if (TCPCTRL_TIME_ID[1] == timeid)
    {
    }

    return 0;
}

/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2005��12��20��
Function        : TCP_Svc_Handler::handle_close
Return          : int
Parameter List  :
Param1: ZCE_HANDLE prchandle
Param2: ACE_Reactor_Mask closemask
Description     : PEER Event Handler�رյĴ���
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int TCP_Svc_Handler::handle_close()
{
    ZLOG_DEBUG("[zergsvr] TCP_Svc_Handler::handle_close : %u|%u.", peer_svr_id_.services_type_, peer_svr_id_.services_id_);

    //��Ҫʹ��cancel_timer(this),�䷱��,������,��Ҫnew,������һ����֪��������

    //ȡ����Event Handler��صĶ�ʱ��
    if (-1 != timeout_time_id_)
    {
        timer_queue()->cancel_timer(timeout_time_id_);
        timeout_time_id_ = -1;
    }


    //ȡ��MASK,���׶�,�������handle_close,
    //�ڲ������remove_handler
    ZCE_Event_Handler::handle_close();

    //�رն˿�,
    socket_peer_.close();

    //�ͷŽ������ݻ�����
    if (rcv_buffer_)
    {
        zbuffer_storage_->free_byte_buffer(rcv_buffer_);
        rcv_buffer_ = NULL;
    }

    //���������ݻ�����
    size_t sz_of_deque = snd_buffer_deque_.size();

    for (size_t i = 0; i < sz_of_deque; i++)
    {
        //�����ʹ������,ͬʱ���л���
        process_send_error(snd_buffer_deque_[i], true);
        snd_buffer_deque_[i] = NULL;
    }

    snd_buffer_deque_.clear();

    //��������Ǽ���״̬���������������ӵķ���.
    if (peer_status_ == PEER_STATUS_ACTIVE || handler_mode_ == HANDLER_MODE_CONNECT)
    {
        //ע����Щ��Ϣ
        svr_peer_info_set_.del_services_peerInfo(peer_svr_id_);

        //����ǷǺ��ҵ�����ǿ�ƹرգ�����һ��֪ͨ��ҵ����̣�������֪ͨ
        if (false == if_force_close_)
        {
            //֪ͨ����ķ�����
#ifdef ZCE_OS_WINDOWS
#pragma warning ( disable : 4815)
#endif
            //�������ĳ���
            Zerg_App_Frame appframe;
#ifdef ZCE_OS_WINDOWS
#pragma warning ( default : 4815)
#endif
            appframe.init_framehead(Zerg_App_Frame::LEN_OF_APPFRAME_HEAD, 0, INNER_REG_SOCKET_CLOSED);
            appframe.send_service_ = peer_svr_id_;

            zerg_comm_mgr_->pushback_recvpipe(&appframe);
        }
    }

    //����ͳ���������Ƶ��Ӱ����������,���Է��������,��Ȼ��о�����̫׼ȷ,������������
    server_status_->increase_by_statid(ZERG_RECV_SUCC_COUNTER, 0, 0, recieve_counter_);
    server_status_->increase_by_statid(ZERG_SEND_SUCC_COUNTER, 0, 0, send_counter_);
    server_status_->increase_by_statid(ZERG_SEND_BYTES_COUNTER, 0, 0, send_bytes_);
    server_status_->increase_by_statid(ZERG_RECV_BYTES_COUNTER, 0, 0, recieve_bytes_);

    recieve_counter_ = 0;
    recieve_bytes_ = 0;
    send_counter_ = 0;
    send_bytes_ = 0;


    peer_status_ = PEER_STATUS_NOACTIVE;

    //���ݲ�ͬ�����ͼ���,

    //������������������,�����һ���µ�����Ҫ����ʱ������������
    if (handler_mode_ == HANDLER_MODE_CONNECT)
    {
        ZLOG_INFO("[zergsvr] Connect peer close, services[%u|%u] socket IP|Port :[%s|%u].",
                  peer_svr_id_.services_type_,
                  peer_svr_id_.services_id_,
                  peer_address_.get_host_addr(),
                  peer_address_.get_port_number()
                 );

        --num_connect_peer_;
        server_status_->set_by_statid(ZERG_CONNECT_PEER_NUMBER, 0, 0,
                                      num_connect_peer_);
        //��ָ��黹�������м�ȥ
        pool_of_cnthdl_.push_back(this);
    }
    else if (handler_mode_ == HANDLER_MODE_ACCEPTED)
    {
        ZLOG_INFO("[zergsvr] Accept peer close, services[%u|%u] socket IP|Port :[%s|%u].",
                  peer_svr_id_.services_type_,
                  peer_svr_id_.services_id_,
                  peer_address_.get_host_addr(),
                  peer_address_.get_port_number()
                 );

        --num_accept_peer_;
        server_status_->set_by_statid(ZERG_ACCEPT_PEER_NUMBER, 0, 0, num_accept_peer_);
        //��ָ��黹�������м�ȥ
        pool_of_acpthdl_.push_back(this);
    }

    return 0;
}


/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2005��11��23��
Function        : TCP_Svc_Handler::preprocess_recvframe
Return          : int
Parameter List  :
Param1: Zerg_App_Frame *proc_frame
Description     :
Calls           :
Called By       :
Other           : ͬʱ���֡ͷ���ݵĽ��빤��
Modify Record   :
******************************************************************************************/
int TCP_Svc_Handler::preprocess_recvframe(Zerg_App_Frame *proc_frame)
{
    int ret = 0;
    //Zerg_App_Frame *proc_frame = reinterpret_cast<Zerg_App_Frame *>( rcv_buffer_->buffer_data_);

    //�������,�������Ĺܵ��޷�ʶ��
    proc_frame->framehead_decode();
    DEBUGDUMP_FRAME_HEAD(proc_frame, "preprocess_recvframe After framehead_decode:", RS_DEBUG);

    //�����ڲ�ѡ��,���ⱻ�������ݻ�������������
    proc_frame->clear_inner_option();

    //������֡�Ƿ��Ƿ��͸����SVR,

    //����Ǵ���,���֡�Ĵ���������
    if (proc_frame->proxy_service_.services_type_ != SERVICES_ID::INVALID_SERVICES_TYPE && if_proxy_ == true)
    {

        if (my_svc_id_ != proc_frame->proxy_service_)
        {
            return SOAR_RET::ERR_ZERG_APPFRAME_ERROR;
        }
        //
        proc_frame->proxy_service_.services_id_ = my_svc_id_.services_id_;
    }
    //���֡�Ľ��ܲ���
    else
    {
        if (my_svc_id_ != proc_frame->recv_service_)
        {
            return SOAR_RET::ERR_ZERG_APPFRAME_ERROR;
        }
        //����ĳЩ���(��Ҫ�ǿͻ���),�Է���֪��ServicesID
        else
        {
            proc_frame->recv_service_ = my_svc_id_;
        }

    }

    //����˿ڽ����ո�ACCEPT��������û���յ�����
    if (PEER_STATUS_JUST_ACCEPT == peer_status_)
    {
        //��¼Service Info,���ں���Ĵ���,(���͵�ʱ��)
        if (proc_frame->proxy_service_.services_type_ != SERVICES_ID::INVALID_SERVICES_TYPE && if_proxy_ == false)
        {
            peer_svr_id_ = proc_frame->proxy_service_;
        }
        else
        {
            //���������Ĳ����ܵĿͻ��ˣ���ôֱ����handle����ID����Ϊ���ʱ��û��qq�š�
            if (SERVICES_ID::DYNAMIC_ALLOC_SERVICES_ID == proc_frame->send_service_.services_id_)
            {
                //����һ��ID���㡣����Ҫ�ǵû����
                proc_frame->send_service_.services_id_ = get_handle_id();
                peer_svr_id_ = proc_frame->send_service_;

                //proc_frame->frame_uid_   = proc_frame->send_service_.services_id_;

                //��������������ȷ���һ��ID���Է�
                //send_simple_zerg_cmd(,peer_svr_info_);
            }
            else
            {
                peer_svr_id_ = proc_frame->send_service_;
            }
        }

        //ע��,���ԭ������Ӧ������,�᷵��ԭ�е�����.replace_services_peerInfo,��Ȼ�ɹ�
        TCP_Svc_Handler *old_hdl = NULL;
        ret = svr_peer_info_set_.replace_services_peerInfo(peer_svr_id_, this, old_hdl);

        //�����ԭ�е�����,���ҵ�ԭ�����Ǹ�����ȥ.
        if (old_hdl != NULL)
        {
            //�����޸�ԭ�����ӵ�״̬,�����ظ���SETɾ��
            old_hdl->peer_status_ = PEER_STATUS_JUST_ACCEPT;
            old_hdl->send_simple_zerg_cmd(ZERG_REPEAT_LOGIN_KICK_OLD_RSP,
                                          peer_svr_id_,
                                          Zerg_App_Frame::DESC_SNDPRC_CLOSE_PEER);

            //��ֱ�ӹر��ˣ������Ȱ����������ˣ��ٹر�
            //old_hdl->handle_close(ACE_INVALID_HANDLE, 0);
        }

        //�������Լ�PEER��״̬
        peer_status_ = PEER_STATUS_ACTIVE;

        ZLOG_INFO("[zergsvr] Accept peer services[%u|%u],IP|Prot[%s|%u] regist success.",
                  peer_svr_id_.services_type_,
                  peer_svr_id_.services_id_,
                  peer_address_.get_host_addr(),
                  peer_address_.get_port_number()
                 );
    }
    //����˿ڽ����ո�ACCEPT��ȥ����û���յ�����
    else if (PEER_STATUS_JUST_CONNECT == peer_status_)
    {
        //�������Լ�PEER��״̬
        peer_status_ = PEER_STATUS_ACTIVE;

        ZLOG_INFO("[zergsvr] Connect peer services[%u|%u],IP|Prot[%s|%u] active success.",
                  peer_svr_id_.services_type_,
                  peer_svr_id_.services_id_,
                  peer_address_.get_host_addr(),
                  peer_address_.get_port_number()
                 );

    }
    else
    {
        if (SERVICES_ID::DYNAMIC_ALLOC_SERVICES_ID != proc_frame->send_service_.services_id_)
        {
            //�����ⷢ���߻��ǲ���ԭ���ķ����ߣ��Ƿ񱻴۸�
            if ((peer_svr_id_ != proc_frame->send_service_) && (peer_svr_id_ != proc_frame->proxy_service_))
            {
                return SOAR_RET::ERR_ZERG_APPFRAME_ERROR;
            }
        }
    }

    //���������ע������󣬻ش�һ��Ӧ��
    //����������ӵĽ������ƣ�Ӧ��������ط��Ӵ��롣
    if (ZERG_CONNECT_REGISTER_REQ == proc_frame->frame_command_)
    {
        send_simple_zerg_cmd(ZERG_CONNECT_REGISTER_RSP, peer_svr_id_);
    }

    //��¼�����˶��ٴ�����
    receive_times_++;

    if (receive_times_ == 0)
    {
        ++receive_times_;
    }

    //
    //��дIP��ַ�Ͷ˿ں�
    proc_frame->send_ip_address_ = peer_address_.get_ip_address();


    return 0;
}

//���ض˿ڵ�״̬,
TCP_Svc_Handler::PEER_STATUS  TCP_Svc_Handler::get_peer_status()
{
    return peer_status_;
}

/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2005��11��27��
Function        : TCP_Svc_Handler::process_connect_register
Return          : int
Parameter List  : NULL
Description     : ����ע�ᷢ��,
Calls           :
Called By       :
Other           : �ո������϶Է�,����һ��ע����Ϣ���Է�.��������������
Modify Record   :
******************************************************************************************/
int TCP_Svc_Handler::process_connect_register()
{

    peer_status_ = PEER_STATUS_JUST_CONNECT;

    //��������һ��ע��CMD�������Ҫ�������������.
    send_simple_zerg_cmd(ZERG_CONNECT_REGISTER_REQ, peer_svr_id_);

    //��������������3���Ժ����ڷ�����EPOLL��������д�¼���ԭ����û��ȡ��CONNECT_MASK
    reactor()->cancel_wakeup(this, ZCE_Event_Handler::CONNECT_MASK);

    //ע���ȡ��MASK
    reactor()->schedule_wakeup(this, ZCE_Event_Handler::READ_MASK);


    //��ӡ��Ϣ
    ZCE_Sockaddr_In      peeraddr;
    socket_peer_.getpeername(&peeraddr);
    ZLOG_INFO("[zergsvr] Connect services[%u|%u] peer socket IP|Port :[%s|%u] Success.",
              peer_svr_id_.services_type_,
              peer_svr_id_.services_id_,
              peeraddr.get_host_addr(),
              peeraddr.get_port_number());

    return 0;
}



/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2005��11��15��
Function        : TCP_Svc_Handler::read_data_from_peer
Return          : int
Parameter List  :
Param1: size_t& szrevc ��ȡ���ֽ�����
Description     : ��PEER��ȡ����
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int TCP_Svc_Handler::read_data_from_peer(size_t &szrevc)
{

    szrevc = 0;
    ssize_t recvret = 0;

    //�������һ���ڴ�
    if (rcv_buffer_ == NULL)
    {
        rcv_buffer_ = zbuffer_storage_->allocate_buffer();
    }

    //ZLOG_INFO("[zergsvr] read_data_from_peer %d .", get_handle());

    //������û�����ȥ����
    recvret = socket_peer_.recv(rcv_buffer_->buffer_data_ + rcv_buffer_->size_of_buffer_,
                                Zerg_Buffer::CAPACITY_OF_BUFFER - rcv_buffer_->size_of_buffer_,
                                0);

    //��ʾ���رջ��߳��ִ���
    if (recvret < 0)
    {
        //��ֻʹ��EWOULDBLOCK ����Ҫע��EAGAIN, ZCE_LIB::last_error() != EWOULDBLOCK && ZCE_LIB::last_error() != EAGAIN
        if (ZCE_LIB::last_error() != EWOULDBLOCK)
        {
            szrevc = 0;

            //�����ж�,�ȴ�����
            if (ZCE_LIB::last_error() == EINTR)
            {
                return 0;
            }

            //ͳ�ƽ��մ���
            server_status_->increase_by_statid(ZERG_RECV_FAIL_COUNTER, 0, 0, 1);

            //��¼����,���ش���
            ZLOG_ERROR("[zergsvr] Receive data error ,services[%u|%u],IP[%s|%u] peer:%u,ZCE_LIB::last_error()=%d|%s.",
                       peer_svr_id_.services_type_,
                       peer_svr_id_.services_id_,
                       peer_address_.get_host_addr(),
                       peer_address_.get_port_number(),
                       socket_peer_.get_handle(),
                       ZCE_LIB::last_error(),
                       strerror(ZCE_LIB::last_error()));
            return SOAR_RET::ERR_ZERG_FAIL_SOCKET_OP_ERROR;
        }

        //ͳ�ƽ��������Ĵ���
        server_status_->increase_by_statid(ZERG_RECV_BLOCK_COUNTER, 0, 0, 1);

        //�������������,ʲô������
        return 0;
    }

    //Socket���رգ�Ҳ���ش����ʾ
    if (recvret == 0)
    {
        return SOAR_RET::ERR_ZERG_SOCKET_CLOSE;
    }

    //��ʱRETӦ��> 0
    szrevc = recvret;

    //������N���ַ�
    rcv_buffer_->size_of_buffer_ += static_cast<size_t>(szrevc);
    recieve_bytes_ += static_cast<size_t>(szrevc);

    return 0;
}




//����Ƿ��յ���һ��������֡,
//���������һ�ֿ���,һ����ȡ�˶��֡�Ŀ���,
int TCP_Svc_Handler::check_recv_full_frame(bool &bfull,
                                           unsigned int &whole_frame_len)
{
    whole_frame_len = 0;
    bfull = false;

    //�����4���ֽڶ�û���ռ���,���ô����������
    //ע���������32λ��������
    if (rcv_buffer_->size_of_buffer_ - rcv_buffer_->size_of_use_ >= sizeof(unsigned int))
    {
        //�����4���ֽ�,���֡�ĳ���
        whole_frame_len = *(reinterpret_cast<unsigned int *>(rcv_buffer_->buffer_data_ + rcv_buffer_->size_of_use_));
        whole_frame_len = ntohl(whole_frame_len);

        //������ĳ��ȴ��ڶ������󳤶�,С����С����,����ȥ,�������������Ǵ�����󣬾��Ǳ�������
        if (whole_frame_len > Zerg_App_Frame::MAX_LEN_OF_APPFRAME || whole_frame_len < Zerg_App_Frame::LEN_OF_APPFRAME_HEAD)
        {
            ZLOG_ERROR("[zergsvr] Recieve error frame,services[%u|%u],IP[%s|%u], famelen %u , MAX_LEN_OF_APPFRAME:%u ,recv and use len:%u|%u.",
                       peer_svr_id_.services_type_,
                       peer_svr_id_.services_id_,
                       peer_address_.get_host_addr(),
                       peer_address_.get_port_number(),
                       whole_frame_len,
                       Zerg_App_Frame::MAX_LEN_OF_APPFRAME,
                       rcv_buffer_->size_of_buffer_,
                       rcv_buffer_->size_of_use_);
            //
            DEBUGDUMP_FRAME_HEAD(reinterpret_cast<Zerg_App_Frame *>(rcv_buffer_->buffer_data_), "Error frame before framehead_decode,", RS_DEBUG);
            return SOAR_RET::ERR_ZERG_GREATER_MAX_LEN_FRAME;
        }
    }


    //������ܵ������Ѿ�����,(������һ������)
    if (rcv_buffer_->size_of_buffer_ - rcv_buffer_->size_of_use_ >= whole_frame_len && whole_frame_len > 0)
    {
        bfull = true;
        ++recieve_counter_;
        ZCE_LOGMSG_DBG(RS_DEBUG, "Receive a whole frame from services[%u|%u] IP|Port [%s|%u] FrameLen:%u.",
                       peer_svr_id_.services_type_,
                       peer_svr_id_.services_id_,
                       peer_address_.get_host_addr(),
                       peer_address_.get_port_number(),
                       whole_frame_len);
    }

    return 0;
}


/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2008��2��4��
Function        : TCP_Svc_Handler::write_all_data_to_peer
Return          : int
Parameter List  : NULL
Description     :
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int TCP_Svc_Handler::write_all_data_to_peer()
{
    int ret = 0;

    for (;;)
    {
        //����һ�����ݰ�
        size_t szsend;
        bool   bfull = false;
        int ret = write_data_to_peer(szsend, bfull);

        //���ִ���,
        if (ret != 0)
        {
            return ret;
        }

        //������ݱ��Ѿ���������
        if (true == bfull)
        {
            //�ɹ����ͷ�����Ŀռ�
            zbuffer_storage_->free_byte_buffer(snd_buffer_deque_[0]);
            snd_buffer_deque_[0] = NULL;
            snd_buffer_deque_.pop_front();
        }
        //���û��ȫ�����ͳ�ȥ���ȴ���һ��дʱ��Ĵ���
        else
        {
            break;
        }

        //����Ѿ�û�����ݿ��Է�����
        if (snd_buffer_deque_.size() == 0)
        {
            break;
        }
    }

    //ȡ�õ�ǰ��MASKֵ
    int  handle_mask = get_mask();


    //���������û�п���д������
    if (snd_buffer_deque_.size() == 0)
    {
        //
        if (handle_mask & ZCE_Event_Handler::WRITE_MASK)
        {
            //ȡ����д��MASKֵ,
            ret = reactor()->cancel_wakeup(this, ZCE_Event_Handler::WRITE_MASK);

            //return -1��ʾ������ȷ���ص���old maskֵ
            if (-1 == ret)
            {
                ZLOG_ERROR("[zergsvr] TNNND cancel_wakeup return(%d) == -1 errno=%d|%s. ",
                           ret,
                           ZCE_LIB::last_error(),
                           strerror(ZCE_LIB::last_error()));
            }

        }

        //�����Ҫ�ر�
        if (true == if_force_close_)
        {
            ZLOG_INFO("[zergsvr] Send to peer services [%u|%u] IP|Port :[%s|%u] complete ,want to close peer on account of frame option.",
                      peer_svr_id_.services_type_,
                      peer_svr_id_.services_id_,
                      peer_address_.get_host_addr(),
                      peer_address_.get_port_number());
            //���ϲ�ȥ�رգ�ҪС�ģ�С�ģ����鷳���ܶ��������ڵ�����
            return SOAR_RET::ERR_ZERG_SOCKET_CLOSE;
        }
    }
    //���û�з��ͳɹ���ȫ�����ͳ�ȥ����׼������д�¼�
    else
    {
        //û��WRITE MASK��׼������д��־
        if (!(handle_mask & ZCE_Event_Handler::WRITE_MASK))
        {
            ret = reactor()->schedule_wakeup(this, ZCE_Event_Handler::WRITE_MASK);

            //schedule_wakeup ����return -1��ʾ�����ٴ�BS ACEһ�Σ���ȷ���ص���old maskֵ
            if (-1 == ret)
            {
                ZLOG_ERROR("[zergsvr] TNNND schedule_wakeup return (%d)== -1 errno=%d|%s. ",
                           ret,
                           ZCE_LIB::last_error(),
                           strerror(ZCE_LIB::last_error()));
            }
        }
    }

    return 0;
}



/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2005��11��15��
Function        : TCP_Svc_Handler::write_data_to_peer
Return          : int
Parameter List  :
Param1: size_t& szsend
Param2: bool& bfull
Description     : ��PEERд����
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int TCP_Svc_Handler::write_data_to_peer(size_t &szsend, bool &bfull)
{
    bfull = false;
    szsend = 0;

    //���û������Ҫ����, �����Ӧ����������
    //#if defined DEBUG || defined _DEBUG
    if (snd_buffer_deque_.empty() == true)
    {
        ZLOG_ERROR("[zergsvr] Goto handle_output|write_data_to_peer ,but not data to send. Please check,buffer deque size=%u.",
                   snd_buffer_deque_.size());
        ZCE_BACKTRACE_STACK(RS_ERROR);
        reactor()->cancel_wakeup(this, ZCE_Event_Handler::WRITE_MASK);
        ZCE_ASSERT(false);
        return 0;
    }

    //#endif //#if defined DEBUG || defined _DEBUG



    //ǰ���м��,����Խ��
    Zerg_Buffer *sndbuffer = snd_buffer_deque_[0];

    ssize_t sendret = socket_peer_.send(sndbuffer->buffer_data_ + sndbuffer->size_of_buffer_,
                                        sndbuffer->size_of_use_ - sndbuffer->size_of_buffer_,
                                        0);

    if (sendret <= 0)
    {

        //�����ж�,�ȴ�������ж���if (ZCE_LIB::last_error() == EINVAL),���������ϸ������,һ��ͬ��,�ϲ�غ������д���,�����������,������handle_input����
        //��ֻʹ��EWOULDBLOCK ����Ҫע��EAGAIN ZCE_LIB::last_error() != EWOULDBLOCK && ZCE_LIB::last_error() != EAGAIN
        if (ZCE_LIB::last_error() != EWOULDBLOCK)
        {
            //����Ӧ�û��ӡ����IP��������ظ�
            ZLOG_ERROR("[zergsvr] Send data error,services[%u|%u] IP|Port [%s|%u],Peer:%d errno=%d|%s .",
                       peer_svr_id_.services_type_,
                       peer_svr_id_.services_id_,
                       peer_address_.get_host_addr(),
                       peer_address_.get_port_number(),
                       socket_peer_.get_handle(),
                       ZCE_LIB::last_error(),
                       strerror(ZCE_LIB::last_error()));
            server_status_->increase_by_statid(ZERG_SEND_FAIL_COUNTER, 0, 0, 1);

            return SOAR_RET::ERR_ZERG_FAIL_SOCKET_OP_ERROR;
        }

        //ͳ�Ʒ��������Ĵ���
        server_status_->increase_by_statid(ZERG_SEND_BLOCK_COUNTER, 0, 0, 1);

        //�������������,ʲô������
        return 0;
    }

    szsend = sendret;

    //������N���ַ�
    sndbuffer->size_of_buffer_ += static_cast<size_t>(szsend);
    send_bytes_ += static_cast<size_t>(szsend);

    //��������Ѿ�ȫ��������
    if (sndbuffer->size_of_use_ == sndbuffer->size_of_buffer_)
    {
        bfull = true;
        ++send_counter_;
        //ZCE_LOGMSG_DBG(RS_DEBUG,"Send a few(n>=1) whole frame To  IP|Port :%s|%u FrameLen:%u.",
        //    peer_address_.get_host_addr(),
        //    peer_address_.get_port_number(),
        //    sndbuffer->size_of_buffer_);
    }

    return 0;
}

//�����ʹ���.
int TCP_Svc_Handler::process_send_error(Zerg_Buffer *tmpbuf, bool frame_encode)
{
    //��¼�Ѿ�ʹ�õ���λ��
    size_t use_start = tmpbuf->size_of_buffer_;
    tmpbuf->size_of_buffer_ = 0;

    //һ�������м�����ж��FRAME��Ҫ��ͷ�����н��룬���Ա���һ����Ū����
    while (tmpbuf->size_of_buffer_ != tmpbuf->size_of_use_)
    {
        Zerg_App_Frame *proc_frame = reinterpret_cast<Zerg_App_Frame *>(tmpbuf->buffer_data_ + tmpbuf->size_of_buffer_);

        //���FRAME�Ѿ�����
        if (frame_encode)
        {
            proc_frame->framehead_decode();
        }

        //����Ѿ�ʹ�õĵ�ַ��ʾ���֡�Ƿ�����,����Ѿ������ˣ����֡�Ͳ�Ҫ����

        //���û�з�����ɣ���¼���������д���
        if (use_start < tmpbuf->size_of_buffer_ + proc_frame->frame_length_)
        {

            //�����Ҫ��¼�������¼���������԰�æ����һЩ����
            if (proc_frame->frame_option_ & Zerg_App_Frame::DESC_SEND_FAIL_RECORD)
            {
                ZLOG_ERROR("[zergsvr] Connect peer ,send frame fail.frame len[%u] frame command[%u] frame uin[%u] snd svcid[%u|%u] proxy svc [%u|%u] recv[%u|%u] address[%s|%u],peer status[%u]. ",
                           proc_frame->frame_length_,
                           proc_frame->frame_command_,
                           proc_frame->frame_uid_,
                           proc_frame->send_service_.services_type_,
                           proc_frame->send_service_.services_id_,
                           proc_frame->proxy_service_.services_type_,
                           proc_frame->proxy_service_.services_id_,
                           proc_frame->recv_service_.services_type_,
                           proc_frame->recv_service_.services_id_,
                           peer_address_.get_host_addr(),
                           peer_address_.get_port_number(),
                           peer_status_
                          );
            }
        }

        //���Ӵ����͵Ĵ���
        server_status_->increase_by_statid(ZERG_SEND_FAIL_COUNTER, 0, 0, 1);

        //
        tmpbuf->size_of_buffer_ += proc_frame->frame_length_;
    }

    //�黹��POOL�м䡣
    zbuffer_storage_->free_byte_buffer(tmpbuf);

    return 0;


}



/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2007��12��24��
Function        : TCP_Svc_Handler::AllocSvcHandlerFromPool
Return          : TCP_Svc_Handler*
Parameter List  :
Param1: HANDLER_MODE handler_mode
Description     : �ӳ�������õ�һ��Handler�����ʹ��
Calls           :
Called By       :
Other           : Connect�Ķ˿�Ӧ����Զ������ȡ����Hanler������
Modify Record   :
******************************************************************************************/
TCP_Svc_Handler *TCP_Svc_Handler::AllocSvcHandlerFromPool(HANDLER_MODE handler_mode)
{
    //
    if (handler_mode == HANDLER_MODE_ACCEPTED)
    {
        if (pool_of_acpthdl_.size() == 0)
        {
            ZLOG_INFO("[zergsvr] Pool is too small to process accept handler,please notice.Pool size:%u,capacity:%u.",
                      pool_of_acpthdl_.size(),
                      pool_of_acpthdl_.capacity()
                     );
            return NULL;
        }

        TCP_Svc_Handler *p_handler = NULL;
        pool_of_acpthdl_.pop_front(p_handler);
        return p_handler;
    }
    //Connect�Ķ˿�Ӧ����Զ������ȡ����Hanler������
    else if (HANDLER_MODE_CONNECT == handler_mode)
    {
        ZCE_ASSERT(pool_of_cnthdl_.size() > 0);
        TCP_Svc_Handler *p_handler = NULL;
        pool_of_cnthdl_.pop_front(p_handler);
        return p_handler;
    }
    //Never go here.
    else
    {
        ZCE_ASSERT(false);
        return NULL;
    }
}


//�������е�Ҫ�Զ����ӵķ�����,����±�������������ӶϿں���û�����ݷ��͵����
void TCP_Svc_Handler::reconnect_allserver(size_t szsucc,
                                          size_t szfail,
                                          size_t szvalid)
{
    //�������е�SERVER
    zerg_auto_connect_.reconnect_allserver(szvalid, szsucc, szfail);
}
//
int TCP_Svc_Handler::uninit_all_staticdata()
{
    //
    svr_peer_info_set_.clear_and_closeall();

    //
    pool_of_acpthdl_.clear();

    return 0;
}




/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2005��11��15��
Function        : TCP_Svc_Handler::process_send_data
Return          : int
Parameter List  :
Param1: Zerg_Buffer* tmpbuf   Ҫ�õ����ݵ�Zerg_Buffer,Ҫ������,
Description     :
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int TCP_Svc_Handler::process_send_data(Zerg_Buffer *tmpbuf)
{
    server_status_->increase_by_statid(ZERG_SEND_FRAME_COUNTER, 0, 0, 1);
    //
    Zerg_App_Frame *proc_frame = reinterpret_cast<Zerg_App_Frame *>(tmpbuf->buffer_data_);
    DEBUGDUMP_FRAME_HEAD(proc_frame, "process_send_data Before framehead_encode:", RS_DEBUG);

    SERVICES_ID *p_sendto_svrinfo = NULL;

    //���͸��������͸�������
    if (proc_frame->proxy_service_.services_type_ != SERVICES_ID::INVALID_SERVICES_TYPE && if_proxy_ == false)
    {
        p_sendto_svrinfo = &(proc_frame->proxy_service_);
    }
    else
    {
        p_sendto_svrinfo = &(proc_frame->recv_service_);
    }

    if (p_sendto_svrinfo->services_id_ == 0)
    {
        // δָ��services_id�Ļ����ʹ�auto connect����һ��
        SERVICES_ID svrinfo;
        int ret = zerg_auto_connect_.get_server(p_sendto_svrinfo->services_type_, &svrinfo);

        if (ret != 0)
        {
            ZLOG_ERROR("process_send_data: service_id==0 but cant't find auto connect had service_type=%d svrinfo",
                       p_sendto_svrinfo->services_type_);
            return SOAR_RET::ERR_ZERG_SEND_FRAME_FAIL;
        }

        // �޸�һ��Ҫ���͵�svrinfo��id
        p_sendto_svrinfo->services_id_ = svrinfo.services_id_;
        ZLOG_DEBUG("process_send_data: service_type=%d service_id=0, change service id to %d",
                   p_sendto_svrinfo->services_type_,
                   p_sendto_svrinfo->services_id_);
    }

    int ret = 0;
    TCP_Svc_Handler *svchanle = NULL;
    ret = svr_peer_info_set_.find_services_peerinfo(*p_sendto_svrinfo, svchanle);

    //�����Ҫ���½������ӵķ�����������������,
    bool backroute_valid = false;
    SERVICES_ID backroute_svcinfo;

    if (svchanle == NULL || (svchanle != NULL  && svchanle->peer_status_ != PEER_STATUS_ACTIVE))
    {

        //������������Ƿ��Ƿ���Ҫ�����ķ����������Ҽ���Ƿ��б���·��
        //ret = zerg_auto_connect_.get_backupsvcinfo(*p_sendto_svrinfo,
        //                                           backroute_valid,
        //                                           backroute_svcinfo);

        //�����Ҫ�������ӳ�ȥ�ķ�����
        if (ret == 0  && svchanle == NULL)
        {
            //������Ƿ�ɹ����첽���ӣ�99.99999%�ǳɹ���,
            zerg_auto_connect_.reconnect_server(*p_sendto_svrinfo);
        }

        //�����������б���·�ɣ�����ѡ�񱸷�·�ɽ��д���
        if (backroute_valid)
        {
            //���û�����ӳɹ�,���߾����״̬�������ڼ���״̬,
            //ΪʲôҪ����Socket Peer״̬���ж�?��Ϊ״̬������Ǽ���״̬,���������޷���������,
            //����ʹ�õ��Ƿ���������,���Ӻ������سɹ�δ���������ĳɹ�

            //����б���·�ɣ������䴦�ڼ���״̬,�����ݽ�������·�ɷ���
            TCP_Svc_Handler *backroute_svchanle = NULL;
            ret = svr_peer_info_set_.find_services_peerinfo(backroute_svcinfo, backroute_svchanle);

            //�������·�ɴ���OK״̬���ñ���·�ɷ���
            if (ret == 0 && backroute_svchanle->peer_status_ == PEER_STATUS_ACTIVE)
            {
                svchanle = backroute_svchanle;
                //�޸Ľ�����
                *p_sendto_svrinfo = backroute_svcinfo;
            }
            else
            {
                //���û���ҵ���Ӧ��
                if (backroute_svchanle == NULL)
                {
                    zerg_auto_connect_.reconnect_server(backroute_svcinfo);
                }

                //
                ZLOG_ERROR("[zergsvr] Want to use back route to send data,but backup svc[%u|%u] not active main svc[%u|%u].",
                           backroute_svcinfo.services_type_,
                           backroute_svcinfo.services_id_,
                           p_sendto_svrinfo->services_type_,
                           p_sendto_svrinfo->services_id_
                          );
            }
        }
    }

    //Double Check����
    //���SVCHANDLEΪ��,��ʾû����ص�����,���д�����
    if (svchanle == NULL)
    {
        //�����û�б���
        ZLOG_ERROR("[zergsvr] [SEND TO NO EXIST HANDLE] ,send to a no exist handle[%u|%u],it could have been existed. frame command[%u]. uin[%u] frame length[%u].",
                   p_sendto_svrinfo->services_type_,
                   p_sendto_svrinfo->services_id_,
                   proc_frame->frame_command_,
                   proc_frame->frame_uid_,
                   proc_frame->frame_length_
                  );
        DEBUGDUMP_FRAME_HEAD(proc_frame, "[SEND TO NO EXIST HANDLE]", RS_ERROR);
        return SOAR_RET::ERR_ZERG_SEND_FRAME_FAIL;
    }


    //�������Ӧ���м������,
    //��·��OK������ACTIVE״̬��ʹ����·�ɷ���
    //��·�ɲ�����ACTIVE״̬�����Ǳ���·�ɴ���ACTIVE״̬��ʹ�ñ���·�ɷ���
    //��·�ɴ��ڣ����ǲ�����ACTIVE״̬������·��Ҳ������ACTIVE״̬�������ݽ�����·�ɣ����嵽���Ͷ���

    //�����͵�FRAME��HANDLE���󣬵�Ȼ����ط�δ��һ���ŵĽ�ȥ����Ϊ�м������,
    //1.����һ���ر�ָ��,
    //2.HANDLE�ڲ��Ķ�������,

    //��������д�������Ϊput_frame_to_sendlist�ڲ������˴��������յȲ���
    //�����Ϊֹ����Ϊ�ɹ�
    svchanle->put_frame_to_sendlist(tmpbuf);


    return 0;
}





//���ͼ򵥵ĵ�ZERG����,����ĳЩ��������Ĵ���
int TCP_Svc_Handler::send_simple_zerg_cmd(unsigned int cmd,
                                          const SERVICES_ID &recv_services_info,
                                          unsigned int option)
{
    //ZCE_LOGMSG_DBG(RS_DEBUG,"Send simple command to services[%u|%u] IP[%s|%u],Cmd %u.",
    //    peer_svr_info_.services_type_,
    //    peer_svr_info_.services_id_,
    //    peer_address_.get_host_addr(),
    //    peer_address_.get_port_number(),
    //    cmd);
    //��Է�����һ��������
    Zerg_Buffer *tmpbuf = zbuffer_storage_->allocate_buffer();
    Zerg_App_Frame *proc_frame = reinterpret_cast<Zerg_App_Frame *>(tmpbuf->buffer_data_);

    proc_frame->init_framehead(Zerg_App_Frame::LEN_OF_APPFRAME_HEAD, option, cmd);
    //ע������
    proc_frame->send_service_ = my_svc_id_;

    //����Լ��Ǵ��������,��д�����������Ϣ,��֤����,
    if (if_proxy_)
    {
        proc_frame->proxy_service_ = my_svc_id_;
    }

    //
    proc_frame->recv_service_ = recv_services_info;

    //
    tmpbuf->size_of_use_ = Zerg_App_Frame::LEN_OF_APPFRAME_HEAD;

    //
    return put_frame_to_sendlist(tmpbuf);
}

//��������
int TCP_Svc_Handler::send_zergheatbeat_reg()
{
    //
    return send_simple_zerg_cmd(ZERG_HEART_BEAT_REQ, peer_svr_id_);
}


/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2006��3��22��
Function        : TCP_Svc_Handler::put_frame_to_sendlist
Return          : int
Parameter List  :
Param1: Zerg_Buffer* tmpbuf
Description     : ���������ݷ��뷢�Ͷ�����
Calls           :
Called By       :
Other           : ���һ��PEERû��������,�ȴ����͵����ݲ��ܶ���PEER_STATUS_NOACTIVE��
Modify Record   : put_frame_to_sendlist�ڲ������˴��������յȲ���
******************************************************************************************/
int TCP_Svc_Handler::put_frame_to_sendlist(Zerg_Buffer *tmpbuf)
{
    int ret = 0;

    Zerg_App_Frame *proc_frame = reinterpret_cast<Zerg_App_Frame *>(tmpbuf->buffer_data_);

    //�����֪ͨ�رն˿�
    if (proc_frame->frame_command_ == INNER_RSP_CLOSE_SOCKET)
    {
        ZLOG_INFO("[zergsvr] Recvice CMD_RSP_CLOSE_SOCKET,services[%u|%u] IP[%s|%u] Svchanle will close.",
                  peer_svr_id_.services_type_,
                  peer_svr_id_.services_id_,
                  peer_address_.get_host_addr(),
                  peer_address_.get_port_number()
                 );
        if_force_close_ = true;
        //����֡
        process_send_error(tmpbuf, false);
        //�������UDP�Ĵ���,�رն˿�,UDP�Ķ���û�����ӵĸ���,
        handle_close();

        //����һ���������ϲ����
        return SOAR_RET::ERR_ZERG_SOCKET_CLOSE;
    }

    //����������,���Һ�̨ҵ��Ҫ��رն˿�,ע�����ת��������
    if (proc_frame->frame_option_ & Zerg_App_Frame::DESC_SNDPRC_CLOSE_PEER)
    {
        ZLOG_INFO("[zergsvr] This Peer Services[%u|%u] IP|Port :[%s|%u] will close when all frame send complete ,because send frame has option Zerg_App_Frame::DESC_SNDPRC_CLOSE_PEER.",
                  peer_svr_id_.services_type_,
                  peer_svr_id_.services_id_,
                  peer_address_.get_host_addr(),
                  peer_address_.get_port_number());
        if_force_close_ = true;
    }

    //ע������ط������ǻ�������Services ID����֤���ͳ�ȥ�����ݶ������Լ���SVCID��ʾ��.
    if (!if_proxy_)
    {
        proc_frame->send_service_ = my_svc_id_;
    }

    //��ͷ�����б���
    proc_frame->framehead_encode();

    //���뷢�Ͷ���,��ע���־λ
    bool bret = snd_buffer_deque_.push_back(tmpbuf);

    if (!bret)
    {
        server_status_->increase_by_statid(ZERG_SEND_LIST_FULL_COUNTER, 0, 0, 1);
        //�������ߴ������Ǹ����ݱȽϺ���?���ֵ����ȶ, ��������д�����(���ܶ���)�������µ�.
        //�ҵĿ���������������Ⱥ���.���ҿ��Ա����ڴ����.
        ZLOG_ERROR("[zergsvr] Services [%u|%u] IP|Port[%s|%u] send buffer cycle deque is full,this data must throw away,Send deque capacity =%u,may be extend it.",
                   peer_svr_id_.services_type_,
                   peer_svr_id_.services_id_,
                   peer_address_.get_host_addr(),
                   peer_address_.get_port_number(),
                   snd_buffer_deque_.capacity());

        //����֡
        process_send_error(tmpbuf, true);
        //����һ������
        return SOAR_RET::ERR_ZERG_SEND_FRAME_FAIL;
    }

    //------------------------------------------------------------------
    //�����ʼ�������Ѿ����뷢�Ͷ��У����տ�����handle_close�Լ�������.

    if (peer_status_ != PEER_STATUS_NOACTIVE)
    {
        ret = write_all_data_to_peer();

        //���ִ���,
        if (ret != 0)
        {
            //Ϊʲô�Ҳ����������,��return -1,��Ϊ��������ر�Socket,handle_input��������,������ظ�����
            //������жϵȴ���,������Լ�����.
            handle_close();

            //���������Ѿ�������У�����OK
            return 0;
        }

        //�ϲ�
        unite_frame_sendlist();
    }

    //ֻ�з��뷢�Ͷ��в���ɹ�.
    return 0;
}


/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2008��1��23��
Function        : TCP_Svc_Handler::unite_frame_sendlist
Return          : void
Parameter List  : NULL
Description     : �ϲ����Ͷ���
Calls           :
Called By       :
Other           : �����2�����ϵĵķ��Ͷ��У�����Կ��Ǻϲ�����
Modify Record   :
******************************************************************************************/
void TCP_Svc_Handler::unite_frame_sendlist()
{
    //�����2�����ϵĵķ��Ͷ��У�����Կ��Ǻϲ�����
    size_t sz_deque = snd_buffer_deque_.size();

    if (sz_deque <= 1)
    {
        return;
    }

    //���������2��Ͱ���������µ�����1��Ͱ��FRAME���ݣ�����кϲ�������
    if (Zerg_App_Frame::MAX_LEN_OF_APPFRAME - snd_buffer_deque_[sz_deque - 2]->size_of_use_ > snd_buffer_deque_[sz_deque - 1]->size_of_use_)
    {
        //��������1���ڵ�����ݷ��뵹����2���ڵ��м䡣����ʵ�ʵ�Cache�����Ƿǳ�ǿ�ģ�
        //�ռ�������Ҳ�ܸߡ�Խ��������Լ��ˡ�
        memcpy(snd_buffer_deque_[sz_deque - 2]->buffer_data_ + snd_buffer_deque_[sz_deque - 2]->size_of_use_,
               snd_buffer_deque_[sz_deque - 1]->buffer_data_,
               snd_buffer_deque_[sz_deque - 1]->size_of_use_);
        snd_buffer_deque_[sz_deque - 2]->size_of_use_ += snd_buffer_deque_[sz_deque - 1]->size_of_use_;

        //��������һ��ʩ�ŵ�
        zbuffer_storage_->free_byte_buffer(snd_buffer_deque_[sz_deque - 1]);
        snd_buffer_deque_[sz_deque - 1] = NULL;
        snd_buffer_deque_.pop_back();
    }

    ////����Ĵ������ںϲ��Ĳ��ԣ�ƽ����ע�͵�
    //else
    //{
    //    ZCE_LOGMSG_DBG(RS_DEBUG,"Goto unite_frame_sendlist sz_deque=%u,Zerg_App_Frame::MAX_LEN_OF_APPFRAME=%u,"
    //        "snd_buffer_deque_[sz_deque-2]->size_of_use_=%u,"
    //        "snd_buffer_deque_[sz_deque-1]->size_of_use_=%u.",
    //        sz_deque,
    //        Zerg_App_Frame::MAX_LEN_OF_APPFRAME,
    //        snd_buffer_deque_[sz_deque-2]->size_of_use_,
    //        snd_buffer_deque_[sz_deque-1]->size_of_use_);
    //}

}



/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2006��3��22��
Function        : TCP_Svc_Handler::push_frame_to_comm_mgr
Return          : int
Parameter List  : NULL
Description     :
Calls           :
Called By       :
Other           : �����������,Zerg_App_Frame�Ѿ�����������.��ע��.
Modify Record   :
******************************************************************************************/
int TCP_Svc_Handler::push_frame_to_comm_mgr()
{

    int ret = 0;

    //
    rcv_buffer_->size_of_use_ = 0;

    //
    while (rcv_buffer_)
    {
        unsigned int whole_frame_len = 0;
        bool bfull = false;
        ret = check_recv_full_frame(bfull, whole_frame_len);

        if (ret != 0)
        {
            return -1;
        }

        //���û���ܵ�
        if (false == bfull)
        {
            if (rcv_buffer_->size_of_use_ > 0)
            {
                //�������ڴ���ܽ���,���Բ���memcpy
                memmove(rcv_buffer_->buffer_data_,
                        rcv_buffer_->buffer_data_ + rcv_buffer_->size_of_use_,
                        rcv_buffer_->size_of_buffer_ - rcv_buffer_->size_of_use_);

                //�ı�buffer����
                rcv_buffer_->size_of_buffer_ = rcv_buffer_->size_of_buffer_ - rcv_buffer_->size_of_use_;
                rcv_buffer_->size_of_use_ = 0;
            }

            break;
        }

        Zerg_App_Frame *proc_frame = reinterpret_cast<Zerg_App_Frame *>(rcv_buffer_->buffer_data_ + rcv_buffer_->size_of_use_);

        //����Ѿ��ռ���һ������
        ret = preprocess_recvframe(proc_frame);

        //�Ѿ�����ͬ��ID������,����֡������
        if (ret != 0)
        {
            //�Ȳ�����,�� �����Ӧ�ĺ����ŵ�����ط�,�ŵ�����ĺ���,Ҫ������������,Υ���ҵĴ�����ѧ.
            if (SOAR_RET::ERR_ZERG_APPFRAME_ERROR == ret || SOAR_RET::ERR_ZERG_SERVER_ALREADY_LONGIN == ret)
            {
                //
                ZLOG_ERROR("[zergsvr] Peer services[%u|%u] IP[%s|%u] appFrame Error,Frame Len:%u,Command:%u,Uin:%u "
                           "Peer SvrType|SvrID:%u|%u,"
                           "Self SvrType|SvrID:%u|%u,"
                           "Send SvrType|SvrID:%u|%u,"
                           "Recv SvrType|SvrID:%u|%u,"
                           "Proxy SvrType|SvrID:%u|%u.",
                           peer_svr_id_.services_type_,
                           peer_svr_id_.services_id_,
                           peer_address_.get_host_addr(),
                           peer_address_.get_port_number(),
                           proc_frame->frame_length_,
                           proc_frame->frame_command_,
                           proc_frame->frame_uid_,
                           peer_svr_id_.services_type_, peer_svr_id_.services_id_,
                           my_svc_id_.services_type_, my_svc_id_.services_id_,
                           proc_frame->send_service_.services_type_,
                           proc_frame->send_service_.services_id_,
                           proc_frame->recv_service_.services_type_,
                           proc_frame->recv_service_.services_id_,
                           proc_frame->proxy_service_.services_type_,
                           proc_frame->proxy_service_.services_id_
                          );
            }
            else
            {
                ZLOG_ERROR("[zergsvr] Peer services [%u|%u] IP[%s|%u] preprocess_recvframe Ret =%d.",
                           peer_svr_id_.services_type_,
                           peer_svr_id_.services_id_,
                           peer_address_.get_host_addr(),
                           peer_address_.get_port_number(),
                           ret);
            }

            //ͳ�ƽ��մ���
            server_status_->increase_by_statid(ZERG_RECV_FAIL_COUNTER, 0, 0, 1);
            return -1;
        }

        //�����ݷ�����յĹܵ�,��������,��Ϊ������¼��־,�����д���Ҳ�޷�����

        zerg_comm_mgr_->pushback_recvpipe(proc_frame);

        //����һ������������
        rcv_buffer_->size_of_use_ += whole_frame_len;

        if (rcv_buffer_->size_of_buffer_ == rcv_buffer_->size_of_use_)
        {
            //���۴�����ȷ���,���ͷŻ������Ŀռ�
            zbuffer_storage_->free_byte_buffer(rcv_buffer_);
            rcv_buffer_ = NULL;
        }
        //�����һ�������յ������Ѿ������������.��ô�ͻ��������������
        //����������⸴�ӵ��жϣ������޶��յ��ĵ�һ�����ݰ�����󳤶�Ϊ֡ͷ�ĳ��ȣ����������ή��Ч�ʡ�
        else if (rcv_buffer_->size_of_buffer_ > rcv_buffer_->size_of_use_)
        {

        }

    }

    return 0;
}


void TCP_Svc_Handler::get_max_peer_num(size_t &maxaccept, size_t &maxconnect)
{
    maxaccept = max_accept_svr_;
    maxconnect = max_connect_svr_;
}


//�õ�Handle��ӦPEER�Ķ˿�
unsigned short TCP_Svc_Handler::get_peer_port()
{
    return peer_address_.get_port_number();
}
//�õ�Handle��ӦPEER��IP��ַ
const char *TCP_Svc_Handler::get_peer_address()
{
    return peer_address_.get_host_addr();
}

void TCP_Svc_Handler::dump_status_staticinfo(std::ostringstream &ostr_stream)
{
    ostr_stream << "Dump TCP_Svc_Handler Static Info:" << std::endl;
    ostr_stream << "MAX ACCEPT SVR:" << static_cast<unsigned int>(max_accept_svr_) << std::endl;
    ostr_stream << "MAX_CONNECT SVR:" << static_cast<unsigned int>(max_connect_svr_) << std::endl;
    ostr_stream << "IF PROXY:" << if_proxy_ << std::endl;
    ostr_stream << "CONNECT TIMEOUT:" << accepted_timeout_ << std::endl;
    ostr_stream << "RECEIVE TIMEOUT:" << receive_timeout_ << std::endl;
    ostr_stream << "NUMBER ACCEPT PEER:" << static_cast<unsigned int>(num_accept_peer_) << std::endl;
    ostr_stream << "NUMBER CONNECT PEER:" << static_cast<unsigned int>(num_connect_peer_) << std::endl;
    ostr_stream << "NUM CONNECT PEER:" << static_cast<unsigned int>(num_connect_peer_) << std::endl;
}



//
void TCP_Svc_Handler::dump_status_info(std::ostringstream &ostr_stream)
{

    ostr_stream << "SELF SVC INFO:" << my_svc_id_.services_type_ << "|" << my_svc_id_.services_id_ << " ";
    ostr_stream << "PEER SVC INFO:" << peer_svr_id_.services_type_ << "|" << peer_svr_id_.services_id_;
    ostr_stream << "IP:" << peer_address_.get_host_addr() << "|" << peer_address_.get_port_number();
    ostr_stream << "STATUS:" << peer_status_ << " HANDLE:" << get_handle();
    ostr_stream << "RECV:" << static_cast<unsigned int>(recieve_bytes_) << "|" << ((rcv_buffer_ != NULL) ? 1 : 0);
    ostr_stream << "SEND:" << static_cast<unsigned int>(send_bytes_) << "|" << static_cast<unsigned int>(snd_buffer_deque_.size()) << std::endl;
}

//Dump ���е�PEER��Ϣ
void TCP_Svc_Handler::dump_svcpeer_info(std::ostringstream &ostr_stream, size_t startno, size_t numquery)
{
    ostr_stream << "Services Peer Size" << static_cast<unsigned int>(svr_peer_info_set_.get_services_peersize()) << std::endl;
    svr_peer_info_set_.dump_svr_peerinfo(ostr_stream, startno, numquery);
}

//�ر���Ӧ������
int TCP_Svc_Handler::close_services_peer(const SERVICES_ID &svr_info)
{
    int ret = 0;
    TCP_Svc_Handler *svchanle = NULL;
    ret = svr_peer_info_set_.find_services_peerinfo(svr_info, svchanle);

    //�����Ҫ���½������ӵķ�����������������,
    if (ret != 0)
    {
        return ret;
    }

    svchanle->handle_close();
    return 0;
}

//�����е�SVR INFO����ѯ��Ӧ��HDL
int TCP_Svc_Handler::find_services_peer(const SERVICES_ID &svr_info, TCP_Svc_Handler *&svchanle)
{
    int ret = 0;
    ret = svr_peer_info_set_.find_services_peerinfo(svr_info, svchanle);

    //�����Ҫ���½������ӵķ�����������������,
    if (ret != 0)
    {
        return ret;
    }

    return 0;
}

const ZCE_Sockaddr_In &TCP_Svc_Handler::get_peer_sockaddr() const
{
    return peer_address_;
}


