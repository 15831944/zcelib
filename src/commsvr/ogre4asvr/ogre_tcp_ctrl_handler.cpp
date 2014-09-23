

#include "ogre_predefine.h"
#include "ogre_svrpeer_info_set.h"
#include "ogre_app_handler.h"
#include "ogre_configure.h"
#include "ogre_buf_storage.h"
#include "ogre_auto_connect.h"

#include "ogre_tcp_ctrl_handler.h"

/****************************************************************************************************
class  Ogre_TCP_Svc_Handler
****************************************************************************************************/
//CONNECT��ȴ����ݵĳ�ʱʱ��
unsigned int   Ogre_TCP_Svc_Handler::connect_timeout_ = 3;
//�������ݵĳ�ʱʱ��
unsigned int   Ogre_TCP_Svc_Handler::receive_timeout_ = 5;

//TIME ID
const int      Ogre_TCP_Svc_Handler::TCPCTRL_TIME_ID[] = {1, 2};

unsigned int   Ogre_TCP_Svc_Handler::error_try_num_ = 3;

//
PeerInfoSetToTCPHdlMap Ogre_TCP_Svc_Handler::svr_peer_info_set_;

Ogre_Connect_Server Ogre_TCP_Svc_Handler::zerg_auto_connect_;

//
size_t         Ogre_TCP_Svc_Handler::num_accept_peer_ = 0;
//
size_t         Ogre_TCP_Svc_Handler::num_connect_peer_ = 0;

//�����Խ��ܵĽ�������
size_t         Ogre_TCP_Svc_Handler::max_accept_svr_ = 0;
//�����Խ��ܵ���������
size_t         Ogre_TCP_Svc_Handler::max_connect_svr_ = 0;

//ACCEPT SVC handler�ĳ���
Ogre_TCP_Svc_Handler::POOL_OF_TCP_HANDLER Ogre_TCP_Svc_Handler::pool_of_acpthdl_;

//CONNECT svc handler�ĳ���
Ogre_TCP_Svc_Handler::POOL_OF_TCP_HANDLER Ogre_TCP_Svc_Handler::pool_of_cnthdl_;

//���캯��
Ogre_TCP_Svc_Handler::Ogre_TCP_Svc_Handler(Ogre_TCP_Svc_Handler::OGRE_HANDLER_MODE hdl_mode ):
    ZEN_Event_Handler(ZEN_Reactor::instance()),
    handler_mode_(hdl_mode),
    rcv_buffer_(NULL),
    recieve_bytes_(0),
    send_bytes_(0),
    peer_status_(PEER_STATUS_JUST_CONNECT),
    timeout_time_id_(-1),
    receive_times_(0),
    if_force_close_(false),
    fp_judge_whole_frame_(NULL)
{
    if ( HANDLER_MODE_CONNECT == hdl_mode )
    {
        snd_buffer_deque_.initialize(MAX_OF_CONNECT_PEER_SEND_DEQUE);
    }
    else if (HANDLER_MODE_ACCEPTED == hdl_mode )
    {
        snd_buffer_deque_.initialize(MAX_OF_ACCEPT_PEER_SEND_DEQUE);
    }
    else
    {
        ZEN_ASSERT(false);
    }
}

//��������
Ogre_TCP_Svc_Handler::~Ogre_TCP_Svc_Handler()
{

}

/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2006��3��22��
Function        : Ogre_TCP_Svc_Handler::init_tcp_svc_handler
Return          : void
Parameter List  :
  Param1: const ZEN_Socket_Stream& sockstream
  Param2: const ZEN_Sockaddr_In& socketaddr   Socket�ĵ�ַ����ʵ���Դ�sockstream�еõ�����Ϊ��Ч�ʺͷ���.
Description     : ��ʼ������,����Accept�Ķ˿ڵĴ���Event Handle����.
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
void Ogre_TCP_Svc_Handler::init_tcp_svc_handler(const ZEN_Socket_Stream &sockstream,
                                                FPJudgeRecvWholeFrame fp_judge_whole)
{
    handler_mode_ = HANDLER_MODE_ACCEPTED;
    rcv_buffer_ = NULL;
    recieve_bytes_ = 0;
    send_bytes_ = 0;
    socket_peer_ = sockstream;
    peer_status_ = PEER_STATUS_JUST_CONNECT;
    timeout_time_id_ = -1;
    receive_times_  = 0;
    if_force_close_ = false;
    fp_judge_whole_frame_ = fp_judge_whole;

    ////����Socket ΪO_NONBLOCK
    int ret = socket_peer_.sock_enable(O_NONBLOCK);
    socket_peer_.getpeername(&remote_address_);
    socket_peer_.getsockname(&local_address_);

    const size_t TMP_ADDR_LEN = 32;
    char local_addr[TMP_ADDR_LEN], remote_addr[TMP_ADDR_LEN];
    strncpy(local_addr, local_address_.get_host_addr(), TMP_ADDR_LEN);
    strncpy(remote_addr, remote_address_.get_host_addr(), TMP_ADDR_LEN);
    ZLOG_INFO( "Listen peer [%s|%u] accept Socket IP Address: [%s|%u] success,.\n",
               local_addr,
               local_address_.get_port_number(),
               remote_addr,
               remote_address_.get_port_number()
             );

    ++num_accept_peer_;
    //ͳ��

    //��������������,REACTOR�Լ���ʵ�п���,��������Ҫ����ACCEPT��Ҫ����CONNECT.
    //����ֻ��,ͷXX��, �������������ʵ������������,�������ᷢ��,
    if (num_accept_peer_ < max_accept_svr_)
    {
        //ע���д�¼�
        ret = reactor()->register_handler(this, ZEN_Event_Handler::READ_MASK | ZEN_Event_Handler::WRITE_MASK);

        //
        if (ret != 0)
        {
            ZLOG_ERROR( "Register Accept handler fail! ret =%d  errno=%u|%s \n", ret, ZEN_OS::last_error(), strerror(ZEN_OS::last_error()));
            handle_close();
            return;
        }

        reactor()->cancel_wakeup(this, ZEN_Event_Handler::WRITE_MASK);
    }
    //Ҫ���Լ��һ��,
    else
    {
        ZLOG_ERROR( "Great than max_accept_svr_ Reject! num_accept_peer_:%u,max_accept_svr_:%u \n",
                    num_accept_peer_,
                    max_accept_svr_);
        handle_close();
        return;
    }

    //��������˳�ʱ����,N������յ�һ����
    if ( connect_timeout_ > 0 || receive_timeout_ > 0)
    {
        ZEN_Time_Value delay(0, 0);
        ZEN_Time_Value interval(0, 0);

        delay.sec(connect_timeout_);
        interval.sec(receive_timeout_);

        //��������˽������ݳ�ʱ����ô����������Ӳ��ֶ��г�ʱ
        if (connect_timeout_ <= 0)
        {
            delay.sec(receive_timeout_);
        }

        timeout_time_id_ = timer_queue()->schedule_timer (this, &TCPCTRL_TIME_ID[0], delay, interval);
    }

    Socket_Peer_Info peer_svr_info(remote_address_);
    //�������Ӵ����MAP
    ret = svr_peer_info_set_.add_services_peerinfo(peer_svr_info, this);

    //�������ɱ�ǲ���Σ����һ��
    if (ret != TSS_RET::TSS_RET_SUCC)
    {
        handle_close();
        return;
    }

    peer_status_ = PEER_STATUS_ACTIVE;
}

