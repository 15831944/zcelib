/******************************************************************************************
Copyright           : 2000-2004, Tencent Technology (Shenzhen) Company Limited.
FileName            : zerg_application.cpp
Author              : Sail(ZENGXING)
Version             :
Date Of Creation    : 2005��11��17��
Description         :

Others              :
Function List       :
    1.  ......
Modification History:
    1.Date  :
      Author  :
      Modification  :
******************************************************************************************/

#include "zerg_predefine.h"
#include "zerg_application.h"
#include "zerg_console_handler.h"
#include "zerg_udp_ctrl_handler.h"
#include "zerg_tcp_ctrl_handler.h"
#include "zerg_ip_restrict.h"
#include "zerg_comm_manager.h"
#include "zerg_configure.h"
#include "zerg_stat_define.h"

/****************************************************************************************************
class  Zerg_Service_App
****************************************************************************************************/

//���ӵ�staticʵ��
Zerg_Service_App *Zerg_Service_App::instance_ = NULL;

//����Ҫ͵͵����
Zerg_Service_App::Zerg_Service_App():
    timer_queue_(NULL),
    zerg_comm_mgr_(NULL),
    conf_timestamp_(0)
{
    config_ = new conf_zerg::ZERG_CONFIG();
}

Zerg_Service_App::~Zerg_Service_App()
{
    if (config_)
    {
        delete config_;
        config_ = NULL;
    }

    //�ر�timer_queue_
    if (timer_queue_)
    {
        delete timer_queue_;
        timer_queue_ = NULL;
    }

    //zerg_acceptor_ ���Լ�ɾ��?
}

/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2005��11��17��
Function        : Zerg_Service_App::init_instance
Return          : int
Parameter List  :
  Param1: int argc
  Param2: char* argv[]
