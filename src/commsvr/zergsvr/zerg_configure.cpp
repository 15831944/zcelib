#include "zerg_predefine.h"
#include "zerg_configure.h"
#include "zerg_udp_ctrl_handler.h"
#include "zerg_tcp_ctrl_handler.h"

//�Լ��ķ������ı�ʾID,


/****************************************************************************************************
class  ZERG_SERVICES_INFO
****************************************************************************************************/
ZERG_SERVICES_INFO::ZERG_SERVICES_INFO():
    zerg_svc_info_(0, 0),
    zerg_ip_addr_("1.1.1.1", 6868)
{
}

ZERG_SERVICES_INFO::~ZERG_SERVICES_INFO()
{
}

//����ʾ��static��ʼ��
Zerg_Server_Config *Zerg_Server_Config::instance_ = NULL;
/****************************************************************************************************
class  Zerg_Server_Config
****************************************************************************************************/
Zerg_Server_Config::Zerg_Server_Config()
{
    //ͳ���ļ���Ĭ��λ��
    zerg_stat_file_ = "./ZERGLING.STAT";
}

Zerg_Server_Config::~Zerg_Server_Config()
{
}

//����SVCINFO�õ�IP��ַ��Ϣ
int Zerg_Server_Config::GetServicesIPInfo(const SERVICES_ID &svc_info,
                                          ZCE_Sockaddr_In     &ipaddr)
{
    int ret = 0;
    unsigned int idc_no;

    //��ӡһ��
    ZLOG_INFO("[zergsvr] Svc_info [%u|%u] ipaddress info [%s|%u].",
        svc_info.services_type_,
        svc_info.services_id_,
        ipaddr.get_host_addr(),
        ipaddr.get_port_number());

    return ret;
}

//Dump���õ�������Ϣ
void Zerg_Server_Config::dump_status_info(std::ostringstream &ostr_stream)
{
    ostr_stream << "Dump Zerg_Server_Config StatusInfo:" << std::endl;

    ostr_stream << std::setw(24) << "SELF_SVR_INFO :" << self_svc_info_.zerg_svc_info_.services_type_ << "|" << self_svc_info_.zerg_svc_info_.services_id_;
    ostr_stream << " SEESIONKEY:" << self_svc_info_.zerg_sessionkey_;
    ostr_stream << " IP:" << self_svc_info_.zerg_ip_addr_.get_host_addr() << "|" << self_svc_info_.zerg_ip_addr_.get_port_number() << std::endl;

    for (unsigned int i = 0; i < slave_svc_ary_.size(); ++i )
    {
        ostr_stream << "SLAVE_SVR_INFO :" << slave_svc_ary_[i].zerg_svc_info_.services_type_ << "|" << slave_svc_ary_[i].zerg_svc_info_.services_id_;
        ostr_stream << " SEESIONKEY:" << slave_svc_ary_[i].zerg_sessionkey_;
        ostr_stream << " IP:" << slave_svc_ary_[i].zerg_ip_addr_.get_host_addr() << "|" << slave_svc_ary_[i].zerg_ip_addr_.get_port_number() << std::endl;
    }

    ostr_stream << std::setw(24) << "RESTORE_PIPE:" << Server_Config_FSM::instance()->if_restore_pipe_ << std::endl;
    ostr_stream << std::setw(24) << "ZERG_CONFG_PATH:" << Server_Config_FSM::instance()->zerg_cfg_file_ << std::endl;
    ostr_stream << std::setw(24) << "ZERG_STAT_PATH:" << zerg_stat_file_ << std::endl;
    ostr_stream << std::setw(24) << "LOG_FILE_PREFIX :" << Server_Config_FSM::instance()->log_file_prefix_ << std::endl;
    //ostr_stream<<std::setw(24)<<"LOG_PRIORITY:"<<Comm_Svrd_Config::instance()->log_priority_<<std::endl;
}

//�õ�Ψһ�ĵ���ʵ��
Zerg_Server_Config *Zerg_Server_Config::instance()
{
    if (instance_ == NULL)
    {
        instance_ = new Zerg_Server_Config();
    }

    return instance_;
}

