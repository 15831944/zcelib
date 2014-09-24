

#ifndef OGRE_UDP_CONTROL_SERVICE_H_
#define OGRE_UDP_CONTROL_SERVICE_H_

//forward declaration
class Ogre4aIPRestrictMgr;

class OgreUDPSvcHandler: public ZCE_Event_Handler
{
protected:
    //
public:
    OgreUDPSvcHandler(const ZCE_Sockaddr_In &upd_addr,
                      ZCE_Reactor *reactor = ZCE_Reactor::instance());
protected:
    ~OgreUDPSvcHandler();

public:
    //ȡ�þ��
    virtual ZCE_SOCKET get_handle(void) const;
    //
    virtual int handle_input(ZCE_HANDLE);
    //
    virtual int handle_close();

public:
    //��ʼ��UPD�˿�
    int InitUDPServices();
    //
    int SendDataToUDP(Ogre4a_AppFrame *send_frame);
public:
    //��ʼ����̬�ķ��Ͷ˿�
    static int OpenUDPSendPeer();
    //����UDP���ݸ�
    static int SendAllDataToUDP(Ogre4a_AppFrame *send_frame);

protected:

    //��PEER��ȡ����
    int ReadDataFromUDP(size_t &szrevc, ZCE_Sockaddr_In &remote_addr);
    //���յ������ݷ���ܵ�
    int pushdata_to_recvpipe();

protected:

    //
    ZCE_Socket_DataGram        dgram_peer_;

    //��ĵ�ַ
    ZCE_Sockaddr_In            udp_bind_addr_;
    //Socket_Peer_Info
    Socket_Peer_Info           peer_svc_info_;

    //
    Ogre4a_AppFrame           *dgram_databuf_;

    //IP���ƹ�����
    Ogre4aIPRestrictMgr       *ip_restrict_;

protected:
    //
    static  std::vector<OgreUDPSvcHandler *> ary_upd_peer_;

};

#endif //#ifndef _OGRE_UDP_CONTROL_SERVICE_H_