/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2006��3��22��
Function        : Ogre_TCP_Svc_Handler::init_tcp_svc_handler
Return          : void
Parameter List  :
  Param1: const Socket_Peer_Info& svrinfo        Socket_Peer_Info��Ϣ
  Param2: const ZEN_Socket_Stream& sockstream  Sokcet Peer
  Param3: const ZEN_Sockaddr_In& socketaddr    ��Ӧ���ӵĵ�ַ��Ϣ
Description     : ��ʼ������,����Connect��ȥ��PEER ��ӦEvent Handle����.
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
void Ogre_TCP_Svc_Handler::init_tcp_svc_handler(const ZEN_Socket_Stream &sockstream,
                                                const ZEN_Sockaddr_In &socketaddr,
                                                FPJudgeRecvWholeFrame fp_judge_whole)
{
    handler_mode_ = HANDLER_MODE_CONNECT;
    rcv_buffer_ = NULL;
    recieve_bytes_ = 0;
    send_bytes_ = 0;
    socket_peer_ = sockstream;
    remote_address_ = socketaddr;
    peer_status_ = PEER_STATUS_NOACTIVE;
    timeout_time_id_ = -1;
    receive_times_ = 0;
    if_force_close_ = false;
    fp_judge_whole_frame_ = fp_judge_whole;

    //����Socket ΪO_NONBLOCK
    int ret = socket_peer_.sock_enable(O_NONBLOCK);
    //
    socket_peer_.getsockname(&local_address_);

    //ע�ᵽ
    ret = reactor()->register_handler(this, ZEN_Event_Handler::CONNECT_MASK);

    //�Ҽ���û�м���register_handlerʧ��,
    if (ret != 0)
    {

        ZLOG_ERROR("Register Connect handler fail! ret =%  errno=%u|%s \n", ret, ZEN_OS::last_error(), strerror(ZEN_OS::last_error()));
        handle_close();
        return;
    }

    Socket_Peer_Info peer_svr_info(remote_address_);
    //�������Ӵ����MAP
    ret = svr_peer_info_set_.add_services_peerinfo(peer_svr_info, this);

    //�������ɱ�ǲ���Σ����һ��
    if (ret != TSS_RET::TSS_RET_SUCC)
    {
        handle_close();
        return;
    }

    ++num_connect_peer_;

    socklen_t opval = 32 * 1024;
    int opvallen = sizeof(socklen_t);
    //����һ��SND,RCV BUFFER,
    socket_peer_.setsockopt(SOL_SOCKET, SO_RCVBUF, reinterpret_cast<void *>(&opval), opvallen);
    socket_peer_.setsockopt(SOL_SOCKET, SO_SNDBUF, reinterpret_cast<void *>(&opval), opvallen);
    int keep_alive = 1;
    opvallen = sizeof(int);
    socket_peer_.setsockopt(SOL_SOCKET, SO_KEEPALIVE, reinterpret_cast<void *>(&keep_alive), opvallen);

#ifndef WIN32
    //����DELAY�����������
    int NODELAY = 1;
    opvallen = sizeof(int);
    socket_peer_.setsockopt(SOL_TCP, TCP_NODELAY, reinterpret_cast<void *>(&NODELAY), opvallen);
#endif
}

//ȡ�þ��
ZEN_SOCKET Ogre_TCP_Svc_Handler::get_handle(void) const
{
    return socket_peer_.get_handle();
}

/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2005��11��27��
Function        : Ogre_TCP_Svc_Handler::handle_input
Return          : int
Parameter List  :
  Param1: ZEN_HANDLE
Description     : ACE��ȡ,�������¼�����������
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int Ogre_TCP_Svc_Handler::handle_input(ZEN_HANDLE)
{
    //��ȡ����
    size_t szrecv;

    int ret = read_data_from_peer(szrecv);

    ZLOG_DEBUG("Read event,[%s|%u] ,TCP handle input event triggered. ret:%d,szrecv:%u.\n",
               remote_address_.get_host_addr(),
               remote_address_.get_port_number(),
               ret,
               szrecv);

    //����κδ��󶼹ر�,
    if (ret != TSS_RET::TSS_RET_SUCC)
    {
        return -1;
    }

    //
    while (rcv_buffer_)
    {
        unsigned int size_frame = 0;
        bool if_recv_whole = false;
        //����ģ�����
        ret = fp_judge_whole_frame_(rcv_buffer_->frame_data_,
                                    rcv_buffer_->ogre_frame_len_ - Ogre4a_AppFrame::LEN_OF_OGRE_FRAME_HEAD,
                                    static_cast<unsigned int>(Ogre4a_AppFrame::MAX_OF_OGRE_DATA_LEN),
                                    if_recv_whole,
                                    size_frame);

        if (TSS_RET::TSS_RET_SUCC != ret)
        {
            ZLOG_ERROR( "Read data error [%s|%u].Judge whole fale error ret=%u.\n",
                        remote_address_.get_host_addr(),
                        remote_address_.get_port_number(),
                        ret);
            return -1;
        }

        //����Ѿ��ռ���һ������
        if (if_recv_whole == true)
        {
            //�����ݷ�����յĹܵ�,��������,��Ϊ������¼��־,�����д���Ҳ�޷�����
            ZEN_LOGMSG_DBG(RS_DEBUG, "Read a whole data [%s|%u] recv buffer len:%u, Frame len:%u.\n",
                           remote_address_.get_host_addr(),
                           remote_address_.get_port_number(),
                           rcv_buffer_->ogre_frame_len_ - Ogre4a_AppFrame::LEN_OF_OGRE_FRAME_HEAD,
                           size_frame);

            //��¼�����˶��ٴ�����
            receive_times_++;

            if (receive_times_ == 0)
            {
                receive_times_ ++;
            }

            push_frame_to_recvpipe(size_frame);
        }
        else
        {
            //
            break;

        }
    }

    return 0;
}

