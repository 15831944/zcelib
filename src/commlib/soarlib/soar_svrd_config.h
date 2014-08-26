
#ifndef SOARING_LIB_SERVER_CONFIG_H_
#define SOARING_LIB_SERVER_CONFIG_H_

#include "soar_zerg_svc_info.h"


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
* @brief ����
*
* @note
*/
class Comm_Svrd_Config
{

protected:
    //���캯��
    Comm_Svrd_Config();
    virtual ~Comm_Svrd_Config();

public:

    /*!
    * @brief      ȡ������Ϣ,ȡ��������Ϣ��, ��Ҫ����������������OK
    * @return     virtual int
    * @param      argc
    * @param      argv
    */
    virtual int init(int argc, const char *argv[]);

    /*!
    * @brief      ���������в���
    * @return     virtual int
    * @param      argc
    * @param      argv
    */
    virtual int proc_start_arg(int argc, const char *argv[]);

    /// ����zerg framwork app������
    virtual int load_cfgfile();

    /// ���¼�������
    virtual int reload_cfgfile();

protected:

    
    /// ʹ�ð���
    virtual int usage(const char *program_name);

public:
    //����ʵ������
    static Comm_Svrd_Config *instance();
    //������ʵ��
    static void clean_instance();

public:
    // ������ʵ��id
    unsigned int instance_id_;
    // ��������Ŀ¼
    std::string   app_run_dir_;


    // �Լ��ķ�����ID
    SERVICES_ID self_svr_id_;
    // framework������
    FRAMEWORK_CONFIG framework_config_;


    //�Ƿ�ָ��ܵ�
    bool if_restore_pipe_;


    /// �Ƿ��̨����, windows������������˴�ֵ�����Է���ķ�ʽ����
    bool app_run_daemon_;
    /// Windows���Ƿ�װ����
    bool app_install_service_;
    /// Windows���Ƿ�ж�ط���
    bool app_uninstall_service_;

    //
    ZCE_Conf_PropertyTree zerg_ptree_;
    //
    ZCE_Conf_PropertyTree framework_ptree_;


    // ��־·��
    std::string log_file_prefix_;

    // ZERG�������ļ�
    std::string zerg_cfg_file_;
    // �Լ��������ļ�
    std::string app_cfg_file_;
    // framework�������ļ�
    std::string framework_cfg_file_;
    // svcid�������ļ�
    std::string svcid_cfg_file_;

    // �Ƿ�ʹ�����÷����������false����ʹ�ñ�������
    bool is_use_cfgsvr_;
    // ���÷�������Ϣ
    std::string master_cfgsvr_ip_;


protected:
    // ����ʵ��
    static Comm_Svrd_Config *instance_;


};

#endif //SOARING_LIB_SERVER_CONFIG_H_

