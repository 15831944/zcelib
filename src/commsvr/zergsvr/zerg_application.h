
#ifndef ZERG_SERVER_APPLICATION_H_
#define ZERG_SERVER_APPLICATION_H_

#include "zerg_tcp_ctrl_handler.h"


class Zerg_Comm_Manager;



/*!
* @brief
*
* @note
*/
class Zerg_Service_App : public Comm_Svrd_Appliction
{


public:
    //����Ҫ͵͵����
    Zerg_Service_App();
    ~Zerg_Service_App();

public:

    ///��ʼ��,����������������
    virtual int on_start(int argc, const char *argv[]);
    ///�����˳���������
    virtual int on_exit();

    ///����,���к���,�����򲻵���,�����˳�,Ϊ�˼ӿ췢�͵��ٶȣ��Զ����������˲�ͬ��΢����
    virtual int on_run();

    ///��������
    virtual int load_config();


    bool if_proxy();

private:


    

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

