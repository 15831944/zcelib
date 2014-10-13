#include "zce_predefine.h"
#include "zce_os_adapt_predefine.h"
#include "zce_trace_log_debug.h"
#include "zce_os_adapt_error.h"
#include "zce_os_adapt_socket.h"

//
ssize_t ZCE_LIB::writev (ZCE_SOCKET handle,
                        const iovec *buffers,
                        int iovcnt)
{
#if defined (ZCE_OS_WINDOWS)

    assert(iovcnt <= IOV_MAX);

    WSABUF wsa_buf[IOV_MAX];

    for (int i = 0; i < iovcnt; ++i)
    {
        wsa_buf[i].buf = static_cast<CHAR *>(buffers[i].iov_base);
        wsa_buf[i].len = static_cast<ULONG>(buffers[i].iov_len);
    }

    DWORD bytes_sent = 0;
    int zce_result = ::WSASend ((SOCKET) handle,
                                wsa_buf,
                                iovcnt,
                                &bytes_sent,
                                0,
                                0,
                                0);

    if (zce_result == SOCKET_ERROR)
    {
        errno = ::WSAGetLastError ();
        return -1;
    }

    return (ssize_t) bytes_sent;

#elif defined (ZCE_OS_LINUX)

    return ::writev (handle, buffers, iovcnt);
#endif
}

//
ssize_t ZCE_LIB::readv (ZCE_SOCKET handle,
                       iovec *buffers,
                       int iovcnt)
{
#if defined (ZCE_OS_WINDOWS)

    DWORD bytes_received = 0;
    int result = 1;

    // Winsock 2 has WSARecv and can do this directly, but Winsock 1 needs
    // to do the recvs piece-by-piece.

    //IOV_MAX���ݸ���ƽ̨��̫һ��
    assert(iovcnt <= IOV_MAX);

    WSABUF wsa_buf[IOV_MAX];

    for (int i = 0; i < iovcnt; ++i)
    {
        wsa_buf[i].buf = static_cast<CHAR *>(buffers[i].iov_base);
        wsa_buf[i].len = static_cast<ULONG>(buffers[i].iov_len);
    }

    DWORD flags = 0;
    result = ::WSARecv ( handle,
                         wsa_buf,
                         iovcnt,
                         &bytes_received,
                         &flags,
                         0,
                         0);

    if (result == SOCKET_ERROR)
    {
        errno = ::WSAGetLastError ();
        return -1;
    }

    return (ssize_t) bytes_received;

#elif defined (ZCE_OS_LINUX)

    return ::readv (handle, buffers, iovcnt);

#endif
}

//
ssize_t ZCE_LIB::recvmsg (ZCE_SOCKET handle,
                         msghdr *msg,
                         int flags)
{

#if defined (ZCE_OS_WINDOWS)

    assert(msg->msg_iovlen < IOV_MAX);

    WSABUF wsa_buf[IOV_MAX];

    for (size_t i = 0; i < msg->msg_iovlen; ++i)
    {
        wsa_buf[i].buf = static_cast<CHAR *>(msg->msg_iov[i].iov_base);
        wsa_buf[i].len = static_cast<ULONG>(msg->msg_iov[i].iov_len);
    }

    DWORD bytes_received = 0;

    int result = ::WSARecvFrom ((SOCKET) handle,
                                wsa_buf,
                                (DWORD)(msg->msg_iovlen),
                                &bytes_received,
                                (DWORD *) &flags,
                                static_cast<sockaddr *>(msg->msg_name),
                                &msg->msg_namelen,
                                0,
                                0);

    if (result != 0)
    {
        errno = ::WSAGetLastError ();
        return -1;
    }

    return bytes_received;

#elif defined (ZCE_OS_LINUX)

    return ::recvmsg (handle, msg, flags);

#endif
}

ssize_t ZCE_LIB::sendmsg (ZCE_SOCKET handle,
                         const struct msghdr *msg,
                         int flags)
{
#if defined (ZCE_OS_WINDOWS)

    //
    assert(msg->msg_iovlen < IOV_MAX);

    WSABUF wsa_buf[IOV_MAX];

    for (size_t i = 0; i < msg->msg_iovlen; ++i)
    {
        wsa_buf[i].buf = static_cast<CHAR *>(msg->msg_iov[i].iov_base);
        wsa_buf[i].len = static_cast<ULONG>(msg->msg_iov[i].iov_len);
    }

    DWORD bytes_sent = 0;
    int result = ::WSASendTo ((SOCKET) handle,
                              wsa_buf,
                              (DWORD)msg->msg_iovlen,
                              &bytes_sent,
                              flags,
                              static_cast<const sockaddr *>(msg->msg_name),
                              msg->msg_namelen,
                              0,
                              0);

    if (result != 0)
    {
        errno = ::WSAGetLastError ();
        return -1;
    }

    return (ssize_t) bytes_sent;

#elif defined (ZCE_OS_LINUX)
    //
    return ::sendmsg (handle, msg, flags);
# endif

}

int ZCE_LIB::socket_init (int version_high, int version_low)
{
#if defined (ZCE_OS_WINDOWS)

    WORD version_requested = MAKEWORD (version_high, version_low);
    WSADATA wsa_data;
    ::WSASetLastError(0);
    int error = ::WSAStartup (version_requested, &wsa_data);

    if (error != 0)
    {
        errno = ::WSAGetLastError ();
        ::fprintf (stderr,
                   "ZCE_LIB::socket_init; WSAStartup failed, "
                   "WSAGetLastError returned %d\n",
                   errno);
    }

# else
    ZCE_UNUSED_ARG (version_high);
    ZCE_UNUSED_ARG (version_low);
# endif
    return 0;
}

int ZCE_LIB::socket_fini (void)
{
#if defined (ZCE_OS_WINDOWS)

    if (WSACleanup () != 0)
    {
        errno = ::WSAGetLastError ();

        ::fprintf (stderr,
                   "ZCE_LIB::socket_fini; WSACleanup failed, "
                   "WSAGetLastError returned %d\n",
                   errno);
    }

# endif
    return 0;
}

//--------------------------------------------------------------------------------------------
//������ȡlen�����ݣ�ֱ�����ִ���
ssize_t ZCE_LIB::recvn (ZCE_SOCKET handle,
                       void *buf,
                       size_t len,
                       int flags )
{
    ssize_t result = 0;
    bool error_occur = false;

    ssize_t onetime_recv = 0, bytes_recv = 0;

    //һ��׼��ȷ���յ���ô���ֽ�
    for (bytes_recv = 0; static_cast<size_t>(bytes_recv) < len; bytes_recv += onetime_recv)
    {

        //ʹ�ö˿ڽ��н���
        onetime_recv = ZCE_LIB::recv (handle,
                                     static_cast <char *> (buf) + bytes_recv,
                                     len - bytes_recv,
                                     flags);

        if (onetime_recv > 0)
        {
            continue;
        }
        //������ִ���,�˳�ѭ��
        else
        {
            //���ִ��󣬽��д���
            error_occur = true;
            result = onetime_recv;
            break;
        }
    }

    //�����������
    if (error_occur)
    {
        return result;
    }

    return bytes_recv;
}

//��������N�����ݣ�ֱ�����ִ���
ssize_t ZCE_LIB::sendn (ZCE_SOCKET handle,
                       const void *buf,
                       size_t len,
                       int flags)
{
    bool error_occur = false;
    ssize_t result = 0, bytes_send = 0, onetime_send = 0;

    //һ��׼��ȷ���յ���ô���ֽڣ�����һ�����ִ��󣬾��˳�
    for (bytes_send = 0; static_cast<size_t>(bytes_send) < len; bytes_send += onetime_send)
    {
        //�������ݣ���
        onetime_send = ZCE_LIB::send (handle,
                                     static_cast <const char *> (buf) + bytes_send,
                                     len - bytes_send,
                                     flags);

        if (onetime_send > 0)
        {
            continue;
        }
        //������ִ���,== 0һ�����Ƕ˿ڶϿ���==-1��ʾ����
        else
        {
            //���ִ��󣬽��д���
            error_occur = true;
            result = onetime_send;
            break;
        }
    }

    //�����˴��󣬷��ش��󷵻�ֵ
    if (error_occur)
    {
        return result;
    }

    return bytes_send;
}

