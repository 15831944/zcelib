#include "soar_predefine.h"
#include "soar_fsm_trans_mgr.h"
#include "soar_svrd_app_fsm.h"
#include "soar_zerg_mmappipe.h"
#include "soar_svrd_cfg_fsm.h"


//
Comm_SvrdApp_Transaction::Comm_SvrdApp_Transaction()
{
}

//
Comm_SvrdApp_Transaction::~Comm_SvrdApp_Transaction()
{
}

//���ӵ���register_func_cmd
int
Comm_SvrdApp_Transaction::init_instance()
{
    int ret = 0;
    ret = Comm_Svrd_Appliction::init_instance();

    if (SOAR_RET::SOAR_RET_SUCC != ret)
    {
        return ret;
    }

    Server_Config_FSM *svd_config = Server_Config_FSM::instance();

    //����������ĳ�ʼ��, �Զ�����ʹ��notify
    Transaction_Manager *p_trans_mgr_ = new Transaction_Manager();
    p_trans_mgr_->initialize(svd_config->framework_config_.trans_info_.trans_cmd_num_,
                             svd_config->framework_config_.trans_info_.trans_num_,
                             self_services_id_,
                             ZCE_Timer_Queue::instance(),
                             Zerg_MMAP_BusPipe::instance());
    Transaction_Manager::instance(p_trans_mgr_);

    ret = register_trans_cmd();

    if (ret != SOAR_RET::SOAR_RET_SUCC)
    {
        ZLOG_ERROR("[framework] register trans cmd fail. ret=%d", ret);
        return ret;
    }

    return SOAR_RET::SOAR_RET_SUCC;
}

//���д���,
int Comm_SvrdApp_Transaction::run_instance()
{

    ZLOG_INFO("[framework] app %s class [%s] run_instance start.",
              get_app_basename(),
              typeid(*this).name());

    Transaction_Manager *trans_mgr = Transaction_Manager::instance();
    //Zerg_MMAP_BusPipe *zerg_mmap_pipe = Zerg_MMAP_BusPipe::instance();
    //
    size_t size_io_event = 0 , num_timer_expire  = 0;

    size_t proc_frame = 0, gen_trans = 0, proc_data_num = 0;
    unsigned int idle = 0;

    ZCE_Time_Value select_interval(0, 0);

    ZCE_Timer_Queue *time_queue = ZCE_Timer_Queue::instance();
    ZCE_Reactor *reactor = ZCE_Reactor::instance();

    for (; app_run_;)
    {
        // ����Ƿ���Ҫ���¼�������
        if (app_reload_)
        {
            // ���¼�������
            reload_config();
        }

        //�����յ�������
        trans_mgr->process_pipe_frame(proc_frame, gen_trans);

        //��ʱ
        num_timer_expire = time_queue->expire();

        // app���������
        proc_data_num = 0;
        proc(proc_data_num);

        // IO�¼�
        size_io_event = 0;
        reactor->handle_events(&select_interval, &size_io_event);

        //���û�д����κ�֡
        if ((proc_frame + num_timer_expire + proc_data_num + size_io_event) <= 0)
        {
            ++idle;
        }
        else
        {
            idle = 0;
        }

        //���æ�������ɻ�
        if (idle < LIGHT_IDLE_SELECT_INTERVAL)
        {
            select_interval.usec(0);
            continue;
        }
        //������кܶ�,��Ϣһ��,�����ȽϿ��У������SELECT�൱��Sleep��
        else if (idle >= HEAVY_IDLE_SLEEP_INTERVAL)
        {
            select_interval.usec(HEAVY_IDLE_INTERVAL_MICROSECOND);
        }
        //else �൱�� else if (idle >= LIGHT_IDLE_SELECT_INTERVAL)
        else
        {
            select_interval.usec(LIGHT_IDLE_INTERVAL_MICROSECOND);
        }
    }

    ZLOG_INFO("[framework] app %s class [%s] run_instance end.",
              get_app_basename(),
              typeid(*this).name());
    return SOAR_RET::SOAR_RET_SUCC;
}

//�˳�����
int Comm_SvrdApp_Transaction::exit_instance()
{
    int ret = 0;
    Transaction_Manager::clean_instance();

    ret = Comm_Svrd_Appliction::exit_instance();

    if ( SOAR_RET::SOAR_RET_SUCC != ret )
    {
        return ret;
    }

    return SOAR_RET::SOAR_RET_SUCC;
}

