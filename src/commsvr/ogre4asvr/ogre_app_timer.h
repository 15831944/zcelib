

#ifndef OGRE_APPLICATION_HANDLER_H_
#define OGRE_APPLICATION_HANDLER_H_

/****************************************************************************************************
class  Ogre_App_Handler APP�Ķ�ʱ��Handler������
****************************************************************************************************/
class  Ogre_App_Handler: public  ZCE_Timer_Handler
{

public:
    //
    Ogre_App_Handler();
protected:
    ~Ogre_App_Handler();

    //
    virtual int handle_timeout(const ZCE_Time_Value &time, const void *arg);

public:
    //���ô������ԵĶ�ʱ��
    int SetErrorRetryTimer();

public:
    //
    static Ogre_App_Handler *instance();
    //
    static void clean_instance();

public:
    //��ʱ��ID,����New����,����
    static const  int      OGRE_APP_TIME_ID[];
    //
    static const  int      STAT_TIME_INTERVAL = 30;
    //N���Ժ���һ�����·���
    static const  int      RETRY_TIME_INTERVAL = 90;

protected:

    //����ʵ��ָ��
    static Ogre_App_Handler   *instance_;

};

#endif //_OGRE_APPLICATION_HANDLER_H_

