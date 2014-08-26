#include "zerg_predefine.h"
#include "zerg_service_info_set.h"
#include "zerg_tcp_ctrl_handler.h"

/****************************************************************************************************
class  Service_Info_Set ,
��˼һ�£�
�Ƿ�Ӧ���������װ����Ҳ��ì�ܣ������TCP_Svc_Handlerֱ��ʹ��STL�ķ�װ���ݣ�
��ô���ǵķ�װ���پۺ͵ķ�ʽ������ô�ĺá���TCP_Svc_Handler��Ҫ�ٴη�װ��������
****************************************************************************************************/
Service_Info_Set::Service_Info_Set()
{
}

Service_Info_Set::~Service_Info_Set()
{
    //ZLOG_INFO("[zergsvr] Service_Info_Set::~Service_Info_Set.");
}

void Service_Info_Set::init_services_peerinfo(size_t max_accept, size_t max_connect)
{
    accept_svr_info_set_.resize(max_accept);
    connect_svr_info_set_.resize(max_connect);
}

//����SERVICEINFO��ѯPEER��Ϣ
int Service_Info_Set::find_services_peerinfo(const SERVICES_ID &svrinfo, TCP_Svc_Handler *&svrhandle)
{
    //�Ȳ������������ٲ��ұ�������
    MapOfSvrPeerInfo::iterator iter;
    if(((iter = connect_svr_info_set_.find(svrinfo)) != connect_svr_info_set_.end())
      ||((iter = accept_svr_info_set_.find(svrinfo)) != accept_svr_info_set_.end())) 
    {
        svrhandle = (*(iter)).second;
    }
    else
    {
        ZLOG_ERROR("[zergsvr] Can't find svchanle info. Svrinfo Type|ID:[%u|%u] .",
            svrinfo.services_type_,
            svrinfo.services_id_);
        return SOAR_RET::ERR_ZERG_NO_FIND_EVENT_HANDLE;
    }

    return SOAR_RET::SOAR_RET_SUCC;
}

int 
Service_Info_Set::find_connect_services_peerinfo(const SERVICES_ID &svrinfo, TCP_Svc_Handler*&svrhandle)
{
    //�Ȳ������������ٲ��ұ�������
    MapOfSvrPeerInfo::iterator iter;
    if(((iter = connect_svr_info_set_.find(svrinfo)) != connect_svr_info_set_.end())) 
    {
        svrhandle = (*(iter)).second;
    }
    else
    {
        ZLOG_ERROR("[zergsvr] Can't find auto connect svchanle info. Svrinfo Type|ID:[%u|%u] .",
            svrinfo.services_type_,
            svrinfo.services_id_);
        return SOAR_RET::ERR_ZERG_NO_FIND_EVENT_HANDLE;
    }

    return SOAR_RET::SOAR_RET_SUCC;
}

/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2007��5��29��
Function        : Service_Info_Set::replace_services_peerInfo
Return          : int
Parameter List  :
  Param1: const SERVICES_ID& svrinfo ԭ�͵�SVC INFO������
  Param2: TCP_Svc_Handler* new_svchdl  �����ӵķ�����
  Param3: TCP_Svc_Handler*& old_svchdl ���ԭ����һ��svrinfo��Ӧ��Hdler,����֪ͨ��,
Description     : ����PEER��Ϣ��
Calls           :
Called By       :
Other           : ���ص�old_svchdl,������������
Modify Record   :
******************************************************************************************/
int Service_Info_Set::replace_services_peerInfo(const SERVICES_ID &svrinfo,
                                                TCP_Svc_Handler *new_svchdl,
                                                TCP_Svc_Handler *&old_svchdl)
{
    MapOfSvrPeerInfo *svr_info_set = get_svr_peer_set(new_svchdl);

    old_svchdl = NULL;
    MapOfSvrPeerInfo::iterator iter = svr_info_set->find(svrinfo);

    //�Ѿ�����صļ�¼��
    if (iter != svr_info_set->end() )
    {
        old_svchdl = (*iter).second;

        const size_t TMP_ADDR_LEN = 32;
        char new_addr[TMP_ADDR_LEN], old_addr[TMP_ADDR_LEN];
        strncpy(new_addr, new_svchdl->get_peer_address(), TMP_ADDR_LEN);
        strncpy(old_addr, old_svchdl->get_peer_address(), TMP_ADDR_LEN);

        ZLOG_INFO("[zergsvr] replace_services_peerInfo:%u|%u ,Find Old IP|Port:[%s|%u],New IP Port[%s|%u],Replace old.",
                  svrinfo.services_type_,
                  svrinfo.services_id_,
                  old_addr,
                  old_svchdl->get_peer_port(),
                  new_addr,
                  new_svchdl->get_peer_port()
                 );
    }
    else
    {
        ZLOG_INFO("[zergsvr] add services peerInfo:%u|%u Succ,IP|Port:[%s|%u],Handler_mode:%u",
            svrinfo.services_type_,
            svrinfo.services_id_,
            new_svchdl->get_peer_address(),
            new_svchdl->get_peer_port(),
            new_svchdl->get_handle_mode());
    }

    (*svr_info_set)[svrinfo] = new_svchdl;

    return SOAR_RET::SOAR_RET_SUCC;
}