Description     : ����������������
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int
Zerg_Service_App::init_instance()
{
    int ret = 0;

    ret = Comm_Svrd_Appliction::init_instance();

    if (ret != SOAR_RET::SOAR_RET_SUCC)
    {
        return ret;
    }

    //��ʼ��ͳ��ģ��
    //��Ϊ���ó�ʼ��ʱ������÷�������ȡip������ͳ�ƣ������Ҫ��ǰ��ʼ��
    ret = Comm_Stat_Monitor::instance()->initialize(true,
                                                    Comm_Svrd_Config::instance()->self_svr_id_,
                                                    0,
                                                    NULL,
                                                    false);

    if (ret != 0)
    {
        ZLOG_ERROR("zce_Server_Status init fail. ret=%d", ret);
        return ret;
    }

    config_ = &(Comm_Svrd_Config::instance()->zerg_config_);
    ret = Zerg_Server_Config::instance()->init(config_);

    if (ret != SOAR_RET::SOAR_RET_SUCC)
    {
        return ret;
    }

    size_t max_accept = 0, max_connect = 0, max_peer = 0;
    TCP_Svc_Handler::get_max_peer_num(max_accept, max_connect);
    max_peer = max_accept + max_connect + 16;

    //���¶�ʱ��
    //size_t old_node_num;
    //ZCE_Timer_Queue::instance()->extend_node(max_peer, old_node_num);

    //Reactor���޸�һ��Ҫ����ǰ��(��ȡ���ú���)�����ٳ���4�ο�
    //��Ȼ��ͬһ�����������˺ü��Ρ����µ�һ����20070929��
#ifdef ZCE_OS_WINDOWS
    //
    ZCE_Reactor::instance(new zce_Select_Reactor(max_peer));
    ZLOG_DEBUG("[zergsvr] ZCE_Reactor and zce_Select_Reactor initialized.");
#else
    ZCE_Reactor::instance(new zce_Epoll_Reactor(max_peer));
    ZLOG_INFO("[zergsvr] ZCE_Reactor and zce_Epoll_Reactor initialized.");
#endif

    //�õ�ϵͳʱ�ӵ�,
    new Zerg_App_Timer_Handler(ZCE_Timer_Queue::instance());

    //���÷��ͽ��ջ��������
    ZBuffer_Storage::instance()->init_buflist_by_hdlnum(max_peer);

    //Zerg_Comm_Manager�����ò��ֿ��Զ�̬�ظ�����
    Zerg_IPRestrict_Mgr::instance();
    //ͨ�Ź�������ʼ��
    zerg_comm_mgr_ = Zerg_Comm_Manager::instance();
    //����̨��ʼ��
    Zerg_Console_Handler::instance(new Zerg_Console_Handler(ZCE_Reactor::instance()));

    //���ض�̬���ò���
    ret = reload_daynamic_config();

    if ( ret != SOAR_RET::SOAR_RET_SUCC )
    {
        ZLOG_INFO("[zergsvr] ReloadDynamicConfig fail,ret = %d.", ret);
        return ret;
    }

    //
    ZLOG_INFO("[zergsvr] ReloadDynamicConfig Succ.Ooooo!Some people believe that God created the world,but.");

    //-----------------------------------------------------------------------------------------------
    //��ʼ����̬����
    ret = TCP_Svc_Handler::init_all_static_data();

    if ( ret != SOAR_RET::SOAR_RET_SUCC )
    {
        return ret;
    }

    //
    ret = UDP_Svc_Handler::init_all_static_data();

    if ( ret != SOAR_RET::SOAR_RET_SUCC )
    {
        return ret;
    }

    //��ʼ�����˿�, ������������������ֵ��ʹ��, ����ʹ��Ĭ��ֵ, window��linux��Ĭ��ֵ��ͬ
    if (Zerg_Server_Config::instance()->backlog_ == 0)
    {
        ret = zerg_comm_mgr_->init_socketpeer(Zerg_Server_Config::instance()->self_svc_info_);
    }
    else
    {
        ret = zerg_comm_mgr_->init_socketpeer(Zerg_Server_Config::instance()->self_svc_info_,
            Zerg_Server_Config::instance()->backlog_);
    }

    if (ret != SOAR_RET::SOAR_RET_SUCC)
    {
        return ret;
    }

    //��ʼ�����ݶ˿�
    for (unsigned int i = 0; i < config_->self_cfg.slave_svr_count; ++i)
    {
        ret = zerg_comm_mgr_->init_socketpeer(Zerg_Server_Config::instance()->slave_svc_ary_[i]);

        if (ret != SOAR_RET::SOAR_RET_SUCC)
        {
            return ret;
        }
    }

    // ��ʼ�������ṩUdp����Ķ˿�
    uint32_t ary_size = static_cast<uint32_t>(Zerg_Server_Config::instance()->external_udp_svr_ary_.size());
    for (unsigned int i = 0; i < ary_size; i ++)
    {
        ret = zerg_comm_mgr_->init_socketpeer(Zerg_Server_Config::instance()->external_udp_svr_ary_[i]);
        if (ret != SOAR_RET::SOAR_RET_SUCC)
        {
            return ret;
        }
    }

    ary_size = static_cast<uint32_t>(Zerg_Server_Config::instance()->extern_svc_ary_.size());
    //��ʼ���ⲿ����˿�
    for (unsigned int i = 0; i < ary_size; ++ i)
    {
        ret = zerg_comm_mgr_->init_socketpeer(Zerg_Server_Config::instance()->extern_svc_ary_[i],
                                              zce_DEFAULT_BACKLOG,
                                              true,
                                              config_->extern_svc_cfg.extern_svc_item[i].proto_cfg_index);

        if (ret != SOAR_RET::SOAR_RET_SUCC)
        {
            return ret;
        }
    }

    ZLOG_INFO("[zergsvr] init_instance Succ.Have Fun.!!!");
    //���̼�أ������ã�����˵������ڳ����ʼ��������������Ա�֤�������ڴ�ĳ�ʼ�����������,

    return SOAR_RET::SOAR_RET_SUCC;
}