//��ĳЩѡ�WIN32Ŀǰֻ֧��O_NONBLOCK
int ZCE_LIB::sock_enable (ZCE_SOCKET handle, int flags)
{

#if defined (ZCE_OS_WINDOWS)

    switch (flags)
    {
        case O_NONBLOCK:
            // nonblocking argument (1)
            // blocking:            (0)
        {
            u_long nonblock = 1;
            int zce_result = ::ioctlsocket (handle, FIONBIO, &nonblock);

            //��������Ϣ���õ�errno����ϸ��ο�����ZCE_LIB���ֿռ����Ľ���
            if ( SOCKET_ERROR == zce_result)
            {
                errno = ::WSAGetLastError ();
            }

            return zce_result;
        }

        default:
        {
            return (-1);
        }

    }

#elif defined (ZCE_OS_LINUX)
    int val = ::fcntl (handle, F_GETFL, 0);

    if (val == -1)
    {
        return -1;
    }

    // Turn on flags.
    ZCE_SET_BITS (val, flags);

    if (::fcntl (handle, F_SETFL, val) == -1)
    {
        return -1;
    }

    return 0;
#endif
}

//�ر�ĳЩѡ�WIN32Ŀǰֻ֧��O_NONBLOCK
int ZCE_LIB::sock_disable(ZCE_SOCKET handle, int flags)
{
#if defined (ZCE_OS_WINDOWS)

    switch (flags)
    {
        case O_NONBLOCK:
            // nonblocking argument (1)
            // blocking:            (0)
        {
            u_long nonblock = 0;
            int zce_result =  ::ioctlsocket (handle, FIONBIO, &nonblock);

            //��������Ϣ���õ�errno����ϸ��ο�����ZCE_LIB���ֿռ����Ľ���
            if ( SOCKET_ERROR == zce_result)
            {
                errno = ::WSAGetLastError ();
            }

            return zce_result;
        }

        default:
            return (-1);
    }

#elif defined (ZCE_OS_LINUX)
    int val = ::fcntl (handle, F_GETFL, 0);

    if (val == -1)
    {
        return -1;
    }

    // Turn on flags.
    ZCE_CLR_BITS (val, flags);

    if (::fcntl (handle, F_SETFL, val) == -1)
    {
        return -1;
    }

    return 0;
#endif
}



//����ڣ�һ��ʱ���ڣ���ĳ��SOCKET�����ע�ĵ����¼��Ƿ񴥷���������������ش����¼�����������ɹ���һ�㴥������ֵ����1
int ZCE_LIB::handle_ready(ZCE_SOCKET handle,
                         ZCE_Time_Value *timeout_tv,
                         HANDLE_READY_TODO ready_todo)
{
    fd_set handle_set_read, handle_set_write, handle_set_exeception;
    fd_set *p_set_read  = NULL, *p_set_write = NULL, *p_set_exception = NULL;
    FD_ZERO(&handle_set_read);
    FD_ZERO(&handle_set_write);
    FD_ZERO(&handle_set_exeception);


    //FD_SET ����� whlie(0)�����һ���澯�����Ӧ����windows�ڲ��Լ�û�д���á�΢��˵VS2005SP1���޸��ˣ�����
#if defined (ZCE_OS_WINDOWS)
#pragma warning(disable : 4127)
#endif

    if (HANDLE_READY_READ == ready_todo)
    {
        FD_SET(handle, &handle_set_read);
        p_set_read = &handle_set_read;
    }
    else if ( HANDLE_READY_WRITE == ready_todo)
    {
        FD_SET(handle, &handle_set_write);
        p_set_write = &handle_set_write;
    }
    else if (HANDLE_READY_EXCEPTION == ready_todo)
    {
        FD_SET(handle, &handle_set_exeception);
        p_set_exception = &handle_set_exeception;
    }
    else if (HANDLE_READY_ACCEPT == ready_todo)
    {
        //accept�¼������ö�ȡ�¼�
        FD_SET(handle, &handle_set_read);
        p_set_read = &handle_set_read;
    }
    else if (HANDLE_READY_CONNECTED == ready_todo)
    {
        //Ϊʲôǰ��д����ô�鷳����ʵ������Ϊ���CONNECTED�ĵ�ù����
        //���ȣ�CONNECT�Ĵ���Ҫ���ֳɹ���ʧ���¼�
        //Windows ������CONNECT, ʧ�ܵ����쳣���ɹ�����д�¼�
        //Windows ����CONNECT, ʧ�ܵ��ö�д�¼����ɹ�����д�¼�
        //LINUX �������������Ƿ�������ʧ�ܵ��ö�д�¼����ɹ�����д�¼�
        //���ԡ���������û�ио�������������
        FD_SET(handle, &handle_set_read);
        p_set_read = &handle_set_read;
        FD_SET(handle, &handle_set_write);
        p_set_write = &handle_set_write;
        FD_SET(handle, &handle_set_exeception);
        p_set_exception = &handle_set_exeception;
    }
    else
    {
        ZCE_ASSERT(false);
    }

#if defined (ZCE_OS_WINDOWS)
#pragma warning(default : 4127)
#endif

    // Wait for data or for the timeout_tv to elapse.
    int select_width = 0;

#if defined (ZCE_OS_WINDOWS)
    //�������0Windows��VC++���и澯
    select_width = 0;
#elif defined (ZCE_OS_LINUX)
    select_width = int (handle) + 1;
#endif

    int result = ZCE_LIB::select (select_width,
                                 p_set_read,
                                 p_set_write,
                                 p_set_exception,
                                 timeout_tv);

    if (0 == result )
    {
        errno = ETIMEDOUT;
        return 0;
    }

    //���ִ���
    if (0 > result )
    {
        return result;
    }

    //���Ǵ������CONNECTED�ɹ���
    if (HANDLE_READY_CONNECTED == ready_todo)
    {
        //�����CONNECTED�������ػ����쳣���ض�����Ϊ�Ǵ���
        if (FD_ISSET(handle, p_set_read)
            || FD_ISSET(handle, p_set_exception))
        {
            return -1;
        }
    }

    return result;
}

//--------------------------------------------------------------------------------------------
//��ΪWINdows ��֧��ȡ��socket �Ƿ���������ģʽ������Windows �����޷���ȡ��socket��ѡ�Ȼ���ж��Ƿ�ȡ������ģʽ
//����������ر�֤���Socket ������ģʽ�ģ�����������


int ZCE_LIB::connect_timeout(ZCE_SOCKET handle,
                            const sockaddr *addr,
                            socklen_t addrlen,
                            ZCE_Time_Value &timeout_tv)
{

    int ret = 0;
    //���ܶԷ������ľ�����г�ʱ����
    ret = ZCE_LIB::sock_enable(handle, O_NONBLOCK);
    if (ret != 0)
    {
        ZCE_LIB::closesocket(handle);
        return -1;
    }

    ret = ZCE_LIB::connect(handle, addr, addrlen);
    //
    if (ret != 0)
    {
        //WINDOWS�·���EWOULDBLOCK��LINUX�·���EINPROGRESS
        int last_err = ZCE_LIB::last_error();

        if (EINPROGRESS != last_err &&  EWOULDBLOCK != last_err)
        {
            ZCE_LIB::closesocket(handle);
            return ret;
        }
    }

    //���г�ʱ����
    ret = ZCE_LIB::handle_ready(handle,
                               &timeout_tv,
                               ZCE_LIB::HANDLE_READY_CONNECTED);

    const int HANDLE_READY_ONE = 1;

    if (ret != HANDLE_READY_ONE)
    {
        ZCE_LIB::closesocket(handle);
        return -1;
    }

    ret = ZCE_LIB::sock_disable(handle, O_NONBLOCK);
    if (ret != 0)
    {
        ZCE_LIB::closesocket(handle);
        return -1;
    }

    return 0;
}


