
#ifndef SOARING_LIB_TIMER_HANDLER_H_
#define SOARING_LIB_TIMER_HANDLER_H_


class Comm_Stat_Monitor;
class Comm_Svrd_Appliction;


/*!
* @brief      ��������ܵĶ�ʱ��������
*             ���Դ���õ���ǰ��ʱ��
* @note       
*/
class Server_Timer_Base : public ZCE_Timer_Handler
{
    

public:
    //���캯��
    Server_Timer_Base(ZCE_Timer_Queue *queue);
    ~Server_Timer_Base();



protected:

    ///��ʼ��
    int initialize();


    // ���������
    void check_monitor(const ZCE_Time_Value &now_time);

    // ϵͳ������״̬����
    void report_status();

    ///ȡ�õ�ǰ��ʱ�䣬���ڶ�ʱ�侫��Ҫ�󲻸ߵĳ���
    ZCE_Time_Value gettimeofday();


protected:

    // ��ʱ����������
    virtual int handle_timeout(const ZCE_Time_Value &now_time,
        const void *act = 0);


protected:
       
    ///��������ʱ��ID
    static const  int      SERVER_TIMER_ID[];

    ///������ÿ100 mesc ����һ��
    static const time_t HEARTBEAT_INTERVAL_MSEC = 100;

    ///��ǰʱ��
    static ZCE_Time_Value cur_time_;

protected:

    // ���
    Comm_Stat_Monitor *stat_monitor_;

    time_t last_check_;

    

};

#endif // SOARING_LIB_TIMER_HANDLER_H_
