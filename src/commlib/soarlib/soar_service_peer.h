#ifndef SOARING_LIB_SERVICES_PEER_H_
#define SOARING_LIB_SERVICES_PEER_H_

#pragma pack (1)
/****************************************************************************************************
class  Socket_Peer_Info ��SOCKET PEER��Ϣ,��ӦIPV4�Ĵ��롣
****************************************************************************************************/
class SOARING_EXPORT Socket_Peer_Info
{
public:
    //��Ч��SERVICE ����
    static const unsigned short  INVALID_PERR_IP_ADDRESS = 0;
    //��Ч��SERVICE ID
    static const unsigned int   INVALID_PERR_PORT   = 0;

public:

    //IP��ַ
    unsigned int            peer_ip_address_;
    //�˿ں���
    unsigned short          peer_port_;

public:
    //����,��������,Ĭ��Ϊ0
    explicit Socket_Peer_Info(unsigned int peer_ip_address = INVALID_PERR_IP_ADDRESS, unsigned short peer_port = INVALID_PERR_PORT);
    explicit Socket_Peer_Info(const ZCE_Sockaddr_In &);
    //
    ~Socket_Peer_Info();

    //����
    void SetSocketPeerInfo(unsigned int peer_ip_address, unsigned short peer_port );
    //����ZCE_Sockaddr_In��IP��ַ����
    void SetSocketPeerInfo(const ZCE_Sockaddr_In &);

    //�ȽϺ���,services_type_,services_id_
    bool operator ==(const Socket_Peer_Info &others) const;
    //�Ƚϲ�ͬ����,
    bool operator !=(const Socket_Peer_Info &others) const;
    //�и���������,
    bool operator <(const Socket_Peer_Info &others) const;
};

#pragma pack ()

class SOARING_EXPORT HashofSocketPeerInfo
{
public:
    size_t operator()(const Socket_Peer_Info &peer_info) const
    {
        return (size_t (peer_info.peer_port_) << 16) + peer_info.peer_ip_address_ ;
    }
};

#endif //#ifndef SOARING_LIB_SERVICES_PEER_H_