ssize_t ZCE_LIB::recvn_timeout (ZCE_SOCKET handle,
                               void *buf,
                               size_t len,
                               ZCE_Time_Value &timeout_tv,
                               int flags)
{

    ssize_t result = 0;
    bool error_occur = false;

    //���ֻ�ȴ�����ʱ��
#if defined  (ZCE_OS_WINDOWS)

    //WIN32��ֻ�ܼ򵥵Ĵ򿪷�������
    ZCE_LIB::sock_enable (handle, O_NONBLOCK);

#elif defined  (ZCE_OS_LINUX)
    //Linux�򵥺ֻܶ࣬��Ҫ����һ�η�����һЩ�������޶���OK��
    //MSG_DONTWAITѡ�ߣ�WIN32��֧��
    flags |= MSG_DONTWAIT;
#endif

    int ret = 0;
    ssize_t onetime_recv = 0, bytes_recv = 0;

    //һ��׼��ȷ���յ���ô���ֽ�
    for (bytes_recv = 0; static_cast<size_t>(bytes_recv) < len; bytes_recv += onetime_recv)
    {

        //�ȴ��˿�׼�����˴��������¼��������ʵ���Ͻ������ۣ����timeout_tv Ӧ�ü�ȥ���ĵ�ʱ��
        //LINUX��SELECT����������飬��WINDOWS�Ĳ���
        ret = ZCE_LIB::handle_ready (handle,
                                    &timeout_tv,
                                    HANDLE_READY_READ);

        const int HANDLE_READY_ONE = 1;

        if ( ret != HANDLE_READY_ONE )
        {
            //
            if ( 0 == ret)
            {
                errno = ETIMEDOUT;
                result = -1;
            }

            error_occur = true;
            result = ret;
            break;
        }

        //ʹ�÷������˿ڽ��н���
        onetime_recv = ZCE_LIB::recv (handle,
                                     static_cast <char *> (buf) + bytes_recv,
                                     len - bytes_recv,
                                     flags);

        if (onetime_recv > 0)
        {
            continue;
        }
        //������ִ���,== 0һ�����Ƕ˿ڶϿ���==-1��ʾ
        else
        {
            //==-1�����Ǳ�ʾ�������󣬽���ѭ������
            if (onetime_recv < 0 && errno == EWOULDBLOCK)
            {
                // Did select() succeed?
                onetime_recv = 0;
                continue;
            }

            //���ִ��󣬽��д���
            error_occur = true;
            result = onetime_recv;
            break;
        }
    }

    //���ֻ�ȴ�����ʱ�䣬�ָ�ԭ������ģʽ
#if defined  (ZCE_OS_WINDOWS)
    ZCE_LIB::sock_disable (handle, O_NONBLOCK);
#endif

    if (error_occur)
    {
        return result;
    }

    return bytes_recv;
}

//���������WIN32������֤���Socket ������ģʽ�ģ�����������
ssize_t ZCE_LIB::sendn_timeout(ZCE_SOCKET handle,
                              const void *buf,
                              size_t len,
                              ZCE_Time_Value &timeout_tv,
                              int flags )
{

    bool error_occur = false;
    ssize_t result = 0, bytes_send = 0, onetime_send = 0;

    //���ֻ�ȴ�����ʱ��
#if defined  (ZCE_OS_WINDOWS)

    //�ȴ�һ��ʱ��Ҫ�������⴦��
    //WIN32��ֻ�ܼ򵥵Ĵ򿪷�������
    ZCE_LIB::sock_enable (handle, O_NONBLOCK);

#elif defined  (ZCE_OS_LINUX)
    //Linux�򵥺ֻܶ࣬��Ҫ����һ�η�����һЩ�������޶���OK��
    //MSG_DONTWAIT��־��WIN32��֧��
    flags |= MSG_DONTWAIT;
#endif

    int ret = 0;

    //һ��׼��ȷ���յ���ô���ֽ�
    for (bytes_send = 0; static_cast<size_t>(bytes_send) < len; bytes_send += onetime_send)
    {
        //�����ڴ��������Ϻ�recv��һ������Ϊsend��������Ҫ�ȴ���������select�˷�
        //ʹ�÷������˿ڽ��н��գ�
        onetime_send = ZCE_LIB::send (handle,
                                     static_cast <const char *> (buf) + bytes_send,
                                     len - bytes_send,
                                     flags);

        if (onetime_send > 0)
        {
            continue;
        }
        //������ִ���,== 0һ�����Ƕ˿ڶϿ���==-1��ʾ����
        else
        {
            //==-1�����Ǳ�ʾ�������󣬽���ѭ������
            if ( onetime_send < 0 && errno == EWOULDBLOCK)
            {
                //׼������select
                onetime_send = 0;

                //�ȴ��˿�׼�����˴������¼��������ʵ���Ͻ������timeout_tv Ӧ�ü�ȥ���ĵ�ʱ��
                ret = ZCE_LIB::handle_ready (handle,
                                            &timeout_tv,
                                            HANDLE_READY_WRITE);

                const int HANDLE_READY_ONE = 1;

                if (ret == HANDLE_READY_ONE)
                {
                    continue;
                }
                else
                {
                    //
                    if ( 0 == ret)
                    {
                        errno = ETIMEDOUT;
                        result = -1;
                    }

                    error_occur = true;
                    result = ret;
                    break;
                }
            }

            //���ִ��󣬽��д���
            error_occur = true;
            result = onetime_send;
            break;
        }
    }

#if defined  ZCE_OS_WINDOWS
    //�رշ�����״̬
    ZCE_LIB::sock_disable (handle, O_NONBLOCK);
#endif

    //�����˴��󣬷��ش��󷵻�ֵ
    if (error_occur)
    {
        return result;
    }

    return bytes_send;
}

//��UDP������,Ҳ���г�ʱ�����������յ��������ݾ��Ƕ����ˣ���ʱ��selectʵ��
ssize_t ZCE_LIB::recvfrom_timeout (ZCE_SOCKET handle,
                                  void *buf,
                                  size_t len,
                                  sockaddr *from,
                                  socklen_t *from_len,
                                  ZCE_Time_Value &timeout_tv,
                                  int flags)
{
    //���ֻ�ȴ�����ʱ��
#if defined  (ZCE_OS_WINDOWS)

    //�ȴ�һ��ʱ��Ҫ�������⴦��
    //WIN32��ֻ�ܼ򵥵Ĵ򿪷�������
    ZCE_LIB::sock_enable (handle, O_NONBLOCK);

#elif defined  (ZCE_OS_LINUX)
    //Linux�򵥺ֻܶ࣬��Ҫ����һ�η�����һЩ�������޶���OK��
    //MSG_DONTWAIT��־��WIN32��֧��
    flags |= MSG_DONTWAIT;
#endif

    ssize_t recv_result = 0;
    int ret = ZCE_LIB::handle_ready (handle,
                                    &timeout_tv,
                                    HANDLE_READY_READ);

    const int HANDLE_READY_ONE = 1;

    if (ret == HANDLE_READY_ONE)
    {
        //ʹ�÷������˿ڽ��н���
        recv_result = ZCE_LIB::recvfrom (handle,
                                        static_cast <char *> (buf) ,
                                        len,
                                        flags,
                                        from,
                                        from_len);
    }
    else
    {
        //
        if (ret == 0)
        {
            errno = ETIMEDOUT;
        }

        recv_result = -1;
    }

    //���ֻ�ȴ�����ʱ�䣬�ָ�ԭ�е�״̬
#if defined  ZCE_OS_WINDOWS
    ZCE_LIB::sock_disable (handle, O_NONBLOCK);
#endif

    return recv_result;
}

