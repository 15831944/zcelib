/// @file       soar_timer_handler.cpp
/// @date       2012/08/13 10:24
///
/// @author     yunfeiyang
///
/// @brief      ��ܶ�ʱ��������
///

#include "soar_predefine.h"
#include "soar_timer_handler.h"
#include "soar_stat_define.h"
#include "soar_stat_monitor.h"
#include "soar_time_provider.h"
#include "soar_svrd_application.h"


int Comm_Timer_Handler::handle_timeout(const ZCE_Time_Value &now_time,
                                       const void *act /*= 0*/)
{
    ZCE_UNUSED_ARG(act);

    // ��ʱ����ʱ��
    update_time(now_time);

    // reload����
    notify_reload();

    // ������
    check_monitor(now_time);

    return SOAR_RET::SOAR_RET_SUCC;
}

void Comm_Timer_Handler::notify_reload()
{

}

// ����ʱ���ṩ�ߵ�ʱ��
void Comm_Timer_Handler::update_time(const ZCE_Time_Value &now_time)
{
    time_provider_->update(now_time);
}

// ������Ƿ�ʱ
void Comm_Timer_Handler::check_monitor(const ZCE_Time_Value &now_time)
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
void
Comm_Timer_Handler::report_status()
{
    // �ϱ����̴��״̬
    stat_monitor_->increase_once(COMM_STAT_APP_ALIVE, game_id_);

    if (svrd_app_ != NULL)
    {
        // ��ȡ������״̬��Ϣ
        int ret = svrd_app_->watch_dog_status(false);

        if (ret != 0)
        {
            ZLOG_ERROR("watch dog get sys/app status failed, ret = %d", ret);
        }
        else
        {
            // �ϱ��������
            stat_monitor_->set_by_statid(COMM_STAT_SYS_CPU_RATIO,
                game_id_, classify_id_, svrd_app_->get_sys_cpu_ratio());

            stat_monitor_->set_by_statid(COMM_STAT_PROCESS_CPU_RATIO,
                game_id_, classify_id_, svrd_app_->get_app_cpu_ratio());

            stat_monitor_->set_by_statid(COMM_STAT_SYS_MEM_USE_RATIO,
                game_id_, classify_id_, svrd_app_->get_sys_mem_ratio());

            stat_monitor_->set_by_statid(COMM_STAT_AVAILABLE_MEM_SIZE,
                game_id_, classify_id_, svrd_app_->get_can_use_mem_size());

        }
    }
}

Comm_Timer_Handler::Comm_Timer_Handler(ZCE_Timer_Queue *queue) : ZCE_Timer_Handler(queue)
{
    time_provider_ = NULL;
    stat_monitor_ = NULL;
    svrd_app_ = NULL;
}

Comm_Timer_Handler::~Comm_Timer_Handler()
{
    time_provider_ = NULL;
    stat_monitor_ = NULL;
    svrd_app_ = NULL;
}

int
Comm_Timer_Handler::init(unsigned int game_id, bool is_app, ZCE_Server_Base *toolkit)
{
    time_provider_ = Comm_Time_Provider::instance();
    ZCE_ASSERT(time_provider_ != NULL);

    stat_monitor_ = Comm_Stat_Monitor::instance();
    ZCE_ASSERT(stat_monitor_ != NULL);

    svrd_app_ = toolkit;

    last_check_ = time(NULL);

    // Timer handle initʱҪ��֤cfgsdk�Ѿ��õ�gameid��
    game_id_ = game_id;

    // ��ȡ��������classify_id
    classify_id_ = (is_app ? COMM_STAT_APP_CID : COMM_STAT_ZERG_CID);

    return SOAR_RET::SOAR_RET_SUCC;
}



