#include "soar_predefine.h"
#include "soar_frame_zerg.h"
#include "soar_svrd_app_non_ctrl.h"
#include "soar_zerg_mmappipe.h"
#include "soar_svrd_config.h"
#include "soar_error_code.h"

Comm_SvrdApp_NonCtrl::Comm_SvrdApp_NonCtrl():
    nonctrl_recv_buffer_(NULL)
{
    nonctrl_recv_buffer_ = new (Zerg_App_Frame::MAX_LEN_OF_APPFRAME) Zerg_App_Frame();
}

Comm_SvrdApp_NonCtrl::~Comm_SvrdApp_NonCtrl()
{
    //�ͷ���Դ
    if (nonctrl_recv_buffer_)
    {
        delete nonctrl_recv_buffer_;
        nonctrl_recv_buffer_ = NULL;
    }
}

//���к���
int Comm_SvrdApp_NonCtrl::run_instance()
{
    ZLOG_INFO("[framework] app %s class [%s] run_instance start.",
              get_app_basename(),
              typeid(*this).name());
    //
    size_t size_io_event = 0 , size_timer_expire  = 0;

    size_t prc_frame = 0, idle = 0, proc_data_num = 0;

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
        popfront_recvpipe(prc_frame);

        size_timer_expire = time_queue->expire();

        // ����������ʽͨ������
        proc_data_num = 0;
        proc(proc_data_num);

        //���û�д����κ�֡
        // ����ܵ��İ�����Ҫ������������˵���ܵ��Ѿ�����
        if (prc_frame < MAX_ONCE_PROCESS_FRAME && size_timer_expire <= 0 && proc_data_num <= 0)
        {
            ++idle;
        }
        else
        {
            idle = 0;
        }

        // �����ѭ��̫�����װ׺ĵ粻��̼
        // ����������˯�����ʱ����100����
        // ���ֻҪ�ڴ�ܵ���С>=100�������ϵͳ�������*ÿ�����ֽ���
        // ����˯���Ժ��ܹ������������ݶ������ڹܵ�����������
        // ����ϵͳ������Դ���ã���ô������������������������
        // ������1Gbit���㣬��ܵ��ٽ��СΪ1Gbit-per(S)/8/10��12MByte���ټ��Ϲܵ�������ڴ�ṹռ��
        // ����Ϊ16MByteҲ�����ԣ�����ֻҪ�ܵ���С����16MByteӦ�þͶ���ס��
        // ��������Ͳ��ÿ�����ô����ˡ�
        if (idle < 1)
        {
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

        //
        reactor->handle_events(&select_interval, &size_io_event);
    }

    ZLOG_INFO("[framework] app %s class [%s] run_instance end.",
              get_app_basename(),
              typeid(*this).name());
    return SOAR_RET::SOAR_RET_SUCC;
}

//�ӹܵ�����ȡһ�����ݽ��д���
int Comm_SvrdApp_NonCtrl::popfront_recvpipe(size_t &proc_frame)
{
    int ret = 0;

    //һ�δ������������
    for (proc_frame = 0;
         zerg_mmap_pipe_->is_empty_bus(Zerg_MMAP_BusPipe::RECV_PIPE_ID) == false
         && proc_frame < MAX_ONCE_PROCESS_FRAME ;
         ++proc_frame)
    {
        //
        ret = zerg_mmap_pipe_->pop_front_recvpipe(nonctrl_recv_buffer_);

        if (ret !=  SOAR_RET::SOAR_RET_SUCC)
        {
            return SOAR_RET::SOAR_RET_SUCC;
        }

        DEBUGDUMP_FRAME_HEAD(nonctrl_recv_buffer_, "FROM RECV PIPE FRAME", RS_DEBUG);

        //����һ���յ�������
        ret = process_recv_appframe(nonctrl_recv_buffer_);

        //
        if (ret !=  SOAR_RET::SOAR_RET_SUCC)
        {
            continue;
        }
    }

    //
    return SOAR_RET::SOAR_RET_SUCC;
}
