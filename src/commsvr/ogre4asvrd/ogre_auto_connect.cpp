

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
    for (size_t i = 0; i < ary_peer_fprecv_module_.size(); ++i)
    {
        if (ZCE_SHLIB_INVALID_HANDLE != ary_peer_fprecv_module_[i].recv_mod_handler_ )
        {
            ZCE_OS::dlclose(ary_peer_fprecv_module_[i].recv_mod_handler_);
        }
    }
}

/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2006��1��12��
Function        : Ogre_Connect_Server::get_configure
Return          : int
Parameter List  :
Description     :
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int Ogre_Connect_Server::get_configure(Zen_INI_PropertyTree &cfg_file)
{
    int ret = 0;

    unsigned int tmp_uint = 0;

    const size_t TMP_BUFFER_LEN = 256;
    char err_outbuf[TMP_BUFFER_LEN + 1] = {0};
    char tmp_key[TMP_BUFFER_LEN + 1] = {0};
    char tmp_value[TMP_BUFFER_LEN + 1] = {0};

    //
    ret = cfg_file.get_uint32_value("AUTOCONNECT", "NUMSVRINFO", tmp_uint);
    snprintf(err_outbuf, TMP_BUFFER_LEN, "AUTOCONNECT|NUMSVRINFO key error.");
    TESTCONFIG((ret == 0), err_outbuf);
    size_t numsvr =  tmp_uint;

    ary_peer_fprecv_module_.resize(numsvr);

    //ѭ���������е�����
    for (size_t i = 1; i <= numsvr; ++i)
    {
        snprintf(tmp_key, sizeof(tmp_key) - 1, "PEERIP%zu", i);
        ret = cfg_file.get_string_value("AUTOCONNECT", tmp_key, tmp_value, TMP_BUFFER_LEN);
        snprintf(err_outbuf, TMP_BUFFER_LEN, "AUTOCONNECT|%s key error.", tmp_key);
        TESTCONFIG((ret == 0 ), err_outbuf);

        snprintf(tmp_key, sizeof(tmp_key) - 1, "PEERPORT%zu", i);
        ret = cfg_file.get_uint32_value("AUTOCONNECT", tmp_key, tmp_uint);
        snprintf(err_outbuf, TMP_BUFFER_LEN, "AUTOCONNECT|%s key error.", tmp_key);
        TESTCONFIG((ret == 0 && tmp_uint != 0), err_outbuf);

        //�ҵ���ص�IP����
        ZCE_Sockaddr_In svraddr;
        ret = svraddr.set(tmp_value, static_cast<unsigned short>(tmp_uint));
        snprintf(err_outbuf, TMP_BUFFER_LEN, "AUTOCONNECT|PEERIP|%s key error.", tmp_key);
        TESTCONFIG((ret == 0 ), err_outbuf);

        snprintf(tmp_key, sizeof(tmp_key) - 1, "PEERMODULE%zu", i);
        ret = cfg_file.get_string_value("AUTOCONNECT", tmp_key, tmp_value, TMP_BUFFER_LEN);
        snprintf(err_outbuf, sizeof(err_outbuf) - 1, "AUTOCONNECT|%s key error.", tmp_key);
        TESTCONFIG((ret == 0 ), err_outbuf);

        //
        ary_peer_fprecv_module_[i - 1].peer_socket_info_.peer_ip_address_ = svraddr.get_ip_address();
        ary_peer_fprecv_module_[i - 1].peer_socket_info_.peer_port_ = svraddr.get_port_number();
        //
        ary_peer_fprecv_module_[i - 1].rec_mod_file_ = tmp_value;

    }

    ZLOG_INFO("Get AutoConnect Config Success.\n");

    return 0;
}

/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2007��12��14��
Function        : Ogre_Connect_Server::connect_all_server
Return          : int
Parameter List  :
  Param1: size_t& szserver Ҫ���ӵķ���������
  Param2: size_t& szsucc   �ɹ���ʼ���ӵķ���������,
