#include "soar_predefine.h"
#include "soar_error_code.h"
#include "soar_zerg_frame.h"

//ZERG�������䴫����Ϣ��ͨ��֡ͷ
Zerg_App_Frame::Zerg_App_Frame(uint32_t cmd,
                               uint32_t lenframe,
                               uint32_t frameoption):
    frame_length_(lenframe),
    frame_option_(frameoption),
    frame_command_(cmd),
    frame_uid_(0),
    send_service_(0, 0),
    recv_service_(0, 0),
    proxy_service_(0, 0),
    transaction_id_(0),
    backfill_trans_id_(0),
    send_ip_address_(0)
{
}

//���캯��,����,�ͻ��˳�ʼ��,
Zerg_App_Frame::Zerg_App_Frame(uint32_t cmd,
                               uint32_t lenframe,
                               uint32_t uin,
                               uint16_t sndsvrtype,
                               uint16_t rcvsvctype,
                               uint32_t frameoption):
    frame_length_(lenframe),
    frame_option_(frameoption),
    frame_command_(cmd),
    frame_uid_(uin),
    send_service_(sndsvrtype, uin),
    recv_service_(rcvsvctype, 0),
    proxy_service_(0, 0),
    transaction_id_(0),
    backfill_trans_id_(0),
    app_id_(0),
    send_ip_address_(0)
{
}
//���캯��,���ڷ��͸�һ���Ǵ��������
Zerg_App_Frame::Zerg_App_Frame(uint32_t cmd,
                               uint32_t lenframe,
                               uint32_t uin,
                               const SERVICES_ID &sndsvr,
                               const SERVICES_ID &rcvsvr,
                               uint32_t frameoption):
    frame_length_(lenframe),
    frame_option_(frameoption),
    frame_command_(cmd),
    frame_uid_(uin),
    send_service_(sndsvr),
    recv_service_(rcvsvr),
    proxy_service_(0, 0),
    transaction_id_(0),
    backfill_trans_id_(0),
    app_id_(0),
    send_ip_address_(0)
{
}

//���캯��,���ڷ��͸�һ�����������
Zerg_App_Frame::Zerg_App_Frame(uint32_t cmd, uint32_t lenframe, uint32_t uin, const SERVICES_ID &sndsvr, const SERVICES_ID &proxysvr, uint16_t rcvsvrtype, uint32_t frameoption):
    frame_length_(lenframe),
    frame_option_(frameoption),
    frame_command_(cmd),
    frame_uid_(uin),
    send_service_(sndsvr),
    recv_service_(rcvsvrtype, 0),
    proxy_service_(proxysvr),
    transaction_id_(0),
    backfill_trans_id_(0),
    app_id_(0),
    send_ip_address_(0)
{
}

Zerg_App_Frame::~Zerg_App_Frame()
{
}

// Assign =�������
Zerg_App_Frame &Zerg_App_Frame::operator = (const Zerg_App_Frame &other)
{
    if (this != &other )
    {
        memcpy(this, &other, other.frame_length_);
    }

    return *this;
}

//����New����
void   *Zerg_App_Frame::operator new (size_t , size_t lenframe)
{
    //assert( FrameLength <= MAX_FRAME_SIZE );
    if (lenframe < sizeof(Zerg_App_Frame))
    {
        lenframe = sizeof(Zerg_App_Frame);
    }

    //�������ݼ��ܺ�ĳ���

    void *ptr = ::new unsigned char[lenframe];

#if defined(DEBUG) || defined(_DEBUG)
    //���֡���ĸ��ط��������⣬����������һ��
    memset(ptr, 0, lenframe);
#endif //DEBUG

    //reinterpret_cast<Zerg_App_Frame*>(ptr)->frame_length_ = static_cast<uint32_t>(lenframe);
    return ptr;
}

//���ɺ�ϰ��,дnew,��дdelete.
//��ʵ��дҲ�������ڴ�й¶,����Ϊ�˲���������ı�����.
void Zerg_App_Frame::operator delete(void *ptrframe, size_t )
{
    char *ptr = reinterpret_cast<char *>(ptrframe) ;
    delete []ptr;
}



