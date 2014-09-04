#include "zerg_predefine.h"
#include "zerg_app_handler.h"
#include "zerg_tcp_ctrl_handler.h"
#include "zerg_comm_manager.h"
#include "zerg_application.h"

/****************************************************************************************************
class  Zerg_App_Timer_Handler
****************************************************************************************************/
ZCE_Time_Value Zerg_App_Timer_Handler::now_time_ = ZCE_LIB::gettimeofday();

//��ʱ��ID,����New����,����
const int Zerg_App_Timer_Handler::ZERGAPP_TIME_ID[] =
{
    0x1,                    //����ID
    0x2,
};

//
//heartbeat_counter_����0��ʼ�����Ǳ����һ��ģ����ʱ��ͷ�������
Zerg_App_Timer_Handler::Zerg_App_Timer_Handler(ZCE_Timer_Queue *time_queue):
    Server_Timer_Base(time_queue),
    heartbeat_counter_(1)
{
    srand(static_cast<unsigned int>(ZCE_LIB::pthread_self()));
    random_reload_point_ = rand() % AUTOCONNECT_RELOAD_INTERVAL;

    last_trigger_reload_time_ = heartbeat_counter_;

    //��ʱ����,ÿ������һ�£��õ���ǰ��ʱ��
    ZCE_Time_Value interval = ZCE_LIB::make_timeval(HEARTBEAT_TIME_INTERVAL, 0);
    timer_queue()->schedule_timer (this, &(ZERGAPP_TIME_ID[0]),
                                   ZCE_Time_Value::ZERO_TIME_VALUE,
                                   interval);
}

Zerg_App_Timer_Handler::~Zerg_App_Timer_Handler()
{
}

//
int Zerg_App_Timer_Handler::handle_timeout(const ZCE_Time_Value &time_now, const void *arg)
{
    //�ȵ���ǰ��ʱ��
    now_time_ = time_now;

    const int timeid = *(static_cast<const int *>(arg));

    //��������
    if (ZERGAPP_TIME_ID[0] == timeid)
    {
        ++heartbeat_counter_;

        if (0 == heartbeat_counter_ % AUTOCONNECT_RETRY_INTERVAL )
        {
            size_t valid = 0;
            size_t succ = 0;
            size_t fail = 0;
            TCP_Svc_Handler::reconnect_allserver(valid, succ, fail);
            if (succ || fail)
            {
                // succ��fail��ζ�ŷ�������������
                if (last_trigger_reload_time_ + RETRY_TRIGGER_RELOAD_INTERVAL <= heartbeat_counter_)
                {
                    last_trigger_reload_time_ = heartbeat_counter_;

                    //���������ϵ����ʱ��ȡip��Ϣ������
                    //TCP_Svc_Handler::reload_config(&Server_Config_FSM::instance()->zerg_config_);
                }
            }
        }

        if (0 == heartbeat_counter_ % SEND_FRAME_CHECK_INTERVAL)
        {
            Zerg_Comm_Manager::instance()->check_freamcount(static_cast<unsigned int>(now_time_.sec()));
        }

        if (random_reload_point_ == heartbeat_counter_ % AUTOCONNECT_RELOAD_INTERVAL)
        {
            //TCP_Svc_Handler::reload_config();
        }

    }

    return 0;
}

//
int Zerg_App_Timer_Handler::timer_close()
{
    //ȡ����Event Handler��صĶ�ʱ��
    timer_queue()->cancel_timer(this);

    delete this;

    return 0;
}

