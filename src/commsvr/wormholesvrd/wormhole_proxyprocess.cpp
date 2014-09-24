#include "wormhole_predefine.h"
#include "wormhole_proxyprocess.h"
#include "wormhole_application.h"
#include "wormhole_stat_define.h"


//===================================================================================================
//class Interface_Proxy_Process
//===================================================================================================
Interface_WH_Proxy::Interface_WH_Proxy()
{
}
Interface_WH_Proxy::~Interface_WH_Proxy()
{
}

//����ӿ�����Ĺ���
Interface_WH_Proxy *Interface_WH_Proxy::create_proxy_factory(PROXY_TYPE proxytype)
{
    Interface_WH_Proxy *tmpintface = NULL;
    ZLOG_INFO("Interface_Proxy_Process::CreatePorxyFactory PROXY_TYPE: %d.", proxytype);

    switch (proxytype)
    {
        // ���Է�����
        case PROXY_TYPE_ECHO:
        {
            tmpintface = new Echo_Proxy_Process();
            break;
        }

        // DBPROXY��ģʽ������UINȡģ�ķ�ʽ�ĵ���������ID
        case PROXY_TYPE_DB_MODAL:
        {
            tmpintface = new DBModalProxyProcess();
            break;
        }

        // ͸תת���ķ�ʽ
        case PROXY_TYPE_TRANSMIT:
        {
            tmpintface = new Transmit_Proxy();
            break;
        }

        // �����ݽ��п����ַ��㲥
        case PROXY_TYPE_COPY_TRANS_ALL:
        {
            tmpintface = new CopyTransmitAllProxyProcess();
            break;
        }

        // DBPROXY��ģʽ������APPID��UIN�ķ�ʽ�ĵ���������ID
        case PROXY_TYPE_DB_MODAL_MG:
        {
            tmpintface = new DBModalMGProxyProcess();
            break;
        }

        default:
            // ����
            ZLOG_ERROR("Error Proxy Type define. Please check you code. ");
            return NULL;
    }

    return tmpintface;
}


int Interface_WH_Proxy::init_proxy_instance()
{
    // int ret =0;
    // ��ʼ��MMAP�ڴ��PIPE
    zerg_mmap_pipe_ = Zerg_MMAP_BusPipe::instance();

    return 0;
}


//��ȡ����
int Interface_WH_Proxy::get_proxy_config(const ZCE_Conf_PropertyTree *conf_tree)
{
    ZCE_UNUSED_ARG(conf_tree);
    return 0;
}


/****************************************************************************************************
 Echo_Proxy_Process ���ԵõĴ������������ PROXY_TYPE_ECHO
****************************************************************************************************/
Echo_Proxy_Process::Echo_Proxy_Process()
{

}
Echo_Proxy_Process::~Echo_Proxy_Process()
{

}

int Echo_Proxy_Process::get_proxy_config(const ZCE_Conf_PropertyTree *conf_tree)
{
    //
    int ret = Interface_WH_Proxy::get_proxy_config(conf_tree);
    if (ret != 0)
    {
        return ret;
    }

    return 0;
}


int Echo_Proxy_Process::process_proxy(Zerg_App_Frame *proc_frame)
{

    ZLOG_DEBUG("Receive a echo frame to process,"
               "send svr:[%u|%u], "
               "recv svr:[%u|%u], "
               "frame_uin:%u, "
               "frame_cmd:%u, "
               "frame_len:%u. ",
               proc_frame->send_service_.services_type_,
               proc_frame->send_service_.services_id_,
               proc_frame->recv_service_.services_type_,
               proc_frame->recv_service_.services_id_,
               proc_frame->frame_uid_,
               proc_frame->frame_command_,
               proc_frame->frame_length_);

    int ret = 0;
    // �ڲ����������
    bool bsnderr;

    if (proc_frame->is_internal_process(bsnderr) == true)
    {
        ZLOG_DEBUG("Receive a internal command, frame_uin:%u, frame_command:%u. ",
                   proc_frame->frame_uid_, proc_frame->frame_command_);
        return 0;
    }

    // �������֡
    proc_frame->exchange_rcvsnd_svcid();

    ret = zerg_mmap_pipe_->push_back_sendpipe(proc_frame);

    //
    if (ret != 0)
    {
        return SOAR_RET::ERR_PROXY_SEND_PIPE_IS_FULL;
    }

    ZLOG_DEBUG( "Echo to [%u|%u], frame_uin:%u, frame_command:%u, frame_len:%u. ",
                proc_frame->recv_service_.services_type_,
                proc_frame->recv_service_.services_id_,
                proc_frame->frame_uid_,
                proc_frame->frame_command_,
                proc_frame->frame_length_);
    return 0;
}




