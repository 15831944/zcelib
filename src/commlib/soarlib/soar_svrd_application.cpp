/*!
* @copyright  2004-2014  Apache License, Version 2.0 FULLSAIL
* @filename   soar_svrd_application.cpp
* @author     Sailzeng <sailerzeng@gmail.com>
* @version    
* @date       2008��1��22��
* @brief      
*             ���ڽ�sonicmao������෢�����ˣ�
*             
* @details    
*             
*             
*             
* @note       
*             
*/

#include "soar_predefine.h"
#include "soar_svrd_application.h"
#include "soar_zerg_mmappipe.h"
#include "soar_service_info.h"
#include "soar_svrd_config.h"
#include "soar_stat_define.h"
#include "soar_stat_monitor.h"
#include "soar_timer_handler.h"


Comm_Svrd_Appliction *Comm_Svrd_Appliction::base_instance_ = NULL;

Comm_Svrd_Appliction::Comm_Svrd_Appliction():
    app_run_(true),
    app_pause_(false),
    app_reload_(false),
    self_services_id_(),
    run_as_win_serivces_(false),
    max_msg_num_(1024),
    zerg_mmap_pipe_(NULL),
    timer_handler_(NULL)
{
    //��������
    app_author_ = "TSS Platform Server Dev Team.";
    argc_ = 0;
    argv_ = NULL;
}

Comm_Svrd_Appliction::~Comm_Svrd_Appliction()
{
    delete timer_handler_;
    timer_handler_ = NULL;
}

int Comm_Svrd_Appliction::run(int argc, const char *argv[])
{
    // �ȴ�������������, ��������
    int ret = proc_start_args(argc, argv);

    if (ret != 0)
    {
        return ret;
    }

    Comm_Svrd_Config *config = Comm_Svrd_Config::instance();

    if (config->app_run_daemon_)
    {
#ifdef ZCE_OS_WINDOWS
        // �����windows�º�̨���У����Է���ķ�ʽ����
        return win_services_run();
#elif defined(ZCE_OS_LINUX)
        daemon_init();
#endif
    }

    //
    return do_run();
}

