#ifndef ZERG_SERVER_CONFIG_H_
#define ZERG_SERVER_CONFIG_H_

/****************************************************************************************************
class  ZERG_SERVICES_INFO
****************************************************************************************************/
struct ZERG_SERVICES_INFO
{
public:
    //
    SERVICES_ID        zerg_svc_info_;
    //
    bool               zerg_sessionkey_;
    //
    ZCE_Sockaddr_In    zerg_ip_addr_;

    // �Ƿ�����ⲿ��
    bool is_recv_external_pkg_;
    // cmd offset
    uint32_t cmd_offset_;
    // cmd len
    uint32_t cmd_len_;

    ZERG_SERVICES_INFO();
    ~ZERG_SERVICES_INFO();
};

//
typedef std::vector<ZERG_SERVICES_INFO>  SELF_SERVICESINFO_LIST;

//��XML��Ϊ�����ļ�����INI��Ϊ�����ļ�,����ͶƱ����,�󲿷���ϲ��INI,
//����ϲ���򵥵����˾Ӷ�,
/****************************************************************************************************
class  Zerg_Server_Config
****************************************************************************************************/
class Zerg_Server_Config
{

    Zerg_Server_Config();
    ~Zerg_Server_Config();

    //

    //����
    static const size_t MAX_SELF_SERVICES_ID = 4;
public:
    //����6:00����Operation key
    static const time_t UPDATE_OPERATION_KEY_TIME = 3600 * 6;

public:

    //ͳ���ļ�
    std::string            zerg_stat_file_;

    //����ͳ���ļ�
    std::string            cmd_stat_file_;

    //ZERG�ı����Ƿ�ʹ��
    bool                   zerg_insurance_;

    //
    bool                   zerg_need_opkey_;
    //�Ƿ���UDP SESSION
    bool                   zerg_udp_session_;

    //��������ü���
    ServicesConfig         services_info_cfg_;

    //�������г���
    unsigned int backlog_;

    //�����⼸��Ϊ�˼ӿ�ʹ���ٶȣ�ʹ����STATIC,����ѧ���ٶ���ǰѡ���ٶ�
public:

    //�Լ��ķ������ı�ʾID,
    static ZERG_SERVICES_INFO         self_svc_info_;

    //SLAVE�ķ������ı�ʾID,����������2���˿ڣ�����N���˿�
    static SELF_SERVICESINFO_LIST     slave_svc_ary_;

    // �����ⲿUdp�˿���Ϣ
    static SELF_SERVICESINFO_LIST     external_udp_svr_ary_;

    //
    static SELF_SERVICESINFO_LIST     extern_svc_ary_;

    //����ʵ��
    static Zerg_Server_Config        *instance_;

public:

    //�õ��ļ����ò���
    //����ӿ��е�֣�Ϊ�˰�һЩ������Ϣ�������������֮ǰ�Ͱ���־���Գ�ʼ����
    //virtual int get_file_configure();

    //Dump���õ�������Ϣ
    void dump_status_info(std::ostringstream &ostr_stream);

    //����SVCINFO�õ�IP��ַ��Ϣ
    int GetServicesIPInfo(const SERVICES_ID &svc_info, ZCE_Sockaddr_In     &ipaddr);

    int init(const conf_zerg::ZERG_CONFIG *config);

    //��ʼ���ⲿ����
    int init_extern_svc();

public:

    //���Ӻ���
    static Zerg_Server_Config *instance();
    //������ָ�뺯��
    static void clean_instance();

    const conf_zerg::ZERG_CONFIG *config_;
};

#endif //_ZERG_SERVER_CONFIG_H_