/****************************************************************************************************
class Transmit_Proxy ֱ�ӽ���ת��������������֡�����κδ���
****************************************************************************************************/
Transmit_Proxy::Transmit_Proxy()
{
}

Transmit_Proxy::~Transmit_Proxy()
{
}

int Transmit_Proxy::get_proxy_config(const ZCE_Conf_PropertyTree *conf_tree)
{
    //
    int ret = Interface_WH_Proxy::get_proxy_config(conf_tree);
    if (ret != 0)
    {
        return ret;
    }

    return 0;
}

int Transmit_Proxy::process_proxy(Zerg_App_Frame *proc_frame)
{
    ZLOG_DEBUG("Receive a transmit frame to process,"
        "send svr:[%u|%u], "
        "recv svr:[%u|%u], "
        "frame_uin:%u, "
        "frame_cmd:%u, "
        "frame_len:%u. ",
        proc_frame->send_service_.services_type_,
        proc_frame->send_service_.services_id_,
        proc_frame->recv_service_.services_type_,
        proc_frame->recv_service_.services_id_,
        proc_frame->frame_uid_,
        proc_frame->frame_command_,
        proc_frame->frame_length_);

    int ret = 0;

    // �ڲ����������,����
    bool bsnderr;

    if (proc_frame->is_internal_process(bsnderr) == true)
    {
        ZLOG_DEBUG("Receive a internal command, frame_uin:%u, frame_command:%u. ",
            proc_frame->frame_uid_, proc_frame->frame_command_);
        return 0;
    }

    ret = zerg_mmap_pipe_->push_back_sendpipe(proc_frame);

    //
    if (ret != 0)
    {
        return SOAR_RET::ERR_PROXY_SEND_PIPE_IS_FULL;
    }

    ZLOG_DEBUG("Transmit to [%u|%u], frame_uin:%u, frame_command:%u, frame_len:%u, trans_id[%u]. ",
        proc_frame->recv_service_.services_type_,
        proc_frame->recv_service_.services_id_,
        proc_frame->frame_uid_,
        proc_frame->frame_command_,
        proc_frame->frame_length_,
        proc_frame->transaction_id_);

    return 0;
}

/****************************************************************************************************
class DBModalProxyProcess ����DBȡģ����Proxyת��������DBServer�ͽ��ڷ����� PROXY_TYPE_DB_MODAL
****************************************************************************************************/
DBModalProxyProcess::DBModalProxyProcess():
    Interface_WH_Proxy()
{
}

DBModalProxyProcess::~DBModalProxyProcess()
{
    std::map<unsigned short, DBModalProxyInfo*>::iterator iter = dbmodal_proxy_map_.begin();

    for (; iter != dbmodal_proxy_map_.end(); iter++)
    {
        // �ͷŷ�����ڴ�
        delete iter->second;
        iter->second = NULL;
    }
}


