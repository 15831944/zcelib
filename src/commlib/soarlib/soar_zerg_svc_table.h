#ifndef SOARING_LIB_SERVICE_CONFIG_H_
#define SOARING_LIB_SERVICE_CONFIG_H_

#include "soar_zerg_svc_info.h"

//��XML��Ϊ�����ļ�����INI��Ϊ�����ļ�,����ͶƱ����,�󲿷���ϲ��INI,
//����ϲ���򵥵����˾Ӷ�,


/****************************************************************************************************
class  Services_Table_Config ����������Ϣ��,ͨ��SERVICEINFO���ҵ�IP��ַ���˿ںŵ�����,
****************************************************************************************************/


//��������
class SOARING_EXPORT Services_Table_Config
{

    typedef unordered_set<SERVICES_INFO, Hash_of_SvcIPInfo, EqualSvrIPInfo> Set_Of_SvcInfo;

public:
    //���캯��,
    Services_Table_Config(size_t sz_table = INIT_NUM_OF_SVR_CONFG);
    ~Services_Table_Config();


    /*!
    * @brief      ����SvrInfo��Ϣ��ѯIP������Ϣ
    * @return     int
    * @param      svc_id    �������,��������ϢSVCINFO
    * @param      ipaddress �������,��������ϢSVCINFO
    * @param      port      �������,�˿���Ϣ
    * @param      idc_no    �������,IDC��Ϣ
    * @param      server_guid_no  �������,���������
    * @param      cfg_info  �������,Ĭ�ϴ���NULL��ʾ�㲻�����������,��֤MAX_NAME_LEN_STRING+1�Ŀռ䳤��,�������Լ�����,
    * @note       
    */
    int find_svcinfo(const SERVICES_ID &svc_id,
                           unsigned int &ipaddress,
                           unsigned short &port,
                           unsigned int &idc_no,
                           unsigned int &server_guid_no,
                           char *cfg_info = NULL) const;

    //����SvrInfo��Ϣ��ѯIP������Ϣ
    int find_svcinfo(const SERVICES_ID &svc_id,
        SERVICES_INFO &svc_info) const;

    //����Ƿ�ӵ����Ӧ��Services Info
    bool hash_svcinfo(const SERVICES_ID &svrinfo) const;

    //����������Ϣ
    int add_svcinfo(const SERVICES_INFO &svc_info);

    //�õ�ĳ�������ļ���������Ϣ,�����ļ�������[SERVICESINFO]�ֶ�
    int loadtable_from_cfg(const char *cfgfile);

    //����SVR������Ϣ.
    void clear();


protected:
    //���õĸ���
    static const int INIT_NUM_OF_SVR_CONFG = 2048;

protected:
    //
    Set_Of_SvcInfo    services_table_;

};

#endif //SOARING_LIB_SERVICE_CONFIG_H_

