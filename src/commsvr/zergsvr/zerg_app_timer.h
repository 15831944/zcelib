
#ifndef ZERG_APPLICATION_TIMER_H_
#define ZERG_APPLICATION_TIMER_H_



/*!
* @brief      ZERG����Ķ�ʱ������ʱ�����ڸ��ִ���
*             
* @note       
*/
class  Zerg_App_Timer_Handler : public  Server_Timer_Base
{
    
public:
    //
    Zerg_App_Timer_Handler();
protected:
    //�Լ������Լ�������
    ~Zerg_App_Timer_Handler();

    ///��ʼ����
    int initialize(ZCE_Timer_Queue *queue);

    ///��ʱ������
    virtual int timer_timeout(const ZCE_Time_Value &time, const void *arg);

    

public:

    //�����������ӵļ��ʱ�䣬��λ����
    static const uint32_t AUTOCONNECT_RETRY_MESC = 8000;

    //����Ƶ�ʼ��ļ��ʱ�䣬��λ����
    static const uint32_t SEND_FRAME_CHECK_MSEC = 10000;

    //reload�������ӵļ��ʱ��, 300s
    static const  int      AUTOCONNECT_RELOAD_INTERVAL = 300000;
    //�Զ������������쳣ʱ�ᴥ��һ��reload����������ȡip, һ��ʱ����ֻ����һ��, 120s
    static const unsigned int  RETRY_TRIGGER_RELOAD_INTERVAL = 120000;

protected:
    
    ///����Ƶ�ʼ���������������������������ȼ���
    uint64_t connect_retry_heart_;

    ///����Ƶ�ʼ��������������
    uint64_t send_check_heart_;


    //��ʱ�����������ƫ��, �������з�����ͬʱ����
    unsigned int random_reload_point_;


};

#endif //ZERG_APPLICATION_TIMER_H_