int DBModalProxyProcess::get_proxy_config(const ZCE_Conf_PropertyTree *conf_tree)
{

    int ret = 0;

    //�õ����˵�����
    ret = Interface_WH_Proxy::get_proxy_config(conf_tree);
    if (ret != 0)
    {
        return ret;
    }

    for (unsigned int i = 0; i < cfg->dbmodal_info_.route_num_; i++)
    {
        DBModalProxyInfo *dbmodal_proxy_info = new DBModalProxyInfo();
        conf_proxysvr::RouteInfo *route_info = &(cfg->dbmodal_info_.route_info_[i]);

        dbmodal_proxy_info->distribute_module_ = route_info->distribute_module_;
        dbmodal_proxy_info->distribute_offset_ = route_info->distribute_offset_;
        dbmodal_proxy_info->router_svr_type_ = route_info->svr_type_;

        ZLOG_INFO("[DBModalProxy] route_svr_type:%u, distribute_offset:%u, distribute_module:%u",
            dbmodal_proxy_info->router_svr_type_,
            dbmodal_proxy_info->distribute_offset_,
            dbmodal_proxy_info->distribute_module_);

        dbmodal_proxy_info->normal_router_cfg_.resize(dbmodal_proxy_info->distribute_module_);
        dbmodal_proxy_info->clone_router_cfg_.resize(dbmodal_proxy_info->distribute_module_);

        for (unsigned int k = 0; k < dbmodal_proxy_info->distribute_module_; k++)
        {
            dbmodal_proxy_info->normal_router_cfg_[k] = route_info->svr_id_[k].nomal_service_id_;

            if (route_info->svr_id_[k].clone_service_id_)
            {
                dbmodal_proxy_info->clone_router_cfg_[k] = route_info->svr_id_[k].clone_service_id_;
            }
            else
            {
                dbmodal_proxy_info->clone_router_cfg_[k] = SERVICES_ID::INVALID_SERVICES_ID;
            }

            ZLOG_INFO("[DBModalProxy] normal service:%u|%u, clone service:%u|%u, passby service:%u|%u",
                dbmodal_proxy_info->router_svr_type_, dbmodal_proxy_info->normal_router_cfg_[k],
                dbmodal_proxy_info->router_svr_type_, dbmodal_proxy_info->clone_router_cfg_[k]);
        }

        dbmodal_proxy_map_.insert(std::make_pair<unsigned short, DBModalProxyInfo*>
            (dbmodal_proxy_info->router_svr_type_, dbmodal_proxy_info));
    }

    return 0;
}

