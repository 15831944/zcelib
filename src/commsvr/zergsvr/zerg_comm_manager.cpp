
#include "zerg_predefine.h"
#include "zerg_tcp_ctrl_handler.h"
#include "zerg_udp_ctrl_handler.h"
#include "zerg_accept_handler.h"
#include "zerg_app_handler.h"
#include "zerg_external_connect_handler.h"
#include "zerg_comm_manager.h"


//ʵ��
Zerg_Comm_Manager *Zerg_Comm_Manager::instance_ = NULL;

Zerg_Comm_Manager::Zerg_Comm_Manager()
    : error_try_num_(3)
    , monitor_size_(0)
    , zerg_mmap_pipe_(NULL)
    , zbuffer_storage_(NULL)
    , server_status_(NULL)
    , send_frame_count_(0)

{
    zerg_mmap_pipe_ = Zerg_MMAP_BusPipe::instance();
    zbuffer_storage_ = ZBuffer_Storage::instance();
    server_status_ = Comm_Stat_Monitor::instance();
    count_start_time_ = static_cast<unsigned int>(Zerg_App_Timer_Handler::now_time_.sec());

    memset(monitor_cmd_, 0, sizeof(monitor_cmd_));
}

Zerg_Comm_Manager::~Zerg_Comm_Manager()
{
    //�ȹر�ACCEPET
    for (size_t i = 0; i < zerg_acceptor_.size(); ++i)
    {
        zerg_acceptor_[i]->handle_close();
    }

    //�ȹر�UDP
    for (size_t i = 0; i < zerg_updsvc_.size(); ++i)
    {
        zerg_updsvc_[i]->handle_close();
    }
}

//��ʼ��
int Zerg_Comm_Manager::get_config(const conf_zerg::ZERG_CONFIG &config)
{
    comm_config_ = Comm_Svrd_Config::instance();

    int ret = 0;
    const size_t LEN_TMP_BUFFER = 256;
    //char key_buf[LEN_TMP_BUFFER+1]={0};
    char err_buf[LEN_TMP_BUFFER + 1] = {0};

    //����������
    monitor_size_ = 0;
    memset(monitor_cmd_, 0, sizeof(monitor_cmd_));

    //���������ݳ��Է��ʹ���
    //ret = cfg_file.get_uint32_value("COMMCFG","TRYERROR",tmp_uint );
    error_try_num_ = config.comm_cfg.retry_error;
    TESTCONFIG((ret == 0 && error_try_num_ < 10), "COMMCFG|TRYERROR key error.");

    //���������ݳ��Է��ʹ���
    //ret = cfg_file.get_uint32_value("MONITORTRACK","NUMMONITOR",tmp_uint );
    monitor_size_ = comm_config_->framework_config_.log_info_.monitor_cmd_count_;
    TESTCONFIG((ret == 0 && monitor_size_ <= MAX_NUMBER_OF_MONITOR_FRAME), "MONITORCMD|NUMMONITOR key error.");

    ZLOG_INFO("[zergsvr] Zerg_Comm_Manager::get_config monitor_size_ = %u", monitor_size_);

    //��ȡ��ص������ص���������������٣���õ���==0���ȽϺ�ʱ������ԶԵ������м�ء�
    //������ò�Ҫ�����еĻ������м�أ�
    for (size_t i = 0; i < monitor_size_; ++i)
    {
        //snprintf(key_buf,LEN_TMP_BUFFER,"MONITORCMD%u",static_cast<unsigned int> (i+1));
        //snprintf(err_buf,LEN_TMP_BUFFER,"MONITORTRACK|%s key error.",key_buf);
        //ret = cfg_file.get_uint32_value("MONITORTRACK",key_buf,tmp_uint);
        monitor_cmd_[i] = comm_config_->framework_config_.log_info_.monitor_cmd_list_[i];
        TESTCONFIG((ret == 0 && monitor_cmd_[i] != 0), err_buf);
    }

    // ��ʼ���ⲿЭ�鴦��handler
    init_extern_proto_handler();

    return SOAR_RET::SOAR_RET_SUCC;
}

