/******************************************************************************************
Copyright           : 2000-2004, Tencent Technology (Shenzhen) Company Limited.
FileName            : zerg_autoconnect.cpp
Author              : Sail(ZENGXING)//Author name here
Version             :
Date Of Creation    : 2005��11��17��
Description         :

Others              :
Function List       :
    1.  ......
Modification History:
    1.Date  :
      Author  :
      Modification  :
******************************************************************************************/
#include "zerg_predefine.h"
#include "zerg_auto_connect.h"
#include "zerg_application.h"
#include "zerg_tcp_ctrl_handler.h"
#include "zerg_udp_ctrl_handler.h"
#include "zerg_configure.h"
//#include "zerg_external_connect_handler.h"
#include "zerg_comm_manager.h"

/****************************************************************************************************
class SERVICES_ROUTE_INFO
****************************************************************************************************/
SERVICES_ROUTE_INFO::SERVICES_ROUTE_INFO():
    back_route_valid_(false)
{
}

SERVICES_ROUTE_INFO::~SERVICES_ROUTE_INFO()
{
}

/****************************************************************************************************
class Zerg_Auto_Connector
****************************************************************************************************/
Zerg_Auto_Connector::Zerg_Auto_Connector():
    size_of_wantconnect_(0)
{
    srand((unsigned int)time(NULL));
}
//
Zerg_Auto_Connector::~Zerg_Auto_Connector()
{
}

/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2006��1��12��
Function        : Zerg_Auto_Connector::get_autoconnect_cfg
Return          : int
Parameter List  :
Description     :
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int Zerg_Auto_Connector::get_autoconnect_cfg(const conf_zerg::ZERG_CONFIG *config)
{
    int ret = 0;
    SERVICES_ROUTE_INFO svc_route;

    int numsvr = (int)config->auto_connect_cfg.auto_connect_num;
    TESTCONFIG((numsvr >= 0 && ret == 0), "AUTOCONNECT|NUMSVRINFO key error.");

    //ѭ���������е�����
    for (int i = 0; i < numsvr; ++ i)
    {
        unsigned short svrtype = config->auto_connect_cfg.auto_connect_svrs[i].main_svr.svr_type;
        unsigned int svrid = config->auto_connect_cfg.auto_connect_svrs[i].main_svr.svr_id;

        svc_route.main_route_info_.set_serviceid(svrtype, svrid);

        //�ҵ���ص�IP����
        ZCE_Sockaddr_In     svr_ip_addr;
        ret = Zerg_Server_Config::instance()->GetServicesIPInfo(svc_route.main_route_info_, svr_ip_addr);

        //�����ѯ����
        if (ret != SOAR_RET::SOAR_RET_SUCC)
        {
            ZLOG_ERROR("[zergsvr] Count find Auto Connect Services Info SvrType=%u,SvrID=%u .Please Check Config file. ",
                       svrtype,
                       svrid);
            return SOAR_RET::ERR_ZERG_CONNECT_NO_FIND_SVCINFO;
        }

        svc_route.main_route_ip_ = svr_ip_addr;
        svc_route.back_route_valid_ = false;
        svc_route.back_route_info_.set_serviceid(0, 0);
        svc_route.back_route_ip_.set(static_cast<uint32_t>(0), 0);

        ZCE_Sockaddr_In     back_addr;

        if (config->auto_connect_cfg.auto_connect_svrs[i].backup_svr.svr_type != 0)
        {
            // �����˱��ݵģ���ʹ��
            unsigned short backtype = config->auto_connect_cfg.auto_connect_svrs[i].backup_svr.svr_type;

            //���뱣֤����·�ɵķ��������ͺ���·�ɷ��������ͷ���������һ��
            if (backtype != svc_route.main_route_info_.services_type_ )
            {
                ZLOG_ERROR("[zergsvr] Backup route services type [%u] not equal to main route services id[%u].",
                           backtype,
                           svc_route.main_route_info_.services_type_);
                return SOAR_RET::ERR_ZERG_CONNECT_NO_FIND_SVCINFO;
            }

            unsigned int backid = config->auto_connect_cfg.auto_connect_svrs[i].backup_svr.svr_id;

            if (backid != 0)
            {
                svc_route.back_route_info_.set_serviceid(backtype, backid);

                ret = Zerg_Server_Config::instance()->GetServicesIPInfo(svc_route.back_route_info_, svr_ip_addr);

                //�����ѯ����
                if (ret != SOAR_RET::SOAR_RET_SUCC)
                {
                    ZLOG_ERROR("[zergsvr] Count find Backup services Connect Services Info SvrType=%u,SvrID=%u .Please Check Config file. ", svrtype, svrid);
                    return SOAR_RET::ERR_ZERG_CONNECT_NO_FIND_SVCINFO;
                }

                svc_route.back_route_valid_ = true;
                svc_route.back_route_ip_ = svr_ip_addr;

            }
        }

        svc_route.external_= (config->auto_connect_cfg.auto_connect_svrs[i].external == 0) ? false : true;
        svc_route.proto_cfg_index_ = config->auto_connect_cfg.auto_connect_svrs[i].proto_cfg_index;

        ary_want_connect_.push_back(svc_route);

        //��type����������ӷ�����Ϣ
        add_auto_connect_info(svc_route.main_route_info_, svc_route.back_route_info_);

        //���ڸ�����C/C++�ķ��ؾ�ָ̬������⣬���Ҫ���������ַ������ֻ���ȴ�ӡ�������ط�
        const size_t TMP_ADDR_LEN = 32;
        char mainroute_addr[TMP_ADDR_LEN + 1], backroute_addr[TMP_ADDR_LEN + 1];

        ZLOG_INFO("[zergsvr] Add one auto connect data, main route services id[%u|%u] ip[%s|%u],it have %s backup route services id[%u|%u] ip[%s|%u]",
                  svc_route.main_route_info_.services_type_,
                  svc_route.main_route_info_.services_id_,
                  svc_route.main_route_ip_.get_host_addr(mainroute_addr, TMP_ADDR_LEN),
                  svc_route.main_route_ip_.get_port_number(),
                  svc_route.back_route_valid_ ? "" : "not",
                  svc_route.back_route_info_.services_type_,
                  svc_route.back_route_info_.services_id_,
                  svc_route.back_route_ip_.get_host_addr(backroute_addr, TMP_ADDR_LEN),
                  svc_route.back_route_ip_.get_port_number()
                 );
    }

    size_of_wantconnect_ = ary_want_connect_.size();
    ZLOG_INFO("[zergsvr] Get AutoConnect Config Success.");

    return SOAR_RET::SOAR_RET_SUCC;
}

