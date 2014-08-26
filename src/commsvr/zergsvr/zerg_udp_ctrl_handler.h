
#ifndef ZERG_UDP_CONTROL_SERVICE_H_
#define ZERG_UDP_CONTROL_SERVICE_H_

//forward declaration
class Zerg_App_Frame;
class Zerg_Buffer;
class UDPSessionKeyMgr;
class Zerg_IPRestrict_Mgr;
class Zerg_Comm_Manager;

class UDP_Svc_Handler: public ZCE_Event_Handler
{
protected:

    //�����ڶ��м����,��������������Ҫ
public:
    // ����handler�ĳ�ʼ��
    UDP_Svc_Handler(const SERVICES_ID &my_svcinfo,
                    const ZCE_Sockaddr_In &addr,
                    bool sessionkey_verify = true,
                    bool is_external_pkg = false,
                    uint32_t cmd_offset = 0,
                    uint32_t cmd_len = 0);

    // ��������handler�ĳ�ʼ��
    UDP_Svc_Handler(const SERVICES_ID &my_svcinfo,
                    const SERVICES_ID &peer_svcinfo,
                    const ZCE_Sockaddr_In &peer_addr,
                    bool is_external_pkg = false,
                    uint32_t cmd_offset = 0,
                    uint32_t cmd_len = 0);
protected:
    ~UDP_Svc_Handler();

public:
    //ȡ�þ��
    virtual ZCE_HANDLE get_handle(void) const;
    //
    virtual int handle_input();
    //
    virtual int handle_close();

    //�����������ݵ�handler��ʼ��
    int init_udpsvr_handler();

public:

    //��ʼ��UPD�˿�
    int init_udp_services();

protected:

    //��PEER��ȡ����
    int read_data_from_udp(size_t &szrevc);

    //����UDP������
    int write_data_to_udp(Zerg_App_Frame *send_frame);

    // ��ȡ�ⲿЭ�����cmd
    int get_external_pkg_cmd(uint32_t &cmd, const uint8_t *buff, const ssize_t buf_len);

public:
    //��ʼ����̬����
    static int init_all_static_data();

    //
    static int send_all_to_udp(Zerg_App_Frame *send_frame);

    //
    static int get_udpctrl_conf(const conf_zerg::ZERG_CONFIG *config);

    //�����е�SVR INFO����ѯ��Ӧ��handle
    static int find_services_peer(const SERVICES_ID &svr_info, UDP_Svc_Handler *&svchanle);

protected:

    //һ�δ�UDP�Ķ˿ڶ�ȡ������������
    static const size_t ONCE_MAX_READ_UDP_NUMBER = 256;

protected:

    //���ݰ�
    ZCE_Socket_DataGram      dgram_peer_;

    // �Լ��ļ�����ַ
    ZCE_Sockaddr_In          self_udp_addr_;

    // �Է��ĵ�ַ
    ZCE_Sockaddr_In          peer_udp_addr_;

    // �Լ���type��id
    SERVICES_ID              self_svc_info_;

    // �Է���type��id
    SERVICES_ID              peer_svc_info_;

    //�Ƿ����SESSIONУ��
    bool                     sessionkey_verify_;

    // �Ƿ��ǽ��տ�ܰ�
    bool                    is_external_pkg_;
    // ����ǽ��շǿ�ܰ�����Ҫ֪��cmdƫ��
    uint32_t                cmd_offset_;
    uint32_t                cmd_len_;

    //���ݻ�������UDPֻ��һ��
    Zerg_Buffer             *dgram_databuf_;
    //IP���ƹ�����
    Zerg_IPRestrict_Mgr      *ip_restrict_;

    // ��handler�����ͣ��ǡ�����handler�����ǡ���������handler��
    bool is_initiative_handler_;

protected:
    //
    typedef std::vector< UDP_Svc_Handler *>  ARY_OF_UDPSVC_HANDLER;

    //UPD�����飬�����ж��UDP
    static ARY_OF_UDPSVC_HANDLER    ary_udpsvc_handler_;

    //UDP��SESSION������
    static UDPSessionKeyMgr        *udp_session_mgr_;

    //ͳ�ƣ�ʹ�õ������ָ��
    static Comm_Stat_Monitor       *server_status_;

    //ͨѶ������,������Ϊ�˼ӿ��ٶ�
    static Zerg_Comm_Manager       *zerg_comm_mgr_;

    //�Ƿ��Ǵ��������
    static bool                     if_proxy_;

    // game_id
    static unsigned int             game_id_;

};

#endif //#ifndef _ZERG_UDP_CONTROL_SERVICE_H_