Description     : �������еķ�����
Calls           :
Called By       :
Other           : ���ڶ����ӵķ�����,Ҳ����˵��һ�����Ӳ���,
Modify Record   :
******************************************************************************************/
int Ogre_Connect_Server::connect_all_server(size_t &szserver, size_t &szsucc)
{
    int ret = 0;

    size_t i = 0;
    szserver = ary_peer_fprecv_module_.size();

    //������еķ�������ģ��ʮ����ȷ
    for (i = 0; i < szserver; ++i)
    {
        ary_peer_fprecv_module_[i].recv_mod_handler_ = ZCE_OS::dlopen(ary_peer_fprecv_module_[i].rec_mod_file_.c_str());

        if ( ZCE_SHLIB_INVALID_HANDLE == ary_peer_fprecv_module_[i].recv_mod_handler_)
        {
            ZLOG_ERROR( "Open Module [%s] fail. recv_mod_handler =%u .\n", ary_peer_fprecv_module_[i].rec_mod_file_.c_str(), ary_peer_fprecv_module_[i].recv_mod_handler_);
            return SOAR_RET::ERROR_LOAD_DLL_OR_SO_FAIL;
        }

        ary_peer_fprecv_module_[i].fp_judge_whole_frame_ = (FPJudgeRecvWholeFrame)ZCE_OS::dlsym(ary_peer_fprecv_module_[i].recv_mod_handler_, StrJudgeRecvWholeFrame);

        if ( NULL == ary_peer_fprecv_module_[i].fp_judge_whole_frame_ )
        {
            ZLOG_ERROR( "Open Module [%s|%s] fail. recv_mod_handler =%u .\n", ary_peer_fprecv_module_[i].rec_mod_file_.c_str(), StrJudgeRecvWholeFrame, ary_peer_fprecv_module_[i].recv_mod_handler_);
            return SOAR_RET::ERROR_LOAD_DLL_OR_SO_FAIL;
        }
    }

    szsucc = 0;

    for (i = 0; i < szserver; ++i)
    {
        ret = connect_server_by_svrinfo(ary_peer_fprecv_module_[i].peer_socket_info_);

        if (ret == 0)
        {
            szsucc++;
        }
    }

    ZLOG_INFO( "Auto NONBLOCK Connect Server,Success Number :%d,Counter:%d .\n", szsucc, szserver);
    //���ؿ�ʼ���ӵķ���������
    return 0;
}

/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2005��11��15��
Function        : Ogre_Connect_Server::connect_server_by_svrinfo
Return          : int
Parameter List  :
  Param1: const Socket_Peer_Info& svrinfo
Description     :
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int Ogre_Connect_Server::connect_server_by_svrinfo(const Socket_Peer_Info &svrinfo)
{

    size_t i = 0;
    size_t ary_len = ary_peer_fprecv_module_.size();

    for (; i < ary_len; ++i)
    {
        if (ary_peer_fprecv_module_[i].peer_socket_info_ == svrinfo )
        {
            break;
        }
    }

    //���û���ҵ�
    if (ary_len == i)
    {
        return SOAR_RET::ERR_OGRE_NO_FIND_SERVICES_INFO;
    }

    ZCE_Sockaddr_In inetaddr(svrinfo.peer_ip_address_, svrinfo.peer_port_);

    ZCE_Socket_Stream tcpscoket;
    tcpscoket.sock_enable(O_NONBLOCK);

    ZLOG_INFO( "Try NONBLOCK connect server IP|Port :[%s|%u] .\n", inetaddr.get_host_addr(), inetaddr.get_port_number());

    //��ס,�����ʱ���־ʹSOCKET�첽����,
    int ret = ogre_connector_.connect(tcpscoket, &inetaddr, true);

    //��Ȼʧ��!?
    if (ret < 0)
    {
        //����UNIX������ V1��˵���� EINPROGRESS,��ACE�Ľ���˵�� EWOULDBLOCK,
        if (ZCE_OS::last_error() != EWOULDBLOCK && ZCE_OS::last_error() != EINPROGRESS )
        {
            tcpscoket.close();
            return SOAR_RET::ERR_OGRE_SOCKET_OP_ERROR;
        }

        //�ӳ�����ȡ��HDL����ʼ��֮��CONNECThdl������������С��0
        Ogre_TCP_Svc_Handler *connect_hdl = Ogre_TCP_Svc_Handler::alloc_svchandler_from_pool(Ogre_TCP_Svc_Handler::HANDLER_MODE_CONNECT);
        ZCE_ASSERT(connect_hdl);
        connect_hdl->init_tcp_svc_handler(tcpscoket, inetaddr, ary_peer_fprecv_module_[i].fp_judge_whole_frame_);

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

size_t Ogre_Connect_Server::num_svr_to_connect() const
{
    return ary_peer_fprecv_module_.size();
}