/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2005��11��27��
Function        : Ogre_TCP_Svc_Handler::handle_output
Return          : int
Parameter List  :
  Param1: ZEN_HANDLE
Description     : ACE��ȡ,�������¼�����������
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int Ogre_TCP_Svc_Handler::handle_output(ZEN_HANDLE)
{

    //���NON BLOCK Connect�ɹ�,Ҳ�����handle_output
    if ( PEER_STATUS_NOACTIVE == peer_status_)
    {

        //�������Ӻ������
        process_connect_register();
        return 0;
    }

    //
    int ret =  write_all_aata_to_peer();

    //���ִ���,
    if (ret != TSS_RET::TSS_RET_SUCC)
    {
        //Ϊʲô�Ҳ����������,��return -1,��Ϊ��������ر�Socket,handle_input��������,������ظ�����
        //������жϵȴ���,������Լ�����.
        return -1;
    }

    return 0;
}

/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2005��12��20��
Function        : Ogre_TCP_Svc_Handler::handle_timeout
Return          : int
Parameter List  :
  Param1: const ZEN_Time_Value& * time  ʱ��,
  Param2: const void* arg               Ψһ��ʾ����
Description     : ��ʱ����
Calls           :
Called By       :
Other           : ACEΪʲô��ֱ��ʹ��TIME ID,
Modify Record   :
******************************************************************************************/
int Ogre_TCP_Svc_Handler::handle_timeout(const ZEN_Time_Value &/*time*/, const void *arg)
{
    const int timeid = *(static_cast<const int *>(arg));

    //������3��û�з�Ӧ
    if (TCPCTRL_TIME_ID[0] == timeid)
    {
        //������ܵ����ݣ���ôʲôҲ����
        if ( receive_times_ > 0)
        {
            receive_times_ = 0;
        }
        //���û���յ�����,��¥��ɱ
        else
        {
            ZLOG_ERROR( "Timeout event,[%s|%u] connect or Recive expire,want to close handle. recieve_counter=%u\n",
                        remote_address_.get_host_addr(),
                        remote_address_.get_port_number(),
                        receive_times_);
            //ԭ����������ط�reutrn -1,���ڷ���return -1��һ�������ĵ�����,(��ʱ����ȡ����ʹ��ָ��ķ�ʽ,��������е�����)
            //���������ֱ�ӵ���handle_close
            handle_close();
            return 0;
        }
    }
    else if (TCPCTRL_TIME_ID[1] == timeid)
    {
    }

    return 0;
}

/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2005��12��20��
Function        : Ogre_TCP_Svc_Handler::handle_close
Return          : int
Parameter List  :
Description     : PEER Event Handler�رյĴ���
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int Ogre_TCP_Svc_Handler::handle_close ()
{
    //��Ҫʹ��cancel_timer(this),�䷱��,������,��Ҫnew,������һ����֪��������

    //ȡ����Event Handler��صĶ�ʱ��
    if ( -1 != timeout_time_id_  )
    {
        timer_queue()->cancel_timer(timeout_time_id_);
        timeout_time_id_ = -1;
    }

    //ȡ�����е�MASK,���׶�,�������handle_close,ֻ����һ��
    reactor()->remove_handler(this, false);

    //�رն˿�,
    socket_peer_.close();

    //�ͷŽ������ݻ�����
    if (rcv_buffer_)
    {
        Ogre_Buffer_Storage::instance()->free_byte_buffer(rcv_buffer_);
        rcv_buffer_ = NULL;
    }

    //���������ݻ�����
    size_t sz_of_deque = snd_buffer_deque_.size();

    for (size_t i = 0; i < sz_of_deque; i++)
    {
        //�����ʹ������,ͬʱ���л���
        process_senderror(snd_buffer_deque_[i]);
        snd_buffer_deque_[i] = NULL;
    }

    snd_buffer_deque_.clear();

    //��������Ǽ���״̬���������������ӵķ���.
    if (peer_status_ == PEER_STATUS_ACTIVE || handler_mode_ == HANDLER_MODE_CONNECT)
    {
        Socket_Peer_Info peer_svr_info(remote_address_);
        //ע����Щ��Ϣ
        svr_peer_info_set_.del_services_peerinfo(peer_svr_info);

    }

    peer_status_ = PEER_STATUS_NOACTIVE;

    //���ݲ�ͬ�����ͼ���,

    //������������������,�����һ���µ�����Ҫ����ʱ������������
    if (handler_mode_ == HANDLER_MODE_CONNECT)
    {
        ZLOG_ERROR( "Close event,[%s|%u] connect peer socket close .\n",
                    remote_address_.get_host_addr(),
                    remote_address_.get_port_number());

        //��ָ��黹�������м�ȥ
        pool_of_cnthdl_.push_back(this);
        --num_connect_peer_ ;
    }
    else if (handler_mode_ == HANDLER_MODE_ACCEPTED)
    {
        ZLOG_INFO( "Close event,[%s|%u] accept peer socket close.\n",
                   remote_address_.get_host_addr(),
                   remote_address_.get_port_number());

        //��ָ��黹�������м�ȥ
        pool_of_acpthdl_.push_back(this);
        --num_accept_peer_;
    }

    return 0;
}

//���ض˿ڵ�״̬,
Ogre_TCP_Svc_Handler::PEER_STATUS  Ogre_TCP_Svc_Handler::get_peer_status()
{
    return peer_status_;
}

/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2005��11��27��
Function        : Ogre_TCP_Svc_Handler::process_connect_register
Return          : int
Parameter List  : NULL
Description     : ����ע�ᷢ��,
Calls           :
Called By       :
Other           : �ո������϶Է�,����һ��ע����Ϣ���Է�.��������������
Modify Record   :
******************************************************************************************/
int Ogre_TCP_Svc_Handler::process_connect_register()
{

    peer_status_ = PEER_STATUS_ACTIVE;

    //��������������3���Ժ����ڷ�����EPOLL��������д�¼���ԭ����û��ȡ��CONNECT_MASK
    reactor()->cancel_wakeup(this, ZEN_Event_Handler::CONNECT_MASK);

    //���������Ҫ����
    if (snd_buffer_deque_.empty() != true )
    {
        reactor()->schedule_wakeup(this, ZEN_Event_Handler::WRITE_MASK | ZEN_Event_Handler::READ_MASK);
    }
    //ֻ��ȡ
    else
    {
        reactor()->schedule_wakeup(this, ZEN_Event_Handler::READ_MASK);
    }

    //��ӡ��Ϣ
    const size_t TMP_ADDR_LEN = 32;
    char local_addr[TMP_ADDR_LEN], remote_addr[TMP_ADDR_LEN];
    strncpy(local_addr, local_address_.get_host_addr(), TMP_ADDR_LEN);
    strncpy(remote_addr, remote_address_.get_host_addr(), TMP_ADDR_LEN);
    ZLOG_INFO( "Local peer[%s|%u] auto connect [%s|%u] success,.\n",
               local_addr,
               local_address_.get_port_number(),
               remote_addr,
               remote_address_.get_port_number()
             );
    return TSS_RET::TSS_RET_SUCC;
}

