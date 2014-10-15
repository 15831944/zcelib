#include "ogre_predefine.h"
#include "ogre_app_timer.h"
#include "ogre_tcp_ctrl_handler.h"
#include "ogre_application.h"

/****************************************************************************************************
class  Ogre_App_Timer_Handler
****************************************************************************************************/
//��ʱ��ID,����New����,����
const int Ogre_App_Timer_Handler::OGRE_APP_TIME_ID[] =
{
    0x1,                    //������ID
    0x2                     //����ID
};


//
Ogre_App_Timer_Handler::Ogre_App_Timer_Handler()
{
}
Ogre_App_Timer_Handler::~Ogre_App_Timer_Handler()
{
}

//
int Ogre_App_Timer_Handler::timer_timeout(const ZCE_Time_Value &time_now, const void *arg)
{
    //�ȵ���ǰ��ʱ��
    Server_Timer_Base::timer_timeout(time_now, arg);

    const int timeid = *(static_cast<const int *>(arg));

    //����һ������������
    if (OGRE_APP_TIME_ID[0] == timeid)
    {
        //Ogre_TCP_Svc_Handler::GetErrorPipeDataToWrite();
    }
    else
    {
    }

    return 0;
}

//���ô������ԵĶ�ʱ��
int Ogre_App_Timer_Handler::SetErrorRetryTimer()
{
    ZCE_Time_Value delay(RETRY_TIME_INTERVAL, 0);
    timer_queue()->schedule_timer (this,
                                   &(OGRE_APP_TIME_ID[0]),
                                   delay);

    return 0;
}

