
#include "soar_predefine.h"
#include "soar_error_code.h"
#include "soar_enum_define.h"
#include "soar_zerg_svc_table.h"

/****************************************************************************************************
class  ServicesConfig  ����������Ϣ��,ͨ��SERVICEINFO���ҵ�IP��ַ���˿ںŵ�����,
****************************************************************************************************/
Services_Table_Config::Services_Table_Config(size_t szconf)
{
    services_table_.rehash(szconf);
}

Services_Table_Config::~Services_Table_Config()
{
}


int Services_Table_Config::find_svcinfo(const SERVICES_ID &svrinfo,
                                       unsigned int &ipaddress,
                                       unsigned short &port,
                                       unsigned int &idc_no,
                                       unsigned int &server_guid_no,
                                       char *cfg_info) const
{
    SERVICES_INFO svc_ip_info;
    svc_ip_info.services_id_ = svrinfo;
    Set_Of_SvcInfo::iterator iter = services_table_.find(svc_ip_info);

    if (iter == services_table_.end() )
    {
        return SOAR_RET::ERROR_SERVICES_INFO_CONFIG;
    }

    ipaddress = iter->ip_addr_;
    port = iter->port_;
    idc_no = iter->idc_no_;
    server_guid_no = iter->server_guid_no_;

    //
    if (cfg_info)
    {
        strncpy(cfg_info, iter->cfg_info_, MAX_NAME_LEN_STRING);
        cfg_info[MAX_NAME_LEN_STRING] = '\0';
    }

    return SOAR_RET::SOAR_RET_SUCC;
}


//����SvrInfo��Ϣ��ѯIP������Ϣ
int Services_Table_Config::find_svcinfo(const SERVICES_ID &svc_id,
    SERVICES_INFO &svc_info) const
{
    svc_info.services_id_ = svc_id;
    Set_Of_SvcInfo::iterator iter = services_table_.find(svc_info);

    if (iter == services_table_.end())
    {
        return SOAR_RET::ERROR_SERVICES_INFO_CONFIG;
    }
    svc_info = *iter;

    return SOAR_RET::SOAR_RET_SUCC;
}

//����Ƿ�ӵ����Ӧ��Services Info
bool Services_Table_Config::hash_svcinfo(const SERVICES_ID &svrinfo) const
{
    SERVICES_INFO svc_ip_info;
    svc_ip_info.services_id_ = svrinfo;
    Set_Of_SvcInfo::iterator iter = services_table_.find(svc_ip_info);

    if (iter == services_table_.end() )
    {
        return false;
    }

    return true;
}

