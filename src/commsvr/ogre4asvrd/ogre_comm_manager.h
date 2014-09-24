

#ifndef OGRE_COMMUNICATE_MANGER_H_
#define OGRE_COMMUNICATE_MANGER_H_

class OgreTCPAcceptHandler;
class OgreUDPSvcHandler;
class Ogre4a_AppFrame;
class Ogre_Connect_Server;

//ͨѶ���й�����
class Ogre_Comm_Manger
{
protected:
    //һ����෢��1024֡
    static const unsigned int MAX_ONCE_SEND_FRAME = 1024;

protected:

    //�����˿ڵ�Handler�������
    std::vector<OgreTCPAcceptHandler *> accept_handler_ary_;
    //UDP�˿ڵ�Handler�������
    std::vector<OgreUDPSvcHandler *>    udp_handler_ary_;

protected:
    //
    static Ogre_Comm_Manger              *instance_;

public:

    Ogre_Comm_Manger();
    ~Ogre_Comm_Manger();

    //��ȡ�����ļ�
    int get_configure(Zen_INI_PropertyTree &cfg_file);
    //
    int init_accept_by_conf(Zen_INI_PropertyTree &cfg_file);
    //
    int init_udp_by_conf(Zen_INI_PropertyTree &cfg_file);

    //��SEND�ܵ��ҵ����е�����ȥ����,
    int get_all_senddata_to_write(size_t &procframe);

    //��鰲ȫ�˿�
    int check_safe_port(ZCE_Sockaddr_In &inetadd);

    //��ʼ��ͨѶ������
    int init_comm_manger(Zen_INI_PropertyTree &cfg_file);
    //ע��ͨѶ������
    int uninit_comm_manger();

public:
    //�õ����ӵ�ʵ��
    static Ogre_Comm_Manger *instance();
    //�����ӵ�ʵ��
    static void clean_instance();
};

#endif //#ifndef _OGRE_COMMUNICATE_MANGER_H_

