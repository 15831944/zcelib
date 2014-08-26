
#ifndef ZERG_TCP_ACCEPT_HANDLER_H_
#define ZERG_TCP_ACCEPT_HANDLER_H_

class TcpHandlerImpl;
class Zerg_IPRestrict_Mgr;
/****************************************************************************************************
class  TCP_Accept_Handler TCP Accept �����EventHandler
****************************************************************************************************/
class TCP_Accept_Handler : public ZCE_Event_Handler
{

    typedef unordered_set<unsigned int> SetOfIPAddress;

protected:

    ~TCP_Accept_Handler();
public:
    //���캯��
    TCP_Accept_Handler(const SERVICES_ID &svcid,
                       const ZCE_Sockaddr_In &addr,
                       bool sessionkey_verify = true);

    //���������Ķ˿�
    int create_listen(unsigned int backlog);

    //�����ж˿�
    virtual int handle_input();

    //�رմ���
    virtual int handle_close ();

    //�õ�Acceptor�ľ��
    virtual ZCE_HANDLE get_handle(void) const;



protected:

    //ACCPET PEER
    ZCE_Socket_Acceptor      peer_acceptor_;

    //�Լ��� SVC INFO
    SERVICES_ID              my_svc_info_;

    //��ĵ�ַ
    ZCE_Sockaddr_In          accept_bind_addr_;

    //�Ƿ���֤�û���sessionkey_verify
    bool                     sessionkey_verify_;

    //IP���ƹ�����
    Zerg_IPRestrict_Mgr      *ip_restrict_;


};

#endif //_ZERG_TCP_ACCEPT_HANDLER_H_