/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2007��6��26��
Function        : Zerg_Auto_Connector::ConnectAllServer
Return          : void
Parameter List  :
  Param1: size_t& szsucc �ɹ���ʼ���ӵķ���������
  Param2: size_t& szfail ����ʧ�ܵķ����������������������첽���ӣ�����ط�����һ�����ǽ���������
Description     : �������еķ�����,����Ѿ������ӣ�������,
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
void Zerg_Auto_Connector::reconnect_tcpserver(size_t &szvalid, size_t &szsucc, size_t &szfail)
{
    int ret = 0;
    szvalid = szsucc = szfail = 0;
    //ѭ�������е�SERVER����һ��,����д�ĺ��ƿ�

    for (size_t i = 0; i < size_of_wantconnect_; ++i)
    {
        if (ary_want_connect_[i].main_route_info_.services_type_ < SVC_UDP_SERVER_BEGIN)
        {
            TCP_Svc_Handler *svchandle = NULL;
            //����Ѿ�����Ӧ�������ˣ�����
            ret = TCP_Svc_Handler::find_connect_services_peer(ary_want_connect_[i].main_route_info_, svchandle);

            if ( SOAR_RET::SOAR_RET_SUCC != ret)
            {
                //��������,
                ret = connect_server_bysvcid(ary_want_connect_[i].main_route_info_,
                                             ary_want_connect_[i].main_route_ip_,
                                             i);

                if (ret == SOAR_RET::SOAR_RET_SUCC)
                {
                    ++szsucc;
                }
                else
                {
                    ++szfail;
                }
            }
            else
            {
                ++szvalid;

                //����Ǽ���״̬�Ž�������������
                if (svchandle->get_peer_status() ==  TCP_Svc_Handler::PEER_STATUS_ACTIVE)
                {
                    if (!ary_want_connect_[i].external_)
                    {
                        svchandle->send_zerg_heart_beat_reg();
                    }
                }
            }

            if (ary_want_connect_[i].back_route_valid_)
            {
                svchandle = NULL;
                //����Ѿ�����Ӧ�������ˣ�����
                ret = TCP_Svc_Handler::find_connect_services_peer(ary_want_connect_[i].back_route_info_, svchandle);

                if ( SOAR_RET::SOAR_RET_SUCC != ret)
                {
                    //��������,
                    ret = connect_server_bysvcid(ary_want_connect_[i].back_route_info_,
                                                 ary_want_connect_[i].back_route_ip_,
                                                 i);

                    if (ret == SOAR_RET::SOAR_RET_SUCC)
                    {
                        ++szsucc;
                    }
                    else
                    {
                        ++szfail;
                    }
                }
                else
                {
                    ++szvalid;

                    //����Ǽ���״̬�Ž�������������
                    if (svchandle->get_peer_status() ==  TCP_Svc_Handler::PEER_STATUS_ACTIVE)
                    {
                        if (!ary_want_connect_[i].external_)
                        {
                            svchandle->send_zerg_heart_beat_reg();
                        }
                    }
                }
            }
        }
    }

    ZLOG_INFO("[zergsvr] Auto NONBLOCK connect server,vaild number:%d ,success Number :%d,fail number:%d .",
              szvalid,
              szsucc,
              szfail);
    return;
}

