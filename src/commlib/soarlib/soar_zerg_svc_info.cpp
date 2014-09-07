#include "soar_predefine.h"
#include "soar_zerg_svc_info.h"

/****************************************************************************************************
class SERVICES_ID ������Ϣ,
****************************************************************************************************/
//���캯������������
SERVICES_ID::SERVICES_ID(unsigned short svrtype, unsigned int svrid):
    services_type_(svrtype),
    services_id_(svrid)
{
}

SERVICES_ID::~SERVICES_ID()
{
}

//
void SERVICES_ID::set_svcid(unsigned short svrtype, unsigned int svrid )
{
    services_type_ = svrtype;
    services_id_ = svrid;
}

//�Ƚ��Ƿ���ͬ�ĺ���
bool SERVICES_ID::operator ==(const SERVICES_ID &others) const
{
    if (services_type_ == others.services_type_ &&   services_id_ == others.services_id_ )
    {
        return true;
    }

    return false;
}

//�Ƚ��Ƿ�ͬ�ĺ���
bool SERVICES_ID::operator !=(const SERVICES_ID &others) const
{
    if (services_type_ != others.services_type_ ||   services_id_ != others.services_id_ )
    {
        return true;
    }

    return false;
}

//�Ƚϴ�С�ĺ���
bool SERVICES_ID::operator <(const SERVICES_ID &others) const
{
    if (services_type_ + services_id_ < others.services_type_ + others.services_id_ )
    {
        return true;
    }

    return false;
}

/******************************************************************************************
struct SERVICES_IP_INFO ����ID��Ϣ + IP��Ϣ
******************************************************************************************/

