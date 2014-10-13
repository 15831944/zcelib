

#ifndef OGRE_SERVER_APPLICATION_H_
#define OGRE_SERVER_APPLICATION_H_

/****************************************************************************************************
class  Ogre4a_Service_App
****************************************************************************************************/
class Ogre4a_Service_App : public Comm_Svrd_Appliction
{

protected:
    //����Ҫ͵͵����
    Ogre4a_Service_App();
    ~Ogre4a_Service_App();

    //�������ʵ��
    Ogre4a_Service_App(const Ogre4a_Service_App &);
    const Ogre4a_Service_App &operator =(const Ogre4a_Service_App &);

public:
    //�����Ƿ��������
    void set_run_sign(bool brun);

    //��ʼ��
    int init_instance();
    //�˳�ʵ����
    int exit_instance();
    //����
    int run_instance();

    virtual int init()
    {
        return 0;
    }

    virtual int re_read_load_cfg()
    {
        return 0;
    }

public:
    //����ʵ����ú���
    static Ogre4a_Service_App *instance();
    //
    static void clean_instance();

public:
    //�Ƿ�����
    bool                     if_run_;

    //ZEN LIB��TimeQueue��������Ҫ�Լ��ͷ�
    ZCE_Timer_Queue          *timer_queue_;

protected:

    //����N�κ�,SLEEP��ʱ����
    static const unsigned int  IDLE_SLEEP_INTERVAL = 512;

    //���EPOLL��IO�������ȼ�����Զ�ʱ��
    static const int           EPOOL_IO_FIRST_PROCESS = 128;

    //����ʵ��ָ��
    static Ogre4a_Service_App   *instance_;

};

#endif //_OGRE_SERVER_APPLICATION_H_

