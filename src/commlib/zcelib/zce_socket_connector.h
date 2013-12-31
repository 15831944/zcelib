#ifndef ZCE_LIB_SOCKET_CONNECTOR_H_
#define ZCE_LIB_SOCKET_CONNECTOR_H_

#include "zce_boost_non_copyable.h"

class ZCE_Time_Value;
class ZCE_Sockaddr;

/************************************************************************************************************
Author          : Sailzeng ZENGXING  Date Of Creation: 2011��6��19��
Class           : ZCE_Socket_Connector
Inherit         :
Description     : connect�Ĺ������ɹ����Ӻ����,���ڲ���ZCE_Socket_Stream
Other           :
Modify Record   :
************************************************************************************************************/
class ZCELIB_EXPORT ZCE_Socket_Connector
{
public:
    //���캯������������
    ZCE_Socket_Connector (void);
    ~ZCE_Socket_Connector (void);

    //����ʱ������
    int connect (ZCE_Socket_Stream &new_stream,
                 const ZCE_Sockaddr *remote_addr ,
                 ZCE_Time_Value  &timeout,
                 bool reuse_addr = false,
                 int protocol = 0,
                 const ZCE_Sockaddr *local_addr = NULL);

    //�������߷�����������
    int connect (ZCE_Socket_Stream &new_stream,
                 const ZCE_Sockaddr *remote_addr ,
                 bool non_blocing,
                 bool reuse_addr = false,
                 int protocol = 0,
                 const ZCE_Sockaddr *local_addr = NULL);

};

#endif //#ifndef ZCE_LIB_SOCKET_CONNECTOR_H_