/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2005��12��12��
Function        : DBModalProxyProcess::process_proxy
Return          : int
Parameter List  :
Param1: Zerg_App_Frame* proc_frame Ҫ�����֡
Description     :
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int DBModalProxyProcess::process_proxy(Zerg_App_Frame *proc_frame)
{
    ZLOG_DEBUG("Receive a dbmode frame to process,"
               "send svr:[%u|%u], "
               "recv svr:[%u|%u], "
               "frame_uin:%u, "
               "frame_cmd:%u, "
               "frame_len:%u. ",
               proc_frame->send_service_.services_type_,
               proc_frame->send_service_.services_id_,
               proc_frame->recv_service_.services_type_,
               proc_frame->recv_service_.services_id_,
               proc_frame->frame_uid_,
               proc_frame->frame_command_,
               proc_frame->frame_length_);

    int ret = 0;
    // �ڲ����������,����
    bool bsnderr;

    if (proc_frame->is_internal_process(bsnderr) == true)
    {
        ZLOG_INFO("Receive a internal command, frame_uin:%u, frame_command:%u. ",
                  proc_frame->frame_uid_, proc_frame->frame_command_);
        return 0;
    }

    std::map<unsigned short, DBModalProxyInfo*>::iterator iter =
        dbmodal_proxy_map_.find(proc_frame->recv_service_.services_type_);

    if (iter != dbmodal_proxy_map_.end())
    {
        // Ҫת��������������, ��ȡ��Ӧ·����Ϣ
        DBModalProxyInfo *dbmodal_proxy_info = iter->second;

        //------------------------------------------------------------------
        unsigned int uin = proc_frame->frame_uid_;

        // ���˵�uinΪ0������
        if (uin == 0 )
        {

            proc_frame->dumpoutput_framehead("[FROM RECV FRAME]", RS_ERROR);

            Comm_Stat_Monitor::instance()->increase_once(WORMHOLE_TRANS_PKG_ERROR);

            return SOAR_RET::ERROR_APPFRAME_ERROR;
        }

        // �ؼ����봦
        unsigned int mod =
            (uin >> dbmodal_proxy_info->distribute_offset_) % dbmodal_proxy_info->distribute_module_;

        // ------------------------------------------------------------------
        proc_frame->recv_service_.services_type_ = dbmodal_proxy_info->router_svr_type_;
        proc_frame->recv_service_.services_id_ =  dbmodal_proxy_info->normal_router_cfg_[mod];

        // ��־����ΪDEBUG�����
        ZLOG_DEBUG("Send to main services [%u|%u], frame_uin:%u, "
            "frame_command:%u, frame_len:%u, trans_id[%u]. ",
                   proc_frame->recv_service_.services_type_,
                   proc_frame->recv_service_.services_id_,
                   proc_frame->frame_uid_,
                   proc_frame->frame_command_,
                   proc_frame->frame_length_,
                   proc_frame->transaction_id_);

        // ֻ������һ��֡
        int ret = zerg_mmap_pipe_->push_back_sendpipe(proc_frame);

        //
        if (ret != 0)
        {
            return SOAR_RET::ERR_PROXY_SEND_PIPE_IS_FULL;
        }

        // ����б���·��,������ת����һ�����ݵĴ���
        if (dbmodal_proxy_info->clone_router_cfg_[mod] != SERVICES_ID::INVALID_SERVICES_ID )
        {

            proc_frame->recv_service_.services_id_ = dbmodal_proxy_info->clone_router_cfg_[mod];
            ZLOG_INFO("Send to backup services [%u|%u], frame_uin:%u,"
                " frame_command:%u, frame_len:%u, back trans_id[%u]. ",
                      proc_frame->recv_service_.services_type_,
                      proc_frame->recv_service_.services_id_,
                      proc_frame->frame_uid_,
                      proc_frame->frame_command_,
                      proc_frame->frame_length_,
                      proc_frame->backfill_trans_id_);

            ret = zerg_mmap_pipe_->push_back_sendpipe(proc_frame);

            if (ret != 0)
            {
                return SOAR_RET::ERR_PROXY_SEND_PIPE_IS_FULL;
            }
        }

    }
    // ����һ��·�ɷ�������飬ֱ��͸��
    else
    {
        //
        // ��һ����־���������
        ZLOG_INFO("Send back [%u|%u], frame_uin:%u, frame_command:%u, frame_len:%u. ",
                  proc_frame->recv_service_.services_type_,
                  proc_frame->recv_service_.services_id_,
                  proc_frame->frame_uid_,
                  proc_frame->frame_command_,
                  proc_frame->frame_length_);

        ret = zerg_mmap_pipe_->push_back_sendpipe(proc_frame);

        //
        if (ret != 0)
        {
            return SOAR_RET::ERR_PROXY_SEND_PIPE_IS_FULL;
        }
    }

    return 0;
}



/****************************************************************************************************
class CopyTransmitAllProxyProcess �����ݸ���ת�����������õķ����� PROXY_TYPE_COPY_TRANS_ALL
****************************************************************************************************/
CopyTransmitAllProxyProcess::CopyTransmitAllProxyProcess():
    Interface_WH_Proxy(),
    copytrans_svctype_(0),
    copytrans_svcnum_(0)
{
    memset(copytrans_svcid_, 0, sizeof(copytrans_svcid_));
}

CopyTransmitAllProxyProcess::~CopyTransmitAllProxyProcess()
{
}

