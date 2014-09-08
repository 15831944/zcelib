
#include "zerg_predefine.h"
#include "zerg_auto_connect.h"
#include "zerg_application.h"
#include "zerg_tcp_ctrl_handler.h"
#include "zerg_configure.h"


/****************************************************************************************************
class Zerg_Auto_Connector
****************************************************************************************************/
Zerg_Auto_Connector::Zerg_Auto_Connector() :
    size_of_wantconnect_(0),
    zerg_svr_cfg_(NULL)
{
}
//
Zerg_Auto_Connector::~Zerg_Auto_Connector()
{
}

//ȡ��������Ϣ
int Zerg_Auto_Connector::get_config(const Zerg_Server_Config *config)
{
    int ret = 0;

    zerg_svr_cfg_ = config;

    size_of_wantconnect_ = config->zerg_cfg_data_.auto_connect_num_;

    //ѭ���������е�����
    SERVICES_INFO svc_route;
    for (size_t i = 0; i < size_of_wantconnect_; ++i)
    {

        svc_route.svc_id_ = config->zerg_cfg_data_.auto_connect_svrs_[i];

        //�ҵ���ص�IP����
        ret = config->get_svcinfo_by_svcid(svc_route.svc_id_,
                                           svc_route);

        //�����ѯ����
        if (ret != 0)
        {
            ZLOG_ERROR("[zergsvr] Count find Auto Connect Services Info SvrType=%u,SvrID=%u .Please Check Config file. ",
                       svc_route.svc_id_.services_type_,
                       svc_route.svc_id_.services_id_);
            return SOAR_RET::ERR_ZERG_CONNECT_NO_FIND_SVCINFO;
        }

        ary_want_connect_.push_back(svc_route);


        //���ڸ�����C/C++�ķ��ؾ�ָ̬������⣬���Ҫ���������ַ������ֻ���ȴ�ӡ�������ط�
        const size_t TMP_ADDR_LEN = 32;
        char mainroute_addr[TMP_ADDR_LEN + 1];

        ZLOG_INFO("[zergsvr] Add one auto connect data, main route services id[%u|%u] ip[%s|%u].",
                  svc_route.svc_id_.services_type_,
                  svc_route.svc_id_.services_id_,
                  svc_route.ip_address_.get_host_addr(mainroute_addr, TMP_ADDR_LEN),
                  svc_route.ip_address_.get_port_number()
                 );
    }

    ZLOG_INFO("[zergsvr] Get number [%lu] auto connect config success.",
              size_of_wantconnect_);

    return 0;
}


//�������еķ�����,����Ѿ������ӣ�������,
void Zerg_Auto_Connector::reconnect_allserver(size_t &szvalid, size_t &szsucc, size_t &szfail)
{
    int ret = 0;
    szvalid = szsucc = szfail = 0;
    //ѭ�������е�SERVER����һ��,����д�ĺ��ƿ�

    for (size_t i = 0; i < size_of_wantconnect_; ++i)
    {
        TCP_Svc_Handler *svchandle = NULL;
        //����Ѿ�����Ӧ�������ˣ�����
        ret = TCP_Svc_Handler::find_services_peer(ary_want_connect_[i].svc_id_, svchandle);

        if (0 != ret)
        {
            //��������,
            ret = connect_server_bysvcid(ary_want_connect_[i].svc_id_, ary_want_connect_[i].ip_address_);

            if (ret == 0)
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
            if (svchandle->get_peer_status() == TCP_Svc_Handler::PEER_STATUS_ACTIVE)
            {
                svchandle->send_zergheatbeat_reg();
            }
        }
    }

    ZLOG_INFO("[zergsvr] Auto NONBLOCK connect server,vaild number:%d ,success Number :%d,fail number:%d .",
              szvalid,
              szsucc,
              szfail);
    return;
}


