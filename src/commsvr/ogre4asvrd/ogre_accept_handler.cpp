#include "ogre_predefine.h"
#include "ogre_accept_handler.h"
#include "ogre_tcp_ctrl_handler.h"
#include "ogre_configure.h"
#include "ogre_ip_restrict.h"

/****************************************************************************************************
class  OgreTCPAcceptHandler TCP Accept �����EventHandler,
****************************************************************************************************/
OgreTCPAcceptHandler::OgreTCPAcceptHandler(const ZCE_Sockaddr_In &listen_addr,
                                           const char *recv_mod_file,
                                           ZCE_Reactor *reactor):
    ZCE_Event_Handler(reactor),
    accept_bind_addr_(listen_addr),
    recv_mod_file_(recv_mod_file),
    recv_mod_handler_(ZCE_SHLIB_INVALID_HANDLE),
    fp_judge_whole_frame_(NULL),
    ip_restrict_(Ogre4aIPRestrictMgr::instance())
{
}

//�Լ���������ͣ�ͳһ�ر���handle_close,����ط����ùر�
OgreTCPAcceptHandler::~OgreTCPAcceptHandler()
{
    if (ZCE_SHLIB_INVALID_HANDLE != recv_mod_handler_)
    {
        ZCE_OS::dlclose(recv_mod_handler_);
    }
}

//
int OgreTCPAcceptHandler::create_listenpeer()
{

    //�����ⲿ.soЭ���ж�ģ��
    recv_mod_handler_ = ZCE_OS::dlopen(recv_mod_file_.c_str());

    if ( ZCE_SHLIB_INVALID_HANDLE == recv_mod_handler_)
    {
        ZLOG_ERROR( "Open Module [%s] fail. recv_mod_handler =%u .\n",
                    recv_mod_file_.c_str(),
                    recv_mod_handler_);
        return SOAR_RET::ERROR_LOAD_DLL_OR_SO_FAIL;
    }

    fp_judge_whole_frame_ = (FPJudgeRecvWholeFrame)ZCE_OS::dlsym(recv_mod_handler_, StrJudgeRecvWholeFrame);

    if ( NULL == fp_judge_whole_frame_)
    {
        ZLOG_ERROR( "Open Module [%s][%s] fail. recv_mod_handler =%u .\n",
                    recv_mod_file_.c_str(),
                    StrJudgeRecvWholeFrame,
                    recv_mod_handler_);
        return SOAR_RET::ERROR_LOAD_DLL_OR_SO_FAIL;
    }

    peer_acceptor_.sock_enable(O_NONBLOCK);

    socklen_t opval = 32 * 1024;
    socklen_t opvallen = sizeof(socklen_t);

    socklen_t sndbuflen, rcvbuflen;
    peer_acceptor_.getsockopt(SOL_SOCKET, SO_RCVBUF, reinterpret_cast<void *>(&rcvbuflen), &opvallen);
    peer_acceptor_.getsockopt(SOL_SOCKET, SO_SNDBUF, reinterpret_cast<void *>(&sndbuflen), &opvallen);
    ZLOG_INFO( "Get Listen Peer SO_RCVBUF:%u SO_SNDBUF %u.\n", rcvbuflen, sndbuflen);

    //����һ��SND,RCV BUFFER,
    peer_acceptor_.setsockopt(SOL_SOCKET, SO_RCVBUF, reinterpret_cast<void *>(&opval), opvallen);
    peer_acceptor_.setsockopt(SOL_SOCKET, SO_SNDBUF, reinterpret_cast<void *>(&opval), opvallen);

    peer_acceptor_.getsockopt(SOL_SOCKET, SO_RCVBUF, reinterpret_cast<void *>(&rcvbuflen), &opvallen);
    peer_acceptor_.getsockopt(SOL_SOCKET, SO_SNDBUF, reinterpret_cast<void *>(&sndbuflen), &opvallen);
    ZLOG_INFO( "Set Listen Peer SO_RCVBUF:%u SO_SNDBUF %u.\n", rcvbuflen, sndbuflen);

    int ret = peer_acceptor_.open(&accept_bind_addr_);

    //�������Bind��Ӧ�Ķ˿�
    if (ret != 0)
    {
        int last_err = ZCE_OS::last_error();
        ZLOG_ERROR( "Bind Listen IP|Port :[%s|%u] Fail.Error: %u|%s.\n",
                    accept_bind_addr_.get_host_addr(),
                    accept_bind_addr_.get_port_number(),
                    last_err,
                    strerror(last_err));
        return SOAR_RET::ERR_OGRE_INIT_LISTEN_PORT_FAIL;
    }

    ZLOG_INFO( "Bind listen IP|Port : [%s|%u] Success.\n",
               accept_bind_addr_.get_host_addr(),
               accept_bind_addr_.get_port_number());

    peer_acceptor_.getsockopt(SOL_SOCKET, SO_RCVBUF, reinterpret_cast<void *>(&rcvbuflen), &opvallen);
    peer_acceptor_.getsockopt(SOL_SOCKET, SO_SNDBUF, reinterpret_cast<void *>(&sndbuflen), &opvallen);
    ZLOG_INFO( "Get listen peer SO_RCVBUF:%u SO_SNDBUF %u.\n", rcvbuflen, sndbuflen);

    //����һ��SND,RCV BUFFER,
    peer_acceptor_.setsockopt(SOL_SOCKET, SO_RCVBUF, reinterpret_cast<void *>(&opval), opvallen);
    peer_acceptor_.setsockopt(SOL_SOCKET, SO_SNDBUF, reinterpret_cast<void *>(&opval), opvallen);

    peer_acceptor_.getsockopt(SOL_SOCKET, SO_RCVBUF, reinterpret_cast<void *>(&rcvbuflen), &opvallen);
    peer_acceptor_.getsockopt(SOL_SOCKET, SO_SNDBUF, reinterpret_cast<void *>(&sndbuflen), &opvallen);
    ZLOG_INFO( "Set Listen Peer SO_RCVBUF:%u SO_SNDBUF %u.\n", rcvbuflen, sndbuflen);

#ifndef WIN32
    //����DELAY�����������
    int NODELAY = 1;
    opvallen = sizeof(int);
    peer_acceptor_.setsockopt(SOL_TCP, TCP_NODELAY, reinterpret_cast<void *>(&NODELAY), opvallen);
#endif

    //
    reactor()->register_handler(this, ZCE_Event_Handler::ACCEPT_MASK);

    return 0;
}

