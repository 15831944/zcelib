
#ifndef SOARING_LIB_TIMER_HANDLER_H_
#define SOARING_LIB_TIMER_HANDLER_H_


class Comm_Stat_Monitor;
class Server_Config_Base;


/*!
* @brief      ��������ܵĶ�ʱ��������
*             ���Դ���õ���ǰ��ʱ��
* @note       
*/
class Server_Timer_Base : public ZCE_Timer_Handler
{
    
    friend class Comm_Svrd_Appliction;
public:

    ///���캯��,��Ϊ��ܵ���ƹ����ʱ�򲻳�ʼ��timer queue��
    Server_Timer_Base();
    ///��������
    ~Server_Timer_Base();

protected:
    
    // ���������
    void check_monitor(const ZCE_Time_Value &now_time);

    // ϵͳ������״̬����
    void report_status();

    ///ȡ�õ�ǰ��ʱ�䣬���ڶ�ʱ�侫��Ҫ�󲻸ߵĳ���
    ZCE_Time_Value gettimeofday();


protected:

    ///��ʼ��
    virtual int initialize(ZCE_Timer_Queue *queue,
        Server_Config_Base *config_base);


    /// ��ʱ����������
    virtual int timer_timeout(const ZCE_Time_Value &now_time,
        const void *act = 0);

    ///��ʱ���ر�
    virtual int timer_close();

protected:
       
    ///��������ʱ��ID
    static const  int  SERVER_TIMER_ID[];

    ///������ÿ100 mesc ����һ��
    static const time_t HEARTBEAT_INTERVAL_MSEC = 100;

public:
    ///��ǰʱ��
    static ZCE_Time_Value now_time_;

protected:

    ///
    ZCE_Time_Value heart_precision_;

    ///����������
    uint64_t  heartbeat_counter_;

    ///�ӿ�ʼ���������ڵĺ������������ֵ��ͨ��heartbeat_counter_�;��ȵõ��ģ�
    ///����׼ȷ�����ڸ��ֳ��Լ���
    uint64_t  heart_total_mesc_;

    // ���
    Comm_Stat_Monitor *stat_monitor_;

    time_t last_check_;

    

};

#endif // SOARING_LIB_TIMER_HANDLER_H_