//UDP�ķ�����ʱ�ǲ��������ģ����ó�ʱ����д���������ȫ��Ϊ�˺�ǰ�����
//����UDP������,����ʱ�������������ʵ���Ͻ���û�г�ʱ����
ssize_t ZCE_LIB::sendto_timeout (ZCE_SOCKET handle,
                                const void *buf,
                                size_t len,
                                const sockaddr *addr,
                                int addrlen,
                                ZCE_Time_Value & /*timeout_tv*/,
                                int flags)
{
    return ZCE_LIB::sendto(handle,
                          buf,
                          len,
                          flags,
                          addr,
                          addrlen
                         );
}

//--------------------------------------------------------------------------------------------
//���麯���ṩ����Ϊ�˴�����ԣ���ʱ�������ṩ
//ʹ��SO_RCVTIMEO��SO_SNDTIMEO�õ�һ�鳬ʱ������

//ע��SO_RCVTIMEO,SO_SNDTIMEO,ֻ��WIN socket 2��֧��
ssize_t ZCE_LIB::recvn_timeout2 (ZCE_SOCKET handle,
                                void *buf,
                                size_t len,
                                ZCE_Time_Value &timeout_tv,
                                int flags)
{
    int ret = 0;

    //��Ȼ������һ��������װ�����������ڲ�ʵ�ֲ�һ�����㻹��Ҫ��Ѫ��
#if defined  ZCE_OS_WINDOWS
    //��ʱ�ĺ���
    DWORD  msec_timeout = static_cast<DWORD>(timeout_tv.total_msec());
    ret = ZCE_LIB::setsockopt(handle, SOL_SOCKET, SO_RCVTIMEO, (const void *)(&msec_timeout), sizeof(DWORD));

#elif defined  ZCE_OS_LINUX
    timeval sockopt_tv = timeout_tv;
    ret = ZCE_LIB::setsockopt(handle, SOL_SOCKET, SO_RCVTIMEO, (const void *)(&sockopt_tv), sizeof(timeval));
#endif

    if (0 != ret )
    {
        return -1;
    }

    ssize_t result = 0, bytes_recv = 0, onetime_recv = 0;
    bool error_occur = false;

    //һ��Ҫ�յ�len���ȵ��ֽ�
    for (bytes_recv = 0; static_cast<size_t>(bytes_recv) < len; bytes_recv += onetime_recv)
    {
        //ʹ�÷������˿ڽ��н���
        onetime_recv = ZCE_LIB::recv (handle,
                                     static_cast <char *> (buf) + bytes_recv,
                                     len - bytes_recv,
                                     flags);

        if (onetime_recv > 0)
        {
            continue;
        }
        //������ִ���,== 0һ�����Ƕ˿ڶϿ���==-1��ʾ
        else
        {
            //���ִ��󣬽��д���
            error_occur = true;
            result = onetime_recv;
            break;
        }
    }

    //
    if (error_occur)
    {
        return result;
    }

    //Ҫ��Ҫ��ԭԭ����SO_RCVTIMEO?���ˣ���������ʱ���õط�Ӧ�û�һֱʹ��

    return bytes_recv;
}

//
ssize_t ZCE_LIB::sendn_timeout2 (ZCE_SOCKET handle,
                                void *buf,
                                size_t len,
                                ZCE_Time_Value &timeout_tv,
                                int flags)
{

    int ret = 0;

    //��Ȼ������һ��������װ�����������ڲ�ʵ�ֲ�һ�����㻹��Ҫ��Ѫ��
#if defined  ZCE_OS_WINDOWS

    DWORD  msec_timeout = static_cast<DWORD>(timeout_tv.total_msec());
    ret = ZCE_LIB::setsockopt(handle, SOL_SOCKET, SO_SNDTIMEO, (const void *)(&msec_timeout), sizeof(DWORD));

#elif defined  ZCE_OS_LINUX
    timeval sockopt_tv = timeout_tv;
    ret = ZCE_LIB::setsockopt(handle, SOL_SOCKET, SO_SNDTIMEO, (const void *)(&sockopt_tv), sizeof(timeval));

#endif

    if (0 != ret )
    {
        return -1;
    }

    ssize_t result = 0, bytes_send = 0, onetime_send = 0;
    bool error_occur = false;

    //һ��Ҫ���͵�len���ȵ��ֽ�
    for (bytes_send = 0; static_cast<size_t>(bytes_send) < len; bytes_send += onetime_send)
    {
        //ʹ�÷������˿ڽ��н���
        onetime_send = ZCE_LIB::send (handle,
                                     static_cast <char *> (buf) + bytes_send,
                                     len - bytes_send,
                                     flags);

        //��ʵ���Ӧ�õ�����ʱʱ�䣬�Ǻǣ�͵����
        if (onetime_send > 0)
        {
            continue;
        }
        //������ִ���,== 0һ�����Ƕ˿ڶϿ���==-1��ʾ
        else
        {
            //���ִ��󣬽��д���
            error_occur = true;
            result = onetime_send;
            break;
        }
    }

    //
    if (error_occur)
    {
        return result;
    }

    return bytes_send;
}

//��UDP������,Ҳ���г�ʱ�����������յ��������ݾ��Ƕ����ˣ���ʱ��SO_RCVTIMEOʵ��
ssize_t ZCE_LIB::recvfrom_timeout2 (ZCE_SOCKET handle,
                                   void *buf,
                                   size_t len,
                                   sockaddr *addr,
                                   socklen_t *addrlen,
                                   ZCE_Time_Value &timeout_tv,
                                   int flags)
{
    int ret = 0;
    //��Ȼ������һ��������װ�����������ڲ�ʵ�ֲ�һ�����㻹��Ҫ��Ѫ��
#if defined (ZCE_OS_WINDOWS)

    DWORD  msec_timeout = static_cast<DWORD>(timeout_tv.total_msec());
    ret = ZCE_LIB::setsockopt(handle, SOL_SOCKET, SO_RCVTIMEO, (const void *)(&msec_timeout), sizeof(DWORD));

#elif defined (ZCE_OS_LINUX)
    timeval sockopt_tv = timeout_tv;
    ret = ZCE_LIB::setsockopt(handle, SOL_SOCKET, SO_RCVTIMEO, (const void *)(&sockopt_tv), sizeof(timeval));
#endif

    //����socket���ƺ����ķ�װ������-1��ʶʧ�ܡ�
    if (0 != ret)
    {
        return -1;
    }

    //ʹ�÷������˿ڽ��н���
    ssize_t recv_result = ZCE_LIB::recvfrom (handle,
                                            static_cast <char *> (buf) ,
                                            len,
                                            flags,
                                            addr,
                                            addrlen);

    return recv_result;
}

//UDP�ķ�����ʱ�ǲ��������ģ����ó�ʱ����д���������ȫ��Ϊ�˺�ǰ�����
//����UDP������,����ʱ�������������ʵ���Ͻ���û�г�ʱ����
ssize_t ZCE_LIB::sendto_timeout2 (ZCE_SOCKET handle,
                                 const void *buf,
                                 size_t len,
                                 const sockaddr *addr,
                                 int addrlen,
                                 ZCE_Time_Value & /*timeout_tv*/,
                                 int flags)
{
    return ZCE_LIB::sendto(handle,
                          buf,
                          len,
                          flags,
                          addr,
                          addrlen
                         );
}

//--------------------------------------------------------------------------------------------

