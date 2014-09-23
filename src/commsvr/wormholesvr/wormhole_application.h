#ifndef WORMHOLE_APPLICATION_H_
#define WORMHOLE_APPLICATION_H_

#include "wormhole_proxyprocess.h"



/*!
* @brief      
*             
* @note       
*/
class Wormhole_Proxy_App : public Comm_SvrdApp_BUS
{

public:
    // �Լ��ĵ���͵͵���ţ�
    Wormhole_Proxy_App();
    ~Wormhole_Proxy_App();

public:
    // �����յ���APPFRAME,��ʹ��const��ԭ������ΪΪ�˼ӿ��ٶȣ��ܶ�ط���ֱ�ӽ�recv_frame�޸�
    virtual int process_recv_frame(Zerg_App_Frame *recv_frame);

    ///��ʼ��,����������������
    virtual int on_start(int argc, const char *argv[]);
    ///�����˳���������
    virtual int on_exit();

    ///����,���к���,�����򲻵���,�����˳�,Ϊ�˼ӿ췢�͵��ٶȣ��Զ����������˲�ͬ��΢����
    virtual int on_run();

    ///��������
    virtual int load_config();

    ///���¼�������
    virtual int reload();



protected:
    // �����PROXY�Ľӿ�
    Interface_WH_Proxy     *interface_proxy_;

    // �����FRAME������
    unsigned int                 process_frame_count_;

};

#endif //WORMHOLE_APPLICATION_H_