/******************************************************************************************
Author          : Sail(ZENGXING)  Date Of Creation: 2009��6��30��
Function        : Zerg_Auto_Connector::reconnect_server
Return          : int
Parameter List  :
  Param1: const SERVICES_ID& main_svrinfo Ҫ������������·����Ϣ
Description     : ����SVRINFO,����Ƿ����������ӵķ���.,
Calls           :
Called By       :
Other           : Ϊʲô����һ��TCP_Svc_Handler��Ϊ��������,��Ϊ�ڷ���Connect������,Ҳ����handle_close.
Modify Record   :
******************************************************************************************/
int Zerg_Auto_Connector::reconnect_server(const SERVICES_ID &reconnect_svrinfo)
{

    for (size_t i = 0; i < size_of_wantconnect_; ++i)
    {
        if (ary_want_connect_[i].main_route_info_ == reconnect_svrinfo ||
            (ary_want_connect_[i].back_route_valid_ == true && reconnect_svrinfo == ary_want_connect_[i].back_route_info_  ) )
        {
            ZCE_Sockaddr_In     inetaddr = ary_want_connect_[i].main_route_ip_;
            return connect_server_bysvcid(reconnect_svrinfo, inetaddr, i);
        }
    }

    //������������¼,���������SVCID��������Ҫ�������ӵķ�����
    return SOAR_RET::ERR_ZERG_ISNOT_CONNECT_SERVICES;
}

// ����udp����������ʵ��Ϊ�˴���socket��handler��Ȼ��ע�ᵽreactor����
void Zerg_Auto_Connector::reconnect_udpserver(size_t& szvalid, size_t& szsucc, size_t& szfail)
{
    int ret = 0;
    szvalid = szsucc = szfail = 0;

    for (size_t i = 0; i < size_of_wantconnect_; ++i)
    {
        // ����udp����������
        if (ary_want_connect_[i].main_route_info_.services_type_ >= SVC_UDP_SERVER_BEGIN)
        {
            UDP_Svc_Handler *svchandle = NULL;
            //����Ѿ�����Ӧ�������ˣ�����
            ret = UDP_Svc_Handler::find_services_peer(ary_want_connect_[i].main_route_info_, svchandle);

            if (ret != SOAR_RET::SOAR_RET_SUCC)
            {
                // û���ҵ�������
                ret = connect_udpserver_bysvcid(ary_want_connect_[i].main_route_info_,
                                                ary_want_connect_[i].main_route_ip_,
                                                (int)i);

                if (ret == SOAR_RET::SOAR_RET_SUCC)
                {
                    ++szsucc;
                }
                else
                {
                    ++szfail;
                }
            }
            else
            {
                // �ҵ���
                ++szvalid;
            }

            if (ary_want_connect_[i].back_route_valid_)
            {
                // ����б���
                svchandle = NULL;
                //����Ѿ�����Ӧ�������ˣ�����
                ret = UDP_Svc_Handler::find_services_peer(ary_want_connect_[i].back_route_info_, svchandle);

                if ( SOAR_RET::SOAR_RET_SUCC != ret)
                {
                    //��������,
                    ret = connect_udpserver_bysvcid(ary_want_connect_[i].back_route_info_,
                                                    ary_want_connect_[i].back_route_ip_,
                                                    (int)i);

                    if (ret == SOAR_RET::SOAR_RET_SUCC)
                    {
                        ++szsucc;
                    }
                    else
                    {
                        ++szfail;
                    }
                }
                else
                {
                    ++szvalid;
                }
            }
        }
    }

    ZLOG_INFO("[zergsvr] Auto connect udp server, vaild number:%d, success Number:%d, fail number:%d.",
              szvalid,
              szsucc,
              szfail);
    return;
}

