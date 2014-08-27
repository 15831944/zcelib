
#ifndef SOARING_LIB_SERVER_CONFIG_TRANS_H_
#define SOARING_LIB_SERVER_CONFIG_TRANS_H_

#include "soar_zerg_svc_info.h"
#include "soar_svrd_cfg_base.h"


struct ZERG_CONFIG
{

};



struct FRAMEWORK_CONFIG
{
    struct LogInfo
    {
    public: /* members */
        char log_level_[32]; // app��־����: debug info error final
        uint32_t log_output_; // ��־�����ʽ: 0001=file 0010=stdout 0100=stderr 1000=windbg,���������ʽ�������
        uint32_t log_div_type_; // ��־�ָʽ:101����С 201��Сʱ 205����
        uint32_t max_log_file_num_; // ��־�ļ������������������־�ļ����ᱻɾ��
        uint32_t max_log_file_size_; // ��־�ļ�����С
    };

    struct TaskInfo
    {
    public: /* members */
        uint32_t task_thread_num_; // task�߳�����
        uint32_t task_thread_stack_size_; // task�̶߳�ջ��С
        uint32_t enqueue_timeout_sec_; // push���ݵ�task����ʱ�ĳ�ʱʱ����
        uint32_t enqueue_timeout_usec_; // push���ݵ�task����ʱ�ĳ�ʱʱ��΢��
    };

    struct TransInfo
    {
    public: /* members */
        uint32_t trans_num_; // ���������
        uint32_t trans_cmd_num_; // ���������ֵĸ���
        uint32_t func_cmd_num_; // ���������ֵĸ���
    };

    struct MonitorInfo
    {
    public: /* members */
        uint32_t filter_statics_id_cnt_; // Ҫ�ų���ͳ��ID����
        uint32_t filter_statics_id_list_[100]; // Ҫ�ų���ͳ��ID�б�
    };

    TransInfo trans_info_; // ���������
    TaskInfo task_info_; // Task�����ã�ʹ��notifytransʱ��Ч
    LogInfo log_info_; // ��־�ʵ���������Ϣ
    MonitorInfo monitor_info_; // ���������Ϣ
};

struct SVCID_CONFIG 
{

};


/*!
* @brief ���ҵ�������������
*
* @note
*/
class Server_Config_FSM :public Server_Config_Base
{

protected:
    //���캯��
    Server_Config_FSM();
    virtual ~Server_Config_FSM();

public:

    /// ����zerg framwork app������
    virtual int load_cfgfile();

protected:

    
    /// ʹ�ð���
    virtual int usage(const char *program_name);

public:
    //����ʵ������
    static Server_Config_FSM *instance();
    //������ʵ��
    static void clean_instance();

public:
    
    //
    FRAMEWORK_CONFIG framework_config_;

    ZCE_Conf_PropertyTree zerg_ptree_;
    ZCE_Conf_PropertyTree framework_ptree_;

protected:
    // ����ʵ��
    static Server_Config_FSM *instance_;


};

#endif //SOARING_LIB_SERVER_CONFIG_TRANS_H_

