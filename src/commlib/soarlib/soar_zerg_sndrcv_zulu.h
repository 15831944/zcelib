/******************************************************************************************
Copyright           : 2000-2004, Fullsail Technology (Shenzhen) Company Limited.
FileName            :
Author              : ԭ����Scottxu ����ֲ,Sailzeng
Version             :
Date Of Creation    : 2006��9��8��
Description         :

Others              :
Function List       :
1.  ......
    Modification History:
1.Date  :
Author  :Sailzeng ����δ����ƶ���COMM,ģ�廯
Modification  :
******************************************************************************************/

#ifndef SOARING_LIB_SNDRCV_TCP_ZULU_H_
#define SOARING_LIB_SNDRCV_TCP_ZULU_H_

#include "soar_zerg_sndrcv_base.h"

/******************************************************************************************
class Zulu_SendRecv_Package
******************************************************************************************/
class Zulu_SendRecv_Package : public Tibetan_SendRecv_Package
{

protected:

    //ZULU �Ƿ�����
    bool                   zulu_connected_;

    //ZULU���ӵ�IP��ַ
    ZCE_Sockaddr_In        zulu_svc_ip_;

    //ʹ�õ�SOCKET��װ
    ZCE_Socket_Stream      zulu_stream_;

public:

    //���캯������������
    Zulu_SendRecv_Package();
    ~Zulu_SendRecv_Package();

protected:

    //����һ��APPFRAME�������ڲ���tibetan_recv_appframe_
    int receive_svc_package(ZCE_Time_Value *time_wait = NULL);

    //���ڲ���tibetan_send_appframe_���ͳ�ȥ
    int send_svc_package(ZCE_Time_Value *time_wait = NULL);

public:

    //������Ӧ��SVC INFO,
    int set_zulu_svcinfo(const char *svc_ip,
                         unsigned short svc_port,
                         const SERVICES_ID &recv_service,
                         const SERVICES_ID &send_service,
                         const SERVICES_ID &proxy_service,
                         size_t frame_len = Zerg_App_Frame::MAX_LEN_OF_APPFRAME);

    //���ӷ�����,time_wait������const����ԭ��ģ���Ϊ����select��ZCE_Time_Value�Ƿ���ʣ��ʱ���
    int connect_zulu_server(ZCE_Time_Value *time_wait);

    //�ر��ڲ������ӡ�
    void close();

    //ȡ�ñ��صĵ�ַ��Ϣ
    int getsockname (ZCE_Sockaddr *addr)  const;

    //���ͺͽ������ݣ�����ǰ�������ӵġ�
    template< class T1, class T2>
    int send_recv_package(unsigned int snd_cmd,
                          unsigned int qq_uin,
                          const T1 &send_info,
                          ZCE_Time_Value *time_wait,
                          bool if_recv,
                          unsigned int rcv_cmd,
                          T2 &recv_info,
                          bool error_continue = true,
                          unsigned int app_id = 0,
                          unsigned int backfill_trans_id = 0);

    //��������
    template< class T1>
    int send_svc_package(unsigned int qq_uin,
                         unsigned int cmd,
                         const T1 &snd_info,
                         ZCE_Time_Value *time_out = NULL,
                         unsigned int app_id = 0,
                         unsigned int backfill_trans_id = 0);

    //��������
    template< class T2>
    int receive_svc_package(unsigned int cmd,
                            T2 &rcv_info ,
                            bool error_continue = true,
                            ZCE_Time_Value *time_out = NULL);

    //����һ�����ݰ����õ������֣�����Ե���get_recv_appframe���к����Ĵ���
    int receive_svc_package(unsigned int &recv_cmd,
                            ZCE_Time_Value *time_out = NULL);

};

/******************************************************************************************
Author          : Sail (ZengXing)  Date Of Creation: 2006��9��8��
Function        : receive_svc_package
Return          : int
Parameter List  :
Param1: unsigned int cmd ���͵�����
Param2: T& info ���͵���Ϣ����
Param3: bool error_continue ����յ������ݲ��������ģ��ͼ����ȴ���ֱ���ȴ���Ӧ������
Param4: ZCE_Time_Value *time_wait ��ʱʱ�������Ҫһֱ����������NULL,
Description     : �����Ľ���һ��APPFRAME����
Calls           :
Called By       :
Other           : cmdΪԤ�����infoΪԤ�ڰ��ṹ
Modify Record   :
******************************************************************************************/
template<class T2>
int Zulu_SendRecv_Package::receive_svc_package(unsigned int cmd,
                                               T2 &info ,
                                               bool error_continue ,
                                               ZCE_Time_Value *time_wait)
{
    int ret = SOAR_RET::SOAR_RET_SUCC;

    //����ѭ����֤��������������Դͳһ�ͷ�
    for (;;)
    {
        ret = receive_svc_package(time_wait);

        if ( SOAR_RET::SOAR_RET_SUCC != ret )
        {
            break;
        }

        //����յ�������֡���ǰ��ڴ���
        if (cmd != tibetan_recv_appframe_->frame_command_ )
        {
            //�����������,��������,���������
            if (error_continue )
            {
                continue;
            }
            else
            {
                ZLOG_ERROR("[framework] recv a error or unexpect frame,expect cmd =%u,recv cmd =%u.", cmd, tibetan_recv_appframe_->frame_command_);
                ret =  SOAR_RET::ERROR_ZULU_RECEIVE_OTHERS_COMMAND;
                break;
            }
        }
        else
        {
            //�������data�����ݣ����н���
            if (tibetan_recv_appframe_->frame_length_ > Zerg_App_Frame::LEN_OF_APPFRAME_HEAD )
            {
                ret = tibetan_recv_appframe_->appdata_decode(info);

                if (ret != 0)
                {
                    return ret;
                }
            }
        }

        //����ѭ��
        break;
    }

    //�����д��󣬽������⴦��
    if (ret != SOAR_RET::SOAR_RET_SUCC)
    {
        zulu_stream_.close();
        zulu_connected_ = false;
        return ret;
    }

    return SOAR_RET::SOAR_RET_SUCC;
}

