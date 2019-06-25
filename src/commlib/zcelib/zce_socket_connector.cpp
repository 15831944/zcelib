#include "zce_predefine.h"
#include "zce_time_value.h"
#include "zce_os_adapt_error.h"
#include "zce_os_adapt_socket.h"
#include "zce_os_adapt_time.h"
#include "zce_socket_addr_base.h"
#include "zce_socket_base.h"
#include "zce_socket_stream.h"
#include "zce_socket_connector.h"

/************************************************************************************************************
Class           : ZCE_Socket_Acceptor
************************************************************************************************************/

ZCE_Socket_Connector::ZCE_Socket_Connector()
{
}

ZCE_Socket_Connector::~ZCE_Socket_Connector()
{
}


int ZCE_Socket_Connector::connect (ZCE_Socket_Stream &new_stream,
                                   const ZCE_Sockaddr *remote_addr,
                                   ZCE_Time_Value &timeout,
                                   bool reuse_addr,
                                   int protocol,
                                   const ZCE_Sockaddr *local_addr)
{

    int ret = 0;

    //�������Ĵ���ֵ
    ZCE_LIB::clear_last_error();

    //���û�г�ʼ��
    if (ZCE_INVALID_SOCKET == new_stream.get_handle () )
    {
        if (local_addr)
        {
            ret = new_stream.open (local_addr,
                                   local_addr->sockaddr_ptr_->sa_family,
                                   protocol,
                                   reuse_addr );
        }
        else
        {
            ret = new_stream.open (remote_addr->sockaddr_ptr_->sa_family,
                                   protocol,
                                   reuse_addr );
        }

        if (ret != 0)
        {
            return ret;
        }
    }

    //����������״̬��SOCKET���г�ʱ����
    ret = new_stream.sock_enable(O_NONBLOCK);

    if (ret != 0)
    {
        new_stream.close();
        return ret;
    }

    //�������ӳ���
    ret = ZCE_LIB::connect(new_stream.get_handle(),
                           remote_addr->sockaddr_ptr_,
                           remote_addr->sockaddr_size_);

    //
    if (ret != 0  )
    {
        //WINDOWS�·���EWOULDBLOCK��LINUX�·���EINPROGRESS
        int last_err =  ZCE_LIB::last_error();

        if ( EINPROGRESS != last_err &&  EWOULDBLOCK != last_err )
        {
            new_stream.close();
            return ret;
        }
    }

    //���г�ʱ����
    ret = ZCE_LIB::handle_ready(new_stream.get_handle(),
                                &timeout,
                                ZCE_LIB::HANDLE_READY_CONNECTED);

    const int HANDLE_READY_ONE = 1;

    if (ret != HANDLE_READY_ONE)
    {
        new_stream.close();
        return -1;
    }

    //�رշ�����״̬
    ret = new_stream.sock_disable(O_NONBLOCK);
    if (ret != 0)
    {
        new_stream.close();
        return -1;
    }

    return 0;
}

//�������Ӵ������Խ��з��������Ӵ���
int ZCE_Socket_Connector::connect (ZCE_Socket_Stream &new_stream,
                                   const ZCE_Sockaddr *remote_addr,
                                   bool non_blocing,
                                   bool reuse_addr,
                                   int protocol,
                                   const ZCE_Sockaddr *local_addr)
{
    int ret = 0;

    //�������Ĵ���ֵ
    ZCE_LIB::clear_last_error();

    zcecket�������Ҫ�󶨣����IP�Ͷ˿�
    //���û�г�ʼ��
    if (ZCE_INVALID_SOCKET == new_stream.get_handle () )
    {
        ret = new_stream.open (local_addr,
                               remote_addr->sockaddr_ptr_->sa_family,
                               protocol,
                               reuse_addr);

        if (ret != 0)
        {
            return ret;
        }
    }

    //���з���������
    if (non_blocing)
    {
        ret = new_stream.sock_enable(O_NONBLOCK);

        if (ret != 0)
        {
            new_stream.close();
            return ret;
        }
    }

    //errno = 0;

    //��������
    ret = ZCE_LIB::connect(new_stream.get_handle(),
                           remote_addr->sockaddr_ptr_,
                           zce          remote_addr->sockaddr_size_);

    //���з����������ӣ�һ�㶼�Ƿ��ش��󡣵���UNIX �����һҲ�ᵽ�˹�����������������0�����Լ����Թ������Ƿ���-1
    if (ret != 0 )
    {
        //WINDOWS�·���EWOULDBLOCK��LINUX�·���EINPROGRESS
        int last_error = ZCE_LIB::last_error();

        if ( non_blocing zceNPROGRESS == last_error ||  EWOULDBLOCK == last_error ) )
        {
            //���ر�socket stream
            return -1;
        }
        else
        {
            new_stream.close();
            return ret;
        }
    }

    return 0;
}

