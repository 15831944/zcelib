#include "ogre_predefine.h"
#include "ogre_comm_manager.h"

#include "ogre_udp_ctrl_handler.h"
#include "ogre_accept_handler.h"
#include "ogre_tcp_ctrl_handler.h"
#include "ogre_buf_storage.h"
#include "ogre_configure.h"
#include "ogre_ip_restrict.h"

Ogre_Comm_Manger *Ogre_Comm_Manger::instance_ = NULL;

Ogre_Comm_Manger::Ogre_Comm_Manger()
{
}

Ogre_Comm_Manger::~Ogre_Comm_Manger()
{

}

//���һ���˿��Ƿ�ȫ.Ȼ��������ý��д���
int Ogre_Comm_Manger::check_safe_port(ZCE_Sockaddr_In &inetadd)
{

    //������˱��ռ��,������õĶ˿�
    if ( false == inetadd.check_safeport() )
    {
        //���ʹ�ñ��մ�(TRUE)
        if (ogre_config_->ogre_cfg_data_.ogre_insurance_)
        {
            ZLOG_ERROR( "Unsafe port [%s|%u],if you need to open this port,please close insurance. \n",
                        inetadd.get_host_addr(),
                        inetadd.get_port_number());
            return SOAR_RET::ERR_OGRE_UNSAFE_PORT_WARN;
        }
        //�����ʹ�ñ���(FALSE)
        else
        {
            //��������
            ZLOG_INFO( "Warn!Warn! Unsafe port [%s|%u] listen.Please notice! \n",
                       inetadd.get_host_addr(),
                       inetadd.get_port_number());
        }
    }
    //
    return 0;
}

//
int Ogre_Comm_Manger::init_accept_by_conf(Zen_INI_PropertyTree &cfg_file)
{
    int ret = 0;
    unsigned int tmp_uint = 0;

    const size_t TMP_BUFFER_LEN = 256;
    char tmp_key[TMP_BUFFER_LEN + 1];
    char tmp_value[TMP_BUFFER_LEN + 1];
    char err_outbuf[TMP_BUFFER_LEN + 1];

    ret = cfg_file.get_uint32_value("TCPACCEPT", "OPENNUMBER", tmp_uint);
    snprintf(err_outbuf, TMP_BUFFER_LEN, "TCPACCEPT|OPENNUMBER key .");
    TESTCONFIG((ret == 0), err_outbuf);
    unsigned int accept_num = tmp_uint;

    //���û�����ô򿪣����ö�ȡ���������
    if (0 == accept_num)
    {
        return 0;
    }

    for (unsigned int i = 1; i <= accept_num; ++i)
    {

        //��ȡbind��IP�˿�
        snprintf(tmp_key, TMP_BUFFER_LEN, "ACCEPTIP%u", i);
        ret = cfg_file.get_string_value("TCPACCEPT", tmp_key, tmp_value, TMP_BUFFER_LEN);
        snprintf(err_outbuf, TMP_BUFFER_LEN, "TCPACCEPT|%s key error.", tmp_key);
        TESTCONFIG((ret == 0), err_outbuf);

        snprintf(tmp_key, TMP_BUFFER_LEN, "ACCEPTPORT%u", i);
        ret  = cfg_file.get_uint32_value("TCPACCEPT", tmp_key, tmp_uint);
        snprintf(err_outbuf, TMP_BUFFER_LEN, "TCPACCEPT|%s key error.", tmp_key);
        TESTCONFIG((ret == 0), err_outbuf);

        ZCE_Sockaddr_In accept_bind_addr;
        ret = accept_bind_addr.set(tmp_value, static_cast<unsigned short>(tmp_uint));
        TESTCONFIG((ret == 0), err_outbuf);

        ret = check_safe_port(accept_bind_addr);

        if (ret != 0)
        {
            return ret;
        }

        //TCP��ȡ���ݵ�ģ��
        snprintf(tmp_key, TMP_BUFFER_LEN, "ACCEPTMODULE%u", i);
        ret = cfg_file.get_string_value("TCPACCEPT", tmp_key, tmp_value, TMP_BUFFER_LEN);
        snprintf(err_outbuf, TMP_BUFFER_LEN, "TCPACCEPT|%s key error.", tmp_key);
        TESTCONFIG((ret == 0), err_outbuf);

        OgreTCPAcceptHandler *accpet_hd = new OgreTCPAcceptHandler(accept_bind_addr, tmp_value);
        accept_handler_ary_.push_back(accpet_hd);
    }

    return 0;
}