//ת���ַ����������ַ����һ������af�ǵ�ַ�壬ת�������dst�У�
//ע���������return 1��ʶ�ɹ���return ������ʶ����return 0��ʶ��ʽƥ�����
int ZCE_LIB::inet_pton (int family,
                       const char *strptr,
                       void *addrptr)
{
#if defined (ZCE_OS_WINDOWS)

    //Ϊʲô��������inet_pton ,(Vista��֧��),��������ע�͵�ԭ���ǣ������ͨ���ˣ�����Ҳû����,XP��WINSERVER2003���޷�ʹ�ã�
    //VISTA,WINSERVER2008��_WIN32_WINNT����0x0600
#if defined ZCE_SUPPORT_WINSVR2008
    return ::inet_pton(family,strptr,addrptr);
#else

    //sscanfȡ�õ���ĸ���
    int get_fields_num = 0;

    if (  AF_INET == family )
    {
        struct in_addr *in_val = reinterpret_cast<in_addr *>(addrptr);

        //Ϊʲô��ֱ����in_val->S_un.S_un_b.s_b1�����
        const int NUM_FIELDS_AF_INET = 4;
        uint32_t u[NUM_FIELDS_AF_INET] = {0};
        get_fields_num = sscanf(strptr,
                                "%u%.%u%.%u%.%u",
                                &(u[0]),
                                &(u[1]),
                                &(u[2]),
                                &(u[3])
                               );

        //������ַ������Ϻ���׼

        if ( NUM_FIELDS_AF_INET != get_fields_num || u[0] > 0xFF || u[1] > 0xFF || u[2] > 0xFF || u[3] > 0xFF )
        {
            return 0;
        }

        uint32_t u32_addr = u[0] << 24 | u[1] << 16 | u[2] << 8 | u[3];
        in_val->S_un.S_addr = htonl(u32_addr);


        //ע�⣬����1��ʶ�ɹ�
        return (1);
    }
    else if ( AF_INET6 == family )
    {
        //fucnking ����RFC1884�ĸ��ǣ���Ϳ��Ǽ򻯣�Ҳ����һ��д���������ν���ת���ѡ�����һֻ������
        //RFC1884 �Բ����Ҳ�֧��IPV6�����Ը�ʽ��IPV4ӳ���ʽ���Ǹ�̫̫�鷳�ˡ�
        //IPV6��0���Ը�ʽ���� '::'�ڿ�ͷ��::FFFF:A:B  '::'���м�� A:B:::C(�㲻֪���м���0��ʡ����)��'::'��β���ϵ�
        //IPV4ӳ���IPV6�ĸ�ʽ���Ա�ʶ��::FFFF:A.B.C.D

        //������ַ������Ϻ���׼
        const int NUM_FIELDS_AF_INET6 = 8;

        const char INET6_STR_UP_CHAR[] = {"1234567890ABCDEF"};

        //����0
        memset(addrptr, 0, sizeof(in_addr6));
        struct in_addr6 *in_val6 = reinterpret_cast<in_addr6 *>(addrptr);

        size_t in_str_len = strlen(strptr);
        //ǰһ���ַ��Ƿ���ð��
        bool pre_char_colon = false;
        //�ַ����Ƿ�
        bool str_abbreviate = false;
        //
        int havedot_ipv4_mapped = 0;

        //
        size_t word_start_pos = 0;

        uint16_t for_word[NUM_FIELDS_AF_INET6] = {0};
        uint16_t back_word[NUM_FIELDS_AF_INET6] = {0};

        size_t forword_num = 0, backword_num = 0;

        for (size_t i = 0; i <= in_str_len; ++i)
        {
            //
            if (':' == strptr [i] )
            {
                //���������ַ�Ҳ�ǣ�����ʶ����д��ʽ
                if ( pre_char_colon == true  )
                {
                    //���û�з�������д
                    if (false == str_abbreviate)
                    {
                        str_abbreviate = true;
                    }
                    //�����ܷ���������д��
                    else
                    {
                        return 0;
                    }

                    continue;
                }

                //.���ֺ󣬲����ܳ��֣�����ʽ����
                if (havedot_ipv4_mapped > 0)
                {
                    return 0;
                }

                //�����ܳ���8��ð��
                if (backword_num + forword_num >= NUM_FIELDS_AF_INET6)
                {
                    return 0;
                }

                //����Ѿ�����д����ô��¼���������ݶ�����
                if (str_abbreviate)
                {
                    get_fields_num = sscanf(strptr + word_start_pos, "%hx:",  &(back_word[backword_num]));
                    ++backword_num;
                }
                else
                {
                    get_fields_num = sscanf(strptr + word_start_pos, "%hx:",  &(for_word[forword_num]));
                    ++forword_num;
                }

                pre_char_colon = true;
                continue;
            }
            //IPV4ӳ��IPV6��д��
            else if ( '.' == strptr [i] )
            {
                //���ǰ����:,����
                if (pre_char_colon)
                {
                    return 0;
                }

                ++havedot_ipv4_mapped;
            }
            else
            {
                //���������ַ�����Ϊ���󣬹�����
                if ( NULL == strchr(INET6_STR_UP_CHAR, toupper(strptr [i])) )
                {
                    return 0;
                }

                //���ǰ��һ����:
                if (pre_char_colon)
                {
                    pre_char_colon = false;
                    word_start_pos = i;
                }

                continue;
            }
        }

        //�����һ��WORD����2��WORD���д���

        //������.�����ҳ�������3�Σ�havedot_ipv4_mapped ��
        if (havedot_ipv4_mapped > 0)
        {
            const int NUM_FIELDS_AF_INET = 4;
            uint32_t u[NUM_FIELDS_AF_INET] = {0};
            get_fields_num = sscanf(strptr + word_start_pos,
                                    "%u%.%u%.%u%.%u",
                                    &(u[0]),
                                    &(u[1]),
                                    &(u[2]),
                                    &(u[3])
                                   );

            //������ַ������Ϻ���׼
            if ( NUM_FIELDS_AF_INET != get_fields_num || u[0] > 0xFF || u[1] > 0xFF || u[2] > 0xFF || u[3] > 0xFF )
            {
                return 0;
            }

            back_word[backword_num] = static_cast<uint16_t>( u[0] << 8 | u[1]);
            ++backword_num;
            back_word[backword_num] = static_cast<uint16_t>( u[2] << 8 | u[3]);
            ++backword_num;
        }
        else
        {
            if (false == pre_char_colon)
            {
                sscanf(strptr + word_start_pos , "%hx",  &(back_word[backword_num]));
                ++backword_num;
            }
        }

        //��ÿһ��WORD���и�ֵ��ǰ��ֵǰ��ģ��ٸ�ֵ�����,�м�������ʡ�Ծ���0�ˡ�������
        //���ת��ֻ����WINDOWS���ã���Щunionֻ��WINDOWS���ж��壩�����Ҫͨ�ã�Ҫ�Ĵ��롣
        for (size_t k = 0; k < forword_num; ++k)
        {
            in_val6->u.Word[k] = htons( for_word[k]);
        }

        for (size_t k = 0; k < backword_num; ++k)
        {
            in_val6->u.Word[NUM_FIELDS_AF_INET6 - backword_num + k] = htons( back_word[k]);
        }

        //����1��ʶ�ɹ�
        return (1);
    }
    //��֧��
    else
    {
        errno = EAFNOSUPPORT;
        return 0;
    }
#endif

#elif defined (ZCE_OS_LINUX)
    //LINuX�����������
    return ::inet_pton(family, strptr, addrptr);
#endif
}

