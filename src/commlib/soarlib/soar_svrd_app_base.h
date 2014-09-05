
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
    Comm_Svrd_Appliction();
    virtual ~Comm_Svrd_Appliction();

public:

    ///������־�����ȼ�
    void set_log_priority(ZCE_LOG_PRIORITY new_log_prio);

    ///�����־�����ȼ�
    ZCE_LOG_PRIORITY get_log_priority();

    ///�õ�APP�İ汾��Ϣ
    const char *get_app_version();

    ///���¼�������
    int reload_config();


    /*!
    * @brief      ��ʼ��������һЩ�����ָ�룬
    * @return     int
    * @param      config_base
    * @param      timer_base
    */
    int initialize(Server_Config_Base *config_base, 
        Server_Timer_Base *timer_base);

protected:

    // app�Ŀ�ʼ����
    virtual int on_start(int argc, const char *argv[]);

    /// app
    virtual int on_run() = 0;

    // app���˳�
    virtual int on_exit();

    // ���¼�������
    // ���reload����app�ӿڵ�˳��: load_app_conf reload
    virtual int reload() = 0;


private:

    //���ö�ʱ��, ��ʱ������
    int register_soar_timer();

    // ��ʼ����־�ʵ�
    int init_log();

public:

    ///ע��ʵ��ָ��
    static void instance(Comm_Svrd_Appliction *inst );

    ///�õ�ʵ��ָ��
    static Comm_Svrd_Appliction *instance();


protected:


    //�Լ��ķ�����ID
    SERVICES_ID          self_services_id_;

    //��windows�ķ���ʽ���У�
    bool                 run_as_win_serivces_;

    //�����Ϣ����
    size_t               max_msg_num_;

    ///��zerg�Ĺܵ�
    Zerg_MMAP_BusPipe   *zerg_mmap_pipe_;


    ///��ܶ�ʱ��������
    Server_Timer_Base   *timer_base_;

    ///���õĴ���Ļ���
    Server_Config_Base  *config_base_;

};

#endif //SOARING_LIB_SERVER_APPLICATION_H_
