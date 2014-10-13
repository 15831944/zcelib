

#include "ogre_predefine.h"
#include "ogre_auto_connect.h"
#include "ogre_application.h"
#include "ogre_tcp_ctrl_handler.h"

//
Ogre_Connect_Server::Ogre_Connect_Server()
{

}
//
Ogre_Connect_Server::~Ogre_Connect_Server()
{
    for (size_t i = 0; i < auto_connect_num_; ++i)
    {
        autocnt_module_ary_[i].close_module();
    }
}

//ȡ������
int Ogre_Connect_Server::get_config(const Ogre_Server_Config *config)
{
    int ret = 0;
    auto_connect_num_ = config->ogre_cfg_data_.auto_connect_num_;

    for (size_t i = 0; i < auto_connect_num_; ++i)
    {
        autocnt_module_ary_[i].peer_info_ = config->ogre_cfg_data_.auto_cnt_peer_ary_[i];

        autocnt_module_ary_[i].peer_id_.set(autocnt_module_ary_[i].peer_info_.peer_socketin_);
        ret = autocnt_module_ary_[i].open_module();
        if (ret != 0)
        {
            return ret;
        }
    }

    return 0;
}


//�������еķ�����
//Ҫ���ӵķ���������,�ɹ���ʼ���ӵķ���������,
int Ogre_Connect_Server::connect_all_server(size_t &szserver, size_t &szsucc)
{
    int ret = 0;

    szsucc = 0;
    szserver = 0;
    //������еķ�������ģ��ʮ����ȷ
    for (size_t i = 0; i < auto_connect_num_; ++i)
    {
        ret = connect_server_by_peerid(autocnt_module_ary_[i].peer_id_);
        ++szserver;
        if (ret == 0)
        {
            ++szsucc;
        }
    }
    
    ZLOG_INFO( "Auto NONBLOCK Connect Server,Success Number :%d,Counter:%d .\n", szsucc, szserver);
    //���ؿ�ʼ���ӵķ���������
    return 0;
}


int Ogre_Connect_Server::connect_server_by_peerid(const SOCKET_PERR_ID &socket_peer)
{

    size_t i = 0;
    for (; i < auto_connect_num_; ++i)
    {
        if (autocnt_module_ary_[i].peer_id_ == socket_peer)
        {
            break;
        }
    }

    //���û���ҵ�
    if (auto_connect_num_ == i)
    {
        return SOAR_RET::ERR_OGRE_NO_FIND_SERVICES_INFO;
    }

    ZCE_Sockaddr_In inetaddr(socket_peer.peer_ip_address_, socket_peer.peer_port_);

    ZCE_Socket_Stream tcpscoket;
    tcpscoket.sock_enable(O_NONBLOCK);

    ZLOG_INFO( "Try NONBLOCK connect server IP|Port :[%s|%u] .\n", 
        inetaddr.get_host_addr(), 
        inetaddr.get_port_number());

    //��ס,�����ʱ���־ʹSOCKET�첽����,
    int ret = ogre_connector_.connect(tcpscoket, &inetaddr, true);

    //��Ȼʧ��!?
    if (ret < 0)
    {
        //����UNIX������ V1��˵���� EINPROGRESS,��ACE�Ľ���˵�� EWOULDBLOCK,
        if (ZCE_LIB::last_error() != EWOULDBLOCK && ZCE_LIB::last_error() != EINPROGRESS )
        {
            tcpscoket.close();
            return SOAR_RET::ERR_OGRE_SOCKET_OP_ERROR;
        }

        //�ӳ�����ȡ��HDL����ʼ��֮��CONNECThdl������������С��0
        Ogre_TCP_Svc_Handler *connect_hdl = Ogre_TCP_Svc_Handler::alloc_svchandler_from_pool(Ogre_TCP_Svc_Handler::HANDLER_MODE_CONNECT);
        ZCE_ASSERT(connect_hdl);
        connect_hdl->init_tcp_svc_handler(tcpscoket, inetaddr, autocnt_module_ary_[i].fp_judge_whole_frame_);

    }
    //tmpret == 0 �Ǿ�������ȥ��¥,������ UNIX������ ˵Ӧ�����б�������ʱ���ܵ�.(�ҵĲ��Ի��Ƿ��ش���)
    //��ACE��˵�����������ش���,����ʱ�������������,ʵ�ڲ�����ֻ�и�������д��ɬ������ʫ��
    else
    {
        ZLOG_ERROR( "My God! NonBlock Socket Connect Success , ACE is a cheat.\n");
        ZLOG_ERROR( "My God! NonBlock Socket Connect Success , ACE is a cheat....\n");
    }

    return 0;
}