//���������������õ����ݷ���������Ϣ
int Zerg_Auto_Connector::get_backupsvcinfo(const SERVICES_ID &main_svrinfo,
                                           bool &backroute_valid,
                                           SERVICES_ID &backroute_svrinfo)
{
    backroute_valid = false;

    for (size_t i = 0; i < size_of_wantconnect_; ++i)
    {
        if (ary_want_connect_[i].main_route_info_ == main_svrinfo)
        {
            ZCE_Sockaddr_In     inetaddr = ary_want_connect_[i].main_route_ip_;
            backroute_valid = ary_want_connect_[i].back_route_valid_;
            backroute_svrinfo = ary_want_connect_[i].back_route_info_;
            return SOAR_RET::SOAR_RET_SUCC;
        }
    }

    //������������¼,���������SVCID��������Ҫ�������ӵķ�����
    return SOAR_RET::ERR_ZERG_CONNECT_NO_FIND_SVCINFO;
}

//����SVRINFO+IP,����Ƿ����������ӵķ���.����������
int Zerg_Auto_Connector::connect_server_bysvcid(const SERVICES_ID &svrinfo,
                                                const ZCE_Sockaddr_In &inetaddr,
                                                size_t index)
{
    ZLOG_DEBUG("[zergsvr] Try NONBLOCK connect services[%u|%u] IP|Port :[%s|%u] .",
               svrinfo.services_type_,
               svrinfo.services_id_,
               inetaddr.get_host_addr(),
               inetaddr.get_port_number()
              );

    ZCE_Socket_Stream tcpscoket;
    tcpscoket.open();

    const socklen_t opval = ZERG_SND_RCV_BUF_OPVAL;
    socklen_t opvallen = sizeof(socklen_t);
    //����һ��SND,RCV BUFFER,
    tcpscoket.setsockopt(SOL_SOCKET, SO_RCVBUF, reinterpret_cast<const void *>(&opval), opvallen);
    tcpscoket.setsockopt(SOL_SOCKET, SO_SNDBUF, reinterpret_cast<const void *>(&opval), opvallen);

    //tcpscoket.sock_enable (O_NONBLOCK);

    //��ס,�����ʱ���־ʹSOCKET�첽����,
    int ret = zerg_connector_.connect(tcpscoket, &inetaddr, true);

    //��Ȼʧ��!?
    if (ret < 0)
    {
        //����UNIX������ V1��˵���� EINPROGRESS,��ACE�Ľ���˵�� EWOULDBLOCK,
        if (ZCE_OS::last_error() != EWOULDBLOCK && ZCE_OS::last_error() != EINPROGRESS )
        {
            tcpscoket.close();
            return SOAR_RET::ERR_ZERG_FAIL_SOCKET_OP_ERROR;
        }

        //HANDLER_MODE_CONNECTģʽ���۲���ʧ��
        TCP_Svc_Handler *p_handler = TCP_Svc_Handler::AllocSvcHandlerFromPool(TCP_Svc_Handler::HANDLER_MODE_CONNECT);
        zce_ASSERT(p_handler);
        //��self_svc_info��ȥ��������������.
        p_handler->init_tcpsvr_handler(Zerg_Server_Config::instance()->self_svc_info_.zerg_svc_info_,
                                       svrinfo,
                                       tcpscoket,
                                       inetaddr);

        if (ary_want_connect_[index].external_)
        {
            TcpHandlerImpl *tcp_handler_impl =
                Zerg_Comm_Manager::instance()->get_extern_proto_handler((unsigned int)ary_want_connect_[index].proto_cfg_index_);
            if (tcp_handler_impl == NULL)
            {
                ZLOG_ERROR("[zergsvr] External proto handler NULL, auto_index %u, proto_index %u, services[%u|%u] IP|Port :[%s|%u].",
                    index, ary_want_connect_[index].proto_cfg_index_,
                    svrinfo.services_type_,
                    svrinfo.services_id_,
                    inetaddr.get_host_addr(),
                    inetaddr.get_port_number());

                p_handler->handle_close();

                return SOAR_RET::ERR_ZERG_GET_EXTERNAL_PROTO_HANDLER;
            }

            p_handler->change_impl(tcp_handler_impl);
        }
    }
    //tmpret == 0 �Ǿ�������ȥ��¥,������ UNIX������ ˵Ӧ�����б�������ʱ���ܵ�.(�ҵĲ��Ի��Ƿ��ش���)
    //��ACE��˵�����������ش���,����ʱ�������������,ʵ�ڲ�����ֻ�и�������д��ɬ������ʫ��
    else
    {
        ZLOG_ERROR("[zergsvr] My God! NonBlock Socket Connect Success , ACE is a cheat.");
    }

    return SOAR_RET::SOAR_RET_SUCC;
}

