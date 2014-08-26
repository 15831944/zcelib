#include "soar_predefine.h"
#include "soar_error_code.h"
#include "soar_zerg_svc_info.h"
#include "soar_server_ver_define.h"
#include "soar_svrd_application.h"
#include "soar_svrd_config.h"


Comm_Svrd_Config *Comm_Svrd_Config::instance_ = NULL;

Comm_Svrd_Config::Comm_Svrd_Config():
    instance_id_(1),
    self_svr_id_(0, 0),
    if_restore_pipe_(true),
    app_run_daemon_(false),
    app_install_service_(false),
    app_uninstall_service_(false),
    is_use_cfgsvr_(false)
{
}

Comm_Svrd_Config::~Comm_Svrd_Config()
{
}

// ȡ������Ϣ,ȡ��������Ϣ��, ��Ҫ����������������OK
int Comm_Svrd_Config::init(int argc, const char *argv[])
{
    // ���������в���
    int ret = SOAR_RET::SOAR_RET_SUCC;
    ret = proc_start_arg(argc, argv);

    if (ret != SOAR_RET::SOAR_RET_SUCC)
    {
        return ret;
    }

    // ��������
    ret = load_cfgfile();
    if (ret != SOAR_RET::SOAR_RET_SUCC)
    {
        return ret;
    }

    return ret;
}

int Comm_Svrd_Config::proc_start_arg(int argc, const char *argv[])
{
    // ȡ������Ŀ¼
    char cur_dir[PATH_MAX + 1];
    cur_dir[PATH_MAX] = 0;
    ZCE_OS::getcwd(cur_dir, sizeof(cur_dir) - 1);

    app_run_dir_ = cur_dir;

    // ָ��RETURN_IN_ORDER ������˳��
    ZCE_Get_Option get_opt(argc, (char **)argv,
                           "umvndhpi:t:r:a:", 1, 0, ZCE_Get_Option::RETURN_IN_ORDER);
    int c;

    while ((c = get_opt()) != EOF)
    {
        switch (c)
        {
            case 'v':
            {
                // ��ӡ�汾��Ϣ
                printf("%s\n", TSS_SERVER_VER_DECLARE);
                exit(0);
                break;
            }

            case 'n':
            {
                // �ӹܵ��ָ�����
                if_restore_pipe_ = true;
                break;
            }

            case 'd':
            {
                // ��̨����
                app_run_daemon_ = true;
                break;
            }

            case 'r':
            {
                // ָ������Ŀ¼, �Է�������ʱ����Ҫָ���˲���
                ZLOG_INFO("app run dir = %s", app_run_dir_.c_str());
                app_run_dir_ = get_opt.optarg;
                break;
            }

            case 'a':
            {
                // ��cfgsvr ip��ַ
                // ָ�������õ�ַ��������÷�����������
                is_use_cfgsvr_ = true;
                master_cfgsvr_ip_ = get_opt.optarg;
                break;
            }

            case 'i':
            {
                // ָ���˷�����ʵ��id
                instance_id_ = static_cast<unsigned short>(atoi(get_opt.optarg));
                break;
            }

            case 't':
            {
                // ָ���˷�����type
                self_svr_id_.services_type_ = static_cast<unsigned short>(atoi(get_opt.optarg));
                break;
            }

            case 'p':
            {
                // �ӷ�����������
                is_use_cfgsvr_ = true;
                break;
            }

            case 'u':
            {
                // windowsж�ط���
                app_uninstall_service_ = true;
                break;
            }

            case 'm':
            {
                // windows��װ����
                app_install_service_ = true;
                break;
            }

            case 'h':
            {
                usage(argv[0]);
                exit(0);
            }

            default:
            {
                printf("unknow argu %c\n", c);
                usage(argv[0]);
                return SOAR_RET::ERR_ZERG_GET_STARTUP_CONFIG_FAIL;
            }
        }
    }

    log_file_prefix_ = app_run_dir_ + "/log/";
    log_file_prefix_ += Comm_Svrd_Appliction::instance()->get_app_basename();
    log_file_prefix_ += "_init";

    // δָ��app�������ļ�����ʹ��Ĭ�ϵ�
    app_cfg_file_ = app_run_dir_ + "/cfg/";
    app_cfg_file_ += Comm_Svrd_Appliction::instance()->get_app_basename();
    app_cfg_file_ += "_config.xml";

    // δָ��ͨѶ����������
    zerg_cfg_file_ = app_run_dir_ + "/cfg/zergsvrd.xml";

    // δָ��svcid�����ļ�
    svcid_cfg_file_ = app_run_dir_ + "/cfg/svcid.xml";

    // ��ܵ������ǲ�����
    framework_cfg_file_ = app_run_dir_ + "/cfg/framework.xml";

    return SOAR_RET::SOAR_RET_SUCC;
}

//
int Comm_Svrd_Config::usage(const char *program_name)
{
    std::cout << "usage: " << program_name << std::endl;
    std::cout << "   -z [zergling cfg path]" << std::endl;
    std::cout << "   -c [services cfg file]" << std::endl;
    std::cout << "   -d run as daemon" << std::endl;
    std::cout << "   -n reset channel mmp" << std::endl;
    std::cout << "   -v show version" << std::endl;
    std::cout << "   -t service type" << std::endl;
    std::cout << "   -i service index" << std::endl;
    std::cout << "   -p pull config from cfgsvr" << std::endl;
    std::cout << "   -m install app as windows servcie" << std::endl;
    std::cout << "   -u uninstall app as windows servcie" << std::endl;
    std::cout << "   -h show help info." << std::endl;
    std::cout << TSS_SERVER_VER_DECLARE << std::endl;

    return SOAR_RET::SOAR_RET_SUCC;
}

//����ʵ������
Comm_Svrd_Config *Comm_Svrd_Config::instance()
{
    if (instance_ == NULL)
    {
        instance_ = new Comm_Svrd_Config();
    }

    return instance_;
}

//������ʵ��
void Comm_Svrd_Config::clean_instance()
{
    delete instance_;
    instance_ = NULL;
    return;
}

int Comm_Svrd_Config::load_cfgfile()
{
    // ����zerg ����
    int ret = 0;

    ret = ZCE_INI_Implement::read(zerg_cfg_file_.c_str(), &zerg_ptree_);
    if (ret != 0)
    {
        return SOAR_RET::ERROR_FRAMEWORK_READ_ZERG_CFG_FAIL;
    }

    ret = ZCE_INI_Implement::read(framework_cfg_file_.c_str(), &framework_ptree_);
    if (ret != 0)
    {
        return SOAR_RET::ERROR_FRAMEWORK_READ_ZERG_CFG_FAIL;
    }

    // ���ü��سɹ�
    ZLOG_INFO("Comm_Svrd_Config: load framework config succ.");
    return SOAR_RET::SOAR_RET_SUCC;
}

int Comm_Svrd_Config::reload_cfgfile()
{
    ZLOG_INFO("app start reload");
    return load_cfgfile();
}


