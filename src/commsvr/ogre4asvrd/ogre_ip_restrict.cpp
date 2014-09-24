
#include "ogre_predefine.h"
#include "ogre_ip_restrict.h"

//����ʵ��
Ogre4aIPRestrictMgr *Ogre4aIPRestrictMgr::instance_ = NULL;

/****************************************************************************************************
class  OgreIPRestrict ����ͨѶ�м��IP��������
****************************************************************************************************/
Ogre4aIPRestrictMgr::Ogre4aIPRestrictMgr()
{
}
//�Լ���������ͣ�ͳһ�ر���handle_close,����ط����ùر�
Ogre4aIPRestrictMgr::~Ogre4aIPRestrictMgr()
{
}

//�������ļ��еõ���ص�����
int Ogre4aIPRestrictMgr::get_ip_restrict_conf(Zen_INI_PropertyTree &cfg_file)
{
    int ret = 0;
    unsigned int tmp_uint = 0;

    const size_t TMP_BUFFER_LEN = 256;
    char tmp_key[TMP_BUFFER_LEN + 1] = {0};
    char tmp_value[TMP_BUFFER_LEN + 1] = {0};
    char err_outbuf[TMP_BUFFER_LEN + 1] = {0};

    ret  = cfg_file.get_uint32_value("RESTRICT", "NUMALLOW", tmp_uint);
    allow_ip_set_.resize(tmp_uint);

    //��ȡ�������ӵķ�����IP��ַ
    for (size_t i = 1; i <= tmp_uint; ++i )
    {
        tmp_key[0] = '\0';
        snprintf(tmp_key, TMP_BUFFER_LEN, "ALLOWIP%zu", i);
        snprintf(err_outbuf, TMP_BUFFER_LEN, "RESTRICT|%s key error.", tmp_key);

        cfg_file.get_string_value("RESTRICT", tmp_key, tmp_value, TMP_BUFFER_LEN);
        TESTCONFIG((ret == 0), err_outbuf);

        ZCE_Sockaddr_In inetadd;
        ret = inetadd.set(tmp_value, 10);
        TESTCONFIG((ret == 0), err_outbuf);

        allow_ip_set_.insert(inetadd.get_ip_address());
    }

    //��ȡ�ܾ����ӵķ�����IP��ַ
    ret  = cfg_file.get_uint32_value("RESTRICT", "NUMREJECT", tmp_uint);
    reject_ip_set_.resize(tmp_uint);

    for (size_t i = 1; i <= tmp_uint; ++i)
    {
        tmp_key[0] = '\0';
        snprintf(tmp_key, TMP_BUFFER_LEN, "REJECTIP%zu", i);
        snprintf(err_outbuf, TMP_BUFFER_LEN, "RESTRICT|%s key error.", tmp_key);

        ret = cfg_file.get_string_value("RESTRICT", tmp_key, tmp_value, TMP_BUFFER_LEN);
        TESTCONFIG((ret == 0), err_outbuf);

        ZCE_Sockaddr_In inetadd;
        ret = inetadd.set(tmp_value, 10);
        TESTCONFIG((ret == 0), err_outbuf);

        reject_ip_set_.insert(inetadd.get_ip_address());
    }

    return 0;
}

//
int Ogre4aIPRestrictMgr::check_ip_restrict(const ZCE_Sockaddr_In &remoteaddress)
{

    //�����������ӵķ�������ַ�м�û��... kill
    if (allow_ip_set_.empty() == false )
    {
        hash_set<unsigned int>::iterator iter = allow_ip_set_.find(remoteaddress.get_ip_address());

        if ( iter == allow_ip_set_.end() )
        {
            ZLOG_INFO( "A NO Allowed IP|Port : %s|%u Connect me.\n", remoteaddress.get_host_addr(), remoteaddress.get_port_number());
            return SOAR_RET::ERR_OGRE_IP_RESTRICT_CHECK_FAIL;
        }
    }

    //�����Ҫ���ܾ����ӵ�IP��ַ...kill
    if (reject_ip_set_.empty() == false )
    {
        hash_set<unsigned int>::iterator iter = reject_ip_set_.find(remoteaddress.get_ip_address());

        if ( iter != reject_ip_set_.end() )
        {
            ZLOG_INFO( "Reject IP|Port : %s|%u connect me.\n", remoteaddress.get_host_addr(), remoteaddress.get_port_number());
            return SOAR_RET::ERR_OGRE_IP_RESTRICT_CHECK_FAIL;
        }
    }

    return 0;
}

//����ʵ������
Ogre4aIPRestrictMgr *Ogre4aIPRestrictMgr::instance()
{
    //���û�г�ʼ��
    if (instance_ == NULL)
    {
        instance_ = new Ogre4aIPRestrictMgr();
    }

    return instance_;
}

//����ʵ��
void Ogre4aIPRestrictMgr::clean_instance()
{
    if (instance_)
    {
        delete instance_;
        instance_ = NULL;
    }
}

