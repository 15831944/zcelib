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

/************************************************************************************************************
Author          : Sailzeng ZENGXING  Date Of Creation: 2011��6��21��
Function        :
Return          : int
Parameter List  :
  Param1: ZCE_Socket_Stream& new_stream
  Param2: const sockaddr* remote_addr
  Param3: socklen_t remote_addr_len
  Param4: const timeval& timeout
  Param5: int reuse_addr
  Param6: int protocol
  Param7: const sockaddr* local_addr
Description     : ���г�ʱ���Ӵ���
Calls           :
Called By       :
Other           :
Modify Record   :
************************************************************************************************************/
int ZCE_Socket_Connector::connect (ZCE_Socket_Stream &new_stream,
                                   const ZCE_Sockaddr *remote_addr ,
                                   ZCE_Time_Value &timeout,
                                   bool reuse_addr ,
                                   int protocol ,
                                   const ZCE_Sockaddr *local_addr)
{

    int ret = 0;

    //�������Ĵ���ֵ
    ZCE_OS::clear_last_error();

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
    ret = ZCE_OS::connect(new_stream.get_handle(),
                          remote_addr->sockaddr_ptr_,
                          remote_addr->sockaddr_size_);

    //
    if (ret != 0  )
    {
        //WINDOWS�·���EWOULDBLOCK��LINUX�·���EINPROGRESS
        int last_err =  ZCE_OS::last_error();

        if ( EINPROGRESS != last_err &&  EWOULDBLOCK != last_err )
        {
            new_stream.close();
            return ret;
        }
    }

    //���г�ʱ����
    ret = ZCE_OS::handle_ready(new_stream.get_handle(),
                               &timeout,
                               ZCE_OS::HANDLE_READY_CONNECT_SUCCESS);

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

/************************************************************************************************************
Author          : Sailzeng ZENGXING  Date Of Creation: 2011��6��21��
Function        :
Return          : int
Parameter List  :
  Param1: ZCE_Socket_Stream& new_stream ���ӵ�SOCKET �����
  Param2: const sockaddr* remote_addr   Զ�˵ĵ�ַ��ʵ�ʽṹ��sockadd_in��sockadd_in6��
  Param3: socklen_t remote_addr_len ��ַ�ĳ��ȣ���ʾǰ���������ʲô��C��������
  Param4: bool non_blocing �Ƿ������������ӡ�
  Param5: int reuse_addr   �Ƿ���Ϊreuse_addrѡ��
  Param6: int protocol     Э�飬����RAW Socket������û��
  Param7: const sockaddr* local_addr ���ص�BIND��ַ��Ϣ��CONNECT����û��
Description     : �������Ӵ������Խ��з��������Ӵ���
Calls           :
Called By       :
Other           :
Modify Record   :
************************************************************************************************************/
int ZCE_Socket_Connector::connect (ZCE_Socket_Stream &new_stream,
                                   const ZCE_Sockaddr *remote_addr ,
                                   bool non_blocing,
                                   bool reuse_addr,
                                   int protocol,
                                   const ZCE_Sockaddr *local_addr)
{
    int ret = 0;

    //�������Ĵ���ֵ
    ZCE_OS::clear_last_error();

    //��ʼ��Socket�������Ҫ�󶨣����IP�Ͷ˿�
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
    ret = ZCE_OS::connect(new_stream.get_handle(),
                          remote_addr->sockaddr_ptr_,
                          remote_addr->sockaddr_size_);

    //���з����������ӣ�һ�㶼�Ƿ��ش��󡣵���UNIX �����һҲ�ᵽ�˹�����������������0�����Լ����Թ������Ƿ���-1
    if (ret != 0 )
    {
        //WINDOWS�·���EWOULDBLOCK��LINUX�·���EINPROGRESS
        int last_error = ZCE_OS::last_error();

        if ( non_blocing && ( EINPROGRESS == last_error ||  EWOULDBLOCK == last_error ) )
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

