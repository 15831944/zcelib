

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




};

#endif //OGRE_SERVER_APPLICATION_H_

