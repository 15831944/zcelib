
#include "soar_predefine.h"
#include "soar_svrd_timer_base.h"
#include "soar_stat_define.h"
#include "soar_stat_monitor.h"
#include "soar_svrd_application.h"


ZCE_Time_Value Comm_Svrd_Timer_Base::cur_time_ = ZCE_OS::gettimeofday();

//��ʱ��ID,����New����,����
const int Comm_Svrd_Timer_Base::SERVER_TIMER_ID[] =
{
    0x1,                    //����ID
    0x2,
};


Comm_Svrd_Timer_Base::Comm_Svrd_Timer_Base(ZCE_Timer_Queue *queue) : 
    ZCE_Timer_Handler(queue)
{
    stat_monitor_ = NULL;
}

Comm_Svrd_Timer_Base::~Comm_Svrd_Timer_Base()
{
    stat_monitor_ = NULL;
}


int Comm_Svrd_Timer_Base::initialize()
{
    stat_monitor_ = Comm_Stat_Monitor::instance();
    ZCE_ASSERT(stat_monitor_ != NULL);

    //��ʱ����,ÿ100msec����һ�£��õ���ǰ��ʱ��
    ZCE_Time_Value interval = ZCE_OS::make_timeval(0, HEARTBEAT_INTERVAL_MSEC);
    timer_queue()->schedule_timer(this,
        &(SERVER_TIMER_ID[0]),
        ZCE_Time_Value::ZERO_TIME_VALUE,
        interval);
    cur_time_ = ZCE_OS::gettimeofday();

    last_check_ = time(NULL);

    return SOAR_RET::SOAR_RET_SUCC;
}


int Comm_Svrd_Timer_Base::handle_timeout(const ZCE_Time_Value &now_time,
                                       const void *act /*= 0*/)
{
    ZCE_UNUSED_ARG(act);

    //
    cur_time_ = ZCE_OS::gettimeofday();


    // ������
    check_monitor(now_time);

    return SOAR_RET::SOAR_RET_SUCC;
}



// ������Ƿ�ʱ
void Comm_Svrd_Timer_Base::check_monitor(const ZCE_Time_Value &now_time)
{
    time_t now_sec = now_time.sec();

    // ����Ƿ���ͬһ��������
    if (now_sec / FIVE_MINUTE_SECONDS != last_check_ / FIVE_MINUTE_SECONDS)
    {
        // ��ӽ��̴����
        report_status();
        stat_monitor_->check_overtime(now_sec);

        if (now_sec - last_check_ > FIVE_MINUTE_SECONDS)
        {
            ZLOG_ERROR("check monitor more than five minutes:real_second=%d %d",
                       now_sec - last_check_, now_sec % FIVE_MINUTE_SECONDS);
        }

        // ������Ϊ�˱�֤ÿ�μ����5������
        last_check_ = now_sec - (now_sec % FIVE_MINUTE_SECONDS);
    }
}

// ϵͳ������״̬����
//void Comm_Timer_Handler::report_status()
//{
//    // �ϱ����̴��״̬
//    stat_monitor_->increase_once(COMM_STAT_APP_ALIVE, game_id_);
//
//    if (svrd_app_ != NULL)
//    {
//        // ��ȡ������״̬��Ϣ
//        int ret = svrd_app_->watch_dog_status(false);
//
//        if (ret != 0)
//        {
//            ZLOG_ERROR("watch dog get sys/app status failed, ret = %d", ret);
//        }
//        else
//        {
//            // �ϱ��������
//            stat_monitor_->set_by_statid(COMM_STAT_SYS_CPU_RATIO,
//                game_id_, 
//                classify_id_,
//                svrd_app_->system_cpu_ratio_);
//
//            stat_monitor_->set_by_statid(COMM_STAT_PROCESS_CPU_RATIO,
//                game_id_, 
//                classify_id_, 
//                svrd_app_->process_cpu_ratio_);
//
//            stat_monitor_->set_by_statid(COMM_STAT_SYS_MEM_USE_RATIO,
//                game_id_, 
//                classify_id_,
//                svrd_app_->mem_use_ratio_);
//
//            stat_monitor_->set_by_statid(COMM_STAT_AVAILABLE_MEM_SIZE,
//                game_id_, 
//                classify_id_, 
//                svrd_app_->can_use_size_);
//
//        }
//    }
//}







