

#include "ogre_predefine.h"
#include "ogre_app_handler.h"
#include "ogre_buf_storage.h"
#include "ogre_configure.h"
#include "ogre_comm_manager.h"
#include "ogre_tcp_ctrl_handler.h"
#include "ogre_ip_restrict.h"
#include "ogre_application.h"

/****************************************************************************************************
class  Ogre4a_Service_App
****************************************************************************************************/

Ogre4a_Service_App *Ogre4a_Service_App::instance_ = NULL;

//����Ҫ͵͵����
Ogre4a_Service_App::Ogre4a_Service_App():
    if_run_(true),
    timer_queue_(NULL)
{
}

Ogre4a_Service_App::~Ogre4a_Service_App()
{
    //�ر�timer_queue_
    if (timer_queue_)
    {
        delete timer_queue_;
        timer_queue_ = NULL;
    }

    //    ogre_acceptor_ ���Լ�ɾ��

}

/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2005��11��17��
Function        : Ogre4a_Service_App::InitInstance
Return          : int
Parameter List  :
Description     :
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int Ogre4a_Service_App::init_instance()
{
    int ret = 0;

    //
    ret = Comm_Svrd_Appliction::init_instance();

    if (ret != 0)
    {
        return ret;
    }

    //�õ��ļ����ò���
    ret = Ogre_Svr_Config::instance()->get_file_configure();

    if (ret != 0)
    {
        ZLOG_ERROR( "Get config fail��Please check config!\n");
        return ret;
    }

    size_t max_accept = 0, max_connect = 0, max_peer = 0;
    Ogre_TCP_Svc_Handler::get_maxpeer_num(max_accept, max_connect);
    ZLOG_INFO( "Ogre max accept number :%u,max connect number:%u.\n",
               max_accept, max_connect);
    max_peer = max_accept + max_connect + 16;

    //���¶�ʱ��
    size_t old_node_num;
    ZCE_Timer_Queue::instance()->extend_node(max_peer, old_node_num);

    //Reactor���޸�һ��Ҫ����ǰ��(��ȡ���ú���)�����ٳ���4�ο�
    //��Ȼ��ͬһ�����������˺ü��Ρ����µ�һ����20070929��
#ifdef ZCE_OS_WINDOWS
    //
    ZCE_Reactor::instance(new ZCE_Select_Reactor(max_peer));
    ZLOG_DEBUG("[zergsvr] ZCE_Reactor and ZCE_Select_Reactor initialized.");
#else
    ZCE_Reactor::instance(new ZCE_Epoll_Reactor(max_peer));
    ZLOG_INFO("[zergsvr] ZCE_Reactor and ZCE_Epoll_Reactor initialized.");
#endif

    //
    Ogre_App_Handler::instance();

    //�������ļ�û�ж�ȡ������ʱ��,ֻ��ʾ������Ϣ

    //ʹ����־����,�õ������ļ�,���ܵõ���־�ļ�����

    //�ȱ����ʼ��Buffer Storage,����Ϊ�����������1/5,����256��

    size_t szlist = (max_peer / 50 < 512 ) ? 512 : max_peer / 50;
    Ogre_Buffer_Storage::instance()->init_buffer_list(szlist);

    //Ogre_Comm_Manger ��ʼ��
    ret = Ogre_Comm_Manger::instance()->init_comm_manger(Ogre_Svr_Config::instance()->cfg_ogre4a_);

    if (ret != 0 )
    {
        ZLOG_ERROR( "Ogre_Comm_Manger::instance()->init_comm_manger() fail !\n");
        return SOAR_RET::ERR_OGRE_INIT_COMM_MANAGER;
    }

    ZLOG_INFO( "InitInstance success.Have fun, my brother!!!\n");
    return 0;
}

/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2005��11��29��
Function        : Ogre4a_Service_App::ExitInstance
Return          : int
Parameter List  : NULL
Description     : �����˳���������
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int Ogre4a_Service_App::exit_instance()
{
    //
    Ogre_Comm_Manger::instance()->uninit_comm_manger();
    Ogre_Comm_Manger::clean_instance();

    //�ͷ�������Դ,��ر����е�handle��?����ʲôReactor
    ZCE_Reactor::instance()->close();
    Ogre_Buffer_Storage::instance()->uninit_buffer_list();

    //
    ZLOG_INFO( "ExitInstance Succ.Have Fun.!!!\n");

    return 0;
}

/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2005��11��29��
Function        : Ogre4a_Service_App::run_instance
Return          : int
Parameter List  : NULL
Description     : ���к���,�����򲻵���,�����˳�
Calls           :
Called By       :
Other           : ����Ҫ�ĺ���,����Ҳ���
Modify Record   :
******************************************************************************************/
int Ogre4a_Service_App::run_instance()
{
    size_t numevent = 0;
    unsigned int idle = 0;
    size_t procframe = 0;
    ZLOG_INFO( "Ogre4a_Service_App::Run Start.\n");
    //microsecond
    const int INTERVAL_MACRO_SECOND = 10000;

    ZCE_Time_Value sleeptime(0, INTERVAL_MACRO_SECOND);
    ZCE_Time_Value interval(0, INTERVAL_MACRO_SECOND);

    for (; if_run_;)
    {
        //ÿ�ζ��������ʼ��ZCE_Time_Value����,��Ҫ����.
        interval.usec(INTERVAL_MACRO_SECOND);
        ZCE_Reactor::instance()->handle_events(&interval, &numevent);

        //ȡ�÷�����������
        Ogre_Comm_Manger::instance()->get_all_senddata_to_write(procframe);
        //û�д����κ��¼�

        //������¼������˾ͼ���
        if (numevent > 0 || procframe > 0)
        {
            idle = 0;
            continue;
        }
        else
        {
            ++idle;
        }

        //����Ը��ݿ���״̬����һЩ�Լ�������,
        if (idle > IDLE_SLEEP_INTERVAL )
        {
            ZCE_OS::sleep(sleeptime);
        }
    }

    return 0;
}

//�����Ƿ��������
void Ogre4a_Service_App::set_run_sign(bool brun)
{
    if_run_ = brun;
}

//����ʵ����ú���
Ogre4a_Service_App *Ogre4a_Service_App::instance()
{
    if (instance_ == NULL)
    {
        instance_ = new Ogre4a_Service_App();
    }

    return instance_;
}

//����ʵ��������
void Ogre4a_Service_App::clean_instance()
{
    if (instance_ != NULL)
    {
        delete instance_;
        instance_ = NULL;
    }
}