//����SVC ID,����Ƿ����������ӵķ���.,
int Zerg_Auto_Connector::reconnect_server(const SERVICES_ID &reconnect_svcid)
{

    for (size_t i = 0; i < size_of_wantconnect_; ++i)
    {
        if (ary_want_connect_[i].svc_id_ == reconnect_svcid)
        {
            ZCE_Sockaddr_In     inetaddr = ary_want_connect_[i].ip_address_;
            return connect_server_bysvcid(reconnect_svcid, inetaddr);
        }
    }

    //������������¼,���������SVCID��������Ҫ�������ӵķ�����
    return SOAR_RET::ERR_ZERG_ISNOT_CONNECT_SERVICES;
}


//����SVRINFO+IP,����Ƿ����������ӵķ���.����������
int Zerg_Auto_Connector::connect_server_bysvcid(const SERVICES_ID &svrinfo, const ZCE_Sockaddr_In     &inetaddr)
{
    ZLOG_DEBUG("[zergsvr] Try NONBLOCK connect services[%u|%u] IP|Port :[%s|%u] .",
               svrinfo.services_type_,
               svrinfo.services_id_,
               inetaddr.get_host_addr(),
               inetaddr.get_port_number()
              );

    ZCE_Socket_Stream sockstream;
    sockstream.open();

    const socklen_t opval = ZERG_SND_RCV_BUF_OPVAL;
    socklen_t opvallen = sizeof(socklen_t);
    //����һ��SND,RCV BUFFER,
    sockstream.setsockopt(SOL_SOCKET, SO_RCVBUF, reinterpret_cast<const void *>(&opval), opvallen);
    sockstream.setsockopt(SOL_SOCKET, SO_SNDBUF, reinterpret_cast<const void *>(&opval), opvallen);

    //tcpscoket.sock_enable (O_NONBLOCK);

    //��ס,�����ʱ���־ʹSOCKET�첽����,��3������true��ʾ�Ƿ�����
    int ret = zerg_connector_.connect(sockstream, &inetaddr, true);

    //��Ȼʧ��!?
    if (ret < 0)
    {
        //����UNIX������ V1��˵���� EINPROGRESS,��ACE�Ľ���˵�� EWOULDBLOCK,
        if (ZCE_LIB::last_error() != EWOULDBLOCK && ZCE_LIB::last_error() != EINPROGRESS)
        {
            sockstream.close();
            return SOAR_RET::ERR_ZERG_FAIL_SOCKET_OP_ERROR;
        }


        //HANDLER_MODE_CONNECTģʽ���۲���ʧ��
        TCP_Svc_Handler *p_handler = TCP_Svc_Handler::AllocSvcHandlerFromPool(
                                         TCP_Svc_Handler::HANDLER_MODE_CONNECT);
        ZCE_ASSERT(p_handler);
        //��self_svc_info��ȥ��������������.
        p_handler->init_tcpsvr_handler(zerg_svr_cfg_->self_svc_id_,
                                       svrinfo,
                                       sockstream,
                                       inetaddr);

        //����������ʱ��close���
        sockstream.release_noclose();
    }
    //tmpret == 0 �Ǿ�������ȥ��¥,������ UNIX������ ˵Ӧ�����б�������ʱ���ܵ�.(�ҵĲ��Ի��Ƿ��ش���)
    //��ACE��˵�����������ش���,����ʱ�������������,ʵ�ڲ�����ֻ�и�������д��ɬ������ʫ��
    else
    {
        ZLOG_ERROR("[zergsvr] My God! NonBlock Socket Connect Success , ACE is a cheat.");
    }

    return 0;
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
    unsigned int id_num = (unsigned int)list_of_want_connect_main_id_[index].size();
    for (unsigned int i = 0; i < id_num; i++)
    {
        unsigned int rand_num = rand() % id_num;

        //���ȿ���·��svr�Ƿ�������״̬
        svrinfo->set_svcid(svr_type, list_of_want_connect_main_id_[index][rand_num]);
        if (is_connected(*svrinfo))
        {
            return 0;
        }

        //��·�ɲ���������״̬, ������ѡ���·��svr
        svrinfo->set_svcid(svr_type, list_of_want_connect_back_id_[index][rand_num]);
        if (is_connected(*svrinfo))
        {
            return 0;
        }
    }

    //ZLOG_ERROR("[zergsvr] Not found auto connect");

    // δ�ҵ���Ӧ��Svr
    return SOAR_RET::ERR_ZERG_CHOOSE_AUTO_CONNECT_SERVICES;
}