/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2007��5��29��
Function        : Service_Info_Set::add_services_peerinfo
Return          : int
Parameter List  :
Param1: const SERVICES_ID& svrinfo SVC INFO������
Param2: TCP_Svc_Handler* new_svchdl  �����ӵķ�����
Description     : ����PEER��Ϣ��
Calls           :
Called By       :
Other           : ���ص�old_svchdl,������������
Modify Record   :
******************************************************************************************/
int Service_Info_Set::add_services_peerinfo(const SERVICES_ID &svrinfo,
                                            TCP_Svc_Handler *new_svchdl)
{
    MapOfSvrPeerInfo *svr_info_set = get_svr_peer_set(new_svchdl);

    MapOfSvrPeerInfo::iterator iter = svr_info_set->find(svrinfo);

    //�Ѿ�����صļ�¼��
    if (iter != svr_info_set->end() )
    {
        TCP_Svc_Handler *old_svchdl = (*iter).second;

        //һ��������˼�����⵼���˴����������д���������ֱ��֪��Ϊʲô������ֱ����ScottxuҪ�����
        const size_t TMP_ADDR_LEN = 32;
        char new_addr[TMP_ADDR_LEN], old_addr[TMP_ADDR_LEN];
        strncpy(new_addr, new_svchdl->get_peer_address(), TMP_ADDR_LEN);
        strncpy(old_addr, old_svchdl->get_peer_address(), TMP_ADDR_LEN);

        ZLOG_ERROR("[zergsvr] add_services_peerinfo:%u|%u Fail,Find Old IP|Port:[%s|%u],New IP Port[%s|%u]",
                   svrinfo.services_type_,
                   svrinfo.services_id_,
                   old_addr,
                   old_svchdl->get_peer_port(),
                   new_addr,
                   new_svchdl->get_peer_port()
                  );
        return SOAR_RET::ERR_ZERG_SERVER_ALREADY_LONGIN;
    }

    ZLOG_INFO("[zergsvr] add services peerInfo:%u|%u Succ,IP|Port:[%s|%u],Handler_mode:%u",
        svrinfo.services_type_,
        svrinfo.services_id_,
        new_svchdl->get_peer_address(),
        new_svchdl->get_peer_port(),
        new_svchdl->get_handle_mode());

    (*svr_info_set)[svrinfo] = new_svchdl;

    return SOAR_RET::SOAR_RET_SUCC;
}

/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2006��3��22��
Function        : Service_Info_Set::del_services_peerInfo
Return          : size_t
Parameter List  :
Param1: const SERVICES_ID& svrinfo
Description     : ����SERVICES_ID,ɾ��PEER��Ϣ,
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
size_t Service_Info_Set::del_services_peerInfo(const SERVICES_ID &svrinfo,
                                               TCP_Svc_Handler *svchdl)
{
    MapOfSvrPeerInfo *svr_info_set = get_svr_peer_set(svchdl);

    MapOfSvrPeerInfo::iterator iter = svr_info_set->find(svrinfo);

    size_t szdel = svr_info_set->erase(svrinfo);

    //���û���ҵ�,99.99%������Ӧ���Ǵ���д��������,���ǲ���û�гɹ������.������handle_close.
    if (szdel <= 0)
    {
        ZLOG_ERROR("[zergsvr] del_services_peerInfo %u|%u Fail,svr_info_set size:%u szdel:%d.",
                  svrinfo.services_type_,
                  svrinfo.services_id_,
                  svr_info_set->size(),
                  szdel
                  );
    }
    else
    {
        ZLOG_INFO("[zergsvr] del_services_peerInfo %u|%u Succ,IP|Port:[%s|%u],Handler_mode:%u.",
            svrinfo.services_type_,
            svrinfo.services_id_,
            svchdl->get_peer_address(),
            svchdl->get_peer_port(),
            svchdl->get_handle_mode());
    }

    //zce_ASSERT(szdel >0 );
    return szdel;
}

