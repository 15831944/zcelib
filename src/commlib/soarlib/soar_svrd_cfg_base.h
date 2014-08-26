
#ifndef SOARING_LIB_SERVER_CONFIG_BASE_H_
#define SOARING_LIB_SERVER_CONFIG_BASE_H_

#include "soar_zerg_svc_info.h"





/*!
* @brief ����
*
* @note
*/
class Server_Config_Base
{

protected:
    //���캯��
    Server_Config_Base();
    virtual ~Server_Config_Base();

public:

    /*!
    * @brief      ȡ������Ϣ,ȡ��������Ϣ��, ��Ҫ����������������OK
    * @return     virtual int
    * @param      argc
    * @param      argv
    */
    virtual int initialize(int argc, const char *argv[]);

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
    static Server_Config_Base *instance();
    //������ʵ��
    static void clean_instance();

public:
    // ������ʵ��id
    unsigned int instance_id_;
    // ��������Ŀ¼
    std::string   app_run_dir_;


    // �Լ��ķ�����ID
    SERVICES_ID self_svr_id_;


    //�Ƿ�ָ��ܵ�
    bool if_restore_pipe_;


    /// �Ƿ��̨����, windows������������˴�ֵ�����Է���ķ�ʽ����
    bool app_run_daemon_;
    /// Windows���Ƿ�װ����
    bool app_install_service_;
    /// Windows���Ƿ�ж�ط���
    bool app_uninstall_service_;

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
    
};

#endif //SOARING_LIB_SERVER_CONFIG_BASE_H_

