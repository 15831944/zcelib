#include "ogre_predefine.h"
#include "ogre_udp_ctrl_handler.h"
#include "ogre_buf_storage.h"
#include "ogre_configure.h"
#include "ogre_ip_restrict.h"

//����UPD�˿ڵľ��
std::vector<OgreUDPSvcHandler *> OgreUDPSvcHandler::ary_upd_peer_;

//���캯��
OgreUDPSvcHandler::OgreUDPSvcHandler(const ZEN_Sockaddr_In &upd_addr, ZEN_Reactor *reactor):
    ZEN_Event_Handler(reactor),
    udp_bind_addr_(upd_addr),
    peer_svc_info_(upd_addr.get_ip_address(), upd_addr.get_port_number()),
    dgram_databuf_(NULL),
    ip_restrict_(Ogre4aIPRestrictMgr::instance())
{
    //���浽PEER����
    ary_upd_peer_.push_back(this);
}

//��������
OgreUDPSvcHandler::~OgreUDPSvcHandler()
{
    if (dgram_databuf_ != NULL)
    {
        Ogre_Buffer_Storage::instance()->free_byte_buffer(dgram_databuf_);
    }

    //ɾ���������PEER����
    std::vector<OgreUDPSvcHandler *>::iterator iter_tmp = ary_upd_peer_.begin();
    std::vector<OgreUDPSvcHandler *>::iterator iter_end = ary_upd_peer_.begin();

    for (; iter_tmp != iter_end; ++iter_tmp)
    {
        if (*iter_tmp == this)
        {
            ary_upd_peer_.erase(iter_tmp);
            break;
        }
    }
}

//��ʼ��һ��UDP PEER
int OgreUDPSvcHandler::InitUDPServices()
{

    dgram_databuf_ = Ogre_Buffer_Storage::instance()->allocate_byte_buffer();

    int ret = 0;
    ret = dgram_peer_.open(&udp_bind_addr_);

    if (ret != 0)
    {
        handle_close();
        return -1;
    }

    ret = reactor()->register_handler (this, ZEN_Event_Handler::READ_MASK );

    if (ret != 0)
    {
        return -2;
    }

    return TSS_RET::TSS_RET_SUCC;
}

//��һ����ʱPEER���ڷ���UDP����
int OgreUDPSvcHandler::OpenUDPSendPeer()
{
    int ret = 0;

    if (ret != 0)
    {
        return TSS_RET::ERR_OGRE_INIT_UDP_PEER;
    }

    return TSS_RET::TSS_RET_SUCC;
}

//ȡ�þ��
ZEN_SOCKET OgreUDPSvcHandler::get_handle(void) const
{
    return dgram_peer_.get_handle();
}

int OgreUDPSvcHandler::handle_input(ZEN_HANDLE)
{
    size_t szrevc = 0;
    int ret = 0;
    ZEN_Sockaddr_In remote_addr;
    //��ȡ����
    ret = ReadDataFromUDP(szrevc, remote_addr);

    ZEN_LOGMSG_DBG(RS_DEBUG, "UDP Read Event[%s|%u].UPD Handle input event triggered. ret:%d,szrecv:%u.\n",
                   remote_addr.get_host_addr(),
                   remote_addr.get_port_number(),
                   ret,
                   szrevc);

    if (ret != TSS_RET::TSS_RET_SUCC)
    {
        //return -1�𣬵������治֪����δ���
        //return -1;
    }

    //��������ɹ�
    if (szrevc > 0)
    {
        pushdata_to_recvpipe();
    }

    return 0;
}

//
int OgreUDPSvcHandler::handle_close ()
{
    //
    if (dgram_peer_.get_handle () != ZEN_INVALID_SOCKET)
    {
        reactor()->remove_handler (this, false);
        dgram_peer_.close ();
    }

    //ɾ���Լ�
    delete this;

    return 0;
}

/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2007��7��18��
Function        : OgreUDPSvcHandler::ReadDataFromUDP
Return          : int == 0��ʾ�ɹ�
Parameter List  :
  Param1: size_t& szrevc �յ����ݵĳ���
  Param2: ZEN_Sockaddr_In &remote_addr Զ�˵ĵ�ַ