/******************************************************************************************
Author          : Sail (ZengXing)  Date Of Creation: 2006��9��8��
Function        : send_svc_package
Return          : int
Parameter List  :
Param1: qq_uin                    QQUIN
Param3: unsigned int cmd          ����ķ�������
Param4: const T1& info             ����ķ��ͽṹ
Param5: ZCE_Time_Value *time_wait ���͵ĳ�ʱʱ��
Param6: unsigned int sndtrans_id  ���յĳ�ʱʱ��
Description     :
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
template< class T1>
int Zulu_SendRecv_Package::send_svc_package(unsigned int qq_uin,
                                            unsigned int cmd,
                                            const T1 &info,
                                            ZCE_Time_Value *time_wait ,
                                            unsigned int app_id,
                                            unsigned int backfill_trans_id)
{
    int ret = 0;

    //��û�����ӵ���������ӷ�����
    if ( false == zulu_connected_ )
    {
        ret = connect_zulu_server(time_wait);

        if (SOAR_RET::SOAR_RET_SUCC != ret)
        {
            return ret;
        }
    }

    tibetan_send_appframe_->frame_command_ = cmd;
    tibetan_send_appframe_->backfill_trans_id_ = backfill_trans_id;
    tibetan_send_appframe_->app_id_ = app_id;
    tibetan_send_appframe_->frame_uin_ = qq_uin;

    //����
    ret = tibetan_send_appframe_->appdata_encode(Zerg_App_Frame::MAX_LEN_OF_APPFRAME_DATA, info);

    if (ret != 0 )
    {
        return ret;
    }

    ret = send_svc_package(time_wait);

    if (ret != SOAR_RET::SOAR_RET_SUCC)
    {
        zulu_stream_.close();
        zulu_connected_ = false;
        return ret;
    }

    return SOAR_RET::SOAR_RET_SUCC;
}

/******************************************************************************************
Author          : Sailzeng <sailerzeng@gmail.com>  Date Of Creation: 2008��4��25��
Function        : Zulu_SendRecv_Package::send_recv_package
Return          : int
Parameter List  :
Param1: unsigned int snd_cmd      ��������
Param2: unsigned int qq_uin       QQUIN��Ч��
Param3: const T1& send_info       ���͵�����
Param4: ZCE_Time_Value* time_wait ʱ��ȴ�
Param5: bool if_recv              �Ƿ����
Param6: unsigned int rcv_cmd      ���յ�����
Param7: T2& recv_info             ���յ�����
Param8: bool error_continue       �������Ƿ����
Description     : ���ͺͽ���PACKAGE
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
template< class T1, class T2>
int Zulu_SendRecv_Package::send_recv_package(unsigned int snd_cmd,
                                             unsigned int qq_uin,
                                             const T1 &send_info,
                                             ZCE_Time_Value *time_wait,
                                             bool if_recv,
                                             unsigned int rcv_cmd,
                                             T2 &recv_info,
                                             bool error_continue,
                                             unsigned int app_id,
                                             unsigned int backfill_trans_id)
{
    int ret = 0;

    //��������
    ret = send_svc_package(qq_uin,
                           snd_cmd,
                           send_info,
                           time_wait,
                           app_id,
                           backfill_trans_id);

    if (ret != SOAR_RET::SOAR_RET_SUCC)
    {
        return ret;
    }

    //ΪʲôҪ����������������������Ѿ��벻�����ˣ���
    if (false == if_recv)
    {
        return SOAR_RET::SOAR_RET_SUCC;
    }

    //��ȡ���ݣ�
    ret = receive_svc_package(rcv_cmd,
                              recv_info,
                              error_continue,
                              time_wait
                             );

    if (ret != SOAR_RET::SOAR_RET_SUCC)
    {
        return ret;
    }

    return SOAR_RET::SOAR_RET_SUCC;
}

#endif //SOARING_LIB_SNDRCV_TCP_ZULU_H_

