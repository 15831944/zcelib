
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
class TcpHandlerImpl;
class ExternalConnectHandler;

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
    int get_config(const conf_zerg::ZERG_CONFIG &cfg_file);

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

protected:

    //һ����෢��2048֡
    static const unsigned int MAX_ONCE_SEND_FRAME  = 4096;

    //���ļ�ص�FRAME������,��ϣ��̫��,��������Ӱ��Ч��
    static const size_t       MAX_NUMBER_OF_MONITOR_FRAME  = 16;

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

protected:
    //�������澯ֵ
    static const unsigned int SEND_FRAME_ALERT_VALUE = 40000;
    //������ʼʱ��
    unsigned int              count_start_time_;
    //Э������ͼ�����
    unsigned int              send_frame_count_;

    // ������ö���
    Comm_Svrd_Config          *comm_config_;

protected:
    //����ʵ��
    static Zerg_Comm_Manager  *instance_;

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

    int send_single_buf( ZByteBuffer * tmpbuf );

};

//ϣ���ӿ���������inline���������������ȣ��Ż��Ŀ�����С��
inline void Zerg_Comm_Manager::pushback_recvpipe(Zerg_App_Frame *recv_frame)
{
    // �����ͨ�ŷ�����������,�������κδ���
    if (true == recv_frame->is_zerg_processcmd())
    {
        // �����ذ�����������շ�������м��
        if (true == recv_frame->is_zerg_heart_beat_rsp())
        {
            stat_heart_beat_gap(recv_frame);
            return;
        }
        else if (true == recv_frame->is_zerg_heart_beat_req())
        {
            // ��������������Ͻ���ʱ�����������APPȥ����
            proc_zerg_heart_beat(recv_frame);
        }
        else
        {
            return;
        }
    }


    //Ϊ�����Ч�ʣ��ȼ���־λ��
    if (recv_frame->frame_option_ & Zerg_App_Frame::DESC_MONITOR_TRACK)
    {
        Zerg_App_Frame::dumpoutput_framehead(recv_frame, "[TRACK MONITOR][RECV]opt", RS_INFO);
    }
    else
    {
        //��δ����ڷ��ͽ���ʱ��Ҫ��飬�����п���©��
        //�����Ҫ���ٵ�����
        for (size_t i = 0; i < monitor_size_; ++i)
        {
            if (monitor_cmd_[i] == recv_frame->frame_command_)
            {
                recv_frame->frame_option_ |= Zerg_App_Frame::DESC_MONITOR_TRACK;
                Zerg_App_Frame::dumpoutput_framehead(recv_frame, "[TRACK MONITOR][RECV]cmd", RS_INFO);
            }
        }
    }

    int ret = zerg_mmap_pipe_->push_back_bus(Zerg_MMAP_BusPipe::RECV_PIPE_ID,
        reinterpret_cast< const ZCE_LIB::dequechunk_node *>(recv_frame));

    if (ret != SOAR_RET::SOAR_RET_SUCC)
    {
        server_status_->increase_once(ZERG_RECV_PIPE_FULL_COUNTER,
            recv_frame->app_id_);
    }
    else
    {
        server_status_->increase_once(ZERG_RECV_FRAME_COUNTER,
            recv_frame->app_id_);
        server_status_->increase_once(ZERG_RECV_FRAME_COUNTER_BY_CMD,
            recv_frame->app_id_,
            recv_frame->frame_command_);
        server_status_->increase_once(ZERG_RECV_FRAME_COUNTER_BY_SVR_TYPE,
            recv_frame->app_id_,
            recv_frame->send_service_.services_type_);
    }

}

#endif //_ZERG_COMMUNICATION_MANAGER_H_