/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2007��10��24��
Function        : Zerg_Service_App::init_socketpeer
Return          : int
Parameter List  :
    Param1: ZERG_SERVICES_INFO& init_svcid
Description     :
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int Zerg_Comm_Manager::init_socketpeer(ZERG_SERVICES_INFO &init_svcid,
                                       unsigned int backlog,
                                       bool is_extern_svc,
                                       unsigned int proto_index)
{
    int ret = 0;

    //���һ�¶˿�
    ret = check_safeport(init_svcid.zerg_ip_addr_);

    if (ret != 0)
    {
        return ret;
    }

    //�����TCP�Ľӿ�
    if (init_svcid.zerg_svc_info_.services_type_ < SVC_UDP_SERVER_BEGIN )
    {
        //����Bind��ַ
        TCP_Accept_Handler *tmp_acceptor = new TCP_Accept_Handler(init_svcid.zerg_svc_info_,
                                                                  init_svcid.zerg_ip_addr_,
                                                                  init_svcid.zerg_sessionkey_);

        if (is_extern_svc)
        {
            if (proto_index < 0 || proto_index >= external_proto_handler_.size())
            {
                ZLOG_ERROR("[zergsvr] Init tcp accept socket proto handler fail, index=%d, size=%d",
                           proto_index, external_proto_handler_.size());
                return SOAR_RET::ERR_ZERG_INIT_ACCEPT_SOCKET_FAIL;
            }

            tmp_acceptor->set_extern_svc_acceptor(external_proto_handler_[proto_index]);
        }

        //����ͬ���ķ�ʽ����LISTER PEER
        ret = tmp_acceptor->create_listen(backlog);

        if (ret != 0 )
        {
            ZLOG_ERROR("[zergsvr] Init tcp accept scoket fail ret = %d.error[%u|%s]",
                       ret,
                       ZCE_OS::last_error(),
                       strerror(ZCE_OS::last_error())
                      );
            return  SOAR_RET::ERR_ZERG_INIT_ACCEPT_SOCKET_FAIL;
        }

        zerg_acceptor_.push_back(tmp_acceptor);
    }
    //�����UDP�Ľӿ�
    else
    {
        //
        UDP_Svc_Handler *tmp_udphdl =  new UDP_Svc_Handler(init_svcid.zerg_svc_info_,
                                                           init_svcid.zerg_ip_addr_,
                                                           init_svcid.zerg_sessionkey_,
                                                           init_svcid.is_recv_external_pkg_,
                                                           init_svcid.cmd_offset_,
                                                           init_svcid.cmd_len_);

        //��ʼ��UDP�Ķ˿�
        ret = tmp_udphdl->init_udp_services();

        if (ret != 0 )
        {
            ZLOG_ERROR("[zergsvr] Init udp scoket fail ret = %d.", ret);
            return  SOAR_RET::ERR_ZERG_INIT_UPD_PORT_FAIL;
        }

        zerg_updsvc_.push_back(tmp_udphdl);
    }

    return SOAR_RET::SOAR_RET_SUCC;
}

/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2007��4��12��
Function        : Zerg_Comm_Manager::check_safeport
Return          : int
Parameter List  :
Param1: ZCE_Sockaddr_In    & ipaddr
Description     : ���һ���˿��Ƿ�ȫ��
Calls           :
Called By       :
Other           :
Modify Record   :
******************************************************************************************/
int Zerg_Comm_Manager::check_safeport(ZCE_Sockaddr_In     &inetadd)
{
    //��Σ�˿ڼ�鳣��
    const unsigned short UNSAFE_PORT1 = 1024;
    const unsigned short UNSAFE_PORT2 = 3306;
    const unsigned short UNSAFE_PORT3 = 36000;
    const unsigned short UNSAFE_PORT4 = 56000;
    const unsigned short SAFE_PORT1 = 80;

    //������˱��ռ��,������õĶ˿�
    if ((inetadd.get_port_number() <= UNSAFE_PORT1 && inetadd.get_port_number() != SAFE_PORT1)  ||
        inetadd.get_port_number() == UNSAFE_PORT2  ||
        inetadd.get_port_number() == UNSAFE_PORT3 ||
        inetadd.get_port_number() == UNSAFE_PORT4 )
    {
        //���ʹ�ñ��մ�(TRUE)
        if (Zerg_Server_Config::instance()->zerg_insurance_)
        {
            ZLOG_ERROR("[zergsvr] Unsafe port %u,if you need to open this port,please close insurance. ",
                       inetadd.get_port_number());
            return SOAR_RET::ERR_ZERG_UNSAFE_PORT_WARN;
        }
        //�����ʹ�ñ���(FALSE)
        else
        {
            //��������
            ZLOG_INFO("[zergsvr] Warn!Warn! Unsafe port %u.Please notice! ",
                      inetadd.get_port_number());
        }
    }

    //
    return SOAR_RET::SOAR_RET_SUCC;
}