/******************************************************************************************
Author          : Sonicmao(MaoJian)  Date Of Creation: 2008��1��22��
Function        : Comm_Svrd_Appliction::Init
Return          : int
Parameter List  :
  Param1: int argc     ������������
  Param2: char* argv[] ����������Ϣ
  Param2: Comm_Svrd_Config *svd_config ������,
Description     :
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int
Comm_Svrd_Appliction::init_instance()
{
    //�����ź�
    process_signal();

    std::string log_file_prefix = Comm_Svrd_Config::instance()->app_run_dir_ + "/log/";
    log_file_prefix += app_base_name_;
    log_file_prefix += "_init";

    //�ȴ���־����¼һ�����ݣ�ֱ����־�������������
    // ��ʼ����־�ù����ķ�ʽ���Ա������������
    ZCE_Trace_LogMsg::instance()->init_size_log(
        log_file_prefix.c_str(),
        false,
        true,
        10 * 1024 * 1024,
        2);

    //���ǻ����ķָ���
    ZLOG_INFO("------------------------------------------------------------------------------------------------------");
    ZLOG_INFO("------------------------------------------------------------------------------------------------------");
    ZLOG_INFO("------------------------------------------------------------------------------------------------------");
    ZLOG_INFO("[framework] %s start init", app_base_name_.c_str());

    int ret = SOAR_RET::SOAR_RET_SUCC;
    //��ʼ��SOCKET��
    ret = ZCE_Server_Toolkit::socket_init();

    if (ret != 0)
    {
        return ret;
    }

    // �л�����Ŀ¼
    ret = ZCE_OS::chdir(Comm_Svrd_Config::instance()->app_run_dir_.c_str());

    if (ret != 0)
    {
        ZLOG_ERROR("[framework] change run directory to %s fail. err=%d",
                   Comm_Svrd_Config::instance()->app_run_dir_.c_str(), errno);
        return ret;
    }

    ZLOG_INFO("[framework] change work dir to %s", Comm_Svrd_Config::instance()->app_run_dir_.c_str());

    // ����Ŀ¼дPID File.
    std::string app_path = Comm_Svrd_Config::instance()->app_run_dir_
                           + "/"
                           + get_app_basename();
    ret = out_pid_file(app_path.c_str(), true);

    if (ret != 0)
    {
        //����д�����ʾ�����������==16����ʾ������PID�ļ�������,
        ZLOG_ERROR("[framework] Create Pid file :%s.pid fail .last error =[%u|%s].",
                   app_path.c_str(), ZCE_OS::last_error(),
                   strerror(ZCE_OS::last_error()));

        ZLOG_ERROR("[framework] If last error == 16, could has a same process already run in this directory."
                   "Please check PID file or system processes.");
        return SOAR_RET::ERROR_WRITE_ERROR_PIDFILE;
    }

    // cfgsdk��ȡ����
    ///*CfgSvrSdk *cfgsvr_sdk = CfgSvrSdk::instance();
    //ret = cfgsvr_sdk->init();*/

    //if (ret != SOAR_RET::SOAR_RET_SUCC)
    //{
    //    ZLOG_ERROR("[framework] cfgsvrsdk init fail. ret=%d", ret);
    //    return ret;
    //}

    ////�������ø����߳�
    //CfgSvrSdk::instance()->start_task();
    //ZLOG_INFO("[framework] cfgsdk init succ. start task succ");

    // ���ؿ������
    Comm_Svrd_Config *svd_config = Comm_Svrd_Config::instance();
    ret = svd_config->init(argc_, argv_);

    if (ret != SOAR_RET::SOAR_RET_SUCC)
    {
        ZLOG_ERROR("[framework] framwork config init fail. ret=%d", ret);
        return ret;
    }

    self_services_id_ = svd_config->self_svr_id_;
    //ȡ��������Ϣ��, ��Ҫ����������ȫ������OK. ���µ�assert��ǿ�Ƽ��
    assert((self_services_id_.services_type_ != SERVICES_ID::INVALID_SERVICES_TYPE) &&
           (self_services_id_.services_id_ != SERVICES_ID::INVALID_SERVICES_ID));

    // ����App����
    ret = load_app_conf();

    if (ret != SOAR_RET::SOAR_RET_SUCC)
    {
        ZLOG_ERROR("[framework] %s load app config fail. ret=%d",
                   __ZCE_FUNCTION__, ret);
        return ret;
    }

    //ʹ��WHEEL�͵Ķ�ʱ������
    ZCE_Timer_Queue::instance(new ZCE_Timer_Wheel(
                                  svd_config->framework_config_.trans_info_.trans_num_ + 1024));

    //��ض�����ӿ�ܵļ�ض���
    Comm_Stat_Monitor::instance()->add_status_item(COMM_STAT_FRATURE_NUM,
                                                   COMM_STAT_ITEM_WITH_NAME);

    //ҵ���������������Ҫ�ܶ��IO����Ĭ��һ��ֵ
#ifdef ZCE_OS_WINDOWS
    ZCE_Reactor::instance(new ZCE_Select_Reactor(1024));
    ZLOG_DEBUG("[framework] ZCE_Reactor and ZCE_Select_Reactor initialized.");
#else
    ZCE_Reactor::instance(new ZCE_Epoll_Reactor(1024));
    ZLOG_DEBUG("[framework] ZCE_Reactor and ZCE_Epoll_Reactor initialized.");
#endif

    //��ʼ���ڴ�ܵ�
    ret = Zerg_MMAP_BusPipe::instance()->getpara_from_zergcfg(
              svd_config->zerg_config_);

    if (ret != SOAR_RET::SOAR_RET_SUCC)
    {
        ZLOG_INFO("[framework] Zerg_MMAP_BusPipe::instance()->getpara_from_zergcfg fail,ret = %d.", ret);
        return ret;
    }

    ret = Zerg_MMAP_BusPipe::instance()->init_after_getcfg(Zerg_App_Frame::MAX_LEN_OF_APPFRAME,
                                                           svd_config->if_restore_pipe_);

    if (ret != SOAR_RET::SOAR_RET_SUCC)
    {
        ZLOG_INFO("[framework] Zerg_MMAP_BusPipe::instance()->init_by_cfg fail,ret = %d.", ret);
        return ret;
    }

    zerg_mmap_pipe_ = Zerg_MMAP_BusPipe::instance();

    ZLOG_INFO("[framework] MMAP Pipe init success,gogogo."
              "The more you have,the more you want. ");

    //ע�����ø��¼�鶨ʱ��
    register_soar_timer();

    // ��ʼ����־
    ret = init_log();

    if (ret != SOAR_RET::SOAR_RET_SUCC)
    {
        ZLOG_ERROR("[framework] init log fail. ret=%d", ret);
        return ret;
    }

    ZLOG_INFO("[framework] Comm_Svrd_Appliction::init_instance Success.");
    return SOAR_RET::SOAR_RET_SUCC;
}

