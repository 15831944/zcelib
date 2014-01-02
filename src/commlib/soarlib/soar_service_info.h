#ifndef SOARING_LIB_SERVICES_INFO_H_
#define SOARING_LIB_SERVICES_INFO_H_

#include "soar_enum_define.h"

#pragma pack (1)
/****************************************************************************************************
class  SERVICES_ID ����������Ϣ,
****************************************************************************************************/
struct SOARING_EXPORT SERVICES_ID
{
public:

    //��Ч��SERVICE ����
    static const unsigned short  INVALID_SERVICES_TYPE = 0;

    //��Ч��SERVICE ID
    static const unsigned int    INVALID_SERVICES_ID   = 0;
    //��ʾ�Ƕ�̬��Ƭ��SERVICE ID
    static const unsigned int    DYNAMIC_ALLOC_SERVICES_ID = static_cast<unsigned int>(-1);

public:

    //���������,�ο�ö��ֵSERVICES_TYPE����Чʱ��INVALID_SERVICES_TYPE
    unsigned short           services_type_;

    //����ı��,�������ֱ��ַ�ʽ,�����ſ�����һ������������ֵ,Ҳ������һ��UIN.
    unsigned int             services_id_;

public:
    //����,��������,Ĭ��Ϊ0
    explicit SERVICES_ID(unsigned short svrtype = INVALID_SERVICES_TYPE, unsigned int svrno = INVALID_SERVICES_ID);
    //
    ~SERVICES_ID();

    void  clear()
    {
        services_type_ = 0;
        services_id_ = 0;
    }

    //
    void set_serviceid(unsigned short svrtype, unsigned int svrid );

    //�ȽϺ���,services_type_,services_id_
    bool operator ==(const SERVICES_ID &others) const;
    //�Ƚϲ�ͬ����,
    bool operator !=(const SERVICES_ID &others) const;
    //�и���������,
    bool operator <(const SERVICES_ID &others) const;
};

#pragma pack ()

//HASH����,���ڵõ�HASH Key

class SOARING_EXPORT HashofSvrInfo
{
public:
    size_t operator()(const SERVICES_ID &svrinfo) const
    {
        return (size_t (svrinfo.services_type_) << 16) + svrinfo.services_id_ ;
    }
};

/******************************************************************************************
struct SERVICES_IP_INFO ����ID��Ϣ + IP��Ϣ
******************************************************************************************/
struct SOARING_EXPORT SERVICES_IP_INFO
{
public:
    //����ID��Ϣ
    SERVICES_ID              services_info_;

    //����IP
    unsigned int             ip_addr_;

    //����˿�
    unsigned short           port_;

    //���������� IDC�ı��
    unsigned int             idc_no_;

    //���������
    unsigned int             server_guid_no_;

    //�����������Ϣ,��һ���ַ���������ģ���Լ�����
    char                     cfg_info_[MAX_NAME_LEN_STRING + 1];

public:
    //
    SERVICES_IP_INFO();
    ~SERVICES_IP_INFO();
};

//�õ�KEY��HASH����
class SOARING_EXPORT HashofSvrIPInfo
{
public:
    size_t operator()(const SERVICES_IP_INFO &svripinfo) const
    {
        return (size_t (svripinfo.services_info_.services_type_) << 16) + svripinfo.services_info_.services_id_ ;
    }
};

class SOARING_EXPORT EqualSvrIPInfo
{
public:
    //ע���ж������������еı���
    bool operator()(const SERVICES_IP_INFO &right, const SERVICES_IP_INFO &left) const
    {
        //���SVC INFO�����,����Ϊ���
        if (right.services_info_ == left.services_info_ )
        {
            return true;
        }

        return false;
    }
};

#endif //#ifndef SOARING_LIB_SERVICES_INFO_H_

