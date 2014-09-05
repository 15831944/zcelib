
#ifndef ZERG_SERVER_APPLICATION_H_
#define ZERG_SERVER_APPLICATION_H_

#include "zerg_tcp_ctrl_handler.h"
#include "zerg_app_handler.h"

class Zerg_Comm_Manager;
/****************************************************************************************************
class  Zerg_Service_App
****************************************************************************************************/
class Zerg_Service_App : public Comm_Svrd_Appliction
{


protected:
    //����Ҫ͵͵����
    Zerg_Service_App();
    ~Zerg_Service_App();

    //�������ʵ��
    Zerg_Service_App(const Zerg_Service_App *);
    const Zerg_Service_App &operator =(const Zerg_Service_App &);


public:

    //��ʼ��
    virtual int on_start(int argc,const char *argv[]);
    //�˳�ʵ����
    virtual int on_exit();
    //����
    virtual int run();

    bool if_proxy();

private:
   

    int reload_daynamic_config();

public:

    //����ʵ������
    static Zerg_Service_App *instance();
    //������ʵ��
    static void clean_instance();

public:


    Zerg_Comm_Manager       *zerg_comm_mgr_;

    //OP KEY�ļ���ʱ���
    time_t                   conf_timestamp_;

protected:
    //���ӵ�staticʵ��
    static Zerg_Service_App *instance_;


};

//

#endif //_ZERG_SERVER_APPLICATION_H_