// ����app����, app���Լ�ʵ��
int Comm_Svrd_Appliction::load_app_conf()
{
    return SOAR_RET::SOAR_RET_SUCC;
}

// app���˳�
void Comm_Svrd_Appliction::exit()
{
}

//�õ�app_base_name_��app_run_name_
int Comm_Svrd_Appliction::create_app_name(const char *argv_0)
{
    app_run_name_ = argv_0;
    // ȡ��base name
    char str_base_name[PATH_MAX + 1];
    str_base_name[PATH_MAX] = '\0';
    ZCE_OS::basename(argv_0, str_base_name, PATH_MAX);

#if defined ZCE_OS_WINDOWS

    //Windows��Ҫȥ��,EXE��׺
    const size_t WIN_EXE_SUFFIX_LEN = 4;
    size_t name_len = strlen(str_base_name);

    if (name_len <= WIN_EXE_SUFFIX_LEN)
    {
        ZLOG_ERROR("[framework] Exe file name is not expect?Path name[%s].", argv_0);
        return -1;
    }

    //����к�׺��ȡ����û�оͷ�Ѽ��
    if (strcasecmp(str_base_name + name_len - WIN_EXE_SUFFIX_LEN, ".EXE") == 0)
    {
        str_base_name[name_len - WIN_EXE_SUFFIX_LEN] = '\0';
    }

#endif

    //����ǵ��԰汾��ȥ����׺����_d
#if defined (DEBUG) || defined (_DEBUG)

    //����ǵ��԰汾��ȥ����׺����_d
    const size_t DEBUG_SUFFIX_LEN = 2;
    size_t debug_name_len = strlen(str_base_name);

    if (debug_name_len <= DEBUG_SUFFIX_LEN)
    {
        ZLOG_ERROR("[framework] Exe file name is not debug _d suffix?str_base_name[%s].", str_base_name);
        return -1;
    }

    if (0 == strcasecmp(str_base_name + debug_name_len - DEBUG_SUFFIX_LEN, "_D") )
    {
        str_base_name[debug_name_len - DEBUG_SUFFIX_LEN] = '\0';
    }

#endif

    app_base_name_ = str_base_name;

    return SOAR_RET::SOAR_RET_SUCC;
}

//���¼�������
int Comm_Svrd_Appliction::reload_instance()
{
    int ret = Comm_Svrd_Config::instance()->reload();
    if (ret != SOAR_RET::SOAR_RET_SUCC)
    {
        ZLOG_ERROR("load frame config fail. ret=%d", ret);
        return ret;
    }

    // ���¼��ؿ�ܵ�����
    g_bill.set_bill_filter(Comm_Svrd_Config::instance()->framework_config_.log_info_.bill_filter_type_count_,
        Comm_Svrd_Config::instance()->framework_config_.log_info_.bill_filter_type_list_);

    return SOAR_RET::SOAR_RET_SUCC;
}

//�˳��Ĺ���
int Comm_Svrd_Appliction::exit_instance()
{
    //����Ҫ���Ӷ��̵߳ĵȴ�
    ZCE_Thread_Wait_Manager::instance()->wait_all();
    ZCE_Thread_Wait_Manager::clean_instance();

    //
    ZCE_Reactor::instance()->close();
    //����Instacne
    //Transaction_Manager::clean_instance();

    Zerg_MMAP_BusPipe::clean_instance();

    Comm_Svrd_Config::clean_instance();
    //
    ZLOG_INFO("[framework] %s exit_instance Succ.Have Fun.!!!",
              app_run_name_.c_str());
    ZLOG_INFO("------------------------------------------------------------------------------------------------------");
    ZLOG_INFO("------------------------------------------------------------------------------------------------------");
    ZLOG_INFO("------------------------------------------------------------------------------------------------------");

    return SOAR_RET::SOAR_RET_SUCC;
}

//���ý�����ͣ���еı�־
void Comm_Svrd_Appliction::set_pause_sign(bool app_pause)
{
    app_pause_ = app_pause;
}