//
int Ogre_Comm_Manger::init_udp_by_conf(Zen_INI_PropertyTree &cfg_file)
{
    int ret = 0;
    unsigned int tmp_uint = 0;

    const size_t TMP_BUFFER_LEN = 256;
    char tmp_key[TMP_BUFFER_LEN + 1];
    char tmp_value[TMP_BUFFER_LEN + 1];
    char err_outbuf[TMP_BUFFER_LEN + 1];

    ret = cfg_file.get_uint32_value("UDPCOMM", "OPENNUMBER", tmp_uint);
    snprintf(err_outbuf, TMP_BUFFER_LEN, "UDPCOMM|OPENNUMBER key .");
    TESTCONFIG((ret == 0), err_outbuf);
    unsigned int udp_num = tmp_uint;

    //���û�����ô򿪣����ö�ȡ���������
    if (0 == udp_num)
    {
        return 0;
    }

    for (unsigned int i = 1; i <= udp_num; ++i)
    {

        //��ȡbind��IP�˿�
        snprintf(tmp_key, TMP_BUFFER_LEN, "UDPIP%u", i);
        ret = cfg_file.get_string_value("UDPCOMM", tmp_key, tmp_value, TMP_BUFFER_LEN);
        snprintf(err_outbuf, TMP_BUFFER_LEN, "UDPCOMM|%s key error.", tmp_key);
        TESTCONFIG((ret == 0), err_outbuf);

        snprintf(tmp_key, TMP_BUFFER_LEN, "UDPPORT%u", i);
        ret  = cfg_file.get_uint32_value("UDPCOMM", tmp_key, tmp_uint);
        snprintf(err_outbuf, TMP_BUFFER_LEN, "UDPCOMM|%s key error.", tmp_key);
        TESTCONFIG((ret == 0), err_outbuf);

        ZCE_Sockaddr_In udp_bind_addr;
        ret = udp_bind_addr.set(tmp_value, static_cast<unsigned short>(tmp_uint));
        TESTCONFIG((ret == 0), err_outbuf);

        ret = check_safe_port(udp_bind_addr);

        if (ret != 0)
        {
            return ret;
        }

        OgreUDPSvcHandler *udp_hd = new OgreUDPSvcHandler(udp_bind_addr);
        udp_handler_ary_.push_back(udp_hd);
    }

    return 0;

}

//�õ�����
int Ogre_Comm_Manger::get_config(const Ogre_Server_Config *config)
{
    int ret = 0;

    //IP����,
    ret = Ogre_IPRestrict_Mgr::instance()->get_config(config);

    if (0 != ret )
    {
        return ret;
    }

    //TCP ��ȡ����
    ret = Ogre_TCP_Svc_Handler::get_config(config);

    if (ret != 0)
    {
        return ret;
    }

    return 0;
}


