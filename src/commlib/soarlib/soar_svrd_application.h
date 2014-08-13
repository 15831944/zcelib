
#ifndef SOARING_LIB_SERVER_APPLICATION_H_
#define SOARING_LIB_SERVER_APPLICATION_H_

#include "soar_service_info.h"
#include "soar_error_code.h"

class Comm_Svrd_Config;
class Zerg_MMAP_BusPipe;
class Comm_Timer_Handler;
/****************************************************************************************************
class  Comm_Svrd_Appliction
���߳��Զ���ģ�͵ĳ����ܻ���.
****************************************************************************************************/
class Comm_Svrd_Appliction : public ZCE_Server_Base
{
protected:

    //���캯������������
    Comm_Svrd_Appliction();
    virtual ~Comm_Svrd_Appliction();

public:

    //���ý�����ͣ���еı�־
    void set_pause_sign(bool app_pause);



    //������־�����ȼ�
    void set_log_priority(ZCE_LOG_PRIORITY new_log_prio);
    //�����־�����ȼ�
    ZCE_LOG_PRIORITY get_log_priority();



    int run(int argc, const char *argv[]);

    int do_run();

    //�õ�APP�İ汾��Ϣ
    const char *get_app_version();

    // windows�����÷�����Ϣ
    void set_service_info(const char *svc_name, const char *svc_desc);

    //���¼�������
    int reload_config();
protected:
    // Ӧ����Ҫʵ�ֵĺ���

    // ����app����, app���Լ�ʵ��
    // ��ܳ�ʼ������app�ӿڵ�˳�� load_app_conf register_notify_task(notifytrans����) init
    virtual int load_app_conf();

    // app�ĳ�ʼ��
    virtual int init() = 0;

    // ���¼�������
    // ���reload����app�ӿڵ�˳��: load_app_conf reload
    virtual int reload() = 0;

    // app���˳�
    virtual void exit();

protected:
    // ����в�ͬ��app��������صĺ���
    // ��ܳ�ʼ��
    virtual int init_instance();

    int proc_start_args(int argc, const char *argv[]);

    // ����˳�
    virtual int exit_instance();

    // ������д���,
    virtual int run_instance() = 0;

    // ������¼�������
    virtual int reload_instance();

private:
    //���ö�ʱ��, ��ʱ������
    int register_soar_timer();

    //�õ�app_base_name_��app_run_name_
    int create_app_name(const char *argv_0);


    // ��ʼ����־�ʵ�
    int init_log();

    //WIN ����Ĵ��룬���ڷ�������ע��ע����
#ifdef ZCE_OS_WINDOWS

    //���з���
    int win_services_run();

    //WIN ������˳�����
    static BOOL exit_signal(DWORD );

    //��װ����
    int win_services_install();
    //ж�ط���
    int win_services_uninstall();
    //�������Ƿ�װ
    bool win_services_isinstalled();

    //�������к���
    static void WINAPI win_service_main();
    //�������̨����Ҫ�Ŀ��ƺ���
    static void WINAPI win_services_ctrl(DWORD op_code);

#else

    //�˳��ź�
    static void exit_signal(int );

    //���¼�������
    static void reload_config_signal(int );
#endif

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

    //Ϊ�˼���ж��so����һ���Ƿ���ͣ�ı���
    bool                         app_pause_;



    //�Լ��ķ�����ID
    SERVICES_ID                  self_services_id_;

    //��windows�ķ���ʽ���У�
    bool                         run_as_win_serivces_;

    //�����Ϣ����
    size_t                       max_msg_num_;



    // ��zerg�Ĺܵ�
    Zerg_MMAP_BusPipe           *zerg_mmap_pipe_;



    // ��������
    int                         argc_;
    const char                **argv_;


private:
    // ��ܶ�ʱ��������
    Comm_Timer_Handler          *timer_handler_;

};

#endif //SOARING_LIB_SERVER_APPLICATION_H_