//���ý����Ƿ����еı�־
void Comm_Svrd_Appliction::set_run_sign(bool app_run)
{
    app_run_ = app_run;
}

void Comm_Svrd_Appliction::set_reload(bool app_reload)
{
    app_reload_ = app_reload;
}

int Comm_Svrd_Appliction::process_signal(void)
{
    //���Ӳ����ź�,������
    ZCE_OS::signal(SIGHUP, SIG_IGN);
    ZCE_OS::signal(SIGPIPE, SIG_IGN);
    ZCE_OS::signal(SIGCHLD, SIG_IGN);

#ifdef ZCE_OS_WINDOWS
    //Windows�������˳���������������Ctrl + C �˳�
    SetConsoleCtrlHandler((PHANDLER_ROUTINE)exit_signal, TRUE);
#else
    //��������źű��Ͽ�Ϊ�˳��ź�
    ZCE_OS::signal(SIGINT, exit_signal);
    ZCE_OS::signal(SIGQUIT, exit_signal);
    ZCE_OS::signal(SIGTERM, exit_signal);

    //���¼��ز�������,����SIGUSR1 kill -10
    ZCE_OS::signal(SIGUSR1, reload_config_signal);
#endif

    //SIGUSR1,SIGUSR2����������ɵ��Լ��Ļ�,
    return SOAR_RET::SOAR_RET_SUCC;
}

int Comm_Svrd_Appliction::daemon_init()
{
    //Daemon �������,�����Ҳ�����Ŀ¼·��,

#if defined (ZCE_OS_LINUX)

    pid_t pid = ZCE_OS::fork();

    if (pid < 0)
    {
        return SOAR_RET::ERROR_FORK_FAIL;
    }
    else if (pid > 0)
    {
        ::exit(0);
    }

#endif

    ZCE_OS::setsid();
    ZCE_OS::umask(0);

#if defined (ZCE_OS_WINDOWS)
    //����Console�ı�����Ϣ
    std::string out_str = get_app_basename();
    out_str += " ";
    out_str += app_author_;
    ::SetConsoleTitle(out_str.c_str());
#endif

    return SOAR_RET::SOAR_RET_SUCC;
}

#ifdef ZCE_OS_WINDOWS

BOOL Comm_Svrd_Appliction::exit_signal(DWORD )
{
    base_instance_->set_run_sign(false);
    return TRUE;
}

#else

void Comm_Svrd_Appliction::exit_signal(int )
{
    base_instance_->set_run_sign(false);
    return;
}

// USER1�źŴ�����
void Comm_Svrd_Appliction::reload_config_signal(int)
{
    // �źŴ������в�����IO�Ȳ�������Ĳ�����������������
    base_instance_->set_reload(true);
    return;
}

#endif

//������־�����ȼ�
void Comm_Svrd_Appliction::set_log_priority(ZCE_LOG_PRIORITY log_prio)
{
    ZCE_Trace_LogMsg::instance()->set_log_priority(log_prio);
}

//�����־�����ȼ�
ZCE_LOG_PRIORITY Comm_Svrd_Appliction::get_log_priority()
{
    return ZCE_Trace_LogMsg::instance()->get_log_priority();
}

//�õ�������Ϣ�����ܰ���·����Ϣ
const char *Comm_Svrd_Appliction::get_app_runname()
{
    return app_run_name_.c_str();
}

//�õ�����������ƣ���ȥ����·����WINDOWS��ȥ���˺�׺
const char *Comm_Svrd_Appliction::get_app_basename()
{
    return app_base_name_.c_str();
}

//ע�ᶨʱ��, ��ʱ��������Ƿ����
int Comm_Svrd_Appliction::register_soar_timer()
{
    // ע���ܶ�ʱ��
    ZCE_ASSERT(timer_handler_ == NULL);
    ZCE_Timer_Queue *timer_queue = ZCE_Timer_Queue::instance();
    timer_handler_ = new Comm_Timer_Handler(timer_queue);

    // ͨ���������ж��Ƿ�app���̻���zerg����
    bool is_app = true;
    if (strncasecmp(app_base_name_.c_str(), "zergsvr", strlen("zergsvr") == 0))
    {
        // zergsvr
        is_app = false;
    }
    
    timer_handler_->init(CfgSvrSdk::instance()->get_game_id(), is_app, this);

    ZCE_Time_Value delay;
    ZCE_Time_Value interval;

    //�´δ���ʱ��
    delay.set(0, 0);

    //ÿ�δ������ʱ�䣬ÿ100���봥��һ��
    interval.set(0, FRAMEWORK_TIMER_INTERVAL);

    //ע�ᶨʱ��
    timer_queue->schedule_timer(timer_handler_,
                                NULL,
                                delay,
                                interval);

    return SOAR_RET::SOAR_RET_SUCC;
}

