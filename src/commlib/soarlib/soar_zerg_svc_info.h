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

    ///��Ч��SERVICE ����
    static const uint16_t  INVALID_SERVICES_TYPE = 0;

    ///��Ч��SERVICE ID
    static const uint32_t    INVALID_SERVICES_ID   = 0;

    ///
    static const uint32_t    BROADCAST_SERVICES_ID = -1;

    //��ʾ�Ƕ�̬��Ƭ��SERVICE ID
    static const uint32_t    DYNAMIC_ALLOC_SERVICES_ID = static_cast<uint32_t>(-1);

public:

    //���������,�ο�ö��ֵSERVICES_TYPE����Чʱ��INVALID_SERVICES_TYPE
    uint16_t           services_type_;

    //����ı��,�������ֱ��ַ�ʽ,�����ſ�����һ������������ֵ,Ҳ������һ��UIN.
    uint32_t           services_id_;

public:
    //����,��������,Ĭ��Ϊ0
    explicit SERVICES_ID(uint16_t svrtype = INVALID_SERVICES_TYPE, uint32_t svrno = INVALID_SERVICES_ID);
    //
    ~SERVICES_ID();

    void  clear()
    {
        services_type_ = 0;
        services_id_ = 0;
    }

    //
    void set_serviceid(uint16_t svrtype, uint32_t svrid );

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
struct SOARING_EXPORT SERVICES_INFO
{
public:
    //����ID��Ϣ
    SERVICES_ID       services_id_;

    //����IP
    uint32_t          ip_addr_;

    //����˿�
    uint16_t          port_;

    //���������� IDC�ı��
    uint32_t          idc_no_;

    //���������
    uint32_t          server_guid_no_;

    //�����������Ϣ,��һ���ַ���������ģ���Լ�����
    char              cfg_info_[MAX_NAME_LEN_STRING + 1];

public:
    //
    SERVICES_INFO();
    ~SERVICES_INFO();
};

//�õ�KEY��HASH����
class SOARING_EXPORT Hash_of_SvcIPInfo
{
public:
    size_t operator()(const SERVICES_INFO &svripinfo) const
    {
        return (size_t (svripinfo.services_id_.services_type_) << 16) + svripinfo.services_id_.services_id_ ;
    }
};

class SOARING_EXPORT EqualSvrIPInfo
{
public:
    //ע���ж������������еı���
    bool operator()(const SERVICES_INFO &right, const SERVICES_INFO &left) const
    {
        //���SVC INFO�����,����Ϊ���
        if (right.services_id_ == left.services_id_ )
        {
            return true;
        }

        return false;
    }
};

#endif //#ifndef SOARING_LIB_SERVICES_INFO_H_

