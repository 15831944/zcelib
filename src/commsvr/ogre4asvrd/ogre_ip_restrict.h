
#ifndef OGRE4A_IP_RESTRICT_H_
#define OGRE4A_IP_RESTRICT_H_

/****************************************************************************************************
class  Ogre4aIPRestrictMgr
****************************************************************************************************/
class Ogre4aIPRestrictMgr
{

    typedef hash_set<unsigned int> SetOfIPAddress;

protected:
    Ogre4aIPRestrictMgr();
    ~Ogre4aIPRestrictMgr();

public:

    //�������ļ��еõ���ص�����
    int get_ip_restrict_conf(Zen_INI_PropertyTree &cfg_file);
    //���IP����
    int check_ip_restrict(const ZEN_Sockaddr_In &remoteaddress);

protected:

    //����������ӵ�IP��ַ
    SetOfIPAddress                  allow_ip_set_;
    //�ܾ��������ӵ�IP��ַ
    SetOfIPAddress                  reject_ip_set_;

protected:
    //����ʵ��
    static Ogre4aIPRestrictMgr     *instance_;

public:

    //����ʵ������
    static Ogre4aIPRestrictMgr *instance();
    //������ʵ��
    static void clean_instance();
};

#endif //_OGRE4A_IP_RESTRICT_H_

