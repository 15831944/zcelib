#include "zerg_predefine.h"
#include "zerg_accept_handler.h"
#include "zerg_tcp_ctrl_handler.h"
#include "zerg_ip_restrict.h"


/****************************************************************************************************
class  TCP_Accept_Handler TCP Accept �����EventHandler,
****************************************************************************************************/
TCP_Accept_Handler::TCP_Accept_Handler(const SERVICES_ID &svcid, const ZCE_Sockaddr_In     &addr, bool sessionkey_verify):
    ZCE_Event_Handler(ZCE_Reactor::instance()),
    my_svc_info_(svcid),
    accept_bind_addr_(addr),
    sessionkey_verify_(sessionkey_verify),
    ip_restrict_(Zerg_IPRestrict_Mgr::instance())
{
}

//�Լ���������ͣ�ͳһ�ر���handle_close,����ط����ùر�
TCP_Accept_Handler::~TCP_Accept_Handler()
{
}

//
int TCP_Accept_Handler::create_listen(unsigned int backlog)
{
    //
    const socklen_t opval = ZERG_SND_RCV_BUF_OPVAL;
    socklen_t opvallen = sizeof(socklen_t);

    //
    socklen_t sndbuflen = 0, rcvbuflen = 0;
    int ret = peer_acceptor_.open(&accept_bind_addr_, true, AF_UNSPEC, backlog);

    //�������Bind��Ӧ�Ķ˿�
    if (ret != 0)
    {

        ZLOG_ERROR("[zergsvr] Bind Listen IP|Port :[%s|%u] Fail.Error: %d|%s.",
                   accept_bind_addr_.get_host_addr(),
                   accept_bind_addr_.get_port_number(),
                   ZCE_OS::last_error(),
                   strerror(ZCE_OS::last_error()));
        return SOAR_RET::ERR_ZERG_INIT_LISTEN_PORT_FAIL;
    }

    peer_acceptor_.sock_enable(O_NONBLOCK);

    ZLOG_INFO("[zergsvr] Bind Listen IP|Port : [%s|%u] Success.errno =%d .",
              accept_bind_addr_.get_host_addr(),
              accept_bind_addr_.get_port_number(),
              errno);

    //��Accept�Ķ˿ڻ�̳���Щѡ��
    peer_acceptor_.getsockopt(SOL_SOCKET, SO_RCVBUF, reinterpret_cast<void *>(&rcvbuflen), &opvallen);
    peer_acceptor_.getsockopt(SOL_SOCKET, SO_SNDBUF, reinterpret_cast<void *>(&sndbuflen), &opvallen);
    ZLOG_INFO("[zergsvr] Get Listen Peer SO_RCVBUF:%u SO_SNDBUF %u.", rcvbuflen, sndbuflen);

    //����һ��SND,RCV BUFFER,
    peer_acceptor_.setsockopt(SOL_SOCKET, SO_RCVBUF, reinterpret_cast<const void *>(&opval), opvallen);
    peer_acceptor_.setsockopt(SOL_SOCKET, SO_SNDBUF, reinterpret_cast<const void *>(&opval), opvallen);

    peer_acceptor_.getsockopt(SOL_SOCKET, SO_RCVBUF, reinterpret_cast<void *>(&rcvbuflen), &opvallen);
    peer_acceptor_.getsockopt(SOL_SOCKET, SO_SNDBUF, reinterpret_cast<void *>(&sndbuflen), &opvallen);
    ZLOG_INFO("[zergsvr] Set Listen Peer SO_RCVBUF:%u SO_SNDBUF %u.", rcvbuflen, sndbuflen);

#ifndef ZCE_OS_WINDOWS

    //����DELAY�����������
    int NODELAY = 1;
    opvallen = sizeof(int);
    peer_acceptor_.setsockopt(SOL_TCP, TCP_NODELAY, reinterpret_cast<void *>(&NODELAY), opvallen);

    //TCP_DEFER_ACCEPT,���ѡ������ʱ�������������ѡ����һ���ܺõı��⹥�����ֶΡ�
    //int val = 1;
    //ret = peer_acceptor_.set_option(IPPROTO_TCP, TCP_DEFER_ACCEPT, &val, sizeof(val));
    //ZLOG_INFO("[zergsvr]  setsockopt TCP_DEFER_ACCEPT val(%d) ret(%d). ", val, ret);

#endif

    //
    reactor()->register_handler(this, ZCE_Event_Handler::ACCEPT_MASK);

    return SOAR_RET::SOAR_RET_SUCC;
}

//
int TCP_Accept_Handler::handle_input(/*handle*/)
{
    ZCE_Socket_Stream  sockstream;
    ZCE_Sockaddr_In       remote_address;
    int ret = peer_acceptor_.accept(sockstream, &remote_address);

    //������ִ���,��δ���? return -1?
    if (ret != 0)
    {
        //���ڸ�����C/C++�ķ��ؾ�ָ̬������⣬���Ҫ���������ַ������ֻ���ȴ�ӡ�������ط�
        const size_t TMP_ADDR_LEN = 64;
        char str_local_addr[TMP_ADDR_LEN], str_remote_addr[TMP_ADDR_LEN];

        int accept_error =  ZCE_OS::last_error();
        ZLOG_ERROR("[zergsvr] Local peer[%s|%u] Accept remote [%s|%u] handler fail! peer_acceptor_.accept ret =%d  errno=%d|%s ",
                   accept_bind_addr_.get_host_addr(str_local_addr, TMP_ADDR_LEN),
                   accept_bind_addr_.get_port_number(),
                   remote_address.get_host_addr(str_remote_addr, TMP_ADDR_LEN),
                   remote_address.get_port_number(),
                   ret,
                   accept_error,
                   strerror(accept_error));
        sockstream.close();

        //�������Щ���������
        if ( accept_error == EWOULDBLOCK || accept_error == EINVAL
             || accept_error == ECONNABORTED || accept_error == EPROTOTYPE )
        {

            return 0;
        }

        //���Ӧ���˳�����???,���Ǽ����ѡ�������
        //return -1;
        return 0;

    }

    ret = ip_restrict_->check_iprestrict(remote_address);

    if (ret != SOAR_RET::SOAR_RET_SUCC)
    {
        sockstream.close();
        return 0;
    }

    TCP_Svc_Handler *phandler = TCP_Svc_Handler::AllocSvcHandlerFromPool(TCP_Svc_Handler::HANDLER_MODE_ACCEPTED);

    if (phandler != NULL)
    {
        phandler->init_tcpsvr_handler(my_svc_info_, sockstream, remote_address, sessionkey_verify_);
    }
    else
    {
        sockstream.close();
    }

    return 0;
}
//
ZCE_HANDLE TCP_Accept_Handler::get_handle(void) const
{
    return (ZCE_HANDLE)peer_acceptor_.get_handle();
}

//
int TCP_Accept_Handler::handle_close ()
{
    //
    if (peer_acceptor_.get_handle () != ZCE_INVALID_SOCKET)
    {
        reactor()->remove_handler (this, false);
        peer_acceptor_.close ();
    }

    //ɾ���Լ�
    delete this;

    return 0;
}