Description     : ��ȡUDP����
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int OgreUDPSvcHandler::ReadDataFromUDP(size_t &szrevc, ZEN_Sockaddr_In &remote_addr)
{
    int ret = 0;
    szrevc = 0;
    ssize_t recvret = 0;

    recvret = dgram_peer_.recvfrom(dgram_databuf_->frame_data_,
                                   Ogre4a_AppFrame::MAX_OF_OGRE_DATA_LEN,
                                   0,
                                   &remote_addr);

    if (recvret < 0)
    {

        if (ZEN_OS::last_error() != EWOULDBLOCK )
        {
            //�����ж�,�ȴ�����
            if (ZEN_OS::last_error() == EINVAL)
            {
                return TSS_RET::TSS_RET_SUCC;
            }

            //��¼����,���ش���
            ZLOG_ERROR( "UDP Read error [%s|%u],receive data error peer:%u ZEN_OS::last_error()=%d|%s.\n",
                        remote_addr.get_host_addr(),
                        remote_addr.get_port_number(),
                        dgram_peer_.get_handle(),
                        ZEN_OS::last_error(),
                        strerror(ZEN_OS::last_error()));
            return TSS_RET::ERR_OGRE_SOCKET_OP_ERROR;
        }
        else
        {
            return TSS_RET::TSS_RET_SUCC;
        }
    }

    //�����������ӵķ�������ַ�м�û��.�����ھܾ��ķ����б���... kill
    ret =  ip_restrict_->check_ip_restrict(remote_addr) ;

    if (ret != TSS_RET::TSS_RET_SUCC)
    {
        return ret;
    }

    //Socket���رգ�Ҳ���ش����ʾ,�����Ҳ�֪���᲻������������...
    if (recvret == 0)
    {
        ZLOG_ERROR( "UDP Read error [%s|%u].UDP Peer recv return 0, I don't know how to process.?\n",
                    remote_addr.get_host_addr(),
                    remote_addr.get_port_number());
        return TSS_RET::ERR_OGRE_SOCKET_CLOSE;
    }

    dgram_databuf_->snd_peer_info_.SetSocketPeerInfo(remote_addr);
    dgram_databuf_->rcv_peer_info_ = this->peer_svc_info_;
    dgram_databuf_->ogre_frame_len_ = Ogre4a_AppFrame::LEN_OF_OGRE_FRAME_HEAD + recvret;
    //���ⷢ����������д�����
    dgram_databuf_->ogre_frame_option_ = 0;
    dgram_databuf_->ogre_frame_option_ |= Ogre4a_AppFrame::OGREDESC_PEER_UDP;

    szrevc = recvret;

    return TSS_RET::TSS_RET_SUCC;
}

int OgreUDPSvcHandler::pushdata_to_recvpipe()
{

    int ret = Zerg_MMAP_BusPipe::instance()->push_back_bus(Zerg_MMAP_BusPipe::RECV_PIPE_ID,
                                                           reinterpret_cast<ZEN_LIB::dequechunk_node *>(dgram_databuf_));

    //���۴�����ȷ���,���ͷŻ������Ŀռ�

    //��־�ں����������,�����.
    if (ret != TSS_RET::TSS_RET_SUCC )
    {
        return TSS_RET::ERR_OGRE_RECEIVE_PIPE_IS_FULL;
    }

    return TSS_RET::TSS_RET_SUCC;
}

/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2007��7��18��
Function        : OgreUDPSvcHandler::SendAllDataToUDP
Return          : int
Parameter List  :
  Param1: Ogre4a_AppFrame* send_frame ���͵�FRAME
Description     : ����UDP���ݡ�
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int OgreUDPSvcHandler::SendAllDataToUDP(Ogre4a_AppFrame *send_frame)
{
    ssize_t szsend = -1;

    const size_t BUFFER_LEN = 32;
    char buffer[BUFFER_LEN + 1];

    ZEN_Sockaddr_In remote_addr(send_frame->rcv_peer_info_.peer_ip_address_,
                                send_frame->rcv_peer_info_.peer_port_);

    //
    size_t i = 0;

    for (; i < ary_upd_peer_.size(); ++i)
    {
        //�ҵ���Ӧ�Ķ˿ڣ��ж��UDP�Ķ˿ڣ�ѡ��ҵ����ע�Ķ˿ڷ���
        if (ary_upd_peer_[i]->peer_svc_info_ == send_frame->snd_peer_info_ )
        {
            szsend = ary_upd_peer_[i]->dgram_peer_.sendto(send_frame->frame_data_,
                                                          send_frame->ogre_frame_len_ - Ogre4a_AppFrame::LEN_OF_OGRE_FRAME_HEAD,
                                                          0,
                                                          &remote_addr);
            break;
        }
    }

    //û���ҵ���Ӧ�Ķ˿ڣ�����һ����Ϣ
    if (i == ary_upd_peer_.size())
    {
        ZLOG_ERROR( "Can't find send peer[%s|%u].Please check code.\n",
                    ZEN_OS::inet_ntoa(send_frame->snd_peer_info_.peer_ip_address_, buffer, BUFFER_LEN),
                    send_frame->snd_peer_info_.peer_port_);
        return TSS_RET::ERR_OGRE_SOCKET_OP_ERROR;
    }

    //����ʧ��
    if (szsend <= 0)
    {
        ZLOG_ERROR( "UDP send error[%s|%u]. Send data error peer:%u ZEN_OS::last_error()=%d|%s.\n",
                    remote_addr.get_host_addr(),
                    remote_addr.get_port_number(),
                    ary_upd_peer_[i]->get_handle(),
                    ZEN_OS::last_error(),
                    strerror(ZEN_OS::last_error()));
        return TSS_RET::ERR_OGRE_SOCKET_OP_ERROR;
    }

    ZEN_LOGMSG_DBG(RS_DEBUG, "UDP Send data to peer [%s|%u]  Socket %u bytes data Succ.\n",
                   remote_addr.get_host_addr(),
                   remote_addr.get_port_number(),
                   szsend);
    return TSS_RET::TSS_RET_SUCC;
}