/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2006��3��20��
Function        : Zerg_Comm_Manager::popall_sendpipe_write
Return          : int
Parameter List  :
  Param1: const size_t want_send_frame
  Param2: size_t& num_send_frame
Description     : �����еĶ����е����ݷ��ͣ�
Calls           :
Called By       :
Other           : �����룬���Ǽ���һ����෢�͵�֡���޶�
Modify Record   :
******************************************************************************************/
int Zerg_Comm_Manager::popall_sendpipe_write(const size_t want_send_frame, size_t &num_send_frame)
{
    num_send_frame = 0;
    int ret = 0;

    while (zerg_mmap_pipe_->is_empty_bus(Zerg_MMAP_BusPipe::SEND_PIPE_ID) == false && num_send_frame < want_send_frame )
    {

        Zerg_Buffer *tmpbuf = zbuffer_storage_->allocate_buffer();
        //
        Zerg_App_Frame *proc_frame = reinterpret_cast<Zerg_App_Frame *>( tmpbuf->buffer_data_);

        //ע��ѹ������ݲ�Ҫ����APPFRAME�������󳤶�,���������Ȩ��ѡ��Ч��
        zerg_mmap_pipe_->pop_front_bus(Zerg_MMAP_BusPipe::SEND_PIPE_ID, reinterpret_cast< ZCE_LIB::dequechunk_node*&>(proc_frame));

        tmpbuf->size_of_use_ = proc_frame->frame_length_;

        if (comm_config_->is_monitor_uin(proc_frame->frame_uin_))
        {
            proc_frame->frame_option_ |= Zerg_App_Frame::DESC_MONITOR_TRACK;
        }

        //�����Ҫ���ٵ�����
        if (proc_frame->frame_option_ & Zerg_App_Frame::DESC_MONITOR_TRACK)
        {
            Zerg_App_Frame::dumpoutput_framehead(proc_frame, "[TRACK MONITOR][SEND]opt", RS_INFO);
        }
        else
        {
            for (size_t i = 0; i < monitor_size_; ++i)
            {
                if (monitor_cmd_[i] == proc_frame->frame_command_)
                {
                    proc_frame->frame_option_ |= Zerg_App_Frame::DESC_MONITOR_TRACK;
                    Zerg_App_Frame::dumpoutput_framehead(proc_frame, "[TRACK MONITOR][SEND]cmd", RS_INFO);
                }
            }
        }

        // APP���������Ļذ����ٴ��Ͻ���ʱ��
        if (true == proc_frame->is_zerg_heart_beat_rsp())
        {
            proc_zerg_heart_beat(proc_frame);
        }

        //����UDP������
        if (proc_frame->frame_option_ & Zerg_App_Frame::DESC_UDP_FRAME)
        {
            //���ʹ�����־��send_all_to_udp�����ڲ���������������ظ���¼
            UDP_Svc_Handler::send_all_to_udp(proc_frame);
            //UDP�İ��黹����
            zbuffer_storage_->free_byte_buffer(tmpbuf);
        }
        else
        {
            // ����ǹ㲥�Ļ�������ӦSvr_type������Svr��
            if (proc_frame->recv_service_.services_id_ == SERVICES_ID::BROADCAST_SERVICES_ID)
            {
                std::vector<SERVICES_ID> vec;
                ret = TCP_Svc_Handler::get_zerg_auto_connect().get_all_conn_server(proc_frame->recv_service_.services_type_, vec);
                if (ret != SOAR_RET::SOAR_RET_SUCC)
                {
                    ZLOG_ERROR("[%s] fetch broadcast pkg error, recv svrinfo:[%u|%u]",
                        __ZCE_FUNCTION__,
                        proc_frame->recv_service_.services_type_,
                        proc_frame->recv_service_.services_id_);

                    continue;
                }

                uint32_t size = (uint32_t)vec.size();
                for (uint32_t i = 0; i < size; i ++)
                {
                    if (i == size -1)
                    {
                        reinterpret_cast<Zerg_App_Frame *>(tmpbuf->buffer_data_)->recv_service_.services_id_ = vec[i].services_id_;
                        // last Buf���ñ����tmpbuf��������΢�ӿ��ٶ�
                        ret = send_single_buf(tmpbuf);
                    }
                    else
                    {
                        Zerg_Buffer *send_buf = zbuffer_storage_->allocate_buffer();
                        memcpy(send_buf->buffer_data_, tmpbuf->buffer_data_, tmpbuf->size_of_use_);
                        reinterpret_cast<Zerg_App_Frame *>(send_buf->buffer_data_)->recv_service_.services_id_ = vec[i].services_id_;
                        send_buf->size_of_use_ = tmpbuf->size_of_use_;
                        ret = send_single_buf(send_buf);
                    }

                    if (ret != SOAR_RET::SOAR_RET_SUCC)
                    {
                        continue;
                    }

                    ++send_frame_count_;
                }
            }
            else
            {
                ret = send_single_buf(tmpbuf);
                if (ret != SOAR_RET::SOAR_RET_SUCC)
                {
                    // û�з��ɹ�������������ͳ��
                    continue;
                }

                ++send_frame_count_;
            }

        }

        ++num_send_frame;
    }

    return SOAR_RET::SOAR_RET_SUCC;
}

