

#ifndef OGRE_SERVER_CONFIG_H_
#define OGRE_SERVER_CONFIG_H_

//��XML��Ϊ�����ļ�����INI��Ϊ�����ļ�,����ͶƱ����,�󲿷���ϲ��INI,
//����ϲ���򵥵����˾Ӷ�,
//
class Ogre_Svr_Config
{

protected:

    Ogre_Svr_Config();
    ~Ogre_Svr_Config();

public:
    //�õ���������
    int get_startup_param(int argc, char *argv[]);

    //�õ��ļ����ò���
    int get_file_configure();

public:
    //��ӡ��������˵��
    static int print_startup_paraminfo(const char *programname);

    //����ʵ����ú���
    static Ogre_Svr_Config *instance();
    //����ʵ��������
    static void clean_instance();

public:

    //�Լ��ķ��������
    SERVICES_ID             self_svr_info_;

    //�Ƿ�ָ������ڴ�Ĺܵ�
    bool                    if_restore_pipe_;
    //�Ƿ��Σ�˿ڱ���
    bool                    zerg_insurance_;

    //OGRE�����ļ���·��
    std::string             ogre_cfg_path_;

    //OGRE����ģ���·��
    std::string             ogre_mod_path_;

    //TCPͨ�ŷ�����������
    Zen_INI_PropertyTree    cfg_ogre4a_;

    //��־�ļ�ǰ׺
    std::string             log_file_prefix_;
    //ACE_TRACE����־�������,�����������Ķ����
    ZCE_LOG_PRIORITY        log_priority_;

    //�Լ��ﶨ��TCP IP��ַ
    ZCE_Sockaddr_In         self_tcp_ip_addr_;

    //�Լ��ﶨ��UDP IP��ַ
    ZCE_Sockaddr_In         self_udp_ip_addr_;

    //����֡�ĳ���
    unsigned int            max_data_len_ ;

protected:
    //����ʵ��ָ��
    static Ogre_Svr_Config *instance_;

};

#endif //_OGRE_SERVER_CONFIG_H_

