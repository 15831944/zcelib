
#ifndef SOARING_LIB_SERVER_APPLICATION_H_
#define SOARING_LIB_SERVER_APPLICATION_H_

#include "soar_zerg_svc_info.h"
#include "soar_error_code.h"

class Server_Config_Base;
class Zerg_MMAP_BusPipe;
class Server_Timer_Base;
/****************************************************************************************************
class  Comm_Svrd_Appliction
���߳��Զ���ģ�͵ĳ����ܻ���.
****************************************************************************************************/
class Comm_Svrd_Appliction : public ZCE_Server_Base
{
protected:

    //���캯������������
    Comm_Svrd_Appliction(Server_Config_Base *config);
    virtual ~Comm_Svrd_Appliction();

public:

    //������־�����ȼ�
    void set_log_priority(ZCE_LOG_PRIORITY new_log_prio);
    //�����־�����ȼ�
    ZCE_LOG_PRIORITY get_log_priority();



    int run(int argc, const char *argv[]);

    //int do_run();

    //�õ�APP�İ汾��Ϣ
    const char *get_app_version();



    //���¼�������
    int reload_config();

protected:

    // app�ĳ�ʼ��
    virtual int init(int argc, const char *argv[]) = 0;

    // ���¼�������
    // ���reload����app�ӿڵ�˳��: load_app_conf reload
    virtual int reload() = 0;

    // app���˳�
    virtual int exit();


private:
    //���ö�ʱ��, ��ʱ������
    int register_soar_timer();

    // ��ʼ����־�ʵ�
    int init_log();

public:

    //�õ�ʵ��ָ��
    static Comm_Svrd_Appliction *instance();

protected:

    // һ��������FRAME����
    static const size_t MAX_ONCE_PROCESS_FRAME  = 2048;

    //����N�κ�,����SELECT�ĵȴ�ʱ����
    static const unsigned int LIGHT_IDLE_SELECT_INTERVAL = 512;

    //����N�κ�,SLEEP��ʱ����
    static const unsigned int HEAVY_IDLE_SLEEP_INTERVAL  = 10240;

    //microsecond
    // 64λtlinux��idle��ʱ�����̫�̻ᵼ��cpu����
    static const int LIGHT_IDLE_INTERVAL_MICROSECOND  = 10000;
    static const int HEAVY_IDLE_INTERVAL_MICROSECOND  = 100000;

    //��ʱ��������Ƿ����ʱ��(��)
    static const int CHECK_CONFIG_UPDATE_TIME = 10;
    // ��ܶ�ʱ�����ʱ�� 100����
    static const unsigned int FRAMEWORK_TIMER_INTERVAL = 100000;

protected:


    //�Լ��ķ�����ID
    SERVICES_ID          self_services_id_;

    //��windows�ķ���ʽ���У�
    bool                 run_as_win_serivces_;

    //�����Ϣ����
    size_t               max_msg_num_;

    // ��zerg�Ĺܵ�
    Zerg_MMAP_BusPipe   *zerg_mmap_pipe_;


    /// ��ܶ�ʱ��������
    Server_Timer_Base   *timer_handler_;

    ///
    Server_Config_Base  *config_;

};

#endif //SOARING_LIB_SERVER_APPLICATION_H_
