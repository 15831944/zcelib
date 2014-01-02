#include "soar_predefine.h"
#include "soar_ogre_frame.h"

/******************************************************************************************
struct  Ogre4a_App_Frame OGRE ���������ڲ�����֡ͷ
******************************************************************************************/
//����DATA����������,��������
size_t Ogre4a_App_Frame::MAX_OF_OGRE_DATA_LEN = 64 * 1024;
//����FRAME�ĳ���,ΪMAX_OF_OGRE_DATA_LEN �� LEN_OF_OGRE_FRAME_HEAD
size_t Ogre4a_App_Frame::MAX_OF_OGRE_FRAME_LEN = Ogre4a_App_Frame::MAX_OF_OGRE_DATA_LEN + Ogre4a_App_Frame::LEN_OF_OGRE_FRAME_HEAD;

Ogre4a_App_Frame::Ogre4a_App_Frame():
    ogre_frame_len_(LEN_OF_OGRE_FRAME_HEAD),
    snd_peer_info_(0, 0),
    ogre_frame_option_(0),
    ogre_send_errnum_(0)
{
}
//
Ogre4a_App_Frame::~Ogre4a_App_Frame()
{
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2005��11��2��
Function        : new
Return          : void* Zerg_App_Frame::operator
Parameter List  :
Param1: size_t
Param2: size_t lenframe
Description     : ����New����
Calls           :
Called By       :
Other           : Ӧ��д��delete
Modify Record   :
******************************************************************************************/
void   *Ogre4a_App_Frame::operator new (size_t , size_t lenframe)
{
    //assert( FrameLength <= MAX_FRAME_SIZE );
    if (lenframe < sizeof(Ogre4a_App_Frame))
    {
        ZCE_ASSERT(false);
        lenframe = sizeof(Ogre4a_App_Frame);
    }

    void *ptr = ::new unsigned char[lenframe];

#if defined(DEBUG) || defined(_DEBUG)
    //���֡���ĸ��ط��������⣬����������һ��
    memset(ptr, 0, lenframe);
#endif //DEBUG

    //reinterpret_cast<Zerg_App_Frame*>(ptr)->m_Length = static_cast<unsigned int>(lenframe);
    return ptr;
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2005��11��30��
Function        : delete
Return          : void Ogre4a_App_Frame::operator
Parameter List  :
Param1: void* ptrframe
Param2: size_t
Description     : ���ɺ�ϰ��,дnew,��дdelete.
Calls           :
Called By       :
Other           : ��ʵ��дҲ�������ڴ�й¶,����Ϊ�˲���������ı�����.
Modify Record   :
******************************************************************************************/
void Ogre4a_App_Frame::operator delete(void *ptrframe, size_t )
{
    char *ptr = reinterpret_cast<char *>(ptrframe) ;
    delete []ptr;
}

//��������DATA����������,
void Ogre4a_App_Frame::SetMaxFrameDataLen(unsigned int  max_framedata)
{
    MAX_OF_OGRE_DATA_LEN = max_framedata;
    //����FRAME�ĳ���,ΪMAX_OF_OGRE_DATA_LEN �� LEN_OF_OGRE_FRAME_HEAD
    MAX_OF_OGRE_FRAME_LEN = MAX_OF_OGRE_DATA_LEN + LEN_OF_OGRE_FRAME_HEAD;
}

//�����Լ�Rcv ,Snd Peer Info
void Ogre4a_App_Frame::ExchangeRcvSndPeerInfo()
{
    Socket_Peer_Info sock_peer = snd_peer_info_;
    snd_peer_info_  = rcv_peer_info_;
    rcv_peer_info_ = sock_peer;
}

//�������˽���Rcv ,Snd Peer Info,
void Ogre4a_App_Frame::ExchangeRcvSndPeerInfo(const Ogre4a_App_Frame *exframe )
{
    ogre_frame_option_ = exframe->ogre_frame_option_;
    rcv_peer_info_ = exframe->snd_peer_info_ ;
    snd_peer_info_ = exframe->rcv_peer_info_ ;
}

//���APPFRAME��ȫ������Ϣ
void Ogre4a_App_Frame::DumOgreFrameHead(const Ogre4a_App_Frame *proc_frame,
                                       const char *outstr,
                                       ZCE_LOG_PRIORITY log_priority)
{

    ZCE_LOGMSG(log_priority, "[framework] [OGRE_FRAME_MONITOR]location:%s ,frame_len:%u,send_ip:%u,send_port:%u,rcv_ip:%u,rcv_port:%u,",
               outstr,
               proc_frame->ogre_frame_len_,
               proc_frame->snd_peer_info_.peer_ip_address_,
               proc_frame->snd_peer_info_.peer_port_,
               proc_frame->rcv_peer_info_.peer_ip_address_,
               proc_frame->rcv_peer_info_.peer_port_
              );

#ifdef ZCE_OS_WINDOWS
    ZCE_UNUSED_ARG(outstr);
    ZCE_UNUSED_ARG(log_priority);
#endif
}

//���APPFRAME��ȫ������Ϣ
void Ogre4a_App_Frame::DumOgreFrameHead(const char *outstr,
                                       ZCE_LOG_PRIORITY log_priority) const
{
    this->DumOgreFrameHead(this, outstr, log_priority);
}

