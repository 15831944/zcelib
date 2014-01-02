#ifndef SOARING_LIB_SERVICE_CONFIG_H_
#define SOARING_LIB_SERVICE_CONFIG_H_

#include "soar_service_info.h"

//��XML��Ϊ�����ļ�����INI��Ϊ�����ļ�,����ͶƱ����,�󲿷���ϲ��INI,
//����ϲ���򵥵����˾Ӷ�,
/****************************************************************************************************
class  ServicesConfig ����������Ϣ��,ͨ��SERVICEINFO���ҵ�IP��ַ���˿ںŵ�����,
****************************************************************************************************/

typedef unordered_set<SERVICES_IP_INFO, HashofSvrIPInfo, EqualSvrIPInfo> SetOfServicesIPInfo;
//��������
class SOARING_EXPORT ServicesConfig
{
protected:
    //���õĸ���
    static const int INIT_NUM_OF_SVR_CONFG = 2048;

protected:

    //
    SetOfServicesIPInfo    services_conf_map_;

public:
    //���캯��,
    ServicesConfig(size_t szconf = INIT_NUM_OF_SVR_CONFG);
    ~ServicesConfig();

    //����SvrInfo��Ϣ��ѯIP������Ϣ
    int FindServicesIPConf(const SERVICES_ID &svrinfo,
                           unsigned int &ipaddress,
                           unsigned short &port,
                           unsigned int &idc_no,
                           unsigned int &server_guid_no,
                           char *cfg_info = NULL);
    //����SvrInfo��Ϣ��ѯIP������Ϣ
    int FindServicesIPConf(const SERVICES_ID &svrinfo,
                           ZCE_Sockaddr_In &inetadd,
                           unsigned int &idc_no,
                           unsigned int &server_guid_no,
                           char *cfg_info = NULL);

    //����Ƿ�ӵ����Ӧ��Services Info
    bool IfHashSERVICES_INFO(const SERVICES_ID &svrinfo);

    //����������Ϣ
    int AddServicesConf(const SERVICES_ID &svrinfo, const ZCE_Sockaddr_In &inetadd, unsigned int idc_no);

    //�õ�ĳ�������ļ���������Ϣ,�����ļ�������[SERVICESINFO]�ֶ�
    //int LoadSvrConfig(const char *cfgfile);
    int LoadSvrConfig(const conf_svcid::SVCID_CONFIG *config);
    //����SVR������Ϣ.
    void ClearSvrConfig();

public:
    //
    static int FindSelfSERVICES_INFO(const char *cfgfile, SERVICES_ID &svrinfo);
};

#endif //SOARING_LIB_SERVICE_CONFIG_H_