//����ԭ������[�������ʮ���ơ� ��> ��������],IPV6����:�ָ�16����ת����128λ����
const char *ZCE_LIB::inet_ntop(int family,
                               const void *addrptr,
                               char *strptr,
                               size_t len)
{

#if defined (ZCE_OS_WINDOWS)

    //���ݲ�ͬ��Э��ؽ��в�ͬ�Ĵ���
    if (  AF_INET == family )
    {
        const struct in_addr *in_val = reinterpret_cast<const in_addr *>(addrptr);
        int ret_len = snprintf(strptr,
                               len,
                               "%u.%u.%u.%u",
                               in_val->S_un.S_un_b.s_b1,
                               in_val->S_un.S_un_b.s_b2,
                               in_val->S_un.S_un_b.s_b3,
                               in_val->S_un.S_un_b.s_b4);

        //��ʽ���ַ���ʧ��
        if (ret_len > static_cast<int>(len) || ret_len <= 0 )
        {
            errno = ENOSPC;
            return NULL;
        }

        return strptr;
    }
    else if ( AF_INET6 == family )
    {
        //�Բ�����ֻ֧��ת����IPV6�ı�׼��ʽ����֧��ת����IPV6�����Ը�ʽ��IPV4ӳ���ʽ��

        const struct in_addr6 *in_val6 = reinterpret_cast<const in_addr6 *>(addrptr);

        const int NUM_FIELDS_AF_INET6 = 8;
        uint16_t u[NUM_FIELDS_AF_INET6];

        //��Ϊ��short������Ҫת���ɱ�������
        u[0] = ::ntohs(in_val6->u.Word[0]);
        u[1] = ::ntohs(in_val6->u.Word[1]);
        u[2] = ::ntohs(in_val6->u.Word[2]);
        u[3] = ::ntohs(in_val6->u.Word[3]);
        u[4] = ::ntohs(in_val6->u.Word[4]);
        u[5] = ::ntohs(in_val6->u.Word[5]);
        u[6] = ::ntohs(in_val6->u.Word[6]);
        u[7] = ::ntohs(in_val6->u.Word[7]);

        int ret_len = snprintf(strptr,
                               len,
                               "%hx:%hx:%hx:%hx:%hx:%hx:%hx:%hx",
                               u[0],
                               u[1],
                               u[2],
                               u[3],
                               u[4],
                               u[5],
                               u[6],
                               u[7]
                              );

        if (ret_len > static_cast<int>(len) || ret_len <= 0 )
        {
            errno = ENOSPC;
            return NULL;
        }

        return strptr;
    }
    else
    {
        errno = EAFNOSUPPORT;
        return NULL;
    }

#elif defined (ZCE_OS_LINUX)
    //LINuX�����������
    return ::inet_ntop(family, addrptr, strptr, len);
#endif
}

//���IP��ַ��Ϣ���ڲ��ǲ�ʹ�þ�̬�������̰߳�ȫ��BUF����IPV4���ٳ���>15.IPV6���ٳ���>39
const char *ZCE_LIB::socketaddr_ntop(const sockaddr *sock_addr,
                                    char *str_ptr,
                                    size_t str_len)
{
    //���ݲ�ͬ�ĵ�ַЭ���壬����ת��
    if (sock_addr->sa_family == AF_INET)
    {
        const sockaddr_in *sockadd_ipv4 = reinterpret_cast<const sockaddr_in *>(sock_addr);
        return ZCE_LIB::inet_ntop(AF_INET,
                                 (void *)(&(sockadd_ipv4->sin_addr)),
                                 str_ptr,
                                 str_len);
    }
    else if (sock_addr->sa_family == AF_INET6)
    {
        const sockaddr_in6 *sockadd_ipv6 = reinterpret_cast<const sockaddr_in6 *>(sock_addr);
        return ZCE_LIB::inet_ntop(AF_INET6,
                                 (void *)(&(sockadd_ipv6->sin6_addr)),
                                 str_ptr, str_len);
    }
    else
    {
        errno = EAFNOSUPPORT;
        return NULL;
    }
}

//���IP��ַ��Ϣ�Լ��˿���Ϣ���ڲ��ǲ�ʹ�þ�̬�������̰߳�ȫ��BUF����IPV4���ٳ���>21.IPV6���ٳ���>45
const char *ZCE_LIB::socketaddr_ntop_ex(const sockaddr *sock_addr,
                                       char *str_ptr,
                                       size_t str_len)
{
    uint16_t addr_port = 0;
    const char *ret_str = NULL;

    //���ݲ�ͬ�ĵ�ַЭ���壬����ת������ʹ�������Ǹ�������ԭ������Ϊ����ͬʱҪ���ж�ȡport�Ĳ���
    if (sock_addr->sa_family == AF_INET)
    {
        const sockaddr_in *sockadd_ipv4 = reinterpret_cast<const sockaddr_in *>(sock_addr);
        addr_port = ntohs(sockadd_ipv4->sin_port);
        ret_str = ZCE_LIB::inet_ntop(AF_INET,
                                    (void *)(&(sockadd_ipv4->sin_addr)),
                                    str_ptr,
                                    str_len);
    }
    else if (sock_addr->sa_family == AF_INET6)
    {
        const sockaddr_in6 *sockadd_ipv6 = reinterpret_cast<const sockaddr_in6 *>(sock_addr);
        addr_port = ntohs(sockadd_ipv6->sin6_port);
        ret_str = ZCE_LIB::inet_ntop(AF_INET6,
                                    (void *)(&(sockadd_ipv6->sin6_addr)),
                                    str_ptr, str_len);
    }
    else
    {
        errno = EAFNOSUPPORT;
        return NULL;
    }

    //������ش���
    if ( NULL == ret_str )
    {
        return NULL;
    }

    size_t add_str_len = strlen(str_ptr);

    //5�����֣�һ�����ӷ���һ�����ַ���־
    const size_t PORT_LEN = 7;

    if (str_len < add_str_len + PORT_LEN)
    {
        return NULL;
    }

    //ǰ���Ѿ������ˣ�������жϷ�����
    snprintf(str_ptr + add_str_len, str_len - add_str_len, "#%u", addr_port);

    return str_ptr;
}

/*
���Ƕ���ҵ����IP����һ����RFC1918�ж���ķ�Internet���ӵ������ַ��
Ҳ��Ϊ˽�е�ַ����Internet��ַ��Ȩ������IANA�����Ƶ�IP��ַ���䷽���У�
���������������ַ����������Internet�ϵ�ר����ʹ�á����ֱ��ǣ�
A�ࣺ10.0.0.0 ~ 10.255.255.255��
B�ࣺ172.16.0.0 ~ 172.31.255.255��
C�ࣺ192.168.0.0 ~ 192.168.255.255��
���е�һ��˽�е�ַ�����ǣ�192.168.0.0������������IP��������õ����Ρ�
IANA��֤��Щ����Ų�����������Internet�ϵ��κ����磬
����κ��˶��������ɵ�ѡ����Щ�����ַ��Ϊ�Լ���˽�������ַ��
������ĺϷ�IP���������£���ҵ���������Բ���˽��IP��ַ�������ַ���䷽����
��ҵ���������롢DMZ��ʹ�úϷ�IP��ַ��
�����ȫ0��Ҳ���Լ��ڲ�ip <==== ������Ǹ�ͬѧ�ӵģ������ĳ�̶ֳ����ǶԵģ���Ϊ0ֻ������ڱ������ж���
*/

#if !defined ZCE_IS_INTERNAL
#define ZCE_IS_INTERNAL(ip_addr)   ((ip_addr >= 0x0A000000 && ip_addr <= 0x0AFFFFFF ) ||  \
                                    (ip_addr >= 0xAC100000 && ip_addr <= 0xAC1FFFFF) ||  \
                                    (ip_addr >= 0xC0A80000 && ip_addr <= 0xC0A8FFFF) ||  \
                                    (ip_addr == INADDR_ANY))
#endif