int Zerg_Auto_Connector::connect_udpserver_bysvcid(const SERVICES_ID& svrinfo,
                                                   const ZCE_Sockaddr_In& inetaddr,
                                                   int index)
{
    ZLOG_DEBUG("[zergsvr] Try connect udp services[%u|%u] IP|Port[%s|%u].",
               svrinfo.services_type_,
               svrinfo.services_id_,
               inetaddr.get_host_addr(),
               inetaddr.get_port_number()
              );

    if (index < 0 || index >= (int)ary_want_connect_.size())
    {
        ZLOG_ERROR("[zergsvr] connect index is invalid. index=%d", index);
        return SOAR_RET::ERR_ZERG_CONNECT_NO_FIND_SVCINFO;
    }


    size_t proto_cfg_index = ary_want_connect_[index].proto_cfg_index_;

    UDP_Svc_Handler *p_handler = new UDP_Svc_Handler(Zerg_Server_Config::instance()->self_svc_info_.zerg_svc_info_,
                                                     svrinfo,
                                                     inetaddr,
                                                     ary_want_connect_[index].external_,
                                                     Zerg_Server_Config::instance()->config_->external_proto_cfg.item[proto_cfg_index].pkg_len_offset,
                                                     Zerg_Server_Config::instance()->config_->external_proto_cfg.item[proto_cfg_index].pkg_len_bytes);
    zce_ASSERT(p_handler);
    //��������������.
    return p_handler->init_udpsvr_handler();
}

size_t Zerg_Auto_Connector::numsvr_connect()
{
    return size_of_wantconnect_;
}

