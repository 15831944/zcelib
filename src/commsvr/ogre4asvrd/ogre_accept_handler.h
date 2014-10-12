
#ifndef OGRE_TCP_ACCEPT_HANDLER_H_
#define OGRE_TCP_ACCEPT_HANDLER_H_


class Ogre_IPRestrict_Mgr;

/*!
* @brief      TCP Accept �����EventHandler
*             
* @note       
*/
class OgreTCPAcceptHandler : public ZCE_Event_Handler
{

public:
    //���캯��
    OgreTCPAcceptHandler(const ZCE_Sockaddr_In &listen_addr,
                         const char *recv_mod_file,
                         ZCE_Reactor *reactor = ZCE_Reactor::instance());
protected:
    ~OgreTCPAcceptHandler();
public:
    //����Bind��IP��ַ
    void set_peer_bindaddr(ZCE_Sockaddr_In &addr);

    //���������Ķ˿�
    int create_listenpeer();

    //�����ж˿�
    virtual int handle_input(ZCE_HANDLE );

    //�رմ���
    virtual int handle_close ();

    //�õ�Acceptor�ľ��
    ZCE_HANDLE get_handle(void) const;

protected:

    //ACCPET PEER
    ZCE_Socket_Acceptor        peer_acceptor_;

    //��ĵ�ַ
    ZCE_Sockaddr_In            accept_bind_addr_;

    //��ȡ���ݵ�ģ����ļ�����
    std::string                recv_mod_file_;
    //TCP��ȡ���ݵ�ģ��HANDLER
    ZCE_SHLIB_HANDLE           recv_mod_handler_;

    //ģ���ܽ�ĺ�������
    FP_JudgeRecv_WholeFrame    fp_judge_whole_frame_;

    //IP���ƹ�����
    Ogre_IPRestrict_Mgr       *ip_restrict_;

};

#endif //_OGRE_TCP_ACCEPT_HANDLER_H_