//���һ����ַ�Ƿ���������ַ
bool ZCE_LIB::is_internal(const sockaddr_in *sock_addr_ipv4)
{
    uint32_t ip_addr = ZCE_LIB::get_ip_address(sock_addr_ipv4);

    //���3���ַ
    if (ZCE_IS_INTERNAL(ip_addr))
    {
        return true;
    }
    return false;
}

bool ZCE_LIB::is_internal(uint32_t ipv4_addr_val)
{
    //���3���ַ
    if (ZCE_IS_INTERNAL(ipv4_addr_val))
    {
        return true;
    }
    return false;
}



//-------------------------------------------------------------------------------------
//����������ת��IP��ַ�ļ�������

//ͨ�������õ���ص�IP��ַ
hostent *ZCE_LIB::gethostbyname(const char *hostname)
{
    return ::gethostbyname(hostname);
}

// GNU extensions
hostent *ZCE_LIB::gethostbyname2(const char *hostname,
                                int af)
{
#if defined (ZCE_OS_WINDOWS)

    hostent *hostent_ptr = ::gethostbyname(hostname);

    //
    if (hostent_ptr->h_addrtype != af)
    {
        return NULL;
    }

    return hostent_ptr;

#elif defined (ZCE_OS_LINUX)
    return ::gethostbyname2(hostname, af);
#endif
}

//�Ǳ�׼����,�õ�ĳ��������IPV4�ĵ�ַ��Ϣ������ʹ�������Ƚ����׺ͷ���
//name ����
//uint16_t service_port �˿ںţ�������
//ary_addr_num  ,������������������ʶary_sock_addr�ĸ��������ʱ��ʶ���صĶ�������
//ary_sock_addr ,������������صĵ�ַ����
int ZCE_LIB::gethostbyname_inary(const char *hostname,
                                uint16_t service_port,
                                size_t *ary_addr_num,
                                sockaddr_in ary_sock_addr[])
{
    //��ʵ�������û������
    struct hostent *hostent_ptr = ::gethostbyname(hostname);

    if (!hostent_ptr)
    {
        return -1;
    }

    if (hostent_ptr->h_addrtype != AF_INET)
    {
        errno = EINVAL;
        return -1;
    }

    //��鷵��
    ZCE_ASSERT(hostent_ptr->h_length == sizeof(in_addr));

    //ѭ���õ����е�IP��ַ��Ϣ
    size_t i = 0;
    char **addr_pptr = hostent_ptr->h_addr_list;

    for (; (i < *ary_addr_num) && (*addr_pptr != NULL); addr_pptr++, ++i)
    {
        ary_sock_addr[i].sin_family = AF_INET;
        //��������������
        memcpy(&(ary_sock_addr[i].sin_addr), addr_pptr, hostent_ptr->h_length);
        //�˿�ת����������
        ary_sock_addr[i].sin_port = htons(service_port);
    }

    //��¼����
    *ary_addr_num = i;

    return 0;
}

//�Ǳ�׼����,�õ�ĳ��������IPV6�ĵ�ַ��Ϣ������ʹ�������Ƚ����׺ͷ���
int ZCE_LIB::gethostbyname_in6ary(const char *hostname,
                                 uint16_t service_port,
                                 size_t *ary_addr6_num,
                                 sockaddr_in6 ary_sock_addr6[])
{
    //��ʵ�������û������
    struct hostent *hostent_ptr = ::gethostbyname(hostname);

    if (!hostent_ptr)
    {
        return -1;
    }

    if (hostent_ptr->h_addrtype != AF_INET6)
    {
        errno = EINVAL;
        return -1;
    }

    //��鷵�صĵ�ַʵϰ�ǲ���IPV6��
    ZCE_ASSERT(hostent_ptr->h_length == sizeof(in6_addr));

    //ѭ���õ����е�IP��ַ��Ϣ
    size_t i = 0;
    char **addr_pptr = hostent_ptr->h_addr_list;

    for (; (i < *ary_addr6_num) && (*addr_pptr != NULL); addr_pptr++, ++i)
    {
        ary_sock_addr6[i].sin6_family = AF_INET6;
        //��������������
        memcpy(&(ary_sock_addr6[i].sin6_addr), addr_pptr, hostent_ptr->h_length);
        //�˿�ת����������
        ary_sock_addr6[i].sin6_port = htons(service_port);
    }

    //��¼����
    *ary_addr6_num = i;

    return 0;
}

//���ݵ�ַ�õ������ĺ���,�Ƽ�ʹ���������getnameinfo ,
hostent *ZCE_LIB::gethostbyaddr(const void *addr,
                               socklen_t len,
                               int family)
{
    return ::gethostbyaddr((const char *)addr, len, family);
};

//�Ǳ�׼������ͨ��IPV4��ַȡ������
int ZCE_LIB::gethostbyaddr_in(const sockaddr_in *sock_addr,
                             char *host_name,
                             size_t name_len)
{
    struct hostent *hostent_ptr = ZCE_LIB::gethostbyaddr(sock_addr,
                                                        sizeof(sockaddr_in),
                                                        sock_addr->sin_family
                                                       );

    //�������ʧ��
    if (!hostent_ptr )
    {
        return -1;
    }

    strncpy(host_name, hostent_ptr->h_name, name_len);

    return 0;
}

//�Ǳ�׼������ͨ��IPV6��ַȡ������
int ZCE_LIB::gethostbyaddr_in6(const sockaddr_in6 *sock_addr6,
                              char *host_name,
                              size_t name_len)
{

    struct hostent *hostent_ptr = ZCE_LIB::gethostbyaddr(sock_addr6,
                                                        sizeof(sockaddr_in6),
                                                        sock_addr6->sin6_family
                                                       );

    //�������ʧ��
    if (!hostent_ptr )
    {
        return -1;
    }

    strncpy(host_name, hostent_ptr->h_name, name_len);

    return 0;
}

//ͨ�������õ���������ַ��Ϣ������ͬʱ�õ�IPV4����IPV6�ĵ�ַ
//hints ����˵����
//���Ҫͬʱ�õ�IPV4��IPV6�ĵ�ַ����ôhints.ai_family =  AF_UNSPEC
//ai_socktype������û�����дһ��ֵ��������ܷ���SOCK_DGRAM,SOCK_STREAM��һ����
//ai_flags ��0һ���OK��AI_CANONNAME��ʾ���ص�result�ĵ�һ���ڵ����ai_canoname������AI_PASSIVE�ڲ���hostnameΪNULLʱ����IP��ַ��Ϣ����0�������۵�һ��������
//ai_protocol����0��
int ZCE_LIB::getaddrinfo( const char *hostname,
                         const char *service,
                         const addrinfo *hints,
                         addrinfo **result )
{
    return ::getaddrinfo(hostname, service, hints, result);
}

//�ͷ�getaddrinfo�õ��Ľ��
void ZCE_LIB::freeaddrinfo(struct addrinfo *result)
{
    return ::freeaddrinfo(result);
}

//�Ǳ�׼����,�õ�ĳ��������IPV4�ĵ�ַ���飬ʹ�������Ƚ����׺ͷ���
int ZCE_LIB::getaddrinfo_inary(const char *hostname,
                              uint16_t service_port,
                              size_t *ary_addr_num,
                              sockaddr_in ary_sock_addr[])
{

    int ret = 0;
    addrinfo hints, *result = NULL;

    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_CANONNAME;
    ret = ZCE_LIB::getaddrinfo(hostname,
                              NULL,
                              &hints,
                              &result);

    if (ret != 0)
    {
        return ret;
    }

    if (!result)
    {
        errno = EINVAL;
        return -1;
    }

    //��鷵�صĵ�ַʵϰ�ǲ���IPV4��
    ZCE_ASSERT(result->ai_addrlen == sizeof(sockaddr_in));

    //ѭ���õ����е�IP��ַ��Ϣ
    size_t i = 0;
    addrinfo *prc_node = result;

    for (; (i < *ary_addr_num) && (prc_node != NULL); prc_node = prc_node->ai_next, ++i)
    {
        memcpy(&(ary_sock_addr[i]), prc_node->ai_addr, prc_node->ai_addrlen);
        //�˿�ת����������
        ary_sock_addr[i].sin_port = htons(service_port);
    }

    //��¼����
    *ary_addr_num = i;

    //�ͷſռ�
    ZCE_LIB::freeaddrinfo(result);

    return 0;
}