//����svr type��ȡserviceid���ж��idʱ�����ȡһ��
int Zerg_Auto_Connector::get_server(unsigned short svr_type, SERVICES_ID *svrinfo)
{
    //��map�л�ȡ��Ӧ��index
    RouteType2Index::iterator iter = want_connect_type_2_index_.find(svr_type);

    if (iter == want_connect_type_2_index_.end())
    {
        return SOAR_RET::ERR_ZERG_CHOOSE_AUTO_CONNECT_SERVICES;
    }

    unsigned int index = iter->second;

    if (index >= list_of_want_connect_main_id_.size())
    {
        //ZLOG_ERROR("[zergsvr] Not found auto connect");
        return SOAR_RET::ERR_ZERG_CHOOSE_AUTO_CONNECT_SERVICES;
    }

    //�����ȡһ��id, ����ظ�����Ϊid����
    unsigned int id_num = (unsigned int) list_of_want_connect_main_id_[index].size();

    for (unsigned int i = 0; i < id_num; i++)
    {
        unsigned int rand_num = rand() % id_num;

        //���ȿ���·��svr�Ƿ�������״̬
        svrinfo->set_serviceid(svr_type, list_of_want_connect_main_id_[index][rand_num]);

        if (is_connected(*svrinfo))
        {
            //ֻ��һ������ʱ, ���ж��Ƿ����
            //���һ�����ѡ��ʱ, Ҳ���ж��Ƿ����
            if (id_num > 1 && i != id_num - 1)
            {
                //�鿴�����Ƿ�������
                if (!is_socket_block(*svrinfo))
                {
                    return SOAR_RET::SOAR_RET_SUCC;
                }
                else
                {
                    ZLOG_COUNT_ERROR("MainServer [%u|%u] block and choose others",
                        svrinfo->services_type_, svrinfo->services_id_);
                    continue;
                }
            }
            else
            {
                return SOAR_RET::SOAR_RET_SUCC;
            }

        }

        //��·�ɲ���������״̬, ������ѡ���·��svr
        if (list_of_want_connect_back_id_[index][rand_num])
        {
            svrinfo->set_serviceid(svr_type, list_of_want_connect_back_id_[index][rand_num]);

            if (is_connected(*svrinfo))
            {
                if (id_num > 1)
                {
                    //�鿴�����Ƿ�������
                    if (!is_socket_block(*svrinfo))
                    {
                        return SOAR_RET::SOAR_RET_SUCC;
                    }
                    else
                    {
                        ZLOG_COUNT_ERROR("BackServer [%u|%u] block and choose others",
                            svrinfo->services_type_, svrinfo->services_id_);
                        continue;
                    }
                }
                else
                {
                    //ֻ��һ�����ӣ�����ҲҪѡ
                    return SOAR_RET::SOAR_RET_SUCC;
                }
            }
        }
    }

    //ZLOG_ERROR("[zergsvr] Not found auto connect");

    // δ�ҵ���Ӧ��Svr
    return SOAR_RET::ERR_ZERG_CHOOSE_AUTO_CONNECT_SERVICES;
}

//����svr type��uin hash��ʽ��ȡserviceid
int Zerg_Auto_Connector::get_server_byuinhash(unsigned short svr_type, SERVICES_ID *svrinfo, unsigned int uin)
{
    //��map�л�ȡ��Ӧ��index
    RouteType2Index::iterator iter = want_connect_type_2_index_.find(svr_type);

    if (iter == want_connect_type_2_index_.end())
    {
        return SOAR_RET::ERR_ZERG_CHOOSE_AUTO_CONNECT_SERVICES;
    }

    unsigned int index = iter->second;

    if (index >= list_of_want_connect_main_id_.size())
    {
        ZLOG_ERROR("[zergsvr] Not found auto connect in get_server_byuinhash");
        return SOAR_RET::ERR_ZERG_CHOOSE_AUTO_CONNECT_SERVICES;
    }

    //����ʵ��ȡģhash
    unsigned int id_num = (unsigned int) list_of_want_connect_main_id_[index].size();
    unsigned int rand = uin % id_num;

    //ֱ��ȡ��·��svr���������Ƿ������Ƿ������������Ǳ���·��
    svrinfo->set_serviceid(svr_type, list_of_want_connect_main_id_[index][rand]);

    return SOAR_RET::SOAR_RET_SUCC;
}

bool Zerg_Auto_Connector::is_connected(const SERVICES_ID &svrinfo)
{
    TCP_Svc_Handler *svchandle = NULL;
    // ��������Ŀǰ����֧��udp
    int ret = TCP_Svc_Handler::find_connect_services_peer(svrinfo, svchandle);

    if (ret != SOAR_RET::SOAR_RET_SUCC)
    {
        // û�ҵ�
        return false;
    }

    return true;
}

