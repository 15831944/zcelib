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
#include "soar_svrd_app_base.h"
#include "soar_zerg_mmappipe.h"
#include "soar_zerg_svc_info.h"
#include "soar_svrd_cfg_fsm.h"
#include "soar_svrd_timer_base.h"
#include "soar_stat_define.h"
#include "soar_stat_monitor.h"




Comm_Svrd_Appliction::Comm_Svrd_Appliction(Server_Config_Base *config) :
    self_services_id_(),
    run_as_win_serivces_(false),
    max_msg_num_(1024),
    zerg_mmap_pipe_(NULL),
    timer_handler_(NULL),
    config_(config)
{
    //��������
    app_author_ = "TSS Platform Server Dev Team.";
}

Comm_Svrd_Appliction::~Comm_Svrd_Appliction()
{
    delete timer_handler_;
    timer_handler_ = NULL;
}

//int Comm_Svrd_Appliction::run(int argc, const char *argv[])
//{

//
//    Server_Config_FSM *config = Server_Config_FSM::instance();
//
//    if (config->app_run_daemon_)
//    {
//#ifdef ZCE_OS_WINDOWS
//        // �����windows�º�̨���У����Է���ķ�ʽ����
//        return win_services_run();
//#elif defined(ZCE_OS_LINUX)
//        daemon_init();
//#endif
//    }
//
//    //
//    return do_run();
//}

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
int Comm_Svrd_Appliction::init(int argc, const char *argv[])
{

    //Comm_Svrd_Appliction ֻ��������һ��ʵ��������������ط���ʼ����staticָ��
    base_instance_ = this;



    int ret = SOAR_RET::SOAR_RET_SUCC;
    //�õ�APP�����֣�ȥ��·������׺������
    ret = create_app_name(argv[0]);
    if (0 != ret)
    {
        ZLOG_ERROR("svr create_app_base_name init fail. ret=%d", ret);
        return ret;
    }

    // ������������
    ret = config_->start_arg(argc, argv);
    if (ret != 0)
    {
        ZLOG_ERROR("svr config start_arg init fail. ret=%d", ret);
        return ret;
    }


#ifdef ZCE_OS_WINDOWS

    if (config_->win_install_service_)
    {
        // ��װ����
        ret = win_services_install();
        // ֱ���˳���
        ::exit(ret);
    }
    if (config_->win_uninstall_service_)
    {
        // ж�ط���
        ret = win_services_uninstall();
        // ֱ���˳���
        ::exit(ret);
    }

#endif


    //�����ź�
    process_signal();

    std::string log_file_prefix = Server_Config_FSM::instance()->app_run_dir_ + "/log/";
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


    //��ʼ��SOCKET��
    ret = ZCE_Server_Base::socket_init();
    if (ret != 0)
    {
        return ret;
    }

    // �л�����Ŀ¼
    ret = ZCE_LIB::chdir(Server_Config_FSM::instance()->app_run_dir_.c_str());
    if (ret != 0)
    {
        ZLOG_ERROR("[framework] change run directory to %s fail. err=%d",
                   Server_Config_FSM::instance()->app_run_dir_.c_str(), errno);
        return ret;
    }

    ZLOG_INFO("[framework] change work dir to %s", Server_Config_FSM::instance()->app_run_dir_.c_str());

    // ����Ŀ¼дPID File.
    std::string app_path = Server_Config_FSM::instance()->app_run_dir_
                           + "/"
                           + get_app_basename();
    ret = out_pid_file(app_path.c_str(), true);

    if (ret != 0)
    {
        //����д�����ʾ�����������==16����ʾ������PID�ļ�������,
        ZLOG_ERROR("[framework] Create Pid file :%s.pid fail .last error =[%u|%s].",
                   app_path.c_str(), ZCE_LIB::last_error(),
                   strerror(ZCE_LIB::last_error()));

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

    // ���ؿ������,�������麯����Ҳ����õ��ǿ�ܵ����ö�ȡ
    ret = config_->initialize(argc, argv);
    if (ret != SOAR_RET::SOAR_RET_SUCC)
    {
        ZLOG_ERROR("[framework] framwork config init fail. ret=%d", ret);
        return ret;
    }

    self_services_id_ = config_->self_svc_id_;
    //ȡ��������Ϣ��, ��Ҫ����������ȫ������OK. ���µ�assert��ǿ�Ƽ��
    ZCE_ASSERT((self_services_id_.services_type_ != SERVICES_ID::INVALID_SERVICES_TYPE) &&
               (self_services_id_.services_id_ != SERVICES_ID::INVALID_SERVICES_ID));

    if (ret != SOAR_RET::SOAR_RET_SUCC)
    {
        ZLOG_ERROR("[framework] %s load app config fail. ret=%d",
                   __ZCE_FUNCTION__, ret);
        return ret;
    }

    //ʹ��WHEEL�͵Ķ�ʱ������
    ZCE_Timer_Queue::instance(new ZCE_Timer_Wheel(
                                  config_->timer_nuamber_));

    //��ض������ӿ�ܵļ�ض���
    Comm_Stat_Monitor::instance()->add_status_item(COMM_STAT_FRATURE_NUM,
                                                   COMM_STAT_ITEM_WITH_NAME);

    //ҵ���������������Ҫ�ܶ��IO������Ĭ��һ��ֵ
#ifdef ZCE_OS_WINDOWS
    ZCE_Reactor::instance(new ZCE_Select_Reactor(1024));
    ZLOG_DEBUG("[framework] ZCE_Reactor and ZCE_Select_Reactor u.");
#else
    ZCE_Reactor::instance(new ZCE_Epoll_Reactor(1024));
    ZLOG_DEBUG("[framework] ZCE_Reactor and ZCE_Epoll_Reactor initialized.");
#endif

    //��ʼ���ڴ�ܵ�
    //ret = Zerg_MMAP_BusPipe::instance()->getpara_from_zergcfg(
    //          svd_config->zerg_config_);

    if (ret != SOAR_RET::SOAR_RET_SUCC)
    {
        ZLOG_INFO("[framework] Zerg_MMAP_BusPipe::instance()->getpara_from_zergcfg fail,ret = %d.", ret);
        return ret;
    }

    ret = Zerg_MMAP_BusPipe::instance()->
          init_after_getcfg(Zerg_App_Frame::MAX_LEN_OF_APPFRAME,
                            config_->if_restore_pipe_);
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




//���¼�������
//int Comm_Svrd_Appliction::reload()
//{
//    int ret = Server_Config_FSM::instance()->reload_cfgfile();
//    if (ret != SOAR_RET::SOAR_RET_SUCC)
//    {
//        ZLOG_ERROR("load frame config fail. ret=%d", ret);
//        return ret;
//    }
//    return SOAR_RET::SOAR_RET_SUCC;
//}

//�˳��Ĺ���
int Comm_Svrd_Appliction::exit()
{
    //����Ҫ���Ӷ��̵߳ĵȴ�
    ZCE_Thread_Wait_Manager::instance()->wait_all();
    ZCE_Thread_Wait_Manager::clean_instance();

    //
    ZCE_Reactor::instance()->close();


    Zerg_MMAP_BusPipe::clean_instance();

    Server_Config_FSM::clean_instance();
    //
    ZLOG_INFO("[framework] %s exit_instance Succ.Have Fun.!!!",
              app_run_name_.c_str());
    ZLOG_INFO("------------------------------------------------------------------------------------------------------");
    ZLOG_INFO("------------------------------------------------------------------------------------------------------");
    ZLOG_INFO("------------------------------------------------------------------------------------------------------");

    return SOAR_RET::SOAR_RET_SUCC;
}








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



//ע�ᶨʱ��, ��ʱ��������Ƿ����
int Comm_Svrd_Appliction::register_soar_timer()
{
    // ע���ܶ�ʱ��
    ZCE_ASSERT(timer_handler_ == NULL);
    ZCE_Timer_Queue *timer_queue = ZCE_Timer_Queue::instance();
    timer_handler_ = new Server_Timer_Base(timer_queue);

    // ͨ���������ж��Ƿ�app���̻���zerg����
    bool is_app = true;
    if (strncasecmp(app_base_name_.c_str(), "zergsvr", strlen("zergsvr") == 0))
    {
        // zergsvr
        is_app = false;
    }

    //timer_handler_->init(CfgSvrSdk::instance()->get_game_id(), is_app, this);

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
    int ret = reload();

    if (ret != SOAR_RET::SOAR_RET_SUCC)
    {
        ZLOG_ERROR("framework reload config error:ret=%d", ret);
        return ret;
    }

    return SOAR_RET::SOAR_RET_SUCC;
}




//int Comm_Svrd_Appliction::do_run()
//{
//    // ���Ҫ�ȳ�ʼ��
//    int ret = init_instance();
//
//    if (ret != SOAR_RET::SOAR_RET_SUCC)
//    {
//        ZLOG_ERROR("application: init_instance fail. ret=%d", ret);
//        return ret;
//    }
//
//    // ����app��init����
//    ret = init();
//
//    if (ret != SOAR_RET::SOAR_RET_SUCC)
//    {
//        ZLOG_ERROR("application: init_app fail. ret=%d", ret);
//        return ret;
//    }
//
//    ZLOG_INFO("[framework]application: init succ. start run");
//    ret = run_instance();
//
//    if (ret != SOAR_RET::SOAR_RET_SUCC)
//    {
//        ZLOG_ERROR("application: run_instance fail. ret=%d", ret);
//    }
//
//    // ����run_instance����ʲôֵ���˳�ʱ������������Ҫ��
//    // app���˳�����
//    exit();
//    // �����ǿ�ܵ��˳�
//    exit_instance();
//
//    ZLOG_INFO("[framework] application exit.");
//
//    return ret;
//}



int Comm_Svrd_Appliction::init_log()
{
    Server_Config_FSM *config = Server_Config_FSM::instance();
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