//���¼�������
int Comm_Svrd_Appliction::reload_config()
{
    // �������¼��صı�־
    app_reload_ = false;

    // �ȿ��reload
    int ret = reload_instance();

    if (ret != SOAR_RET::SOAR_RET_SUCC)
    {
        ZLOG_ERROR("framework reload config error:ret=%d", ret);
        return ret;
    }

    // app load config
    ret = load_app_conf();

    if (ret != SOAR_RET::SOAR_RET_SUCC)
    {
        ZLOG_ERROR("load app config error, ret=%d", ret);
        return ret;
    }

    // Ȼ��app reload
    ret = reload();

    if (ret != SOAR_RET::SOAR_RET_SUCC)
    {
        ZLOG_ERROR("app reload config error, ret=%d", ret);
        return ret;
    }

    return SOAR_RET::SOAR_RET_SUCC;
}

#if defined ZCE_OS_WINDOWS

//���з���
int Comm_Svrd_Appliction::win_services_run()
{
    char service_name[PATH_MAX + 1];
    service_name[PATH_MAX] = '\0';
    strncpy(service_name, app_base_name_.c_str(), PATH_MAX);

    SERVICE_TABLE_ENTRY st[] =
    {
        { service_name, (LPSERVICE_MAIN_FUNCTION)win_service_main },
        { NULL, NULL }
    };

    BOOL b_ret =::StartServiceCtrlDispatcher(st);

    if (b_ret)
    {
        //LogEvent(_T("Register Service Main Function Success!"));
    }
    else
    {
        UINT error_info = ::GetLastError();
        ZCE_UNUSED_ARG(error_info);
        //LogEvent(_T("Register Service Main Function Error!"));
    }

    return 0;
}

//��װ����
int Comm_Svrd_Appliction::win_services_install()
{

    if (win_services_isinstalled())
    {
        printf("install service fail. service %s already exist", app_base_name_.c_str());
        return 0;
    }

    //�򿪷�����ƹ�����
    SC_HANDLE handle_scm = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    if (handle_scm == NULL)
    {
        //::MessageBox(NULL, _T("Couldn't open service manager"), app_base_name_.c_str(), MB_OK);
        printf("can't open service manager.\n");
        return FALSE;
    }

    // Get the executable file path
    char file_path[MAX_PATH + 1];
    file_path[MAX_PATH] = '\0';
    ::GetModuleFileName(NULL, file_path, MAX_PATH);

    //��������
    SC_HANDLE handle_services = ::CreateService(
                                    handle_scm,
                                    app_base_name_.c_str(),
                                    service_name_.c_str(),
                                    SERVICE_ALL_ACCESS,
                                    SERVICE_WIN32_OWN_PROCESS,
                                    SERVICE_DEMAND_START,
                                    SERVICE_ERROR_NORMAL,
                                    file_path,
                                    NULL,
                                    NULL,
                                    _T(""),
                                    NULL,
                                    NULL);

    if (handle_services == NULL)
    {
        printf("install service %s fail. err=%d\n", app_base_name_.c_str(),
               GetLastError());
        ::CloseServiceHandle(handle_scm);
        //MessageBox(NULL, _T("Couldn't create service"), app_base_name_.c_str(), MB_OK);
        return -1;
    }

    // �޸�����
    SC_LOCK lock = LockServiceDatabase(handle_scm);

    if (lock != NULL)
    {
        SERVICE_DESCRIPTION desc;
        desc.lpDescription = (LPSTR)service_desc_.c_str();

        ChangeServiceConfig2(handle_services, SERVICE_CONFIG_DESCRIPTION, &desc);
        UnlockServiceDatabase(handle_scm);
    }

    ::CloseServiceHandle(handle_services);
    ::CloseServiceHandle(handle_scm);
    printf("install service %s succ.\n", app_base_name_.c_str());

    return SOAR_RET::SOAR_RET_SUCC;
}

