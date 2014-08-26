#ifndef ZERG_TCP_HANDLER_IMPL_H_
#define ZERG_TCP_HANDLER_IMPL_H_

class ZByteBuffer;

// ����Э����շ��������߼�����ӿ�
class TcpHandlerImpl
{
public:
    virtual ~TcpHandlerImpl();
public:
    // �Ƿ���ע���������������ò���
    virtual bool need_register();
    // �������ջ���Ĳ�ͬ����
    virtual void init_buf(ZByteBuffer* buf);
    virtual void adjust_buf(ZByteBuffer* buf);

    // ��ȡ��һ��������ͷ����ɽ���
    virtual Comm_App_Frame* get_recvframe(ZByteBuffer* buf, unsigned int len);

    // ��ȡ��һ�����İ���
    virtual int get_pkg_len(unsigned int &whole_frame_len, ZByteBuffer* buf) = 0;

    virtual bool check_sender() = 0;
};

#endif
