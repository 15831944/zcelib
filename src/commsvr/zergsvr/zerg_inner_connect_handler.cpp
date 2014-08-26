#include "zerg_predefine.h"
#include "zerg_inner_connect_handler.h"
#include "zerg_buf_storage.h"

InnerConnectHandler * InnerConnectHandler::inst_ = NULL;

InnerConnectHandler* InnerConnectHandler::instance()
{
    if (inst_ == NULL)
    {
        inst_ = new InnerConnectHandler;
    }
    return inst_;
}

InnerConnectHandler::~InnerConnectHandler()
{
}

InnerConnectHandler::InnerConnectHandler()
{

}

int InnerConnectHandler::preprocess_recvframe(Comm_App_Frame *proc_frame)
{
    zce_UNUSED_ARG(proc_frame);
    return SOAR_RET::SOAR_RET_SUCC;
}
int InnerConnectHandler::get_pkg_len(unsigned int &whole_frame_len, ZByteBuffer* buf)
{
    //�����4���ֽڶ�û���ռ���,���ô����������
    //ע���������32λ��������
    if (buf->size_of_buffer_ - buf->size_of_use_ >= sizeof(unsigned int) )
    {
        //�����4���ֽ�,���֡�ĳ���
        whole_frame_len = *(reinterpret_cast<unsigned int *>(buf->buffer_data_ + buf->size_of_use_));
        whole_frame_len = ntohl(whole_frame_len);

        //������ĳ��ȴ��ڶ������󳤶�,С����С����,����ȥ,�������������Ǵ�����󣬾��Ǳ�������
        if (whole_frame_len > Comm_App_Frame::MAX_LEN_OF_APPFRAME || whole_frame_len < Comm_App_Frame::LEN_OF_APPFRAME_HEAD)
        {
            return SOAR_RET::ERR_ZERG_GREATER_MAX_LEN_FRAME;
        }

    }
    return SOAR_RET::SOAR_RET_SUCC;
}

bool InnerConnectHandler::check_sender()
{
    return true;
}

