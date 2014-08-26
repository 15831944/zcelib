
#ifndef ZERG_IP_RESTRICT_H_
#define ZERG_IP_RESTRICT_H_

/****************************************************************************************************
class  Zerg_IPRestrict_Mgr
****************************************************************************************************/
class Zerg_IPRestrict_Mgr
{

    typedef unordered_set<unsigned int> SetOfIPAddress;

protected:
    //
    Zerg_IPRestrict_Mgr();
    ~Zerg_IPRestrict_Mgr();

public:
    //�������ļ��еõ���ص�����
    int get_iprestrict_conf(const conf_zerg::ZERG_CONFIG &cfg_file);
    //���IP����
    int check_iprestrict(const ZCE_Sockaddr_In &remoteaddress);

protected:

    //����������ӵ�IP��ַ
    SetOfIPAddress           allow_ip_set_;
    //�ܾ��������ӵ�IP��ַ
    SetOfIPAddress           reject_ip_set_;

protected:
    //����ʵ��
    static Zerg_IPRestrict_Mgr        *instance_;

public:

    //����ʵ������
    static Zerg_IPRestrict_Mgr *instance();
    //������ʵ��
    static void clean_instance();

};

#endif //_ZERG_IP_RESTRICT_H_