int CopyTransmitAllProxyProcess::get_proxy_config()
{
    //
    int ret = Interface_WH_Proxy::get_proxy_config(cfg);

    if (ret != 0)
    {
        return ret;
    }

    unsigned int tmp_unit = 0;

    tmp_unit = cfg->copy_info_.svr_type_;
    ZLOG_DEBUG("Copy service type: %u.", tmp_unit);
    copytrans_svctype_ = static_cast<unsigned short>(tmp_unit);

    tmp_unit = cfg->copy_info_.copy_num_;
    ZLOG_DEBUG("Copy service num: %u.", tmp_unit);
    copytrans_svcnum_ = static_cast<size_t>(tmp_unit);

    const size_t SIZE_OF_BUF_LEN = 256;
    char tmp_buf[256], err_buf[256];

    // ��ȡ���е�·������
    for (unsigned int i = 0; i < copytrans_svcnum_; ++i)
    {
        snprintf(tmp_buf, SIZE_OF_BUF_LEN, "COPYSVCID%u", i + 1);
        tmp_unit = cfg->copy_info_.svr_id_[i].service_id_;

        snprintf(err_buf, SIZE_OF_BUF_LEN, "COPYTRANS|%s key error.", tmp_buf);
        TESTCONFIG(tmp_unit != 0 , err_buf);
        ZLOG_DEBUG("Copytrans%d: %u|%u.", i, copytrans_svctype_, tmp_unit);
        copytrans_svcid_[i] = tmp_unit;
    }

    // ����Ƿ����ظ���ID
    std::list <unsigned int> check_list;

    for (size_t i = 0; i < copytrans_svcnum_; ++i)
    {
        check_list.push_back(copytrans_svcid_[i]);
    }

    // ������Ч�ʵĵط�
    check_list.sort();
    check_list.unique();

    if (check_list.size() != copytrans_svcnum_ )
    {
        ZLOG_ERROR("Cfg file have repeat COPYSVCID,Please check.");
        return SOAR_RET::ERROR_GET_CFGFILE_CONFIG_FAIL;
    }

    return 0;
}

//
int CopyTransmitAllProxyProcess::process_proxy(Zerg_App_Frame *proc_frame)
{
    int ret = 0;

    // �����ͷ������
    proc_frame->dumpoutput_framehead("[FROM RECV FRAME]", RS_DEBUG);

    // �ڲ����������,����
    bool bsnderr;

    if (proc_frame->is_internal_process(bsnderr) == true)
    {
        ZLOG_DEBUG("Receive a internal command, frame_uin:%u, frame_command:%u. ",
                   proc_frame->frame_uid_, proc_frame->frame_command_);
        return 0;
    }

    // ���������Ƿ�ã��Ҳ�֪����
    if (proc_frame->recv_service_.services_type_ != copytrans_svctype_)
    {
        ZLOG_ERROR("Can't Porcess services_type_%u. ", proc_frame->recv_service_.services_type_);
        return SOAR_RET::ERR_PROXY_RCVSVC_TYPE_ERROR;
    }

    // ��������N��֡��ת������ͬ�ķ�����
    for (size_t i = 0; i < copytrans_svcnum_; ++i)
    {
        // �޸�Ϊ�µ�ID
        proc_frame->recv_service_.services_id_ = copytrans_svcid_[i];
        ret = zerg_mmap_pipe_->push_back_sendpipe(proc_frame);

        //
        if (ret != 0)
        {
            return ret;
        }

        ZLOG_DEBUG( "Copy to [%u|%u], frame_uin:%u, frame_command:%u, frame_len:%u, trans_id[%u]. ",
                    proc_frame->recv_service_.services_type_,
                    proc_frame->recv_service_.services_id_,
                    proc_frame->frame_uid_,
                    proc_frame->frame_command_,
                    proc_frame->frame_length_,
                    proc_frame->transaction_id_);
    }

    return 0;
}

const DBModalMGRouteItem *DBModalMGProxyInfo::find_route(unsigned int uin)
{
    if (route_cfg_.size() == 0)
    {
        // û������·��,��һ���Ǹ�����
        ZLOG_ERROR("[%s] no route configed", __ZCE_FUNC__);
        return NULL;
    }

    DBModalMGRouteItem tmp;
    // ȡuin�ĵ�16λ��Ϊhashֵ,Ӧ�ù������
    tmp.hash_ = (uin & 0xFFFF);

    std::vector<DBModalMGRouteItem>::iterator iter
        = std::upper_bound(route_cfg_.begin(), route_cfg_.end(), tmp);

    // ���ָ��·�ɱ��ĩβ,��ôʵ����Ӧ������·�ɱ�ĵ�һ��
    if(iter == route_cfg_.end())
    {
        iter = route_cfg_.begin();
    }

    return &(*iter);
}

