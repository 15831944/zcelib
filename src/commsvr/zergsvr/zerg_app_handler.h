
#ifndef ZERG_APPLICATION_HANDLER_H_
#define ZERG_APPLICATION_HANDLER_H_

/****************************************************************************************************
class  Zerg_App_Timer_Handler
****************************************************************************************************/
class  Zerg_App_Timer_Handler : public  Server_Timer_Base
{

public:
    //��ʱ��ID,����New����,����
    static const  int      ZERGAPP_TIME_ID[];

    //������ʱ����
    static const  int      HEARTBEAT_TIME_INTERVAL = 1;
    //�����������ӵļ��ʱ�䣬5s,
    static const  int      AUTOCONNECT_RETRY_INTERVAL = 5;
    //reload�������ӵļ��ʱ��, 300s
    static const  int      AUTOCONNECT_RELOAD_INTERVAL = 300;
    //�Զ������������쳣ʱ�ᴥ��һ��reload����������ȡip, һ��ʱ����ֻ����һ��, 120s
    static const unsigned int  RETRY_TRIGGER_RELOAD_INTERVAL = 120;

    //����Ƶ�ʼ��
    static const unsigned int SEND_FRAME_CHECK_INTERVAL = 10;

    //��ǰ��ʱ��
    static ZCE_Time_Value  now_time_;

protected:
    //����������
    unsigned int           heartbeat_counter_;

    //��ʱ�����������ƫ��, �������з�����ͬʱ����
    unsigned int           random_reload_point_;

    //�Զ������������쳣ʱ�ᴥ��һ��reload����������ȡip, һ��ʱ����ֻ����һ�� 
    unsigned int           last_trigger_reload_time_;

public:
    //
    Zerg_App_Timer_Handler(ZCE_Timer_Queue *time_queue);
protected:
    //�Լ������Լ�������
    ~Zerg_App_Timer_Handler();

    //
    virtual int handle_timeout(const ZCE_Time_Value &time, const void *arg);
    //
    virtual int timer_close();

};

#endif //_ZERG_APPLICATION_HANDLER_H_