//����������Ϣ
int Services_Table_Config::add_svcinfo(const SERVICES_INFO &svc_info)
{

    std::pair<Set_Of_SvcInfo::iterator, bool> insert_result = services_table_.insert(svc_info);

    if (insert_result.second == false)
    {
        return SOAR_RET::ERROR_SERVICES_INFO_CONFIG;
    }

    return SOAR_RET::SOAR_RET_SUCC;
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2005��11��17��
Function        : ServicesConfig::LoadSvrConfig
Return          : int == 0��ʾ�ɹ�
Parameter List  :
  Param1: const char* cfgfile
Description     : �õ�ĳ�����������ļ���������Ϣ
Calls           :
Called By       :
Other           : �����ļ�������[SERVICESINFO]�ֶ�.ԭ�е����ݻ����.
Modify Record   :
******************************************************************************************/
//int ServicesConfig::LoadSvrConfig(const char *cfgfile)
//{
//    int ret = 0;
//    const int BUFFER_LEN = 128;
//    char tmpkey[BUFFER_LEN],tmpbuf[BUFFER_LEN],outbuf[BUFFER_LEN];
//
//    ZCE_INI_Implemention   read_cfg;
//    zce_INI_PropertyTree   svrinfo_cfg;
//
//    ZLOG_INFO("[framework] Read Sevices Info Configure File. [%s] .",cfgfile);
//    ret = read_cfg.read(cfgfile,svrinfo_cfg);
//    snprintf(outbuf,BUFFER_LEN,"Can't Open Sevices Info Configure file %s.",cfgfile);
//    TESTCONFIG(ret == 0,outbuf);
//
//    uint32_t num_svr;
//    ret = svrinfo_cfg.get_uint32_value("SERVICESINFO","NUMSVRINFO",num_svr);
//    TESTCONFIG(( ret == 0 && num_svr != 0),"SERVICESINFO|NUMSVRINFO key error.");
//
//
//    uint32_t int_value;
//
//    SERVICES_IP_INFO svc_ip_info;
//
//    //�����ԭ��������.
//    services_conf_map_.clear();
//
//    for (uint32_t i=1;i<=num_svr;++i)
//    {
//        //��¼���������
//        svc_ip_info.server_guid_no_ = i;
//
//        snprintf(tmpkey,BUFFER_LEN,"SERVICESTYPE%u",i);
//        ret = svrinfo_cfg.get_uint32_value("SERVICESINFO",tmpkey,int_value);
//        snprintf(outbuf,BUFFER_LEN,"SERVICESINFO|%s key error.",tmpkey);
//        TESTCONFIG((ret ==0 && int_value != 0),outbuf);
//        svc_ip_info.services_info_.services_type_ = static_cast<unsigned short>( int_value);
//
//        snprintf(tmpkey,BUFFER_LEN,"SERVICESID%u",i);
//        ret = svrinfo_cfg.get_uint32_value("SERVICESINFO",tmpkey,int_value);
//        snprintf(outbuf,128,"SERVICESINFO|%s key error.",tmpkey);
//        TESTCONFIG((ret ==0 && int_value != 0),outbuf);
//        svc_ip_info.services_info_.services_id_ = static_cast<unsigned int>(int_value);
//
//        snprintf(tmpkey,BUFFER_LEN,"SERVICEIP%u",i);
//        ret = svrinfo_cfg.get_string_value("SERVICESINFO",tmpkey,tmpbuf,BUFFER_LEN);
//        snprintf(outbuf,BUFFER_LEN,"SERVICESINFO|%s key error.",tmpkey);
//        TESTCONFIG((ret ==0 ),outbuf);
//
//        snprintf(tmpkey,BUFFER_LEN,"SERVICESPORT%u",i);
//        ret = svrinfo_cfg.get_uint32_value("SERVICESINFO",tmpkey,int_value);
//        snprintf(outbuf,BUFFER_LEN,"SERVICESINFO|%s key error.",tmpkey);
//        TESTCONFIG((ret ==0 && int_value != 0),outbuf);
//
//        //���IP��ַ��д��,���Ƕ��һ��
//        ZCE_Sockaddr_In inetadd;
//        ret = inetadd.set(tmpbuf,
//            static_cast<unsigned short>(int_value));
//
//        snprintf(outbuf,BUFFER_LEN,"SERVICESINFO|SERVICEIP|%s key error.",tmpkey);
//        TESTCONFIG((ret == 0),outbuf);
//
//        svc_ip_info.ip_addr_ = inetadd.get_ip_address();
//        svc_ip_info.port_ = inetadd.get_port_number();
//
//        //�õ�IDC��Ϣ
//        snprintf(tmpkey,BUFFER_LEN,"SERVICESIDC%u",i);
//        ret = svrinfo_cfg.get_uint32_value("SERVICESINFO",tmpkey,int_value);
//        snprintf(outbuf,BUFFER_LEN,"SERVICESINFO|SERVICEIP|%s key error.",tmpkey);
//        TESTCONFIG((ret == 0),outbuf);
//        svc_ip_info.idc_no_ = int_value;
//
//
//        //������Ϣ��Ҫ����32����ĸLEN_OF_NAME_BUFFER= MAX_NAME_LEN_STRING+1
//        char cfg_info[MAX_NAME_LEN_STRING+1];
//        //�õ�IDC��Ϣ
//        snprintf(tmpkey,BUFFER_LEN,"SVCCFGINFO%u",i);
//        ret = svrinfo_cfg.get_string_value("SERVICESINFO",tmpkey,cfg_info,MAX_NAME_LEN_STRING);
//        snprintf(outbuf,BUFFER_LEN,"SERVICESINFO|SERVICEIP|%s key error.",tmpkey);
//        TESTCONFIG((ret == 0),outbuf);
//        strncpy(svc_ip_info.cfg_info_ ,cfg_info,MAX_NAME_LEN_STRING);
//        svc_ip_info.cfg_info_[MAX_NAME_LEN_STRING]='\0';
//
//        //������ظ�����ֻ��Ĭ����
//        std::pair<SetOfServicesIPInfo::iterator,bool> insert_result = services_conf_map_.insert(svc_ip_info);
//        if (insert_result.second == false)
//        {
//            ZLOG_ERROR("[framework] ServicesConfig::LoadSvrConfig svrinfo repeat type:%u id:%u",
//                svc_ip_info.services_info_.services_type_, svc_ip_info.services_info_.services_id_);
//            return SOAR_RET::ERROR_SERVICES_INFO_CONFIG;
//        }
//    }
//
//    return SOAR_RET::SOAR_RET_SUCC;
//}



void Services_Table_Config::clear()
{
    services_table_.clear();
}

///******************************************************************************************
//Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2006��5��7��
//Function        : FindSelfSERVICES_INFO
//Return          : static int
//Parameter List  :
//  Param1: const char* cfgfile
//  Param2: SERVICES_ID& svrinfo
//Description     : ��ѯ�Լ���Services Info����,�����ļ�������[SELFCFG]�ֶ�
//Calls           :
//Called By       :
//Other           :
//Modify Record   :
//******************************************************************************************/
//int Services_Table_Config::FindSelfSERVICES_INFO(const char *cfgfile, SERVICES_ID &svrinfo)
//{
//    //ֻʹ��һ��,�е��˷�,���Բ�ҪƵ��ʹ���������
//    int ret = 0;
//    const int BUFFER_LEN = 128;
//    char outbuf[BUFFER_LEN];
//    uint32_t int_value;
//
//    ZCE_INI_Implement   read_cfg;
//    ZCE_INI_PropertyTree   self_svr_cfg;
//
//    ZLOG_INFO("[framework] Read Self Services Configure File. [%s] .", cfgfile);
//    ret = read_cfg.read(cfgfile, self_svr_cfg);
//    snprintf(outbuf, 256, "Can't Self Services Configure file [%s].", cfgfile);
//    TESTCONFIG(ret == 0, outbuf);
//
//    //��ȡ�Լ���SVR INFO����
//    ret = self_svr_cfg.get_uint32_value("SELFCFG", "SELFSVRTYPE", int_value);
//    TESTCONFIG((ret == 0 && int_value != 0), "SELFCFG|SELFSVRTYPE key error.");
//    svrinfo.services_type_ = static_cast<unsigned short>(int_value);
//
//    ret = self_svr_cfg.get_uint32_value("SELFCFG", "SELFSVRID", int_value);
//    TESTCONFIG((ret == 0 && int_value != 0), "SELFCFG|SELFSVRTYPE key error.");
//    svrinfo.services_id_ = static_cast<unsigned int>(int_value);
//
//    return SOAR_RET::SOAR_RET_SUCC;
//}
//
