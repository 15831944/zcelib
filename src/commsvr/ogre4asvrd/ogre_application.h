

#ifndef OGRE_SERVER_APPLICATION_H_
#define OGRE_SERVER_APPLICATION_H_

/****************************************************************************************************
class  Ogre_Service_App
****************************************************************************************************/
class Ogre_Service_App : public Soar_Svrd_Appliction
{

public:

    //����Ҫ͵͵����
    Ogre_Service_App();
    virtual ~Ogre_Service_App();

public:

    ///��ʼ��,����������������
    virtual int on_start(int argc, const char *argv[]);

    ///�����˳���������
    virtual int on_exit();

    ///����,���к���,�����򲻵���,�����˳�,Ϊ�˼ӿ췢�͵��ٶȣ��Զ����������˲�ͬ��΢����
    virtual int on_run();

protected:

    //����N�κ�,SLEEP��ʱ����
    static const unsigned int  IDLE_SLEEP_INTERVAL = 512;

    //���EPOLL��IO�������ȼ�����Զ�ʱ��
    static const int           EPOOL_IO_FIRST_PROCESS = 128;


};

#endif //_OGRE_SERVER_APPLICATION_H_

