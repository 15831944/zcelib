#ifndef ARBITER_APPLICATION_H_
#define ARBITER_APPLICATION_H_

#include "arbiter_proxyprocess.h"

/****************************************************************************************************
class  Arbiter_Appliction
****************************************************************************************************/
class ArbiterAppliction : public Comm_SvrdApp_NonCtrl
{
protected:

    // ��ʱ������ʱ����, ����̫С, ��������Ϊ200����
    static const unsigned int TIMER_INTERVAL = 200000;

protected:
    // �Լ��ĵ���͵͵���ţ�
    ArbiterAppliction();
    ~ArbiterAppliction();

protected:
    // �����PROXY�Ľӿ�
    InterfaceProxyProcess     *interface_proxy_;

    // �����FRAME������
    unsigned int                 process_frame_count_;

    // �����ļ�
    conf_proxysvr::LPCONFIG      conf_;

protected:
    static ArbiterAppliction   *instance_;

public:
    // �����յ���APPFRAME,��ʹ��const��ԭ������ΪΪ�˼ӿ��ٶȣ��ܶ�ط���ֱ�ӽ�recv_frame�޸�
    virtual int process_recv_appframe(Comm_App_Frame *recv_frame);

    // ��������
    virtual int load_app_conf();
    virtual int merge_app_cfg_file();

    virtual int init();

    virtual void exit();

    // ���¼���·�ɱ�
    virtual int reload();

public:
    // ����ʵ������
    static ArbiterAppliction *instance();

    // ������ʵ��
    static void clean_instance();

    const conf_proxysvr::LPCONFIG get_config()
    {
        return conf_;
    }

};

#endif //ARBITER_APPLICATION_H_