int 
Zerg_Service_App::reload()
{
    int ret = reload_daynamic_config();

    if ( ret != SOAR_RET::SOAR_RET_SUCC )
    {
        ZLOG_INFO("[zergsvr] reload config fail,ret = %d.", ret);
        return ret;
    }

    TCP_Svc_Handler::adjust_svc_handler_pool();

    return SOAR_RET::SOAR_RET_SUCC;
}

//���¼��������ļ�
int Zerg_Service_App::reload_daynamic_config()
{
    int ret = 0;

    //�������ÿ��Զ�̬�ظ�����
    //��IP���Ʋ��ֲ���
    ret = Zerg_IPRestrict_Mgr::instance()->get_iprestrict_conf(*config_);

    if ( SOAR_RET::SOAR_RET_SUCC != ret )
    {
        return ret;
    }

    //ͨ�Ź�������ȡ�����ļ�
    ret = zerg_comm_mgr_->get_config(*config_);

    if (SOAR_RET::SOAR_RET_SUCC != ret )
    {
        return ret;
    }

    //����̨��ʼ��
    ret = Zerg_Console_Handler::instance()->initialize(*config_);
    ZLOG_INFO("[zergsvr] Console initialize ret =%d .", ret);

    if (ret != 0 )
    {
        //����̨������������ֹ��������,
        if ( SOAR_RET::ERROR_CONSOLE_IS_CLOSED  == ret)
        {
            ZLOG_INFO("[zergsvr] Console close .ret = %u", ret);
        }
        else
        {
            ZLOG_ERROR("[zergsvr] Console initialize fail.ret = %u", ret);
        }
    }

    //��̬�޸���־�ļ���
    set_log_priority(zce_LogTrace_Basic::log_priorities((const char *)(config_->log_cfg.log_level)));

    // �������¼���
    ret = Zerg_Server_Config::instance()->init(config_);

    if (ret != SOAR_RET::SOAR_RET_SUCC)
    {
        ZLOG_ERROR("zerg reload config fail. ret=%d", ret);
        return ret;
    }

    return SOAR_RET::SOAR_RET_SUCC;
}

/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2005��11��29��
Function        : Zerg_Service_App::exit_instance
Return          : int
Parameter List  : NULL
Description     : �����˳���������
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int Zerg_Service_App::exit_instance()
{
    ZLOG_INFO("[zergsvr] exit_instance Succ.Have Fun.!!!");

    //�ͷ����еľ�̬��Դ���ر����еľ��
    TCP_Svc_Handler::uninit_all_staticdata();

    //�����������ʵ��
    Zerg_Comm_Manager::clean_instance();

    //������
    Zerg_IPRestrict_Mgr::clean_instance();
    Zerg_Console_Handler::clean_instance();
    Comm_Stat_Monitor::clean_instance();

    //�ͷ�������Դ,��ر����е�handle��,ZCE_Reactor �ᣬACE��Reactor��ʵ��
    if (ZCE_Reactor::instance())
    {
        ZCE_Reactor::instance()->close();
    }

    //
    if (ZCE_Timer_Queue::instance())
    {
        ZCE_Timer_Queue::instance()->close();
    }

    ZBuffer_Storage::instance()->uninit_buffer_list();

    return SOAR_RET::SOAR_RET_SUCC;
}

