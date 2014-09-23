

#ifndef OGRE_TCP_SVR_PEER_INFO_SET_H_
#define OGRE_TCP_SVR_PEER_INFO_SET_H_

class Ogre_TCP_Svc_Handler;
/****************************************************************************************************
class  PeerInfoSetToTCPHdlMap
****************************************************************************************************/
class PeerInfoSetToTCPHdlMap
{
protected:
    typedef hash_map<Socket_Peer_Info, Ogre_TCP_Svc_Handler *, HashofSocketPeerInfo > MapOfSocketPeerInfo;
    //
    MapOfSocketPeerInfo  peer_info_set_;

public:
    //���캯��,
    PeerInfoSetToTCPHdlMap();
    ~PeerInfoSetToTCPHdlMap();

    //��ʼ��
    void init_services_peerinfo(size_t szpeer);

    //��ѯ������Ϣ
    int find_services_peerinfo(const Socket_Peer_Info &svrinfo, Ogre_TCP_Svc_Handler*&);
    //����������Ϣ
    int add_services_peerinfo(const Socket_Peer_Info &svrinfo, Ogre_TCP_Svc_Handler *);
    //ɾ��������Ϣ
    size_t del_services_peerinfo(const Socket_Peer_Info &svrinfo);
    //
    void clear_and_close();
};

#endif //_OGRE_TCP_SVR_PEER_INFO_SET_H_

