
#ifndef ZERG_SERVER_APPLICATION_H_
#define ZERG_SERVER_APPLICATION_H_

#include "zerg_tcp_ctrl_handler.h"
#include "zerg_app_handler.h"

class Zerg_Comm_Manager;
/****************************************************************************************************
class  Zerg_Service_App
****************************************************************************************************/
class Zerg_Service_App : public Comm_Svrd_Appliction
{

protected:

    //
    static const size_t NORMAL_MAX_ONCE_SEND_FRAME   = 4096;
    //
    static const size_t SENDBUSY_MAX_ONCE_SEND_FRAME = 12288;

    //
    static const size_t SEND_BUSY_JUDGE_STANDARD     = NORMAL_MAX_ONCE_SEND_FRAME / 2;
    //
    static const size_t SEND_IDLE_JUDGE_STANDARD     = 128;

    //
    static const time_t IDLE_REACTOR_WAIT_USEC       = 1000;
    //
    static const time_t NORMAL_REACTOR_WAIT_USEC     = 100;
    //
    static const time_t BUSY_REACTOR_WAIT_USEC       = 20;
    //
    static const time_t SEND_BUSY_REACTOR_WAIT_USEC  = 0;

    //
    static const size_t DEFAULT_IO_FIRST_RATIO       = 32;

protected:
    //����Ҫ͵͵����
    Zerg_Service_App();
    ~Zerg_Service_App();

    //�������ʵ��
    Zerg_Service_App(const Zerg_Service_App *);
    const Zerg_Service_App &operator =(const Zerg_Service_App &);

public:

    //ZEN��TimeQueue��������Ҫ�Լ��ͷ�
    ZCE_Timer_Queue         *timer_queue_;

    Zerg_Comm_Manager       *zerg_comm_mgr_;

    //OP KEY�ļ���ʱ���
    time_t                   conf_timestamp_;

protected:
    //���ӵ�staticʵ��
    static Zerg_Service_App *instance_;

public:

    //��ʼ��
    int init_instance();
    //�˳�ʵ����
    int exit_instance();
    //����
    int run_instance();

    bool if_proxy();

private:
    int load_app_conf()
    {
        // ��� �Ѿ�������zerg������
        return 0;
    }
    int init()
    {
        // ��� �Ѿ�������zerg������
        //zce_UNUSED_ARG(config);
        return 0;
    }
    void exit() {}
    int reload();

    int reload_daynamic_config();

public:

    //����ʵ������
    static Zerg_Service_App *instance();
    //������ʵ��
    static void clean_instance();

    conf_zerg::ZERG_CONFIG *config_;

};

//

#endif //_ZERG_SERVER_APPLICATION_H_