/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2005��11��15��
Function        : Ogre_TCP_Svc_Handler::read_data_from_peer
Return          : int
Parameter List  :
  Param1: size_t& szrevc
Description     : ��PEER��ȡ����
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int Ogre_TCP_Svc_Handler::read_data_from_peer(size_t &szrevc)
{
    szrevc = 0;

    ssize_t recvret = 0;

    //�������һ���ڴ�
    if (rcv_buffer_ == NULL)
    {
        rcv_buffer_ = Ogre_Buffer_Storage::instance()->allocate_byte_buffer();
        rcv_buffer_->snd_peer_info_.SetSocketPeerInfo(remote_address_);
        rcv_buffer_->rcv_peer_info_.SetSocketPeerInfo(local_address_);
        //
        rcv_buffer_->ogre_frame_option_ = 0;
        rcv_buffer_->ogre_frame_option_ |= Ogre4a_AppFrame::OGREDESC_PEER_TCP;
    }

    //
    size_t data_len = rcv_buffer_->ogre_frame_len_ - Ogre4a_AppFrame::LEN_OF_OGRE_FRAME_HEAD ;

    if (data_len < Ogre4a_AppFrame::MAX_OF_OGRE_DATA_LEN)
    {
        recvret = socket_peer_.recv(rcv_buffer_->frame_data_ + data_len,
                                    Ogre4a_AppFrame::MAX_OF_OGRE_FRAME_LEN - rcv_buffer_->ogre_frame_len_,
                                    0);
    }
    //�����İ��ڶ�û�н�����������
    else
    {
        ZLOG_ERROR( "Read error[%s|%u],Buffer size is not enought or this is a error (attack) data.\n",
                    remote_address_.get_host_addr(),
                    remote_address_.get_port_number()
                  );
        return TSS_RET::ERR_OGRE_SOCKET_CLOSE;
    }

    //��ʾ���رջ��߳��ִ���
    if (recvret < 0)
    {
        //��ֻʹ��EWOULDBLOCK ����Ҫע��EAGAIN, ZEN_OS::last_error() != EWOULDBLOCK && ZEN_OS::last_error() != EAGAIN
        if (ZEN_OS::last_error() != EWOULDBLOCK )
        {
            szrevc = 0;

            //�����ж�,�ȴ�����
            if (ZEN_OS::last_error() == EINVAL)
            {
                return TSS_RET::TSS_RET_SUCC;
            }

            //��¼����,���ش���
            ZLOG_ERROR( "Read error,[%s|%u] receive data error peer:%u ZEN_OS::last_error()=%d|%s.\n",
                        remote_address_.get_host_addr(),
                        remote_address_.get_port_number(),
                        socket_peer_.get_handle(),
                        ZEN_OS::last_error(),
                        strerror(ZEN_OS::last_error()));

            return TSS_RET::ERR_OGRE_SOCKET_OP_ERROR;
        }

        //�������������,ʲô������
        return TSS_RET::TSS_RET_SUCC;
    }

    //Socket���رգ�Ҳ���ش����ʾ
    if (recvret == 0)
    {
        return TSS_RET::ERR_OGRE_SOCKET_CLOSE;
    }

    //��ʱRETӦ��> 0
    szrevc = recvret;

    //������N���ַ�
    rcv_buffer_->ogre_frame_len_ += static_cast<unsigned int>(szrevc) ;
    recieve_bytes_ +=  static_cast<size_t>(szrevc);

    return TSS_RET::TSS_RET_SUCC;

}

/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2005��11��15��
Function        : Ogre_TCP_Svc_Handler::write_data_to_peer
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
int Ogre_TCP_Svc_Handler::write_data_to_peer(size_t &szsend, bool &if_full)
{
    if_full = false;
    szsend = 0;

    //Ϊʲô��������أ�ԭ������Щʱ��ע��д��־λ��ʧ�ܣ����Ϊʲô���Ѻ���
    //#if defined DEBUG || defined _DEBUG
    //���û������Ҫ����, �����Ӧ����������
    if (snd_buffer_deque_.empty() == true)
    {
        ZLOG_ERROR( "Write error,[%s|%u] goto handle_output|write_data_to_peer ,but not data to send. Please check you code.\n",
                    remote_address_.get_host_addr(),
                    remote_address_.get_port_number()
                  );
        reactor()->cancel_wakeup (this, ZEN_Event_Handler::WRITE_MASK);
        return TSS_RET::TSS_RET_SUCC;
    }

    //#endif //#if defined DEBUG || defined _DEBUG

    //ǰ���м��,����Խ��
    Ogre4a_AppFrame *sndbuffer = snd_buffer_deque_[0];

    ssize_t sendret = socket_peer_.send(sndbuffer->frame_data_ + send_bytes_  ,
                                        (sndbuffer->ogre_frame_len_ - send_bytes_ - Ogre4a_AppFrame::LEN_OF_OGRE_FRAME_HEAD));

    if (sendret < 0)
    {
        int last_error = ZEN_OS::last_error();

        szsend = 0;
        //����Ӧ�û��ӡ����IP��������ظ�
        ZLOG_ERROR( "Write error[%s|%u],send data error. peer:%d errno=%u|%s \n",
                    remote_address_.get_host_addr(),
                    remote_address_.get_port_number(),
                    socket_peer_.get_handle(),
                    last_error,
                    strerror(last_error));

        //EINVAL:�����ж�,�ȴ�������ж���if (ZEN_OS::last_error() == EINVAL),���������ϸ������,һ��ͬ��,�ϲ�غ������д���,�����������,������handle_input����
        //EWOULDBLOCK:��ֻʹ��EWOULDBLOCK ����Ҫע��EAGAIN ZEN_OS::last_error() != EWOULDBLOCK && ZEN_OS::last_error() != EAGAIN
        if (  EWOULDBLOCK  != last_error && EINVAL != last_error )
        {
            return TSS_RET::ERR_OGRE_SOCKET_OP_ERROR;
        }

        //�������������,ʲô������
        return TSS_RET::TSS_RET_SUCC;
    }

    szsend  = sendret;
    send_bytes_ += static_cast<size_t>(szsend);

    //������ܵ�����
    if (sndbuffer->ogre_frame_len_ - Ogre4a_AppFrame::LEN_OF_OGRE_FRAME_HEAD == send_bytes_ )
    {
        if_full = true;
        ZEN_LOGMSG_DBG(RS_DEBUG, "Send a whole frame To  IP|Port :%s|%u FrameLen:%u.\n", remote_address_.get_host_addr(), remote_address_.get_port_number(), send_bytes_);
        send_bytes_ = 0;
    }

    return TSS_RET::TSS_RET_SUCC;
}