//ж�ط���
int Comm_Svrd_Appliction::win_services_uninstall()
{
    if (!win_services_isinstalled())
    {
        printf("uninstall fail. service %s is not exist.\n", app_base_name_.c_str());
        return 0;
    }

    SC_HANDLE handle_scm = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    if (handle_scm == NULL)
    {
        //::MessageBox(NULL, _T("Couldn't open service manager"), app_base_name_.c_str(), MB_OK);
        printf("uninstall fail. can't open service manager");
        return FALSE;
    }

    SC_HANDLE handle_services = ::OpenService(handle_scm,
                                              app_base_name_.c_str(),
                                              SERVICE_STOP | DELETE);

    if (handle_services == NULL)
    {
        ::CloseServiceHandle(handle_scm);
        //::MessageBox(NULL, _T("Couldn't open service"), app_base_name_.c_str(), MB_OK);
        printf("can't open service %s\n", app_base_name_.c_str());
        return -1;
    }

    SERVICE_STATUS status;
    ::ControlService(handle_services, SERVICE_CONTROL_STOP, &status);

    //ɾ������
    BOOL bDelete = ::DeleteService(handle_services);
    ::CloseServiceHandle(handle_services);
    ::CloseServiceHandle(handle_scm);

    if (bDelete)
    {
        printf("uninstall service %s succ.\n", app_base_name_.c_str());
        return 0;
    }

    printf("uninstall service %s fail.\n", app_base_name_.c_str());
    //LogEvent(_T("Service could not be deleted"));
    return -1;

}

//�������Ƿ�װ
bool Comm_Svrd_Appliction::win_services_isinstalled()
{
    bool b_result = false;

    //�򿪷�����ƹ�����
    SC_HANDLE handle_scm = ::OpenSCManager(NULL, NULL, SC_MANAGER_ALL_ACCESS);

    if (handle_scm != NULL)
    {
        //�򿪷���
        SC_HANDLE handle_service = ::OpenService(handle_scm, app_base_name_.c_str(), SERVICE_QUERY_CONFIG);

        if (handle_service != NULL)
        {
            b_result = true;
            ::CloseServiceHandle(handle_service);
        }

        ::CloseServiceHandle(handle_scm);
    }

    return b_result;
}

//�������к���
void WINAPI Comm_Svrd_Appliction::win_service_main()
{
    //WIN�����õ�״̬
    static SERVICE_STATUS_HANDLE handle_service_status = NULL;

    SERVICE_STATUS status;

    status.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
    status.dwCurrentState = SERVICE_STOPPED;
    status.dwControlsAccepted = SERVICE_ACCEPT_STOP;
    status.dwWin32ExitCode = 0;
    status.dwServiceSpecificExitCode = 0;
    status.dwCheckPoint = 0;
    status.dwWaitHint = 0;

    // Register the control request handler
    status.dwCurrentState = SERVICE_START_PENDING;
    status.dwControlsAccepted = SERVICE_ACCEPT_STOP;

    //ע��������
    handle_service_status = ::RegisterServiceCtrlHandler(Comm_Svrd_Appliction::instance()->get_app_basename(),
                                                         win_services_ctrl);

    if (handle_service_status == NULL)
    {
        //LogEvent(_T("Handler not installed"));
        return;
    }

    SetServiceStatus(handle_service_status, &status);

    status.dwWin32ExitCode = S_OK;
    status.dwCheckPoint = 0;
    status.dwWaitHint = 0;
    status.dwCurrentState = SERVICE_RUNNING;
    SetServiceStatus(handle_service_status, &status);

    base_instance_->do_run();

    status.dwCurrentState = SERVICE_STOPPED;
    SetServiceStatus(handle_service_status, &status);
    //LogEvent(_T("Service stopped"));
}

//�������̨����Ҫ�Ŀ��ƺ���
void WINAPI Comm_Svrd_Appliction::win_services_ctrl(DWORD op_code)
{
    switch (op_code)
    {
        case SERVICE_CONTROL_STOP:
            //
            base_instance_->app_run_ = false;
            break;

        case SERVICE_CONTROL_PAUSE:
            break;

        case SERVICE_CONTROL_CONTINUE:
            break;

        case SERVICE_CONTROL_INTERROGATE:
            break;

        case SERVICE_CONTROL_SHUTDOWN:
            break;

        default:
            //LogEvent(_T("Bad service request"));
            break;
    }
}

