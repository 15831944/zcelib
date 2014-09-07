#ifndef SOARING_LIB_SERVICE_CONFIG_H_
#define SOARING_LIB_SERVICE_CONFIG_H_

#include "soar_zerg_svc_info.h"

//��XML��Ϊ�����ļ�����INI��Ϊ�����ļ�,����ͶƱ����,�󲿷���ϲ��INI,
//����ϲ���򵥵����˾Ӷ�,


/****************************************************************************************************
class  Services_Table_Config ����������Ϣ��,ͨ��SERVICEINFO���ҵ�IP��ַ���˿ںŵ�����,
****************************************************************************************************/


//��������
class SOARING_EXPORT SERVICES_INFO_TABLE
{

    typedef unordered_set<SERVICES_INFO, HASH_OF_SVCINFO, EQUAL_OF_SVCINFO> Set_Of_SvcInfo;

public:
    //���캯��,
    SERVICES_INFO_TABLE(size_t sz_table = INIT_NUM_OF_SVR_CONFG);
    ~SERVICES_INFO_TABLE();


    /*!
    * @brief      ����SvrInfo��Ϣ��ѯIP������Ϣ
    * @return     int
    * @param      svc_id      �������,��������ϢSVCINFO
    * @param      ip_address  �������,��������ϢSVCINFO
    * @param      idc_no      �������,IDC��Ϣ
    * @param      business_id �������,���������
    * @note
    */
    int find_svcinfo(const SERVICES_ID &svr_id,
                     ZCE_Sockaddr_In &ip_address,
                     unsigned int &idc_no,
                     unsigned int &business_id) const;

    //����SvrInfo��Ϣ��ѯIP������Ϣ
    int find_svcinfo(const SERVICES_ID &svc_id,
                     SERVICES_INFO &svc_info) const;

    //����Ƿ�ӵ����Ӧ��Services Info
    bool hash_svcinfo(const SERVICES_ID &svrinfo) const;

    //����������Ϣ
    int add_svcinfo(const SERVICES_INFO &svc_info);

    //�õ�ĳ�������ļ���������Ϣ,�����ļ�������[SERVICESINFO]�ֶ�
    int loadtable_from_cfg(const ZCE_Conf_PropertyTree *conf_tree);

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