/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2008��2��4��
Function        : Ogre_TCP_Svc_Handler::write_all_aata_to_peer
Return          : int
Parameter List  : NULL
Description     : �����ķ�������Ҫ���͵����ݣ����Լ�����Ŭ��
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int Ogre_TCP_Svc_Handler::write_all_aata_to_peer()
{
    int ret = 0;

    //�ڿ��Է��͵��Ǻ�һ�ν����е�����ȫ���������,
    for (;;)
    {
        //����һ�����ݰ�
        size_t szsend ;
        bool   bfull = false;
        int ret =  write_data_to_peer(szsend, bfull);

        //���ִ���,
        if (ret != TSS_RET::TSS_RET_SUCC)
        {
            return ret;
        }

        //������ݱ��Ѿ���������
        if ( true == bfull )
        {
            Ogre_Buffer_Storage::instance()->free_byte_buffer(snd_buffer_deque_[0]);
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
    int handle_mask = get_mask();

    //���������û�п���д������
    if (snd_buffer_deque_.size() == 0)
    {
        //�����д��־��ȡ��֮
        if ( handle_mask & ZEN_Event_Handler::WRITE_MASK )
        {
            //ȡ����д��MASKֵ,
            ret = reactor()->cancel_wakeup(this, ZEN_Event_Handler::WRITE_MASK);

            //return -1��ʾ������ȷ���ص���old maskֵ
            if ( -1  ==  ret )
            {
                int last_err = ZEN_OS::last_error();
                ZLOG_ERROR( "TNNND cancel_wakeup return(%d) == -1 errno=%d|%s. \n",
                            ret,
                            last_err,
                            strerror(last_err));
                //��Ϊ��������ǳ��������⣬�ر�֮
                return TSS_RET::ERR_OGRE_SOCKET_CLOSE;
            }
        }

        //�����Ҫ�ر�
        if (true == if_force_close_ )
        {
            ZLOG_INFO( "Send to peer handle[%u] IP|Port :[%s|%u] complete ,want to close peer on account of frame option.\n",
                       socket_peer_.get_handle(),
                       remote_address_.get_host_addr(),
                       remote_address_.get_port_number());
            //���ϲ�ȥ�رգ�ҪС�ģ�С�ģ����鷳���ܶ��������ڵ�����,��Ϊ�������ط������������
            return TSS_RET::ERR_OGRE_SOCKET_CLOSE;
        }
    }
    //���û�з��ͳɹ���ȫ�����ͳ�ȥ����׼������д�¼�
    else
    {
        //û��WRITE MASK��׼������д��־
        if (!(handle_mask & ZEN_Event_Handler::WRITE_MASK))
        {
            ret = reactor()->schedule_wakeup(this, ZEN_Event_Handler::WRITE_MASK);

            //schedule_wakeup ����return -1��ʾ�����ٴ�BS ACEһ�Σ���ȷ���ص���old maskֵ
            if ( -1 == ret)
            {
                int last_err = ZEN_OS::last_error();
                ZLOG_ERROR( "TNNND schedule_wakeup return (%d)== -1 errno=%d|%s. \n",
                            ret,
                            last_err,
                            strerror(last_err));
                //��Ϊ��������ǳ��������⣬�ر�֮
                return TSS_RET::ERR_OGRE_SOCKET_CLOSE;
            }
        }
    }

    return TSS_RET::TSS_RET_SUCC;
}

//�����ʹ���.
int Ogre_TCP_Svc_Handler::process_senderror(Ogre4a_AppFrame *inner_frame)
{
    int ret = 0;

    //�������֡��ȷ��ʾҪ���д�������,
    if (inner_frame->ogre_frame_option_ & Ogre4a_AppFrame::OGREDESC_SEND_FAIL_RECORD  )
    {
        ////ԭ��һֱ����ʹ�ô���ܵ��������·��͵ȴ����������룬ʵ�ڶ��࣬����
        //// �������ܵ���Ϊ��
        //if(  Zerg_MMAP_BusPipe::instance()->IsExistZergPipe(Zerg_MMAP_BusPipe::ERROR_PIPE_ID) == true)
        //{
        //}
        ZLOG_ERROR( " Peer handle [%u] ,send frame fail.frame len[%u] frame command[%u] frame uin[%u] address[%s|%u],peer status[%u]. \n",
                    socket_peer_.get_handle(),
                    inner_frame->ogre_frame_len_,
                    remote_address_.get_host_addr(),
                    remote_address_.get_port_number(),
                    peer_status_
                  );
    }
    //�������������ȷҪ֪ͨ����ķ���,��RETRY����
    else if (inner_frame->ogre_frame_option_  & Ogre4a_AppFrame::OGREDESC_SNDPRC_NOTIFY_APP)
    {

        //��ʾ���֡�Ƿ��ʹ���,�ؽ��������Ӧ��,
        inner_frame->ogre_frame_option_ |= Ogre4a_AppFrame::OGREDESC_SEND_ERROR;

        //��־�ں����������,�����.
        ret = Zerg_MMAP_BusPipe::instance()->push_back_bus(Zerg_MMAP_BusPipe::RECV_PIPE_ID,
                                                           reinterpret_cast<const ZEN_LIB::dequechunk_node *>(inner_frame));

        if (ret != TSS_RET::TSS_RET_SUCC)
        {
            return TSS_RET::ERR_OGRE_RECEIVE_PIPE_IS_FULL;
        }
    }

    //�黹��POOL�м䡣
    Ogre_Buffer_Storage::instance()->free_byte_buffer(inner_frame);

    return TSS_RET::TSS_RET_SUCC;
}

/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2006��3��22��
Function        : Ogre_TCP_Svc_Handler::get_configure
Return          : int
Parameter List  :
  Param1: const char* cfgfilename �����ļ�����
Description     : �������ļ���ȡ������Ϣ
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int Ogre_TCP_Svc_Handler::get_configure(Zen_INI_PropertyTree &cfg_file)
{
    int ret = 0;
    ret = Zerg_MMAP_BusPipe::instance()->getpara_from_zergcfg(cfg_file);

    if (ret != TSS_RET::TSS_RET_SUCC)
    {
        return ret;
    }

    unsigned int tmp_uint = 0 ;
    //��CONNECT���յ����ݵ�ʱ��
    ret  = cfg_file.get_uint32_value("COMMCFG", "CONNECTTIMEOUT", tmp_uint);
    connect_timeout_ = tmp_uint;
    TESTCONFIG((ret == 0 && connect_timeout_ <= 50 ), "COMMCFG|CONNECTTIMEOUT key error.");

    //RECEIVEһ�����ݵĳ�ʱʱ��,Ϊ0��ʾ������
    ret = cfg_file.get_uint32_value("COMMCFG", "RECEIVETIMEOUT", tmp_uint );
    receive_timeout_ = tmp_uint;
    TESTCONFIG((ret == 0 && receive_timeout_ <= 1800 ), "COMMCFG|RECEIVETIMEOUT key error.");

    //���������ҵķ���������
    ret = cfg_file.get_uint32_value("COMMCFG", "MAXACCEPTSVR", tmp_uint);
    max_accept_svr_ = tmp_uint;
    TESTCONFIG((ret == 0 && max_accept_svr_ <= 409600 && max_accept_svr_ > 0), "COMMCFG|MAXACCEPTSVR key error.");

    //���������ݳ��Է��ʹ���
    ret = cfg_file.get_uint32_value("COMMCFG", "TRYERROR", tmp_uint );
    error_try_num_ = tmp_uint;
    TESTCONFIG((ret == 0 && error_try_num_ < 10), "COMMCFG|TRYERROR key error.");

    //�Զ����Ӳ��ֶ�ȡ����
    ret = zerg_auto_connect_.get_configure(cfg_file);

    if (ret != 0)
    {
        return ret;
    }

    //���Ҫ�������������Զ����ӷ��������
    max_connect_svr_ = zerg_auto_connect_.num_svr_to_connect() ;
    //
    return TSS_RET::TSS_RET_SUCC;
}

/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2006��3��22��
Function        : Ogre_TCP_Svc_Handler::init_all_static_data
Return          : int
Parameter List  : NULL
Description     : �����ò�����ʼ��
Calls           :
Called By       :
Other           : һЩ�������������ȡ,�������Ĳ�����Ҫ����������
Modify Record   :
******************************************************************************************/
int Ogre_TCP_Svc_Handler::init_all_static_data()
{
    int ret = 0;

    //������һ����Сֵ
    max_accept_svr_ += 8;
    max_connect_svr_ += 8;
    ZLOG_INFO("MaxAcceptSvr:%u MaxConnectSvr:%u.\n", max_accept_svr_, max_connect_svr_);

    //ΪCONNECT��HDLԤ�ȷ����ڴ棬��Ϊһ������
    ZLOG_INFO( "Connet Hanlder:size of Ogre_TCP_Svc_Handler [%u],one connect handler have deqeue length [%u],number of connect handler [%u]."
               "About need  memory [%u] bytes.\n",
               sizeof(Ogre_TCP_Svc_Handler),
               MAX_OF_CONNECT_PEER_SEND_DEQUE,
               max_connect_svr_,
               (max_connect_svr_ * (sizeof(Ogre_TCP_Svc_Handler) + MAX_OF_CONNECT_PEER_SEND_DEQUE * sizeof(size_t)))
             );
    pool_of_cnthdl_.initialize(max_connect_svr_);

    for (size_t i = 0; i < max_connect_svr_; ++i)
    {
        Ogre_TCP_Svc_Handler *p_handler = new Ogre_TCP_Svc_Handler(HANDLER_MODE_CONNECT);
        pool_of_cnthdl_.push_back(p_handler);
    }

    //ΪACCEPT��HDLԤ�ȷ����ڴ棬��Ϊһ������
    ZLOG_INFO( "Accept Hanlder:size of Ogre_TCP_Svc_Handler [%u],one accept handler have deqeue length [%u],number of accept handler [%u]."
               "About need  memory [%u] bytes.\n",
               sizeof(Ogre_TCP_Svc_Handler),
               MAX_OF_ACCEPT_PEER_SEND_DEQUE,
               max_accept_svr_,
               (max_accept_svr_ * (sizeof(Ogre_TCP_Svc_Handler) + MAX_OF_ACCEPT_PEER_SEND_DEQUE * sizeof(size_t)))
             );
    pool_of_acpthdl_.initialize(max_accept_svr_ );

    for (size_t i = 0; i < max_accept_svr_; ++i)
    {
        Ogre_TCP_Svc_Handler *p_handler = new Ogre_TCP_Svc_Handler(HANDLER_MODE_ACCEPTED);
        pool_of_acpthdl_.push_back(p_handler);
    }

    //��ʼ�����еĶ˿ڶ�Ӧ��,
    svr_peer_info_set_.init_services_peerinfo(max_accept_svr_ + max_connect_svr_ + 64);

    //�������е�SERVER,��������ش����˳�
    size_t sz_of_svr = 0, sz_of_succ = 0;
    ret = zerg_auto_connect_.connect_all_server(sz_of_svr, sz_of_succ);
    ZLOG_INFO( "Have %u server to auto connect ,success %u ,ret =%d.\n",
               sz_of_svr, sz_of_succ, ret);

    if (ret != TSS_RET::TSS_RET_SUCC)
    {
        return ret;
    }

    return TSS_RET::TSS_RET_SUCC;
}

//
int Ogre_TCP_Svc_Handler::unInit_all_static_data()
{
    //
    svr_peer_info_set_.clear_and_close();
    return TSS_RET::TSS_RET_SUCC;
}

/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2007��12��24��
Function        : Ogre_TCP_Svc_Handler::alloc_svchandler_from_pool
Return          : Ogre_TCP_Svc_Handler*
Parameter List  :
Param1: OGRE_HANDLER_MODE handler_mode
Description     : �ӳ�������õ�һ��Handler�����ʹ��
Calls           :
Called By       :
Other           : Connect�Ķ˿�Ӧ����Զ������ȡ����Hanler������
Modify Record   :
******************************************************************************************/
Ogre_TCP_Svc_Handler *Ogre_TCP_Svc_Handler::alloc_svchandler_from_pool(OGRE_HANDLER_MODE handler_mode)
{
    //
    if (handler_mode == HANDLER_MODE_ACCEPTED )
    {
        if (pool_of_acpthdl_.size() == 0)
        {
            ZLOG_INFO( "Pool is too small to process accept handler,please notice.Pool size:%u,capacity:%u.\n",
                       pool_of_acpthdl_.size(),
                       pool_of_acpthdl_.capacity()
                     );
            return NULL;
        }

        Ogre_TCP_Svc_Handler *p_handler = NULL;
        pool_of_acpthdl_.pop_front(p_handler);
        return p_handler;
    }
    //Connect�Ķ˿�Ӧ����Զ������ȡ����Hanler������
    else if ( HANDLER_MODE_CONNECT == handler_mode )
    {
        ZEN_ASSERT(pool_of_cnthdl_.size() > 0);
        Ogre_TCP_Svc_Handler *p_handler = NULL;
        pool_of_cnthdl_.pop_front(p_handler);
        return p_handler;
    }
    //Never go here.
    else
    {
        ZEN_ASSERT(false);
        return NULL;
    }
}

/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2005��11��15��
Function        : Ogre_TCP_Svc_Handler::process_send_data
Return          : int
Parameter List  :
    Param1: ZByteBuffer* tmpbuf   Ҫ�õ����ݵ�ZByteBuffer,Ҫ������,
Description     :
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int Ogre_TCP_Svc_Handler::process_send_data(Ogre4a_AppFrame *ogre_frame )
{

    int ret = 0;

    //���ھ�̬����Ĺ�ϵ����������������
    const size_t TMP_IP_ADDRESS_LEN = 32;
    char remote_ip_str[TMP_IP_ADDRESS_LEN], local_ip_str[TMP_IP_ADDRESS_LEN];

    Socket_Peer_Info svrinfo = ogre_frame->rcv_peer_info_;

    Ogre_TCP_Svc_Handler *svchanle = NULL;
    ret = svr_peer_info_set_.find_services_peerinfo(svrinfo, svchanle);

    //�����Ҫ���½������ӵķ�����������������,
    if ( ret != 0 )
    {
        //Ϊʲô����һ��Ogre_TCP_Svc_Handler��Ϊ����,��Ϊ�ڷ���Connect������,Ҳ����handle_close.
        ret = zerg_auto_connect_.connect_server_by_svrinfo(svrinfo);

        //����ط���һ��Double Check,����������ӳɹ�
        if (ret == TSS_RET::TSS_RET_SUCC)
        {
            ret = svr_peer_info_set_.find_services_peerinfo(svrinfo, svchanle);
        }
    }

    //���SVCHANDLEΪ��,��ʾû����ص�����,���д�����
    if (svchanle == NULL)
    {

        //�޷�����
        ZLOG_INFO( "Can't find handle remote address[%s|%u],send fail ,local address [%s|%u],frame len[%u].\n",
                   ZEN_OS::inet_ntoa(ogre_frame->rcv_peer_info_.peer_ip_address_, remote_ip_str, TMP_IP_ADDRESS_LEN),
                   ogre_frame->rcv_peer_info_.peer_port_,
                   ZEN_OS::inet_ntoa(ogre_frame->snd_peer_info_.peer_ip_address_, local_ip_str, TMP_IP_ADDRESS_LEN),
                   ogre_frame->snd_peer_info_.peer_port_,
                   ogre_frame->ogre_frame_len_
                 );
        return TSS_RET::ERR_OGRE_SEND_FRAME_FAIL;
    }

    //�����֪ͨ�رն˿�
    if (ogre_frame->ogre_frame_option_ & Ogre4a_AppFrame::OGREDESC_CLOSE_PEER )
    {
        ZLOG_INFO( "Recvice DESC_CLOSE_PEER,Svchanle will close, svrinfo [IP|Port:%s|%u ].\n",
                   ZEN_OS::inet_ntoa(svrinfo.peer_ip_address_, local_ip_str, TMP_IP_ADDRESS_LEN),
                   svrinfo.peer_port_);
        //�������UDP�Ĵ���,�رն˿�,UDP�Ķ���û�����ӵĸ���,
        svchanle->handle_close();
        return TSS_RET::ERR_OGRE_SOCKET_CLOSE;
    }

    //�����͵�FRAME��HANDLE���󣬵�Ȼ����ط�δ��һ���ŵĽ�ȥ����Ϊ�м������,
    //1.����һ���ر�ָ��,
    //2.HANDLE�ڲ��Ķ�������,

    svchanle->put_frame_to_sendlist(ogre_frame);

    return TSS_RET::TSS_RET_SUCC;
}

/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2006��3��22��
Function        : Ogre_TCP_Svc_Handler::put_frame_to_sendlist
Return          : int
Parameter List  :
Param1: ZByteBuffer* tmpbuf
Description     : ���������ݷ��뷢�Ͷ�����
Calls           :
Called By       :
Other           : ���һ��PEERû��������,�ȴ����͵����ݲ��ܶ���PEER_STATUS_NOACTIVE��
Modify Record   :
******************************************************************************************/
int Ogre_TCP_Svc_Handler::put_frame_to_sendlist(Ogre4a_AppFrame *ogre_frame)
{
    int ret = 0;

    //����һ��û�������ϵ�PEER,�ȴ����Ӷ��е����ݲ��ܹ���,
    if (peer_status_ == PEER_STATUS_NOACTIVE &&
        snd_buffer_deque_.size() >= MAX_LEN_OF_SEND_LIST)
    {
        //�������ߴ������Ǹ����ݱȽϺ���?���ֵ����ȶ, ��������д�����(���ܶ���)�������µ�.
        //�ҵĿ���������������Ⱥ���.���ҿ��Ա����ڴ����.
        process_senderror(ogre_frame);
        return TSS_RET::ERR_OGRE_SEND_FRAME_FAIL;
    }

    //����������,���Һ�̨ҵ��Ҫ��رն˿�,ע�����ת��������
    if ( ogre_frame->ogre_frame_option_ & Ogre4a_AppFrame::OGREDESC_SNDPRC_CLOSE_PEER)
    {
        ZLOG_INFO( "This Peer handle[%u] IP|Port :[%s|%u] complete ,will close when all frame send complete ,because send frame has option Ogre4a_AppFrame::OGREDESC_SNDPRC_CLOSE_PEER.\n",
                   socket_peer_.get_handle(),
                   remote_address_.get_host_addr(),
                   remote_address_.get_port_number());
        if_force_close_ = true;
    }

    //���뷢�Ͷ���,
    bool bret = snd_buffer_deque_.push_back(ogre_frame);

    if ( false == bret )
    {
        //�������ߴ������Ǹ����ݱȽϺ���?���ֵ����ȶ, ��������д�����(���ܶ���)�������µ�.
        //�ҵĿ���������������Ⱥ���.���ҿ��Ա����ڴ����.
        ZLOG_ERROR( "Peer handle [%u] IP|Port[%s|%u] send buffer cycle deque is full,this data must throw away,Send deque capacity =%u,may be extend it.\n",
                    socket_peer_.get_handle(),
                    remote_address_.get_host_addr(),
                    remote_address_.get_port_number(),
                    snd_buffer_deque_.capacity());

        //����֡
        process_senderror(ogre_frame);
        //����һ������
        return TSS_RET::ERR_OGRE_SEND_FRAME_FAIL;
    }

    //���Է���������ݰ�

    if (peer_status_ != PEER_STATUS_NOACTIVE)
    {

        ret = write_all_aata_to_peer();

        //���ִ���,
        if (ret != TSS_RET::TSS_RET_SUCC)
        {
            //����Ѿ������˵���������ر�֮�������жϴ������ϲ��Ѿ�ת���˴���
            handle_close();

            //���������Ѿ�������У�����OK
            return TSS_RET::TSS_RET_SUCC;
        }
    }

    //�ϲ�,�ϲ������ڸ�Ч�������ڶ˿ڶ��ݵ������кô�
    unite_frame_sendlist();

    //ֻ�з��뷢�Ͷ��в���ɹ�.
    return TSS_RET::TSS_RET_SUCC;
}

/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2008��1��23��
Function        : Ogre_TCP_Svc_Handler::unite_frame_sendlist
Return          : void
Parameter List  : NULL
Description     : �ϲ����Ͷ���
Calls           :
Called By       :
Other           : �����2�����ϵĵķ��Ͷ��У�����Կ��Ǻϲ�����
Modify Record   :
******************************************************************************************/
void Ogre_TCP_Svc_Handler::unite_frame_sendlist()
{
    //�����2�����ϵĵķ��Ͷ��У�����Կ��Ǻϲ�����
    size_t sz_deque = snd_buffer_deque_.size();

    if ( sz_deque <= 1)
    {
        return;
    }

    //���������2��Ͱ���������µ�����1��Ͱ��FRAME���ݣ�����кϲ�������
    if ( (Ogre4a_AppFrame::MAX_OF_OGRE_FRAME_LEN - snd_buffer_deque_[sz_deque - 2]->ogre_frame_len_) >  \
         (snd_buffer_deque_[sz_deque - 1]->ogre_frame_len_ - Ogre4a_AppFrame::LEN_OF_OGRE_FRAME_HEAD))
    {
        //��������1���ڵ�����ݷ��뵹����2���ڵ��м䡣����ʵ�ʵ�Cache�����Ƿǳ�ǿ�ģ�
        //�ռ�������Ҳ�ܸߡ�Խ��������Լ��ˡ�
        memcpy((snd_buffer_deque_[sz_deque - 2]->frame_data_ + snd_buffer_deque_[sz_deque - 2]->ogre_frame_len_ - Ogre4a_AppFrame::LEN_OF_OGRE_FRAME_HEAD),
               snd_buffer_deque_[sz_deque - 1]->frame_data_,
               (snd_buffer_deque_[sz_deque - 1]->ogre_frame_len_ - Ogre4a_AppFrame::LEN_OF_OGRE_FRAME_HEAD));

        snd_buffer_deque_[sz_deque - 2]->ogre_frame_len_ += (snd_buffer_deque_[sz_deque - 1]->ogre_frame_len_ - Ogre4a_AppFrame::LEN_OF_OGRE_FRAME_HEAD);

        //��������һ��ʩ�ŵ�
        Ogre_Buffer_Storage::instance()->free_byte_buffer(snd_buffer_deque_[sz_deque - 1]);
        snd_buffer_deque_[sz_deque - 1] = NULL;
        snd_buffer_deque_.pop_back();
    }

    ////����Ĵ������ںϲ��Ĳ��ԣ�ƽ����ע�͵�
    //else
    //{
    //    ACE_DEBUGEX((LM_DEBUG,"Goto unite_frame_sendlist sz_deque=%u,Ogre4a_AppFrame::MAX_OF_OGRE_FRAME_LEN=%u,"
    //        "snd_buffer_deque_[sz_deque-2]->ogre_frame_len_=%u,"
    //        "snd_buffer_deque_[sz_deque-1]->ogre_frame_len_=%u\n",
    //        sz_deque,
    //        Ogre4a_AppFrame::MAX_OF_OGRE_FRAME_LEN,
    //        snd_buffer_deque_[sz_deque-2]->ogre_frame_len_,
    //        snd_buffer_deque_[sz_deque-1]->ogre_frame_len_));
    //}

}

/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2006��3��22��
Function        : Ogre_TCP_Svc_Handler::push_frame_to_recvpipe
Return          : int
Parameter List  : NULL
Description     :
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int Ogre_TCP_Svc_Handler::push_frame_to_recvpipe(unsigned int sz_data)
{

    int ret = Zerg_MMAP_BusPipe::instance()->push_back_bus(Zerg_MMAP_BusPipe::RECV_PIPE_ID,
                                                           reinterpret_cast<ZEN_LIB::dequechunk_node *>(rcv_buffer_));

    //���յ��˺���һ��֡������,
    if (rcv_buffer_->ogre_frame_len_ > sz_data + Ogre4a_AppFrame::LEN_OF_OGRE_FRAME_HEAD )
    {
        memmove(rcv_buffer_->frame_data_,
                rcv_buffer_->frame_data_ + sz_data ,
                rcv_buffer_->ogre_frame_len_ - sz_data - Ogre4a_AppFrame::LEN_OF_OGRE_FRAME_HEAD );

        //�ı�buffer����
        rcv_buffer_->ogre_frame_len_ = rcv_buffer_->ogre_frame_len_ - sz_data;

    }
    else if (rcv_buffer_->ogre_frame_len_ == sz_data + Ogre4a_AppFrame::LEN_OF_OGRE_FRAME_HEAD)
    {
        //���۴�����ȷ���,���ͷŻ������Ŀռ�
        Ogre_Buffer_Storage::instance()->free_byte_buffer(rcv_buffer_);
        rcv_buffer_ = NULL;
    }
    //�������
    else
    {
        ZEN_ASSERT(false);
    }

    //���۴�����ȷ���,���ͷŻ������Ŀռ�

    //��־�ں����������,�����.
    if (ret != TSS_RET::TSS_RET_SUCC )
    {
        return TSS_RET::ERR_OGRE_RECEIVE_PIPE_IS_FULL;
    }

    return TSS_RET::TSS_RET_SUCC;
}

//
void Ogre_TCP_Svc_Handler::get_maxpeer_num(size_t &maxaccept, size_t &maxconnect)
{
    maxaccept = max_accept_svr_;
    maxconnect = max_connect_svr_;
}

//�õ�Handle��ӦPEER�Ķ˿�
unsigned short Ogre_TCP_Svc_Handler::get_peer_port()
{
    return remote_address_.get_port_number();
}
//�õ�Handle��ӦPEER��IP��ַ
const char *Ogre_TCP_Svc_Handler::get_peer_address()
{
    return remote_address_.get_host_addr();
}