#endif //#if defined ZCE_OS_WINDOWS

//�õ�ʵ��ָ��
Comm_Svrd_Appliction *Comm_Svrd_Appliction::instance()
{
    return base_instance_;
}

int
Comm_Svrd_Appliction::proc_start_args(int argc, const char *argv[])
{
    int ret = 0;
    argc_ = argc;
    argv_ = argv;

    //Comm_Svrd_Appliction ֻ��������һ��ʵ��������������ط���ʼ����staticָ��
    base_instance_ = this;

    //�õ�APP�����֣�ȥ��·������׺������
    ret = create_app_name(argv[0]);

    if (0 != ret )
    {
        ZLOG_ERROR("svr create_app_base_name init fail. ret=%d", ret);
        return ret;
    }

    Comm_Svrd_Config *svd_config = Comm_Svrd_Config::instance();

    // ������������
    ret = svd_config->proc_start_arg(argc, argv);

    if (ret != 0)
    {
        ZLOG_ERROR("svr config init fail. ret=%d", ret);
        return ret;
    }

#ifdef ZCE_OS_WINDOWS

    if (svd_config->app_install_service_)
    {
        // ��װ����
        ret = win_services_install();
        // ֱ���˳���
        ::exit(ret);
    }

    if (svd_config->app_uninstall_service_)
    {
        // ж�ط���
        ret = win_services_uninstall();
        // ֱ���˳���
        ::exit(ret);
    }

#endif

    return SOAR_RET::SOAR_RET_SUCC;
}

int
Comm_Svrd_Appliction::do_run()
{
    // ���Ҫ�ȳ�ʼ��
    int ret = init_instance();

    if (ret != SOAR_RET::SOAR_RET_SUCC)
    {
        ZLOG_ERROR("application: init_instance fail. ret=%d", ret);
        return ret;
    }

    // ����app��init����
    ret = init();

    if (ret != SOAR_RET::SOAR_RET_SUCC)
    {
        ZLOG_ERROR("application: init_app fail. ret=%d", ret);
        return ret;
    }

    ZLOG_INFO("[framework]application: init succ. start run");
    ret = run_instance();

    if (ret != SOAR_RET::SOAR_RET_SUCC)
    {
        ZLOG_ERROR("application: run_instance fail. ret=%d", ret);
    }

    // ����run_instance����ʲôֵ���˳�ʱ����������Ҫ��
    // app���˳�����
    exit();
    // �����ǿ�ܵ��˳�
    exit_instance();

    ZLOG_INFO("[framework] application exit.");

    return ret;
}

void
Comm_Svrd_Appliction::set_service_info(const char *svc_name, const char *svc_desc)
{
    if (svc_name != NULL)
    {
        service_name_ = svc_name;
    }

    if (svc_desc != NULL)
    {
        service_desc_ = svc_desc;
    }
}

int
Comm_Svrd_Appliction::init_log()
{
    Comm_Svrd_Config *config = Comm_Svrd_Config::instance();
    int ret = 0;
    if (ret != SOAR_RET::SOAR_RET_SUCC)
    {
        ZLOG_ERROR("[framework] init bill fail. ret=%d", ret);
        return ret;
    }

    if (ret != SOAR_RET::SOAR_RET_SUCC)
    {
        ZLOG_ERROR("[framework] init stat fail. ret=%d", ret);
        return ret;
    }

    ZLOG_DEBUG("log instance finalize .");
    //�ر�ԭ������־�������
    ZCE_Trace_LogMsg::instance()->finalize();

    // ��ʼ����־
    ZCE_Trace_LogMsg::instance()->initialize((ZCE_LOGFILE_DEVIDE)config->framework_config_.log_info_.log_div_type_,
                                             config->log_file_prefix_.c_str(),
                                             false,
                                             true,
                                             config->framework_config_.log_info_.max_log_file_size_,
                                             config->framework_config_.log_info_.max_log_file_num_,
                                             config->framework_config_.log_info_.log_output_,
                                             LOG_HEAD_RECORD_CURRENTTIME | LOG_HEAD_RECORD_LOGLEVEL);

    ZLOG_DEBUG("log instance reinit .");

    return SOAR_RET::SOAR_RET_SUCC;
}

