

#ifndef OGRE_APPLICATION_HANDLER_H_
#define OGRE_APPLICATION_HANDLER_H_

/****************************************************************************************************
class  Ogre_App_Timer_Handler APP�Ķ�ʱ��Handler������
****************************************************************************************************/
class  Ogre_App_Timer_Handler: public  ZCE_Timer_Handler
{

public:
    //
    Ogre_App_Timer_Handler();
protected:
    ~Ogre_App_Timer_Handler();

    //
    virtual int timer_timeout(const ZCE_Time_Value &time, const void *arg);

public:
    //���ô������ԵĶ�ʱ��
    int SetErrorRetryTimer();

public:
    //��ʱ��ID,����New����,����
    static const  int      OGRE_APP_TIME_ID[];
    //
    static const  int      STAT_TIME_INTERVAL = 30;
    //N���Ժ���һ�����·���
    static const  int      RETRY_TIME_INTERVAL = 90;


};

#endif //_OGRE_APPLICATION_HANDLER_H_