/******************************************************************************************
Author          : stefzhou  Date Of Creation: 2012��6��23��
Function        : Zerg_Auto_Connector::add_auto_connect_info
Return          : void
Parameter List  :
  Param1: const SERVICES_ID &main_svrinfo �������ӵ���·����Ϣ
  Param1: const SERVICES_ID &back_svrinfo �������ӵı���·����Ϣ
Description     : ��type�����������ӵķ�����Ϣ�����������ȡһ���������ӷ���
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
void Zerg_Auto_Connector::add_auto_connect_info(const SERVICES_ID &main_svrinfo,
                                                const SERVICES_ID &back_svrinfo)
{
    RouteType2Index::iterator iter = want_connect_type_2_index_.find(main_svrinfo.services_type_);

    // ����type�Ƿ��Ѿ���ӵ�map
    if (iter == want_connect_type_2_index_.end())
    {
        //typeû����ӵ�map��˵��Ϊ��һ���������ӷ���, ��Ҫ����һ���µ�����
        std::vector<unsigned int> main_id_list;
        std::vector<unsigned int> back_id_list;
        main_id_list.push_back(main_svrinfo.services_id_);
        back_id_list.push_back(back_svrinfo.services_id_);

        list_of_want_connect_main_id_.push_back(main_id_list);
        list_of_want_connect_back_id_.push_back(back_id_list);

        want_connect_type_2_index_.insert(std::make_pair<unsigned short, unsigned int>( \
                                                (unsigned short)main_svrinfo.services_type_, \
                                                (unsigned int)list_of_want_connect_main_id_.size() - 1));
    }
    else
    {
        //type�Ѿ���ӵ�map��˵�����ǵ�һ���������ӷ���, ֻ��Ҫ�������id��id�б�
        unsigned int index = iter->second;
        // ����Ϊ�β�ʹ�����ã�
        std::vector<unsigned int> main_id_list = list_of_want_connect_main_id_[index];
        std::vector<unsigned int> back_id_list = list_of_want_connect_back_id_[index];
        main_id_list.push_back(main_svrinfo.services_id_);
        back_id_list.push_back(back_svrinfo.services_id_);

        list_of_want_connect_main_id_[index] = main_id_list;
        list_of_want_connect_back_id_[index] = back_id_list;
    }
}

// ���¼���������������
int
Zerg_Auto_Connector::reload_cfg(const conf_zerg::ZERG_CONFIG *config)
{
    // ���¼�����������ʱ��δ�仯�����Ӳ��ܶϿ���ֻ�Ͽ�ȥ�������ӣ������¼ӵ�����
    // �ȱ������е���������
    RouteType2Index old_want_connect_type_2_index = want_connect_type_2_index_;
    ListOfMainRouteId old_list_of_want_connect_main_id = list_of_want_connect_main_id_;
    ListOfBackRouteId old_list_of_want_connect_back_id = list_of_want_connect_back_id_;

    // ������������
    list_of_want_connect_back_id_.clear();
    list_of_want_connect_main_id_.clear();
    want_connect_type_2_index_.clear();
    ary_want_connect_.clear();

    int ret = get_autoconnect_cfg(config);

    if (ret != SOAR_RET::SOAR_RET_SUCC)
    {
        // ���¼���ʧ�ܣ������û���
        ZLOG_ERROR("zergsvr reload autoconnect fail. ret=%d", ret);
        want_connect_type_2_index_ = old_want_connect_type_2_index;
        list_of_want_connect_main_id_ = old_list_of_want_connect_main_id;
        list_of_want_connect_back_id_ = old_list_of_want_connect_back_id;
        return ret;
    }

    // �ر���Щ�Ķ�������
    RouteType2Index::iterator iter = old_want_connect_type_2_index.begin();

    for (; iter != old_want_connect_type_2_index.end(); ++ iter)
    {
        std::vector<unsigned int> &old_main_id_list = old_list_of_want_connect_main_id[iter->second];
        std::vector<unsigned int> &old_back_id_list = old_list_of_want_connect_back_id[iter->second];

        for (unsigned int i = 0; i < old_main_id_list.size(); ++ i)
        {
            SERVICES_ID service(iter->first, old_main_id_list[i]);

            // �ж��Ƿ��ڵ�ǰ����������
            if (!is_current_auto_connect(service, true))
            {
                // ���ڵ�ǰ���������У���رյ�ǰ����
                ZLOG_INFO("zergsvr close old auto connect. svr_type=%u svr_id=%u",
                          service.services_type_, service.services_id_);
                TCP_Svc_Handler::close_connect_services_peer(service);
            }
        }

        for (unsigned int i = 0; i < old_back_id_list.size() && old_back_id_list[i] != 0; ++ i)
        {
            SERVICES_ID service(iter->first, old_back_id_list[i]);

            // �ж��Ƿ��ڵ�ǰ����������
            if (!is_current_auto_connect(service, false))
            {
                // ���ڵ�ǰ���������У���رյ�ǰ����
                TCP_Svc_Handler::close_connect_services_peer(service);
            }
        }

    }

    return SOAR_RET::SOAR_RET_SUCC;
}

// �ж�һ��svr�Ƿ��ڵ�ǰ������������
bool
Zerg_Auto_Connector::is_current_auto_connect(const SERVICES_ID &service, bool is_main_service)
{
    RouteType2Index::iterator iter = want_connect_type_2_index_.find(service.services_type_);

    if (iter == want_connect_type_2_index_.end())
    {
        return false;
    }

    // �ҵ���Ӧ��id�б�
    std::vector<unsigned int> *list_id = NULL;

    if (is_main_service)
    {
        list_id = &list_of_want_connect_main_id_[iter->second];
    }
    else
    {
        list_id = &list_of_want_connect_back_id_[iter->second];
    }

    // ���б����Ƿ���ڶ�Ӧ��id
    for (unsigned int i = 0; i < list_id->size(); ++ i)
    {
        if (list_id->at(i) == service.services_id_)
        {
            // svr_type��id��һ������Ҫ���һ�¶�Ӧ��ip�Ͷ˿��Ƿ��б仯
            TCP_Svc_Handler *svchandle = NULL;
            int ret = TCP_Svc_Handler::find_connect_services_peer(service, svchandle);

            if (ret != SOAR_RET::SOAR_RET_SUCC)
            {
                // ������Ӳ�����
                return true;
            }

            // �ҵ�����Ƚ�һ��ip�Ͷ˿�
            ZCE_Sockaddr_In     svr_ip_addr;
            ret = Zerg_Server_Config::instance()->GetServicesIPInfo(service, svr_ip_addr);

            if (ret != SOAR_RET::SOAR_RET_SUCC)
            {
                // ������service�Ѿ��Ҳ�����Ӧ��IP���ã������Ѿ�ȥ����
                return false;
            }

            if (svr_ip_addr != svchandle->get_peer_sockaddr())
            {
                // ��ַ�����
                return false;
            }

            return true;
        }
    }

    return false;
}

bool
Zerg_Auto_Connector::is_socket_block(const SERVICES_ID &svrinfo)
{
    TCP_Svc_Handler *svchandle = NULL;
    int ret = TCP_Svc_Handler::find_connect_services_peer(svrinfo, svchandle);

    if (ret != SOAR_RET::SOAR_RET_SUCC)
    {
        // û�ҵ�
        return true;
    }

    return svchandle->if_socket_block();
}

int Zerg_Auto_Connector::get_all_conn_server( unsigned short svr_type,
                                              std::vector<SERVICES_ID> &svrinfo_vec )
{
    //��map�л�ȡ��Ӧ��index
    RouteType2Index::iterator iter = want_connect_type_2_index_.find(svr_type);

    if (iter == want_connect_type_2_index_.end())
    {
        return SOAR_RET::ERR_ZERG_CHOOSE_AUTO_CONNECT_SERVICES;
    }

    unsigned int index = iter->second;

    if (index >= list_of_want_connect_main_id_.size())
    {
        //ZLOG_ERROR("[zergsvr] Not found auto connect");
        return SOAR_RET::ERR_ZERG_CHOOSE_AUTO_CONNECT_SERVICES;
    }

    //�����ȡһ��id, ����ظ�����Ϊid����
    unsigned int id_num = (unsigned int) list_of_want_connect_main_id_[index].size();

    for (unsigned int i = 0; i < id_num; i++)
    {
        SERVICES_ID svrinfo;

        //���ȿ���·��svr�Ƿ�������״̬
        svrinfo.set_serviceid(svr_type, list_of_want_connect_main_id_[index][i]);

        if (is_connected(svrinfo))
        {
            svrinfo_vec.push_back(svrinfo);
            continue;
        }

        //��·�ɲ���������״̬, ������ѡ���·��svr
        if (list_of_want_connect_back_id_[index][i])
        {
            svrinfo.set_serviceid(svr_type, list_of_want_connect_back_id_[index][i]);

            if (is_connected(svrinfo))
            {
                svrinfo_vec.push_back(svrinfo);
                continue;
            }
        }

        ZLOG_ERROR("[%s] fail get should connect svr, svrinfo:[%u|%u]",
            __zce_FUNCTION__,
            svrinfo.services_type_,
            svrinfo.services_id_);
    }

    return SOAR_RET::SOAR_RET_SUCC;
}



