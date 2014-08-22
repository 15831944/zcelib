#include "soar_predefine.h"
#include "soar_ogre_svc_peer.h"

/****************************************************************************************************
class  Socket_Peer_Info ��������Ϣ,Ϊ��ͬʱ��ʾIP+�˿ڵ�ģʽ������UNION��ʽ
****************************************************************************************************/

Socket_Peer_Info::Socket_Peer_Info(unsigned int peer_ip_address, unsigned short peer_port):
    peer_ip_address_(peer_ip_address),
    peer_port_(peer_port)
{
}

Socket_Peer_Info::Socket_Peer_Info(const ZCE_Sockaddr_In &inet_addr):
    peer_ip_address_(inet_addr.get_ip_address()),
    peer_port_(inet_addr.get_port_number())
{
}

Socket_Peer_Info::~Socket_Peer_Info()
{
}

//
void Socket_Peer_Info::SetSocketPeerInfo(unsigned int peer_ip_address, unsigned short peer_port )
{
    peer_ip_address_ = peer_ip_address;
    peer_port_ = peer_port;
}

//����ACE��IP��ַ����
void Socket_Peer_Info::SetSocketPeerInfo(const ZCE_Sockaddr_In &inet_addr)
{
    peer_ip_address_ = inet_addr.get_ip_address();
    peer_port_ = inet_addr.get_port_number();
}

//�ȽϺ���,
bool Socket_Peer_Info::operator ==(const Socket_Peer_Info &others) const
{
    if (others.peer_ip_address_ == peer_ip_address_ && others.peer_port_ == peer_port_)
    {
        return true;
    }

    return false;
}
//�Ƚϲ�ͬ����,
bool Socket_Peer_Info::operator !=(const Socket_Peer_Info &others) const
{
    return !(*this == others);
}
//�и���������,
bool Socket_Peer_Info::operator <(const Socket_Peer_Info &others) const
{
    if (peer_ip_address_ + peer_port_ < others.peer_ip_address_ + others.peer_port_ )
    {
        return true;
    }

    return false;
}

