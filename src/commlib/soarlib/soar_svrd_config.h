
#ifndef SOARING_LIB_SERVER_CONFIG_H_
#define SOARING_LIB_SERVER_CONFIG_H_

#include "soar_zerg_svc_info.h"

/****************************************************************************************************
class  Comm_Svrd_Appliction �̷���������,����==����
****************************************************************************************************/
class Comm_Svrd_Config
{

private:
    Comm_Svrd_Config();
    virtual ~Comm_Svrd_Config();

public:
    //ȡ������Ϣ,ȡ��������Ϣ��, ��Ҫ����������������OK
    int init(int argc, const char *argv[]);

    //���¼�������
    int reload();

    // ���������в���
    int proc_start_arg(int argc, const char *argv[]);

    // ����zerg framwork app������
    int load_config();

    // �ж�uin�Ƿ�Ϊ���uin
    bool is_monitor_uin(unsigned int uin);

private:
    // ʹ�ð���
    int usage(const char *program_name);

public:
    //����ʵ������
    static Comm_Svrd_Config *instance();
    //������ʵ��
    static void clean_instance();

public:
    // �Լ��ķ�����ID
    SERVICES_ID self_svr_id_;

    // ��������Ŀ¼
    std::string app_run_dir_;

    //�Ƿ�ָ��ܵ�
    bool if_restore_pipe_;

    // �Ƿ��̨����, windows������������˴�ֵ�����Է���ķ�ʽ����
    bool app_run_daemon_;

    // �Ƿ�װ����
    bool app_install_service_;

    // �Ƿ�ж�ط���
    bool app_uninstall_service_;

    //// zerg������
    //conf_zerg::ZERG_CONFIG zerg_config_;

    //// framework������
    //conf_framework::FRAMEWORK_CONFIG framework_config_;

    //// svcid������
    //conf_svcid::SVCID_CONFIG svcid_config_;

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

    // ���÷�������Ϣ
    std::string master_cfgsvr_ip_;

    // ������ʵ��id
    unsigned short instance_id_;

    // �Ƿ�ʹ�����÷����������false����ʹ�ñ�������
    bool is_use_cfgsvr_;

    // ���uin
    std::set<unsigned int> monitor_uin_set_;
protected:
    // ����ʵ��
    static Comm_Svrd_Config *instance_;


};

#endif //SOARING_LIB_SERVER_CONFIG_H_

