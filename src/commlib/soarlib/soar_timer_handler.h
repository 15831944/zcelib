/// @file       soar_timer_handler.h
/// @date       2012/08/13 10:21
///
/// @author     yunfeiyang
///
/// @brief      ��ܶ�ʱ��������
///
#ifndef SOARING_LIB_TIMER_HANDLER_H_
#define SOARING_LIB_TIMER_HANDLER_H_

class Comm_Time_Provider;
class Comm_Stat_Monitor;
class Comm_Svrd_Appliction;
class Comm_Timer_Handler : public ZCE_Timer_Handler
{
public:
    Comm_Timer_Handler(ZCE_Timer_Queue *queue);
    ~Comm_Timer_Handler();

protected:
    // ��ʱ����������
    virtual int handle_timeout(const ZCE_Time_Value &now_time,
                               const void *act = 0);

public:
    // �����������¼����¼�
    void notify_reload();

    // updateʱ�������ʱ��
    void update_time(const ZCE_Time_Value &now_time);

    // ���������
    void check_monitor(const ZCE_Time_Value &now_time);

    // ϵͳ������״̬����
    void report_status();

    
    ///
    /// @fn     init
    /// @brief  ��ʼ��
    ///
    /// @param  game_id ������̰���Ϸ����������Ϸid, ������0
    /// @param  is_app ��app���̻���zerg����
    /// @param  toolkit server���ʵ��
    ///
    /// @return int
    ///
    int init(unsigned int game_id, bool is_app, ZCE_Server_Base *toolkit);

private:
    // ʱ���ṩʵ��
    Comm_Time_Provider *time_provider_;
    // ���
    Comm_Stat_Monitor *stat_monitor_;
    // App����
    ZCE_Server_Base *svrd_app_;

    time_t last_check_;

    // ��ϷID
    unsigned int game_id_;

    // ������classify_id
    uint32_t classify_id_;

};

#endif // SOARING_LIB_TIMER_HANDLER_H_