//�����еĶ����е����ݷ��ͣ���SEND�ܵ��ҵ����е�����ȥ����,
//�����룬���Ǽ���һ����෢�͵�֡���޶�
int Ogre_Comm_Manger::get_all_senddata_to_write(size_t &procframe)
{
    int ret = 0;

    //
    for (procframe = 0;
         Soar_MMAP_BusPipe::instance()->is_empty_bus(Soar_MMAP_BusPipe::SEND_PIPE_ID) == false &&
         procframe < MAX_ONCE_SEND_FRAME; ++procframe)
    {
        Ogre4a_App_Frame *send_frame = Ogre_Buffer_Storage::instance()->allocate_byte_buffer();

        //
        ret = Soar_MMAP_BusPipe::instance()->pop_front_bus(Soar_MMAP_BusPipe::SEND_PIPE_ID,
                                                           reinterpret_cast< ZCE_LIB::dequechunk_node*&>(send_frame));

        if (ret != 0)
        {
            //�黹����
            Ogre_Buffer_Storage::instance()->free_byte_buffer(send_frame);
            continue;
        }

        //���FRAME�ĳ���
        if (send_frame->ogre_frame_len_ > Ogre4a_App_Frame::MAX_OF_OGRE_FRAME_LEN)
        {
            ZLOG_ALERT("Ogre_Comm_Manger::get_all_senddata_to_write len %u\n",
                       send_frame->ogre_frame_len_);
            DEBUGDUMP_OGRE_HEAD(send_frame, "Ogre_Comm_Manger::get_all_senddata_to_write", RS_ALERT);
            ZCE_ASSERT(false);
            return SOAR_RET::ERR_OGRE_SEND_FRAME_TOO_LEN;
        }

        //�����TCP
        if (send_frame->ogre_frame_option_ & Ogre4a_App_Frame::OGREDESC_PEER_TCP )
        {
            ret = Ogre_TCP_Svc_Handler::process_send_data(send_frame);

            if (ret != 0)
            {
                //�黹����
                Ogre_Buffer_Storage::instance()->free_byte_buffer(send_frame);
                continue;
            }
        }

        //�����UDP
        else if (send_frame->ogre_frame_option_ & Ogre4a_App_Frame::OGREDESC_PEER_UDP )
        {
            //��������
            OgreUDPSvcHandler::SendAllDataToUDP(send_frame);
            Ogre_Buffer_Storage::instance()->free_byte_buffer(send_frame);
        }
        //�㶼����д������η��ͣ�
        else
        {
            ZLOG_ERROR( "Ogre frame have not send option,Please Check you code.\n");
            Ogre_Buffer_Storage::instance()->free_byte_buffer(send_frame);
        }
    }

    return 0;
}

//��ʼ��ͨѶ������
int Ogre_Comm_Manger::init_comm_manger()
{
    int ret = 0;



    //��ʼ�����еļ����˿�
    for (size_t i = 0; i < accept_handler_ary_.size(); ++i)
    {
        ret = accept_handler_ary_[i]->create_listenpeer();

        if (ret != 0)
        {
            return ret;
        }
    }

    ret = OgreUDPSvcHandler::OpenUDPSendPeer();

    if (ret != 0)
    {
        return ret;
    }

    //��ʼ�����е�UDP�˿�
    for (size_t i = 0; i < udp_handler_ary_.size(); ++i)
    {
        ret = udp_handler_ary_[i]->InitUDPServices();

        if (ret != 0)
        {
            return ret;
        }
    }

    //��ʼ����̬����
    ret = Ogre_TCP_Svc_Handler::init_all_static_data();

    if (ret != 0)
    {
        return ret;
    }

    return 0;
}

//ע��ͨѶ������
int Ogre_Comm_Manger::uninit_comm_manger()
{
    int ret = 0;

    //��ʼ�����еļ����˿�
    for (size_t i = 0; i < accept_handler_ary_.size(); ++i)
    {
        ret = accept_handler_ary_[i]->handle_close();

        if (ret != 0)
        {
            return ret;
        }
    }

    //��ʼ�����е�UDP�˿�
    for (size_t i = 0; i < udp_handler_ary_.size(); ++i)
    {
        ret = udp_handler_ary_[i]->handle_close();

        if (ret != 0)
        {
            return ret;
        }
    }

    //
    Ogre_TCP_Svc_Handler::unInit_all_static_data();
    //
    Ogre_IPRestrict_Mgr::clean_instance();

    return 0;
}

//�õ����ӵ�ʵ��
Ogre_Comm_Manger *Ogre_Comm_Manger::instance()
{
    if (instance_ == NULL)
    {
        instance_ = new Ogre_Comm_Manger();
    }

    return instance_;
}

//�����ӵ�ʵ��
void Ogre_Comm_Manger::clean_instance()
{
    if (instance_)
    {
        delete instance_;
        instance_ = NULL;
    }
}

