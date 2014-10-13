
#ifndef SOARING_LIB_OGRE_SERVER_INTERFACE_HEAD_H_
#define SOARING_LIB_OGRE_SERVER_INTERFACE_HEAD_H_

#include "soar_ogre_svc_peer.h"

/******************************************************************************************
struct  Ogre4a_App_Frame OGRE ���������ڲ�����֡ͷ
******************************************************************************************/
#pragma pack (1)

class Ogre4a_App_Frame
{
public:
    //FRAME��ͷ���ߴ�,
    static const size_t  LEN_OF_OGRE_FRAME_HEAD = 20;

    //����DATA����������,��������
    static size_t        MAX_OF_OGRE_DATA_LEN;
    //����FRAME�ĳ���,ΪMAX_OF_OGRE_DATA_LEN �� LEN_OF_OGRE_FRAME_HEAD
    static size_t        MAX_OF_OGRE_FRAME_LEN;

    //֡������,��m_Option�ֶ�ʹ��
    enum OGRE_APPFRAME_OPTION
    {

        //����PEERΪUDP
        OGREDESC_PEER_UDP            = 0x1,
        //����PEERΪTCP
        OGREDESC_PEER_TCP            = 0x2,

        //PEER���ر�
        OGREDESC_PEER_CLOSED         = 0x4,
        //ǿ�ƹر�PEER,���ܷ��Ͷ����м��Ƿ�������,ע���DESC_SNDPRC_CLOSE_PEER������
        OGREDESC_CLOSE_PEER          = 0x8,

        //ĳ������֡����ʧ��,֪ͨ����ķ���
        OGREDESC_SEND_ERROR          = 0x10,
        //�������ʧ��,��¼����ʧ�ܵ�����
        OGREDESC_SEND_FAIL_RECORD     = 0x20,
        //�������ʧ��,֪ͨ�����Ӧ�ý���
        OGREDESC_SNDPRC_NOTIFY_APP   = 0x40,
        //������ͳɹ���,ֱ�ӶϿ����ӣ����ڲ���TCP�Ķ�����
        OGREDESC_SNDPRC_CLOSE_PEER   = 0x80,
    };

public:
    //֡�ĳ��ȣ�����֡ͷ�Ľṹ
    unsigned int                   ogre_frame_len_;

    //�Զ���Ϣ
    SOCKET_PERR_ID               snd_peer_info_;
    //�Զ���Ϣ
    SOCKET_PERR_ID               rcv_peer_info_;

    //֡��ѡ���
    unsigned int                   ogre_frame_option_: 28;
    //�ٺ�4BIT��Ϊ���ʹ������
    unsigned int                   ogre_send_errnum_: 4;

    //Appdata ��һ���䳤�ȵ��ַ������б�ʾ,
#ifdef ZCE_OS_WINDOWS
#pragma warning ( disable : 4200)
#endif
    char                           frame_data_[];
#ifdef ZCE_OS_WINDOWS
#pragma warning ( default : 4200)
#endif

public:

    //
    Ogre4a_App_Frame();
    //
    Ogre4a_App_Frame(const SOCKET_PERR_ID);
    //
    ~Ogre4a_App_Frame();

public:
    //

public:
    //����FRAME DATA������
    inline void ResetFrameHead();
    //�������
    inline void fill_write_data(const unsigned int size_data, const char *src_data);
    //��ȡ����
    inline void GetReadData(unsigned int &size_data, char *dest_data);
    //�õ�ʹ�õ��ָ��
    inline char *GetUsePoint();
    //����FRAME�ĳ���
    inline void AddFrameLen(unsigned int add_frame_len);

    //�����Լ�Rcv ,Snd Peer Info
    void ExchangeRcvSndPeerInfo();
    //�������˽���Rcv ,Snd Peer Info,
    void ExchangeRcvSndPeerInfo(const Ogre4a_App_Frame *exframe );

    //DUMP OGRE FRAME��ͷ����Ϣ
    void DumOgreFrameHead(const char *outstr,
                          ZCE_LOG_PRIORITY log_priority) const;

public:
    //����New����
    static void   *operator new (size_t , size_t lenframe);
    //������delte������ͨ��������ʵû��ʲô����,
    static void operator delete(void *ptrframe, size_t );

public:

    //��������DATA����������,
    static void SetMaxFrameDataLen(unsigned int  max_framedata);
    //DUMP OGRE FRAME��ͷ����Ϣ
    static void DumOgreFrameHead(const Ogre4a_App_Frame *proc_frame,
                                 const char *outstr,
                                 ZCE_LOG_PRIORITY log_priority);

};

#pragma pack ()

//---------------------------------------------------------------------------------------------
//��ӡ���ͷ����Ϣ�Ŀ��ƺ�
#if defined _DEBUG || defined DEBUG
#define DEBUGDUMP_OGRE_HEAD(x,y,z)      Ogre4a_App_Frame::DumOgreFrameHead(x,y,z);
#else
#define DEBUGDUMP_OGRE_HEAD(x,y,z)
#endif

//---------------------------------------------------------------------------------------------
//��������

//����FRAME DATA������
inline void Ogre4a_App_Frame::ResetFrameHead()
{
    ogre_frame_len_ = LEN_OF_OGRE_FRAME_HEAD;
    ogre_frame_option_ = 0;
    ogre_send_errnum_ = 0;
}
//�������
inline void Ogre4a_App_Frame::fill_write_data(const unsigned int size_data, const char *src_data)
{
    memcpy(frame_data_ + ogre_frame_len_ - LEN_OF_OGRE_FRAME_HEAD, src_data, size_data);
    ogre_frame_len_ += size_data;
}
//��ȡ����
inline void Ogre4a_App_Frame::GetReadData(unsigned int &size_data, char *dest_data)
{
    size_data = ogre_frame_len_ - LEN_OF_OGRE_FRAME_HEAD;
    memcpy(dest_data, frame_data_ + LEN_OF_OGRE_FRAME_HEAD, size_data);
}
//�õ�ʹ�õ��ָ��
inline char *Ogre4a_App_Frame::GetUsePoint()
{
    return frame_data_ + ogre_frame_len_ - LEN_OF_OGRE_FRAME_HEAD;
}
//����FRAME�ĳ���
inline void Ogre4a_App_Frame::AddFrameLen(unsigned int add_frame_len)
{
    ogre_frame_len_ += add_frame_len;
}

/******************************************************************************************
�ӿڶ���
******************************************************************************************/
//
//��.so���غ�������
static const char STR_JUDGE_RECV_WHOLEFRAME[] = "JudgeRecvWholeFrame";
//
typedef int ( *FP_JudgeRecv_WholeFrame)(const char *recv_buf,
                                      unsigned int buf_len,
                                      unsigned int max_buf_len,
                                      bool &if_whole,
                                      unsigned int &frame_len);

#endif //#ifndef SOARING_LIB_OGRE_SERVER_INTERFACE_HEAD_H_