//�������ʵ��
void Zerg_Server_Config::clean_instance()
{
    if (instance_)
    {
        delete instance_;
    }

    instance_ = NULL;
    return;
}

int Zerg_Server_Config::init(const conf_zerg::ZERG_CONFIG *config)
{
    config_ = config;

    //
    int ret = 0;

    //unsigned int tmp_uint=0;
    std::string tmpstr;
    const size_t LEN_TMP_BUFFER = 256;
    char key_buf[LEN_TMP_BUFFER + 1] = {0};
    char err_buf[LEN_TMP_BUFFER + 1] = {0};
    char out_buf[LEN_TMP_BUFFER + 1] = {0};


    //TCP��ȡ����
    ret = TCP_Svc_Handler::get_tcpctrl_conf(config_);

    if (SOAR_RET::SOAR_RET_SUCC != ret )
    {
        return ret;
    }

    //UDP��ȡ����
    ret = UDP_Svc_Handler::get_udpctrl_conf(config_);

    if (SOAR_RET::SOAR_RET_SUCC != ret )
    {
        return ret;
    }

    //self_services_id_�ǻ����ȡ������
    self_svc_info_.zerg_svc_info_ = Server_Config_FSM::instance()->self_svr_id_;

    ZCE_Sockaddr_In     inetadd;
    snprintf(err_buf, LEN_TMP_BUFFER, "SELFCFG|SELFSVRTYPE|SELFSVRID key error.");
    ret = GetServicesIPInfo(self_svc_info_.zerg_svc_info_, inetadd);
    ZLOG_INFO("[zergsvr] Self svc id [%u|%u] ip address [%s] ",
              self_svc_info_.zerg_svc_info_.services_type_,
              self_svc_info_.zerg_svc_info_.services_id_,
              inetadd.get_host_addr_port(out_buf, LEN_TMP_BUFFER));
    TESTCONFIG((ret == 0 ), err_buf);
    //
    self_svc_info_.zerg_ip_addr_ = inetadd;



    //��ȡSLAVE�˿ڵ�����
    unsigned int slave_num = 0;
    slave_num = config_->self_cfg.slave_svr_count;
    TESTCONFIG((ret == 0 && slave_num <= 3 ), err_buf);

    //�����ж��Ƿ�Ҫ����ĳЩģ��
    if ( self_svc_info_.zerg_sessionkey_ == true)
    {
        zerg_need_opkey_ = true;

        //��¼�Ƿ�Ҫ����UDP SESESSION
        if (self_svc_info_.zerg_svc_info_.services_type_ > SVC_UDP_SERVER_BEGIN)
        {
            zerg_udp_session_ = true;
        }
    }

    snprintf(key_buf, LEN_TMP_BUFFER, "./ZERGLING.%u.%u.STAT",
             self_svc_info_.zerg_svc_info_.services_type_,
             self_svc_info_.zerg_svc_info_.services_id_);
    zerg_stat_file_ = key_buf;

    //
    ZLOG_INFO("[zergsvr] Slave peer number [%u].", slave_num);

    //���ô���BACKUP SERVICES
    slave_svc_ary_.clear();
    for (unsigned int i = 0; i < slave_num; ++i )
    {
        ZERG_SERVICES_INFO slave_svc_info;

        slave_svc_info.zerg_svc_info_.services_type_ = config_->self_cfg.slave_svrs[i].svr_type ;
        TESTCONFIG((ret == 0 && slave_svc_info.zerg_svc_info_.services_type_ != 0), err_buf);

        slave_svc_info.zerg_svc_info_.services_id_ = config_->self_cfg.slave_svrs[i].svr_id;
        TESTCONFIG((ret == 0 && slave_svc_info.zerg_svc_info_.services_id_ != 0), err_buf);

        ret = GetServicesIPInfo(slave_svc_info.zerg_svc_info_, inetadd);
        TESTCONFIG((ret == 0 ), err_buf);

        slave_svc_info.zerg_ip_addr_ = inetadd;

        slave_svc_info.zerg_sessionkey_
            = (config_->self_cfg.slave_svrs[i].use_encrypt == 1);
        slave_svc_info.is_recv_external_pkg_ = false;
        TESTCONFIG((ret == 0 ), err_buf);

        //�����ж��Ƿ�Ҫ����ĳЩģ��
        if (slave_svc_info.zerg_sessionkey_ == true)
        {
            zerg_need_opkey_ = true;

            //��¼�Ƿ�Ҫ����UDP SESESSION
            if ( slave_svc_info.zerg_svc_info_.services_type_ > SVC_UDP_SERVER_BEGIN )
            {
                zerg_udp_session_ = true;
            }
        }

        //
        ZLOG_INFO("[zergsvr] Slave [%u] svc id [%u|%u] [%s] ip address  [%s] . ",
                  i,
                  slave_svc_info.zerg_svc_info_.services_type_,
                  slave_svc_info.zerg_svc_info_.services_id_,
                  slave_svc_info.zerg_svc_info_.services_type_ > SVC_UDP_SERVER_BEGIN ? "UDP" : "TCP",
                  slave_svc_info.zerg_ip_addr_.get_host_addr_port(out_buf, LEN_TMP_BUFFER));

        //�������
        slave_svc_ary_.push_back(slave_svc_info);
    }

    // �����¸��ⲿ�ṩUdp����Ķ˿�
    external_udp_svr_ary_.clear();
    for (uint32_t i = 0; i < config_->udpsvr_num;  i ++)
    {
        ZERG_SERVICES_INFO external_udpsvr_info;
        inetadd.set(config_->udpsvr_cfg[i].ip, config_->udpsvr_cfg[i].udp_port);
        external_udpsvr_info.zerg_ip_addr_ = inetadd;
        external_udpsvr_info.is_recv_external_pkg_ = true;
        external_udpsvr_info.zerg_svc_info_.services_type_ = SVC_UDP_SERVER_BEGIN;
        external_udpsvr_info.zerg_svc_info_.services_id_ = 0;

        external_udpsvr_info.cmd_offset_ = config->udpsvr_cfg[i].cmd_offset;
        if (config->udpsvr_cfg[i].cmd_flag != 0)
        {
            // ���λ��������־λ
            external_udpsvr_info.cmd_offset_ |= 0x80000000;
        }

        external_udpsvr_info.cmd_len_ = config->udpsvr_cfg[i].cmd_len;

        // �������
        external_udp_svr_ary_.push_back(external_udpsvr_info);
    }

    // ��ʼ���ⲿ������Ϣ
    init_extern_svc();

    //SESSION KEY����Чʱ��,
    ZLOG_INFO("[zergsvr] Config session useful life %u.", SESSION_USEFUL_LIFE);

    ZLOG_INFO("[zergsvr] Get File Configure Success.");

    return SOAR_RET::SOAR_RET_SUCC;
}

int Zerg_Server_Config::init_extern_svc()
{
    extern_svc_ary_.clear();
    unsigned int extern_svc_num = config_->extern_svc_cfg.extern_svc_num;
    ZLOG_INFO("add extern svc, count=%d", extern_svc_num);
    for (unsigned int i = 0; i < extern_svc_num; ++i)
    {
        ZERG_SERVICES_INFO extern_svc_info;
        extern_svc_info.zerg_sessionkey_ = 0;
        extern_svc_info.zerg_svc_info_ = SERVICES_ID(0, 0);
        extern_svc_info.zerg_ip_addr_ = ZCE_Sockaddr_In(self_svc_info_.zerg_ip_addr_.get_ip_address(),
            config_->extern_svc_cfg.extern_svc_item[i].port);
        ZLOG_INFO("extern svc info, IP:%s|port:%u",
            extern_svc_info.zerg_ip_addr_.get_host_addr(),
            extern_svc_info.zerg_ip_addr_.get_port_number());
        extern_svc_ary_.push_back(extern_svc_info);
    }

    return SOAR_RET::SOAR_RET_SUCC;
}