bool Zerg_Auto_Connector::is_connected(const SERVICES_ID &svrinfo)
{
    TCP_Svc_Handler *svchandle = NULL;
    // ��������Ŀǰ����֧��udp
    int ret = TCP_Svc_Handler::find_services_peer(svrinfo, svchandle);

    if (ret != 0)
    {
        // û�ҵ�
        return false;
    }

    return true;
}



// ���¼���������������
int Zerg_Auto_Connector::reload_cfg(const Zerg_Server_Config *config)
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

    int ret = get_config(config);
    if (ret != 0)
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
    for (; iter != old_want_connect_type_2_index.end(); ++iter)
    {
        std::vector<unsigned int> &old_main_id_list = old_list_of_want_connect_main_id[iter->second];
        std::vector<unsigned int> &old_back_id_list = old_list_of_want_connect_back_id[iter->second];

        for (unsigned int i = 0; i < old_main_id_list.size(); ++i)
        {
            SERVICES_ID service(iter->first, old_main_id_list[i]);
            // �ж��Ƿ��ڵ�ǰ����������
            if (!is_current_auto_connect(service, true))
            {
                // ���ڵ�ǰ���������У���رյ�ǰ����
                ZLOG_INFO("zergsvr close old auto connect. svr_type=%u svr_id=%u",
                          service.services_type_, service.services_id_);
                TCP_Svc_Handler::close_services_peer(service);
            }
        }

        for (unsigned int i = 0; i < old_back_id_list.size() && old_back_id_list[i] != 0; ++i)
        {
            SERVICES_ID service(iter->first, old_back_id_list[i]);
            // �ж��Ƿ��ڵ�ǰ����������
            if (!is_current_auto_connect(service, false))
            {
                // ���ڵ�ǰ���������У���رյ�ǰ����
                TCP_Svc_Handler::close_services_peer(service);
            }
        }

    }

    return 0;
}

// �ж�һ��svr�Ƿ��ڵ�ǰ������������
bool Zerg_Auto_Connector::is_current_auto_connect(const SERVICES_ID &service,
                                                  bool is_main_service)
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
        list_id = &list_of_want_connect_main_id_[iter->second];
    }

    // ���б����Ƿ���ڶ�Ӧ��id
    for (unsigned int i = 0; i < list_id->size(); ++i)
    {
        if (list_id->at(i) == service.services_id_)
        {
            // svr_type��id��һ������Ҫ���һ�¶�Ӧ��ip�Ͷ˿��Ƿ��б仯
            TCP_Svc_Handler *svchandle = NULL;
            int ret = TCP_Svc_Handler::find_services_peer(service, svchandle);
            if (ret != 0)
            {
                // ������Ӳ�����
                return true;
            }

            // �ҵ�����Ƚ�һ��ip�Ͷ˿�
            SERVICES_INFO     svr_info;
            ret = zerg_svr_cfg_->get_svcinfo_by_svcid(service, svr_info);
            if (ret != 0)
            {
                // ������service�Ѿ��Ҳ�����Ӧ��IP���ã������Ѿ�ȥ����
                return false;
            }

            if (svr_info.ip_address_ != svchandle->get_peer_sockaddr())
            {
                // ��ַ�����
                return false;
            }

            return true;
        }
    }

    return false;
}