//
size_t Service_Info_Set::get_services_peersize()
{
    return connect_svr_info_set_.size() + accept_svr_info_set_.size();
}

/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2006��10��17��
Function        : Service_Info_Set::dump_svr_peerinfo
Return          : void
Parameter List  :
  Param1: char* ret_string
  Param2: size_t& str_len
  Param3: size_t startno        ��ѯ
  Param4: size_t numquery       ��ѯ
Description     :
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
void Service_Info_Set::dump_svr_peerinfo(std::ostringstream &ostr_stream, size_t startno, size_t numquery)
{
    //�������������Ӻ����ӵ�˳��ɨ���ӡ
    size_t connect_size = connect_svr_info_set_.size();

    if (startno + numquery <= connect_size)
    {
        //��ӡ����ȫ��λ���������ӳ�
        dump_peerinfo_set(connect_svr_info_set_, ostr_stream, startno, numquery);
    }
    else if (startno >= connect_size)
    {
        //��ӡ����ȫ��λ�ڱ������ӳ�
        dump_peerinfo_set(accept_svr_info_set_, ostr_stream, startno - connect_size, numquery);
    }
    else
    {
        //��ӡ������һ����λ���������ӳ�, һ����λ�ڱ������ӳ�
        //�ȴ�ӡ�������ӳ�
        dump_peerinfo_set(connect_svr_info_set_, ostr_stream, startno, connect_size - startno);

        //�ٴ�ӡ�������ӳ�
        dump_peerinfo_set(accept_svr_info_set_, ostr_stream, 0, numquery - connect_size + startno);
    }

    return;
}

//�ر����е�PEER
void Service_Info_Set::clear_and_closeall()
{
    const size_t SHOWINFO_NUMBER = 500;

    ZLOG_INFO("[zergsvr] Has %u peer want to close. Please wait. ACE that is accursed.", connect_svr_info_set_.size());

    //������������Ǿ��Ե���
    while (connect_svr_info_set_.size() > 0)
    {
        if (connect_svr_info_set_.size() % SHOWINFO_NUMBER == 0 )
        {
            ZLOG_INFO("[zergsvr] Now remain %u peer want to close. Please wait. ACE that is accursed.", connect_svr_info_set_.size());
        }

        MapOfSvrPeerInfo::iterator iter = connect_svr_info_set_.begin();
        TCP_Svc_Handler *svrhandle = (*(iter)).second;

        //TCP_Svc_Handler::handle_close������del_services_peerInfo
        svrhandle->handle_close();
    }

    //������������Ǿ��Ե���
    while (accept_svr_info_set_.size() > 0)
    {
        if (accept_svr_info_set_.size() % SHOWINFO_NUMBER == 0 )
        {
            ZLOG_INFO("[zergsvr] Now remain %u peer want to close. Please wait. ACE that is accursed.", accept_svr_info_set_.size());
        }

        MapOfSvrPeerInfo::iterator iter = accept_svr_info_set_.begin();
        TCP_Svc_Handler *svrhandle = (*(iter)).second;

        //TCP_Svc_Handler::handle_close������del_services_peerInfo
        svrhandle->handle_close();
    }
}

Service_Info_Set::MapOfSvrPeerInfo* 
Service_Info_Set::get_svr_peer_set(TCP_Svc_Handler *svchdl)
{
    if (svchdl->get_handle_mode() == TCP_Svc_Handler::HANDLER_MODE_CONNECT 
        || svchdl->get_handle_mode() == TCP_Svc_Handler::HANDLER_MODE_CONNECT_EXTERNAL)
    {
        return &connect_svr_info_set_;
    }
    else
    {
        return &accept_svr_info_set_;
    }
}

void 
Service_Info_Set::dump_peerinfo_set(const MapOfSvrPeerInfo &peer_set, 
                                    std::ostringstream &ostr_stream, 
                                    size_t startno, 
                                    size_t numquery)
{
    MapOfSvrPeerInfo::const_iterator iter_tmp = peer_set.begin();
    MapOfSvrPeerInfo::const_iterator iter_end = peer_set.end();

    //����һ���ǳ��ǳ���ʱ�Ĳ���.
    for (size_t i = 0; iter_tmp != iter_end ; ++iter_tmp, ++i)
    {
        //��ѯ��ʼ
        if (i < startno)
        {
            continue;
        }

        //���ڲ�ѯ�ĸ�������
        if (numquery > 0 && i > numquery + startno )
        {
            return;
        }

        SERVICES_ID svr_info = (*(iter_tmp)).first;
        TCP_Svc_Handler *svrhandle = (*(iter_tmp)).second;
        svrhandle->dump_status_info(ostr_stream);
    }
}