//��鷢��Ƶ��
void Zerg_Comm_Manager::check_freamcount(unsigned int now)
{
    if (now <= count_start_time_)
    {
        ZLOG_ERROR("[zergsvr] Zerg_Comm_Manager::check_freamcount time err.now:%u, count_start_time_:%u",
                   now,
                   count_start_time_);
        return;
    }

    unsigned int interval = now - count_start_time_;
    unsigned int frame_per_sec = send_frame_count_ / interval;

    if (frame_per_sec > SEND_FRAME_ALERT_VALUE)
    {
        ZLOG_ALERT("[zergsvr] Zerg_Comm_Manager::check_freamcount ALERT frame_per_sec:%u, send_frame_count_:%u, interval:%u.",
                   frame_per_sec,
                   send_frame_count_,
                   interval);
    }
    else
    {
        ZLOG_INFO("[zergsvr] Zerg_Comm_Manager::check_freamcount frame_per_sec:%u, send_frame_count_:%u, interval:%u.",
                  frame_per_sec,
                  send_frame_count_,
                  interval);
    }

    send_frame_count_ = 0;
    count_start_time_ = now;
}

int Zerg_Comm_Manager::init_extern_proto_handler()
{
    external_proto_handler_.clear();
    for (size_t i = 0; i < Zerg_Server_Config::instance()->config_->external_proto_cfg.count; ++i)
    {
        ExternalConnectHandler* h = new ExternalConnectHandler(Zerg_Server_Config::instance()->config_->external_proto_cfg.item[i].pkg_len_offset,
            Zerg_Server_Config::instance()->config_->external_proto_cfg.item[i].pkg_len_bytes,
            Zerg_Server_Config::instance()->config_->external_proto_cfg.item[i].app_frame_cmd);
        external_proto_handler_.push_back(h);
    }
    return SOAR_RET::SOAR_RET_SUCC;
}

TcpHandlerImpl* Zerg_Comm_Manager::get_extern_proto_handler(unsigned int index)
{
    if (index >= external_proto_handler_.size())
    {
        return NULL;
    }
    else
    {
        return external_proto_handler_[index];
    }
}

//����ʵ������
Zerg_Comm_Manager *Zerg_Comm_Manager::instance()
{
    //���û�г�ʼ��
    if (instance_ == NULL)
    {
        instance_ = new Zerg_Comm_Manager();
    }

    return instance_;
}

