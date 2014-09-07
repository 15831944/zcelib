
#ifndef SOARING_LIB_SERVER_CONFIG_BASE_H_
#define SOARING_LIB_SERVER_CONFIG_BASE_H_

#include "soar_zerg_svc_info.h"



//============================================================================================
/*!
* @brief      
*             
* @note       
*/
struct SOAR_LOG_CFG_DATA
{
public:

    ///������־�ļ�������
    static const size_t DEF_RESERVE_FILE_NUM = 8;

public:

    ///app��־����: @ref ZCE_LOG_PRIORITY
    ZCE_LOG_PRIORITY log_level_ = RS_DEBUG;

    ///��־�����ʽ: @ref LOG_OUTPUT_WAY ,���������ʽ�������
    uint32_t log_output_ = LOG_OUTPUT_FILE | LOG_OUTPUT_ERROUT;

    // ��־�ָʽ:101����С 201��Сʱ 205����
    ZCE_LOGFILE_DEVIDE log_div_type_ = LOGDEVIDE_BY_DAY;

    // ��־�ļ������������������־�ļ����ᱻɾ��
    uint32_t reserve_file_num_ = DEF_RESERVE_FILE_NUM;

    // ��־�ļ�����С,��log_div_type_ �� LOGDEVIDE_BY_SIZE ʱ��Ч��
    uint32_t max_log_file_size_ = 32 * 1024 * 1024;
};


//============================================================================================
/*!
* @brief ����
*
* @note
*/
class Server_Config_Base
{

public:
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
    virtual int start_arg(int argc, const char *argv[]);

    /// ����zerg framwork app������
    virtual int load_cfgfile();

    /// ���¼�������
    virtual int reload_cfgfile();


    /*!
    * @brief      ��ȡ������־�������һЩ������Ϣ���Ա���ٻ���
    * @param      out_lvl �������
    */
    virtual void dump_cfg_info(ZCE_LOG_PRIORITY out_lvl);

protected:

    /// ʹ�ð���
    virtual int usage(const char *program_name);

    //����

    ///�������ж�ȡself_svc_id_��
    int get_selfsvcid_cfg(const char *cfg_file_name, 
        const ZCE_Conf_PropertyTree *conf_tree);

    ///�������ж�ȡ��־������
    int get_log_cfg(const char *cfg_file_name, 
        const ZCE_Conf_PropertyTree *conf_tree);


public:
    //
    static const size_t MAX_ALL_TIMER_NUMBER = 1024;

public:

    // �Լ��ķ�����ID
    SERVICES_ID self_svc_id_;

    // ������ʵ��id
    unsigned int instance_id_ = 0;

    //�Ƿ�ָ��ܵ�
    bool if_restore_pipe_ = true;


    ///�Ƿ��̨����, windows������������˴�ֵ�����Է���ķ�ʽ����
    bool app_run_daemon_ = false;

    /// Windows���Ƿ�װ����
    bool win_install_service_ = false;
    /// Windows���Ƿ�ж�ط���
    bool win_uninstall_service_ = false;


    ///�Ƿ�ʹ�����÷����������false����ʹ�ñ�������
    bool is_use_cfgsvr_ = false;
    ///���÷�������Ϣ
    ZCE_Sockaddr_In master_cfgsvr_ip_;

    size_t timer_nuamber_ = 1024;

    ///��������Ŀ¼
    std::string app_run_dir_;

    ///��־·��
    std::string log_file_prefix_;

    ///ZERG�������ļ�
    std::string zerg_cfg_file_;
    ///�Լ��������ļ�
    std::string app_cfg_file_;
    ///framework�������ļ�
    std::string framework_cfg_file_;
    ///svcid�������ļ�
    std::string svcid_table_file_;

    ///��־�ʵ�����������   
    SOAR_LOG_CFG_DATA log_config_;
    
};

#endif //SOARING_LIB_SERVER_CONFIG_BASE_H_