/****************************************************************************************************
class DBModalMGProxyProcess ����APPID��UIN����Proxyת��������������ҵ�� PROXY_TYPE_DB_MODAL_MG
****************************************************************************************************/
DBModalMGProxyProcess::DBModalMGProxyProcess():
    Interface_WH_Proxy()
{
}

DBModalMGProxyProcess::~DBModalMGProxyProcess()
{
    clear_all_entrys();
}

int DBModalMGProxyProcess::get_proxy_config()
{
    int ret = 0;

    // �õ����˵�����
    ret = Interface_WH_Proxy::get_proxy_config(cfg);

    if (ret != 0)
    {
        return ret;
    }

    clear_all_entrys();

    for(size_t i = 0; i < cfg->dbmodal_info_mg_.entry_cnt_; i++)
    {
        for(size_t j = 0; j < cfg->dbmodal_info_mg_.entrys_[i].route_cnt_; j++)
        {
            DBModalMGProxyInfo *dbmodal_mg_proxy_info = add_proxy(&cfg->dbmodal_info_mg_.entrys_[i].route_info_[j]);

            if(cfg->dbmodal_info_mg_.entrys_[i].app_id_cnt_ > 0)
            {
                for(size_t k = 0; k < cfg->dbmodal_info_mg_.entrys_[i].app_id_cnt_; k++)
                {
                    ret = add_entry(cfg->dbmodal_info_mg_.entrys_[i].app_id_[k],
                        dbmodal_mg_proxy_info->router_svr_type_, dbmodal_mg_proxy_info);
                    if(ret != 0)
                    {
                        return ret;
                    }
                }
            }
            else
            {
                // û��ָ��appid����Ĭ��·��
                ret = add_entry(0, dbmodal_mg_proxy_info->router_svr_type_, dbmodal_mg_proxy_info);
                if(ret != 0)
                {
                    return ret;
                }
            }
        }
    }

    return ret;
}