//�Ǳ�׼����,�õ�ĳ��������IPV6�ĵ�ַ���飬ʹ�������Ƚ����׺ͷ���
int ZCE_LIB::getaddrinfo_in6ary(const char *hostname,
                               uint16_t service_port,
                               size_t *ary_addr6_num,
                               sockaddr_in6 ary_sock_addr6[])
{
    int ret = 0;
    addrinfo hints, *result = NULL;

    memset(&hints, 0, sizeof(addrinfo));
    hints.ai_family = AF_INET6;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags = AI_CANONNAME;
    ret = ZCE_LIB::getaddrinfo(hostname,
                              NULL,
                              &hints,
                              &result);

    if (ret != 0)
    {
        return ret;
    }

    if (!result)
    {
        errno = EINVAL;
        return -1;
    }

    //��鷵�صĵ�ַʵϰ�ǲ���IPV4��
    ZCE_ASSERT(result->ai_addrlen == sizeof(sockaddr_in6));

    //ѭ���õ����е�IP��ַ��Ϣ
    size_t i = 0;
    addrinfo *prc_node = result;

    for (; (i < *ary_addr6_num) && (prc_node != NULL); prc_node = prc_node->ai_next, ++i)
    {
        memcpy(&(ary_sock_addr6[i]), prc_node->ai_addr, prc_node->ai_addrlen);
        //�˿�ת����������
        ary_sock_addr6[i].sin6_port = htons(service_port);
    }

    //��¼����
    *ary_addr6_num = i;

    //�ͷſռ�
    ZCE_LIB::freeaddrinfo(result);

    return 0;
}

//ͨ��IP��ַ��Ϣ����������.���������������뺯��
int ZCE_LIB::getnameinfo(const struct sockaddr *sa,
                        socklen_t salen,
                        char *host,
                        size_t hostlen,
                        char *serv,
                        size_t servlen,
                        int flags)
{
#if defined (ZCE_OS_WINDOWS)
    return ::getnameinfo(sa, salen, host, static_cast<DWORD>(hostlen), serv, static_cast<DWORD>(servlen), flags);
#elif defined (ZCE_OS_LINUX)
    return ::getnameinfo(sa, salen, host, hostlen, serv, servlen, flags);
#endif
}

//�Ǳ�׼������ͨ��IPV4��ַȡ������
int ZCE_LIB::getnameinfo_in(const sockaddr_in *sock_addr,
                           char *host_name,
                           size_t name_len)
{
    return ZCE_LIB::getnameinfo(reinterpret_cast<const sockaddr *>(sock_addr),
                               sizeof(sockaddr_in),
                               host_name,
                               name_len,
                               NULL,
                               0,
                               NI_NAMEREQD);
}

//�Ǳ�׼������ͨ��IPV6��ַȡ������
int ZCE_LIB::getnameinfo_in6(const sockaddr_in6 *sock_addr6,
                            char *host_name,
                            size_t name_len)
{
    return ZCE_LIB::getnameinfo(reinterpret_cast<const sockaddr *>(sock_addr6),
                               sizeof(sockaddr_in6),
                               host_name,
                               name_len,
                               NULL,
                               0,
                               NI_NAMEREQD);
}

//-------------------------------------------------------------------------------------
//IPV4��IPV6֮���໥ת���ĺ��������ǷǱ�׼������

//��һ��IPV4�ĵ�ַӳ��ΪIPV6�ĵ�ַ
int ZCE_LIB::inaddr_map_inaddr6(const in_addr *src, in6_addr *dst)
{
    //��0
    memset(dst, 0, sizeof(in6_addr));

    //
    memcpy(reinterpret_cast<char *>(dst) + sizeof(in6_addr) - sizeof(in_addr),
           reinterpret_cast<const char *>(src),
           sizeof(in_addr));

    //ӳ���ַ�ĵ�����3��WORDΪ0xFFFF
    dst->s6_addr[10] = 0xFF;
    dst->s6_addr[11] = 0xFF;

    return 0;
}

//��һ��IPV4��Sock��ַӳ��ΪIPV6�ĵ�ַ
int ZCE_LIB::sockin_map_sockin6(const sockaddr_in *src, sockaddr_in6 *dst)
{
    return ZCE_LIB::inaddr_map_inaddr6(&(src->sin_addr),
                                      &(dst->sin6_addr));
}

//�ж�һ����ַ�Ƿ���IPV4ӳ��ĵ�ַ
bool ZCE_LIB::is_in6_addr_v4mapped(const in6_addr *in6)
{
    //������ӳ���ַ�ͼ��ݵ�ַ���ж��ˡ���˵���ݵ�ַ�Ժ�ᱻ��̭
    if (in6->s6_addr[0] == 0
        && in6->s6_addr[1] == 0
        && in6->s6_addr[2] == 0
        && in6->s6_addr[3] == 0
        && in6->s6_addr[4] == 0
        && in6->s6_addr[5] == 0
        && in6->s6_addr[6] == 0
        && in6->s6_addr[7] == 0
        && in6->s6_addr[8] == 0
        && in6->s6_addr[9] == 0
        && ((in6->s6_addr[10] == 0xFF && in6->s6_addr[11] == 0xFF) || (in6->s6_addr[10] == 0 && in6->s6_addr[11] == 0))
       )
    {
        return true;
    }

    return false;
}

//���һ��IPV6�ĵ�ַ��IPV4ӳ������ģ�ת����IPV4�ĵ�ַ
int ZCE_LIB::mapped_in6_to_in(const in6_addr *src, in_addr *dst)
{
    //�ȼ���Ƿ���ӳ��ĵ�ַ
    if ( false == ZCE_LIB::is_in6_addr_v4mapped(src) )
    {
        errno = EINVAL;
        return -1;
    }

    memcpy(reinterpret_cast<char *>(dst) ,
           reinterpret_cast<const char *>(src) + sizeof(in6_addr) - sizeof(in_addr),
           sizeof(in_addr));
    return 0;
}
//���һ��IPV6��socketaddr_in6��ַ��IPV4ӳ������ģ�ת����IPV4��socketaddr_in��ַ
int ZCE_LIB::mapped_sockin6_to_sockin(const sockaddr_in6 *src, sockaddr_in *dst)
{
    return ZCE_LIB::mapped_in6_to_in(&(src->sin6_addr),
                                    &(dst->sin_addr));
}

//�Զ˿ڽ��м�飬һЩ�˿��Ǻڿ��ص�ɨ��Ķ˿ڣ�
bool ZCE_LIB::check_safeport(uint16_t check_port)
{
    //��Σ�˿ڼ�鳣��
    const unsigned short UNSAFE_PORT1 = 1024;
    const unsigned short UNSAFE_PORT2 = 3306;
    const unsigned short UNSAFE_PORT3 = 36000;
    const unsigned short UNSAFE_PORT4 = 56000;
    const unsigned short SAFE_PORT1 = 80;

    //������˱��ռ��,������õĶ˿�
    if ((check_port <= UNSAFE_PORT1 && check_port != SAFE_PORT1) ||
        check_port == UNSAFE_PORT2 ||
        check_port == UNSAFE_PORT3 ||
        check_port == UNSAFE_PORT4)
    {
        return false;
    }
    //
    return true;
}