/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2005��11��29��
Function        : Zerg_Service_App::run_instance
Return          : int
Parameter List  : NULL
Description     : ���к���,�����򲻵���,�����˳�
Calls           :
Called By       :
Other           : ����Ҫ�ĺ���,����Ҳ���,
Modify Record   : Ϊ�˼ӿ췢�͵��ٶȣ��Զ����������˲�ͬ��΢����
******************************************************************************************/
int Zerg_Service_App::run_instance()
{
    size_t num_io_event = 0, num_send_frame = 0, want_send_frame = NORMAL_MAX_ONCE_SEND_FRAME;

    ZLOG_INFO("[zergsvr] Zerg_Service_App::run_instance start.");

    //microsecond
    ZCE_Reactor *preactor = ZCE_Reactor::instance();
    ZCE_Timer_Queue *p_timer_queue = ZCE_Timer_Queue::instance();

    ZCE_Time_Value run_interval(0, IDLE_REACTOR_WAIT_USEC);

    for (size_t i = 0; app_run_; ++i)
    {
        // ����Ƿ���Ҫ���¼�������
        if (app_reload_)
        {
            // ���¼�������
            reload_config();
        }

        //����Ƚ�æ��
        if ( num_send_frame >= SEND_BUSY_JUDGE_STANDARD )
        {
            //��������Ƿ��ͱȽ�æ��
            if (num_io_event == 0)
            {
                //ȡ�÷�����������
                run_interval.usec(SEND_BUSY_REACTOR_WAIT_USEC);
                want_send_frame = SENDBUSY_MAX_ONCE_SEND_FRAME;
            }
            else
            {
                run_interval.usec(BUSY_REACTOR_WAIT_USEC);
                want_send_frame = NORMAL_MAX_ONCE_SEND_FRAME;
            }
        }
        //����Ƚ�����
        else if ( num_send_frame < SEND_IDLE_JUDGE_STANDARD )
        {
            if (num_io_event == 0)
            {
                run_interval.usec(IDLE_REACTOR_WAIT_USEC);
                want_send_frame = NORMAL_MAX_ONCE_SEND_FRAME;
            }
            else
            {
                run_interval.usec(NORMAL_REACTOR_WAIT_USEC);
                want_send_frame = NORMAL_MAX_ONCE_SEND_FRAME;
            }
        }
        else
        {
            run_interval.usec(BUSY_REACTOR_WAIT_USEC);
            want_send_frame = NORMAL_MAX_ONCE_SEND_FRAME;
        }

        //
        preactor->handle_events(&run_interval, &num_io_event);

        //ÿ�ζ��������ʼ��ZCE_Time_Value����,��Ҫ����.
        zerg_comm_mgr_->popall_sendpipe_write(want_send_frame, num_send_frame);

        //������Ͷ��к�æ���ٽ���һ�η���
        if ((num_send_frame > SEND_BUSY_JUDGE_STANDARD && num_io_event == 0) || num_send_frame >= NORMAL_MAX_ONCE_SEND_FRAME )
        {
            zerg_comm_mgr_->popall_sendpipe_write(want_send_frame, num_send_frame);
        }

        //ż������һ�¶�ʱ��
        if (i % DEFAULT_IO_FIRST_RATIO == 0)
        {
            p_timer_queue->expire();
        }
    }

    ZLOG_INFO("[zergsvr] Zerg_Service_App::run_instance end.");

    return SOAR_RET::SOAR_RET_SUCC;
}

bool 
Zerg_Service_App::if_proxy()
{
    unsigned short self_svc_type = Comm_Svrd_Config::instance()->self_svr_id_.services_type_;
    if (self_svc_type == SVC_PROXY_SERVER
        || self_svc_type == SVC_PROXY_SERVER_MOBILE)
    {
        return true;
    }

    if (Comm_Svrd_Config::instance()->if_proxy_)
    {
        return true;
    }

    return false;
}

//�õ�Ψһ�ĵ���ʵ��
Zerg_Service_App *Zerg_Service_App::instance()
{
    if (instance_ == NULL)
    {
        instance_ = new Zerg_Service_App();
    }

    return instance_;
}

//�������ʵ��
void Zerg_Service_App::clean_instance()
{
    if (instance_)
    {
        delete instance_;
    }

    instance_ = NULL;
    return;
}