int DBModalMGProxyProcess::process_proxy(Zerg_App_Frame *proc_frame)
{
    ZLOG_DEBUG("Receive a dbmodemg frame to process,"
               "send svr:[%u|%u], "
               "recv svr:[%u|%u], "
               "frame_uin:%u, "
               "frame_cmd:%u, "
               "frame_len:%u. ",
               proc_frame->send_service_.services_type_,
               proc_frame->send_service_.services_id_,
               proc_frame->recv_service_.services_type_,
               proc_frame->recv_service_.services_id_,
               proc_frame->frame_uid_,
               proc_frame->frame_command_,
               proc_frame->frame_length_);

    int ret = 0;
    // �ڲ����������,����
    bool bsnderr;

    if (proc_frame->is_internal_process(bsnderr) == true)
    {
        ZLOG_INFO("Receive a internal command, frame_uin:%u, frame_command:%u. ",
                  proc_frame->frame_uid_, proc_frame->frame_command_);
        return 0;
    }

    uint32_t recv_service;
    const DBModalMGRouteItem *route_item =
        find_proxy(proc_frame->app_id_, proc_frame->recv_service_.services_type_, proc_frame->frame_uid_, recv_service);

    if(route_item != NULL)
    {
        proc_frame->recv_service_.services_type_ = recv_service;
        proc_frame->recv_service_.services_id_ =  route_item->normal_router_;

        // ��־����ΪDEBUG�����
        ZLOG_DEBUG("Send to main services [%u|%u], frame_uin:%u, frame_command:%u, frame_len:%u, trans_id[%u]. ",
                   proc_frame->recv_service_.services_type_,
                   proc_frame->recv_service_.services_id_,
                   proc_frame->frame_uid_,
                   proc_frame->frame_command_,
                   proc_frame->frame_length_,
                   proc_frame->transaction_id_);

        // ֻ������һ��֡
        int ret = zerg_mmap_pipe_->push_back_sendpipe(proc_frame);

        //
        if (ret != 0)
        {
            return SOAR_RET::ERR_PROXY_SEND_PIPE_IS_FULL;
        }

        // ����б���·��,������ת����һ�����ݵĴ���
        if (route_item->clone_router_ != SERVICES_ID::INVALID_SERVICES_ID )
        {
            proc_frame->recv_service_.services_id_ =  route_item->clone_router_;
            ZLOG_INFO("Send to backup services [%u|%u], frame_uin:%u, frame_command:%u, frame_len:%u, back trans_id[%u]. ",
                      proc_frame->recv_service_.services_type_,
                      proc_frame->recv_service_.services_id_,
                      proc_frame->frame_uid_,
                      proc_frame->frame_command_,
                      proc_frame->frame_length_,
                      proc_frame->backfill_trans_id_);

            ret = zerg_mmap_pipe_->push_back_sendpipe(proc_frame);

            if (ret != 0)
            {
                return SOAR_RET::ERR_PROXY_SEND_PIPE_IS_FULL;
            }

        }

        // �������··��,������ת����һ����·�Ĵ���
        if (route_item->passby_router_ != SERVICES_ID::INVALID_SERVICES_ID )
        {
            proc_frame->recv_service_.services_id_ =  route_item->passby_router_;
            ZLOG_INFO("Send to passby services [%u|%u], frame_uin:%u, frame_command:%u, frame_len:%u, back trans_id[%u]. ",
                proc_frame->recv_service_.services_type_,
                proc_frame->recv_service_.services_id_,
                proc_frame->frame_uid_,
                proc_frame->frame_command_,
                proc_frame->frame_length_,
                proc_frame->backfill_trans_id_);

            ret = zerg_mmap_pipe_->push_back_sendpipe(proc_frame);

            if (ret != 0)
            {
                return SOAR_RET::ERR_PROXY_SEND_PIPE_IS_FULL;
            }

        }

    }
    //����һ��·�ɷ�������飬ֱ��͸��
    else
    {
        //
        //��һ����־���������
        ZLOG_INFO("Send back [%u|%u], frame_uin:%u, frame_command:%u, frame_len:%u. ",
                  proc_frame->recv_service_.services_type_,
                  proc_frame->recv_service_.services_id_,
                  proc_frame->frame_uid_,
                  proc_frame->frame_command_,
                  proc_frame->frame_length_);

        ret = zerg_mmap_pipe_->push_back_sendpipe(proc_frame);

        //
        if (ret != 0)
        {
            return SOAR_RET::ERR_PROXY_SEND_PIPE_IS_FULL;
        }
    }

    //else ELSE��������DB Modal PROXYֻ�ǽ�����DBһ������ݽ����޸ġ�


    return 0;
}