//
int OgreTCPAcceptHandler::handle_input(ZCE_HANDLE /*handle*/)
{
    ZCE_Socket_Stream  sockstream;
    ZCE_Sockaddr_In   remoteaddress;
    int ret = 0;
    ret = peer_acceptor_.accept(sockstream, &remoteaddress);

    //������ִ���,��δ���? return -1?
    if (ret != 0)
    {

        sockstream.close();

        //��¼����
        int accept_error =  ZCE_OS::last_error();
        ZLOG_ERROR( "Accept [%s|%u] handler fail! peer_acceptor_.accept ret =%d  errno=%u|%s \n",
                    remoteaddress.get_host_addr(),
                    remoteaddress.get_port_number(),
                    ret,
                    accept_error,
                    strerror(accept_error));

        //�������Щ���������
        if ( accept_error == EWOULDBLOCK || accept_error == EINVAL
             || accept_error == ECONNABORTED || accept_error == EPROTOTYPE )
        {
            return 0;
        }

        //���Ӧ���˳�����
        //return -1;
        return 0;

    }

    //�����������ӵķ�������ַ�м�û��.�����ھܾ��ķ����б���... kill
    ret =  ip_restrict_->check_ip_restrict(remoteaddress) ;

    if (ret != 0)
    {
        return ret;
    }

    Ogre_TCP_Svc_Handler *phandler = Ogre_TCP_Svc_Handler::alloc_svchandler_from_pool(Ogre_TCP_Svc_Handler::HANDLER_MODE_ACCEPTED);

    if (phandler != NULL)
    {
        phandler->init_tcp_svc_handler(sockstream, fp_judge_whole_frame_);
    }
    else
    {
        sockstream.close();
    }

    return 0;
}
//
ZCE_SOCKET OgreTCPAcceptHandler::get_handle(void) const
{
    return peer_acceptor_.get_handle();
}

//���õ�ַ
void OgreTCPAcceptHandler::set_peer_bindaddr(ZCE_Sockaddr_In &addr)
{
    accept_bind_addr_ = addr;
}

//
int OgreTCPAcceptHandler::handle_close ()
{
    //
    if (peer_acceptor_.get_handle () != ZCE_INVALID_SOCKET)
    {
        reactor()->remove_handler (this, true);
        peer_acceptor_.close ();
    }

    //ɾ���Լ�
    delete this;

    return 0;
}

