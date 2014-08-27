
#ifndef SOARING_LIB_SVRD_APP_NONCTRL_H_
#define SOARING_LIB_SVRD_APP_NONCTRL_H_

#include "soar_svrd_app_base.h"

class Zerg_App_Frame;
//������������APP FRAME
class Comm_SvrdApp_NonCtrl : public Comm_Svrd_Appliction
{
protected:

    //���ܵ�������
    Zerg_App_Frame          *nonctrl_recv_buffer_;

protected:
    //
    Comm_SvrdApp_NonCtrl();
    virtual ~Comm_SvrdApp_NonCtrl();

public:

    //���д���,
    virtual int run_instance();

protected:

    //������յ���Frame,
    virtual int popfront_recvpipe(size_t &procframe);
    //�����յ���APPFRAME����ʹ��const��ԭ������ΪΪ�˼ӿ��ٶȣ��ܶ�ط���ֱ�ӽ�recv_frame�޸�
    virtual int process_recv_appframe(Zerg_App_Frame *recv_frame) = 0;

    // ������Ҫ���������
    virtual int proc(size_t &proc_data_num)
    {
        proc_data_num = 0;
        return 0;
    }

};

#endif //#ifndef SOARING_LIB_SVRD_APP_NONCTRL_H_