//���AppData���ݵ�APPFrame
int Zerg_App_Frame::fill_appdata(const size_t szdata, const char *vardata)
{
    // �жϳ����Ƿ��㹻
    if (szdata > MAX_LEN_OF_APPFRAME_DATA)
    {
        return SOAR_RET::ERROR_APPFRAME_BUFFER_SHORT;
    }

    //��д�������ĳ���
    memcpy(frame_appdata_, vardata, szdata);
    frame_length_ = static_cast<uint32_t>( Zerg_App_Frame::LEN_OF_APPFRAME_HEAD + szdata);
    return SOAR_RET::SOAR_RET_SUCC;
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2005��11��3��
Function        : Zerg_App_Frame::framehead_encode
Return          : void
Parameter List  : NULL
Description     : �����е�uint16_t,uint32_tת��Ϊ������
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
void Zerg_App_Frame::framehead_encode()
{
    frame_length_ = htonl(frame_length_);
    frame_option_ = htonl(frame_option_);
    frame_command_ = htonl(frame_command_);

    frame_uid_ = htonl(frame_uid_);

    //
    recv_service_.services_type_ = htons(recv_service_.services_type_);
    recv_service_.services_id_ = htonl(recv_service_.services_id_);

    send_service_.services_type_ = htons(send_service_.services_type_);
    send_service_.services_id_ = htonl(send_service_.services_id_);
    //
    proxy_service_.services_type_ = htons(proxy_service_.services_type_);
    proxy_service_.services_id_ = htonl(proxy_service_.services_id_);

    transaction_id_ = htonl(transaction_id_);
    backfill_trans_id_ = htonl(backfill_trans_id_);
    app_id_ = htonl(app_id_);

    send_serial_number_ = htonl(send_serial_number_);

}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2005��11��3��
Function        : Zerg_App_Frame::framehead_decode
Return          : void
Parameter List  : NULL
Description     : �����е�uint16_t,uint32_tת��Ϊ������
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
void Zerg_App_Frame::framehead_decode()
{
    frame_length_ = ntohl(frame_length_);
    frame_option_ = ntohl(frame_option_);
    frame_command_ = ntohl(frame_command_);
    frame_uid_ = ntohl(frame_uid_);

    //
    recv_service_.services_type_ = ntohs(recv_service_.services_type_);
    recv_service_.services_id_ = ntohl(recv_service_.services_id_);

    send_service_.services_type_ = ntohs(send_service_.services_type_);
    send_service_.services_id_ = ntohl(send_service_.services_id_);
    //
    proxy_service_.services_type_ = ntohs(proxy_service_.services_type_);
    proxy_service_.services_id_ = ntohl(proxy_service_.services_id_);

    transaction_id_ = ntohl(transaction_id_);
    backfill_trans_id_ = ntohl(backfill_trans_id_);
    app_id_ = ntohl(app_id_);

    send_serial_number_ = ntohl(send_serial_number_);
}

//��ʼ����ͷ������
void Zerg_App_Frame::init_framehead(uint32_t lenframe, uint32_t option, uint32_t cmd)
{
    frame_length_ = static_cast<uint32_t>(lenframe);
    frame_command_ = cmd;
    frame_option_ = option ;
    inner_option_.frame_version_ = TSS_APPFRAME_V1;

    frame_uid_ = 0;

    send_service_.set_serviceid(0, 0);
    recv_service_.set_serviceid(0, 0);
    proxy_service_.set_serviceid(0, 0);
    send_serial_number_ = 0;
    transaction_id_ = 0;
    backfill_trans_id_ = 0;
    app_id_ = 0;
}

//��д�����߷�����Ϣ
void Zerg_App_Frame::set_send_svcid(uint16_t svrtype, uint32_t svrid)
{
    send_service_.services_type_ = svrtype;
    send_service_.services_id_  = svrid;
}

void Zerg_App_Frame::set_recv_svcid(uint16_t svrtype, uint32_t svrid)
{
    recv_service_.services_type_ = svrtype;
    recv_service_.services_id_ = svrid;
}

void Zerg_App_Frame::set_proxy_svcid(uint16_t svrtype, uint32_t svrid)
{
    proxy_service_.services_type_ = svrtype;
    proxy_service_.services_id_ = svrid;
}

void Zerg_App_Frame::set_all_svcid(const SERVICES_ID &rcvinfo, const SERVICES_ID &sndinfo, const SERVICES_ID &proxyinfo)
{
    recv_service_ = rcvinfo;
    send_service_ = sndinfo;
    proxy_service_ = proxyinfo;
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2005��11��29��
Function        : Zerg_App_Frame::exchange_rcvsnd_svcid
Return          : void
Parameter List  :
  Param1: void
Description     : ���������ߺͷ�����,����Ҫ��������ʱ
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
void Zerg_App_Frame::exchange_rcvsnd_svcid(void )
{

    SERVICES_ID tmpsvrinfo = recv_service_;
    recv_service_ = send_service_;
    send_service_ = tmpsvrinfo;
}

void Zerg_App_Frame::exchange_rcvsnd_svcid(Zerg_App_Frame &exframe )
{
    recv_service_ = exframe.send_service_;
    send_service_ = exframe.recv_service_;
    proxy_service_ = exframe.proxy_service_;
    transaction_id_ = exframe.transaction_id_;
    backfill_trans_id_ = exframe.backfill_trans_id_;
    app_id_ = exframe.app_id_;
    frame_uid_ = exframe.frame_uid_;
}

void Zerg_App_Frame::fillback_appframe_head(Zerg_App_Frame &exframe )
{
    recv_service_ = exframe.send_service_;
    send_service_ = exframe.recv_service_;
    proxy_service_ = exframe.proxy_service_;
    transaction_id_ = exframe.backfill_trans_id_;
    //��ȥ������idӦ�������������id
    backfill_trans_id_ = exframe.transaction_id_;
    app_id_ = exframe.app_id_;
    frame_uid_ = exframe.frame_uid_;
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2005��11��29��
Function        : Zerg_App_Frame::dump_appframe_info
Return          : void
Parameter List  :
  Param1: std::ostringstream& strstream ���Ե�������,
Description     : ����AppFrame,ת�����е�������ϢΪһ������Զ�����״̬
Calls           :
Called By       :
Other           : ���������һ���ǳ�,�ǳ�����ʱ�Ĳ���,����,�����汾ǧ��Ҫʹ��
Modify Record   :
******************************************************************************************/
void Zerg_App_Frame::dump_appframe_info(std::ostringstream &strstream) const
{
    dump_appframe_head(strstream);
    dump_appframe_data(strstream);
    return;
}

//Dump���е�������Ϣ,һ���ֽ��ֽڵĸ�����,
void Zerg_App_Frame::dump_appframe_data(std::ostringstream &strstream) const
{
    //
    char tmpstr[MAX_LEN_OF_APPFRAME * 2 + 1];
    size_t datalen = frame_length_ - LEN_OF_APPFRAME_HEAD;
    size_t out_len = MAX_LEN_OF_APPFRAME * 2 + 1;

    ZCE_LIB::base16_encode((unsigned char *)frame_appdata_, datalen, (unsigned char *)tmpstr, &out_len);

    strstream << " data:" <<  tmpstr << std::endl;

}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2005��12��1��
Function        : dump_appframe_head
Return          : void
Parameter List  :
  Param1: std::ostringstream& strstream �����Str String
Description     : �����ͷ��Ϣ
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
void Zerg_App_Frame::dump_appframe_head(std::ostringstream &strstream) const
{
    strstream << "Len:" << frame_length_
              << " Framedesc:0x " << frame_option_
              << " Command:" << frame_command_
              << " Uin:" << frame_uid_
              << " TransactionID:" << transaction_id_
              << " BackfillTransID:" << backfill_trans_id_
              << " ProcessHandler:" << app_id_
              << " Sendip:" << send_ip_address_;
    strstream << "Rcvsvr:" << recv_service_.services_type_
              << "|" << recv_service_.services_id_
              << " Sndsvr:" << send_service_.services_type_
              << "|" << send_service_.services_id_
              << " Proxysvr:" << proxy_service_.services_type_
              << "|" << proxy_service_.services_id_;
    return;
}

//���APPFRAME��ͷ����Ϣ
void Zerg_App_Frame::dumpoutput_framehead(const char *outstr, ZCE_LOG_PRIORITY log_priority) const
{
    std::ostringstream strstream;
    dump_appframe_head(strstream);
    ZCE_LOGMSG(log_priority, "[framework] [%s]%s" , outstr, strstream.str().c_str());
}
//���APPFRAME��ȫ������Ϣ
void Zerg_App_Frame::dumpoutput_frameinfo(const char *outstr, ZCE_LOG_PRIORITY log_priority)  const
{

    std::ostringstream strstream;
    dump_appframe_info(strstream);
    ZCE_LOGMSG(log_priority, "[framework] [%s]%s" , outstr, strstream.str().c_str());
}

//���APPFRAME��ͷ����Ϣ
void Zerg_App_Frame::dumpoutput_framehead(const Zerg_App_Frame *proc_frame, const char *outstr, ZCE_LOG_PRIORITY log_priority)
{
    proc_frame->dumpoutput_framehead(outstr, log_priority);
}
//���APPFRAME��ȫ������Ϣ
void Zerg_App_Frame::dumpoutput_frameinfo(const Zerg_App_Frame *proc_frame , const char *outstr, ZCE_LOG_PRIORITY log_priority)
{
    proc_frame->dumpoutput_frameinfo(outstr, log_priority);
}

//Cloneһ��APP FRAME
Zerg_App_Frame *Zerg_App_Frame::clone() const
{
    Zerg_App_Frame *proc_frame = new(frame_length_) Zerg_App_Frame();
    memcpy(proc_frame, this, frame_length_);
    return proc_frame;
}

//Cloneһ��APP FRAME
void Zerg_App_Frame::clone(Zerg_App_Frame *clone_frame) const
{
    memcpy(clone_frame, this, frame_length_);
    return ;
}

//Cloneһ��APP FRAME ��ͷ��
void Zerg_App_Frame::clone_head(Zerg_App_Frame *clone_frame) const
{
    memcpy(clone_frame, this, LEN_OF_APPFRAME_HEAD);
    return ;
}