//����ʵ��
void Zerg_Comm_Manager::clean_instance()
{
    if (instance_)
    {
        delete instance_;
        instance_ = NULL;
    }
}

void
Zerg_Comm_Manager::stat_heart_beat_gap(const Zerg_App_Frame * proc_frame)
{
    ZCE_Time_Value recv_time, send_time, dst_recv_time, dst_send_time;
    recv_time.gettimeofday();

    // ��У������������û������������ֱ�Ӷ���
    if (proc_frame->frame_length_ == Zerg_App_Frame::LEN_OF_APPFRAME_HEAD)
    {
        return;
    }

    HeartBeatPkg heart_beat_pkg;

    int ret = proc_frame->appdata_decode(heart_beat_pkg);

    if (ret != SOAR_RET::SOAR_RET_SUCC)
    {
        ZLOG_ERROR("[%s] app data decode fail, ret=%d, send svr %d:%d, ",
            __ZCE_FUNCTION__, ret,
            proc_frame->send_service_.services_type_,
            proc_frame->send_service_.services_id_);
        return ;
    }

    // ��ȡ����ʱ��㷢��ʱ��
    send_time.set(heart_beat_pkg.zerg_send_req_time_.sec_, heart_beat_pkg.zerg_send_req_time_.usec_);
    dst_recv_time.set(heart_beat_pkg.zerg_recv_req_time_.sec_, heart_beat_pkg.zerg_recv_req_time_.usec_);
    dst_send_time.set(heart_beat_pkg.zerg_recv_rsp_time_.sec_, heart_beat_pkg.zerg_recv_rsp_time_.usec_);

    unsigned int total_use_time = (unsigned int)(recv_time.total_msec() - send_time.total_msec());
    unsigned int app_use_time = (unsigned int)(dst_send_time.total_msec() - dst_recv_time.total_msec());
    unsigned int zerg_use_time = total_use_time - app_use_time;

    ZLOG_DEBUG("[%s] heart beat gap dst type [%dms]: total [%dms], app [%dms], zerg [%dms]", __ZCE_FUNCTION__,
        proc_frame->send_service_.services_type_,
        total_use_time, app_use_time, zerg_use_time);

    stat_heart_beat_distribute(proc_frame, total_use_time, ZERG_HEART_BEAT_RECV_TIME_GAP);
    stat_heart_beat_distribute(proc_frame, app_use_time, ZERG_HEART_BEAT_APP_RECV_TIME_GAP);
}

void
Zerg_Comm_Manager::proc_zerg_heart_beat(Zerg_App_Frame * recv_frame)
{
    ZCE_Time_Value zerg_recv_time;
    HeartBeatPkg heart_beat_pkg;

    // ��У������������û������������ֱ�Ӷ���
    if (recv_frame->frame_length_ == Zerg_App_Frame::LEN_OF_APPFRAME_HEAD)
    {
        return;
    }

    int ret = recv_frame->appdata_decode(heart_beat_pkg);

    if (ret != SOAR_RET::SOAR_RET_SUCC)
    {
        ZLOG_ERROR("[%s] app data decode fail, ret=%d, send svr %d:%d",
            __ZCE_FUNCTION__, ret,
            recv_frame->send_service_.services_type_,
            recv_frame->send_service_.services_id_);
        return ;
    }

    // ���Ϸ��͵�ʱ��
    zerg_recv_time.gettimeofday();

    if (recv_frame->is_zerg_heart_beat_req())
    {
        heart_beat_pkg.zerg_recv_req_time_.sec_ = (uint32_t)zerg_recv_time.sec();
        heart_beat_pkg.zerg_recv_req_time_.usec_ = (uint32_t)zerg_recv_time.usec();
    }
    else if (recv_frame->is_zerg_heart_beat_rsp())
    {
        heart_beat_pkg.zerg_recv_rsp_time_.sec_ = (uint32_t)zerg_recv_time.sec();
        heart_beat_pkg.zerg_recv_rsp_time_.usec_ = (uint32_t)zerg_recv_time.usec();
    }
    else
    {
        ZLOG_ERROR("[%s] error command %d", __ZCE_FUNCTION__,
            recv_frame->frame_command_);
        return;
    }

    // �ٴ��ȥ
    ret = recv_frame->appdata_encode(Zerg_App_Frame::MAX_LEN_OF_APPFRAME_DATA, heart_beat_pkg);

    if (ret != SOAR_RET::SOAR_RET_SUCC )
    {
        ZLOG_ERROR("[%s]app data encode fail. ret=%d", __ZCE_FUNCTION__, ret);
        return ;
    }
}

