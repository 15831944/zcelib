
#ifndef ZERG_COMMUNICATION_MANAGER_H_
#define ZERG_COMMUNICATION_MANAGER_H_

#include "zerg_configure.h"
#include "zerg_stat_define.h"

//��С���䣺�����ǰ�ǹ�ɣ����߽�����ëǮ
//��������¼һ�£�Ϊʲôÿ���˿�Ҫ����һ��SVC ID��������⣬
//�Ҹ�������ͻῼ��һ�ߡ�����ÿ�ζ��ῼ�ǵ�UDP��͸���������,̫TMD��������ˡ�
//����̫���Ľ������ʵ�ܼ�
//���һ��SVCID��Ӧ����˿ڣ��㲻���ֱ�ʶ������ô����������֪�����ӵ�SVCID�������Ǹ��˿���

class TCP_Accept_Handler;
class UDP_Svc_Handler;
class Zerg_MMAP_BusPipe;
class ZBuffer_Storage;
class Zerg_Server_Config;



/****************************************************************************************************
class  Zerg_Comm_Manager
****************************************************************************************************/
class Zerg_Comm_Manager
{

    //
    typedef std::vector<TCP_Accept_Handler *> TCPACCEPT_HANDLER_LIST;
    //
    typedef std::vector<UDP_Svc_Handler *> UDPSVC_HANDLER_LIST;

protected:

    //���캯������������
    Zerg_Comm_Manager();
    ~Zerg_Comm_Manager();

public:

    //��ʼ��,�������ļ���ȡ����
    int get_config(const Zerg_Server_Config config);

    //��ʼ��Socket, backlogĬ��ֵ��open��ʹ�õ�Ĭ��ֵ����һ��
    int init_socketpeer(ZERG_SERVICES_INFO &init_svcid,
        unsigned int backlog = ZCE_DEFAULT_BACKLOG,
        bool is_extern_svc = false,
        unsigned int proto_index = 0);

    //���˿��Ƿ�ȫ,��ȫ�˿ڱ��벻ʹ�ñ���(FALSE)
    int check_safeport(ZCE_Sockaddr_In     &inetadd);

    //ȡ�÷������ݽ��з���
    int popall_sendpipe_write(size_t want_, size_t &proc_frame_num);

    //
    inline void pushback_recvpipe(Zerg_App_Frame *recv_frame);

    //��鷢��Ƶ��
    void check_freamcount(unsigned int now);

    TcpHandlerImpl* get_extern_proto_handler(unsigned int index);

protected:
    int init_extern_proto_handler();



public:

    //����ʵ������
    static Zerg_Comm_Manager *instance();
    //������ʵ��
    static void clean_instance();

    // ͳ���������շ����
    void stat_heart_beat_gap(const Zerg_App_Frame * proc_frame);
    // ͳ���շ��ֲ�
    void stat_heart_beat_distribute(const Zerg_App_Frame * proc_frame, unsigned int use_time,
        ZERG_MONITOR_FEATURE_ID feature_id);

    // �յ����������󣬴��Ͻ���ʱ���
    void proc_zerg_heart_beat(Zerg_App_Frame * recv_frame);

    int send_single_buf( Zerg_Buffer * tmpbuf );


protected:

    //һ����෢��2048֡
    static const unsigned int MAX_ONCE_SEND_FRAME = 4096;

    //���ļ�ص�FRAME������,��ϣ��̫��,��������Ӱ��Ч��
    static const size_t       MAX_NUMBER_OF_MONITOR_FRAME = 16;


protected:
    //����ʵ��
    static Zerg_Comm_Manager  *instance_;


protected:

    //ACCEPET��HANDLER����
    TCPACCEPT_HANDLER_LIST    zerg_acceptor_;
    //UPD��HANDLER����
    UDPSVC_HANDLER_LIST       zerg_updsvc_;


    //���ڴ��������,���Է��͵Ĵ���,ֻ���˱�֤һ��������˲��
    unsigned int              error_try_num_;

    //��������������Ϊ�˼ӿ��ٶȣ����ñ�����
    size_t                    monitor_size_;
    //��ص�����
    unsigned int              monitor_cmd_[MAX_NUMBER_OF_MONITOR_FRAME];

    //�ڴ�ܵ����ʵ�����󣬱���������Ϊ�˼���
    Zerg_MMAP_BusPipe        *zerg_mmap_pipe_;
    //���ͺͽ��ջ����BUFF��ʵ�����󣬱���������Ϊ�˼���
    ZBuffer_Storage          *zbuffer_storage_;
    //ͳ�ƣ�ʹ�õ������ָ�룬����������Ϊ�˼���
    Comm_Stat_Monitor          *server_status_;


    //�������澯ֵ
    static const unsigned int SEND_FRAME_ALERT_VALUE = 40000;
    //������ʼʱ��
    unsigned int              count_start_time_;
    //Э������ͼ�����
    unsigned int              send_frame_count_;

    // ������ö���
    Server_Config_FSM          *comm_config_;

};



#endif //_ZERG_COMMUNICATION_MANAGER_H_