DBModalMGProxyInfo *DBModalMGProxyProcess::add_proxy(conf_proxysvr::RouteInfo *route_info)
{
    DBModalMGProxyInfo *dbmodal_mg_proxy_info = new DBModalMGProxyInfo();

    dbmodal_mg_proxy_info->router_svr_type_ = route_info->svr_type_;

    ZLOG_INFO("[DBModalMGProxy] route_svr_type:%u", dbmodal_mg_proxy_info->router_svr_type_);

    for(size_t i = 0; i < route_info->distribute_module_; i++)
    {
        DBModalMGRouteItem item;

        item.normal_router_ = route_info->svr_id_[i].nomal_service_id_;

        if(route_info->svr_id_[i].clone_service_id_)
        {
            item.clone_router_ = route_info->svr_id_[i].clone_service_id_;
        }
        else
        {
            item.clone_router_ = SERVICES_ID::INVALID_SERVICES_ID;
        }

        if(route_info->svr_id_[i].passby_service_id_)
        {
            item.passby_router_ = route_info->svr_id_[i].passby_service_id_;
        }
        else
        {
            item.passby_router_ = SERVICES_ID::INVALID_SERVICES_ID;
        }

        //����·��id����hash,�ܹؼ���һ��
        unsigned int svc_id = item.normal_router_;
        for(size_t i = 0; i < 100; i++, svc_id += 1000)
        {
            item.hash_ = ZCE_LIB::crc16((const unsigned char *)&svc_id, sizeof(svc_id));

            dbmodal_mg_proxy_info->route_cfg_.push_back(item);
        }
    }

    std::sort(dbmodal_mg_proxy_info->route_cfg_.begin(), dbmodal_mg_proxy_info->route_cfg_.end());

    for(size_t i = 0; i < dbmodal_mg_proxy_info->route_cfg_.size(); i++)
    {
        ZLOG_INFO("[DBModalMGProxy] service:%u|%u|%u|%u",
            dbmodal_mg_proxy_info->route_cfg_[i].hash_,
            dbmodal_mg_proxy_info->route_cfg_[i].normal_router_,
            dbmodal_mg_proxy_info->route_cfg_[i].clone_router_,
            dbmodal_mg_proxy_info->route_cfg_[i].passby_router_);
    }

    dbmodal_mg_proxys_.push_back(dbmodal_mg_proxy_info);

    return dbmodal_mg_proxy_info;
}

int DBModalMGProxyProcess::add_entry(uint32_t app_id, uint32_t service_type, DBModalMGProxyInfo *proxy_info)
{
    DBModalMGKey key;
    std::map<DBModalMGKey, DBModalMGProxyInfo*>::iterator itor;

    key.app_id_ = app_id;
    key.service_type_ = service_type;
    itor = dbmodal_mg_proxy_map_.find(key);
    if(itor != dbmodal_mg_proxy_map_.end())
    {
        ZLOG_ERROR("[%s] proxy entry is conflict, app id: %u, service_type: %u",
            __ZCE_FUNC__, app_id, service_type);
        return -1;
    }

    dbmodal_mg_proxy_map_.insert(std::map<DBModalMGKey, DBModalMGProxyInfo*>::value_type(key, proxy_info));

    ZLOG_INFO("[%s] add proxy entry succ, app id: %u, service_type: %u",
        __ZCE_FUNC__, app_id, service_type);

    return 0;
}

const DBModalMGRouteItem *DBModalMGProxyProcess::find_proxy(uint32_t app_id, uint32_t service_type, uint32_t uin, uint32_t &recv_service)
{
    DBModalMGKey key;
    std::map<DBModalMGKey, DBModalMGProxyInfo*>::iterator itor;

    //���ȳ��Բ���ֱ�����õ�·�ɱ�
    key.app_id_ = app_id;
    key.service_type_ = service_type;
    itor = dbmodal_mg_proxy_map_.find(key);
    if(itor != dbmodal_mg_proxy_map_.end())
    {
        recv_service = itor->second->router_svr_type_;
        return itor->second->find_route(uin);
    }

    //û���ҵ�ֱ�����õ�·�ɱ�,����ʹ��Ĭ��·�ɱ�
    key.app_id_ = 0;
    itor = dbmodal_mg_proxy_map_.find(key);
    if(itor != dbmodal_mg_proxy_map_.end())
    {
        recv_service = itor->second->router_svr_type_;
        return itor->second->find_route(uin);
    }

    //Ĭ��·�ɱ�Ҳ������,����ǳ�������
    ZLOG_ERROR("[%s] proxy entry is not exist, app id: %u, service_type: %u",
        __ZCE_FUNC__, app_id, service_type);

    return NULL;
}

void DBModalMGProxyProcess::clear_all_entrys()
{
    for(size_t i = 0; i < dbmodal_mg_proxys_.size(); i++)
    {
        delete dbmodal_mg_proxys_[i];
    }
    dbmodal_mg_proxys_.clear();
    dbmodal_mg_proxy_map_.clear();
}
