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


    /*!
    * @brief      �����յ���APPFRAME,
    * @return     int
    * @param      recv_frame  �����Frame����ʹ��const��ԭ������ΪΪ�˼ӿ��ٶȣ��ܶ�ط���ֱ�ӽ�recv_frame�޸�
    * @note       on_run������Comm_SvrdApp_BUS�����Ѿ�ʵ���ˣ�
    */
    virtual int process_recv_frame(Zerg_App_Frame *recv_frame);

    ///��ʼ��,����������������
    virtual int on_start(int argc, const char *argv[]);
    ///�����˳���������
    virtual int on_exit();

    ///��������
    virtual int load_config();

    ///���¼�������
    virtual int re_read_load_cfg();



protected:

    /// �����PROXY�Ľӿ�
    Interface_WH_Proxy *interface_proxy_ = NULL;



};

#endif //WORMHOLE_APPLICATION_H_

