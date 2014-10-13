

#ifndef OGRE_TCP_SVR_PEER_INFO_SET_H_
#define OGRE_TCP_SVR_PEER_INFO_SET_H_

class Ogre_TCP_Svc_Handler;
/****************************************************************************************************
class  PeerInfoSetToTCPHdlMap
****************************************************************************************************/
class PeerInfoSetToTCPHdlMap
{


public:
    //���캯��,
    PeerInfoSetToTCPHdlMap();
    ~PeerInfoSetToTCPHdlMap();

    //��ʼ��
    void init_services_peerinfo(size_t szpeer);

    //��ѯ������Ϣ
    int find_services_peerinfo(const SOCKET_PERR_ID &svrinfo, Ogre_TCP_Svc_Handler*&);
    //����������Ϣ
    int add_services_peerinfo(const SOCKET_PERR_ID &svrinfo, Ogre_TCP_Svc_Handler *);
    //ɾ��������Ϣ
    size_t del_services_peerinfo(const SOCKET_PERR_ID &svrinfo);
    //
    void clear_and_close();

protected:
    //
    typedef std::unordered_map<SOCKET_PERR_ID, Ogre_TCP_Svc_Handler *, HashofSocketPeerInfo > MapOfSocketPeerInfo;
    //
    MapOfSocketPeerInfo  peer_info_set_;

};

#endif //OGRE_TCP_SVR_PEER_INFO_SET_H_