void
Zerg_Comm_Manager::stat_heart_beat_distribute(const Zerg_App_Frame * proc_frame, unsigned int use_time,
                                              ZERG_MONITOR_FEATURE_ID feature_id)
{
    // ����ͳ��
    server_status_->increase_by_statid(feature_id, proc_frame->app_id_,
        proc_frame->send_service_.services_type_, use_time);
    server_status_->increase_once(feature_id + 1, proc_frame->app_id_,
        proc_frame->send_service_.services_type_);

    // �ֲ�ͳ��
    const uint32_t FIFTY_MSEC = 50;
    const uint32_t HNDRD_MSEC = 100;
    const uint32_t FIF_HNDRD_MSEC = 500;
    if (use_time < FIFTY_MSEC)
    {
        server_status_->increase_once(feature_id + 2, proc_frame->app_id_,
            proc_frame->send_service_.services_type_);
    }
    else if ((use_time >= FIFTY_MSEC) && (use_time < HNDRD_MSEC))
    {
        server_status_->increase_once(feature_id + 3, proc_frame->app_id_,
            proc_frame->send_service_.services_type_);
    }
    else if ((use_time >= HNDRD_MSEC) && (use_time < FIF_HNDRD_MSEC))
    {
        server_status_->increase_once(feature_id + 4, proc_frame->app_id_,
            proc_frame->send_service_.services_type_);
    }
    else
    {
        server_status_->increase_once(feature_id + 5, proc_frame->app_id_,
            proc_frame->send_service_.services_type_);
    }
}

int Zerg_Comm_Manager::send_single_buf( Zerg_Buffer * tmpbuf )
{
    //���ʹ�����־��process_send_data�����ڲ���������������ظ���¼
    int ret = TCP_Svc_Handler::process_send_data(tmpbuf);

    Zerg_App_Frame *proc_frame = reinterpret_cast<Zerg_App_Frame *>(tmpbuf->buffer_data_);

    //���ʧ�ܹ黹���棬����ɹ�������£�����뷢�Ͷ��У����뷢�Ͷ��еĹ黹�������һ��
    if (ret != SOAR_RET::SOAR_RET_SUCC)
    {
        //��¼��������
        if (proc_frame->frame_option_ & Zerg_App_Frame::DESC_SEND_FAIL_RECORD )
        {
            ZLOG_ERROR("[zergsvr] A Frame frame len[%u] cmd[%u] uin[%u] recv_service[%u|%u] proxy_service[%u|%u] send_service[%u|%u] option [%u],ret =%d Discard!",
                       proc_frame->frame_length_,
                       proc_frame->frame_command_,
                       proc_frame->frame_command_,
                       proc_frame->recv_service_.services_type_,
                       proc_frame->recv_service_.services_id_,
                       proc_frame->proxy_service_.services_type_,
                       proc_frame->proxy_service_.services_id_,
                       proc_frame->send_service_.services_type_,
                       proc_frame->send_service_.services_id_,
                       proc_frame->frame_option_,
                       ret
                      );
        }

        //
        server_status_->increase_once(ZERG_SEND_FAIL_COUNTER,
            proc_frame->app_id_);
        if (proc_frame->recv_service_.services_type_ == 0)
        {
            // ��Ӧ�ó���0��services_type
            server_status_->increase_once(ZERG_SEND_FAIL_COUNTER_BY_SVR_TYPE,
                proc_frame->app_id_, proc_frame->recv_service_.services_type_);
        }
        //
        zbuffer_storage_->free_byte_buffer(tmpbuf);

        return ret;
    }

    return SOAR_RET::SOAR_RET_SUCC;
}





