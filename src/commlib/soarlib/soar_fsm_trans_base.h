
#ifndef SOARING_LIB_TRANSACTION_BASE_H_
#define SOARING_LIB_TRANSACTION_BASE_H_

#include "soar_zerg_frame.h"
#include "soar_fsm_trans_mgr.h"
#include "soar_error_code.h"

class Zerg_App_Frame;
class Soar_MMAP_BusPipe;

//���������װ������̬����ΪSTATIC��,�������������ʵ����������

class SOARING_EXPORT Transaction_Base : public ZCE_Timer_Handler
{
    friend class Transaction_Manager;
protected:
    //����״̬
    enum TRANSACTION_RUN_STATE
    {
        //��ʼ����״̬
        INIT_RUN_STATE       = 1,
        //������״̬
        RIGHT_RUN_STATE      = 2,
        //��ʱ��״̬
        TIMEOUT_RUN_STATE    = 3,
        //�쳣��״̬
        EXCEPTION_RUN_STATE  = 5,
    };

    //������ʽ
    enum TRANSACTION_PROCESS
    {
        //�ȴ�״̬,����ĳ����������Ϣ��ʹ��ڵȴ�״̬��
        WAIT_PROCESS   = 1,
        //����ִ��״̬,���ڽ���ʱ�䴦��ֳ�С��ʱ��Ƭ��
        NEXT_PROCESS   = 2,
        // ���ｫ�����˳���ֳɳɹ��˳���ʧ���˳���Ŀ����Ϊ�˿���ܼ����������w
        //������ɹ�����״̬,����ɾ��������,����handle_close
        EXIT_PROCESS_SUCC   = 3,
        //������ʧ�ܽ���״̬,����ɾ��������,����handle_close
        EXIT_PROCESS_FAIL   = 4,
    };

    enum WAIT_TIMEOUT_VALUE
    {
        TOO_SHORT_SOARING_LIB_TIME_OUT         = 5,
        SHORT_SOARING_LIB_TIME_OUT             = 10,
        NORMAL_SOARING_LIB_TIME_OUT            = 15,
        MIDDING_SOARING_LIB_TIME_OUT           = 30,
        LONG_SOARING_LIB_TIME_OUT              = 60,
        LONG_LONG_SOARING_LIB_TIME_OUT         = 120,
        TOO_LONG_SOARING_LIB_TIME_OUT          = 240,
        TOO_TOO_LONG_TIME_OUT               = 480,
    };



protected:

    //˽�еĹ��캯��,�����ʹ��NEW�õ�
    virtual ~Transaction_Base();

public:
    //���캯��
    Transaction_Base(ZCE_Timer_Queue *timer_queue,
                     Transaction_Manager *pmngr,
                     bool trans_locker = false);

    //ÿ�����½��г�ʼ��ʱ������飬�ȵȣ�������֤���������������ȵ��ã����ƹ��캯��
    virtual void re_init() = 0;
    //���պ�Ĵ���������Դ���ͷţ��ȵȣ�������֤�����������������ã���������������
    virtual void finish();

    //�����ĵ��ú���,
    virtual Transaction_Base *create_self(ZCE_Timer_Queue *timer_queue,
                                          Transaction_Manager *pmngr) = 0;

    //��ʼ��,���������������̳еĳ�ʼ��������β��ʹ��,
    virtual TRANSACTION_PROCESS on_init() = 0;

    //���к���,�����̳еĺ���,ϵͳ��״̬�������е�ʱ������������,TOUCH OUT������Ҳ�����������
    virtual TRANSACTION_PROCESS on_active() = 0;

    //��ʱ����������Ĭ��ʵ�֣�������ڳ�ʱʱҪ�������ݣ����ҪҪ�����������
    virtual TRANSACTION_PROCESS on_timeout();

    //�쳣����������Ĭ��ʵ�֣�������ڶ�ʱ����Ҫ�������ݣ����ҪҪ�����������
    virtual TRANSACTION_PROCESS on_exception();

    //�˳��ĵ��õĺ���.
    virtual int on_exit();

    //�õ�������FRAME���ݣ�ע��ѽ��������˽������ݵ�ʱ����(�׶�)���������ֵ����ʧЧ�ˡ��мǡ��мǡ�
    inline Zerg_App_Frame *get_recv_appframe();

    //�õ���ǰ�Ľ׶�
    inline int get_phase();
    //�õ�����ID
    inline int get_transaction_id() const;

    inline unsigned int get_wait_cmd() const;

    //�õ������IP��ַ���������
    inline unsigned int get_req_ipaddress() const;
    //�õ����������
    inline unsigned int get_req_cmd() const;

    //�õ�������
    inline  Transaction_Manager *get_trans_mgr() const;

    //��������ӿڱ�¶����,��Ҫ�ǿ��ǵ���һ�����������ط�Ҫ����Transָ�룬Ȼ��ʹ��
public:

    //--------------------------------------------------------------------
    //�������麯����ز�Ҫ���ʹ��,���;������ڻ���,���͸���������������send_msg_to,request_send_to_proxy

    //���������ݵ�SERVICES_ID������(������Proxy),ʹ�������UIN��
    template< class T> int request_send_to_peer(unsigned int cmd,
                                                const SERVICES_ID &rcv_svc,
                                                const T &info,
                                                unsigned int app_id = 0,
                                                unsigned int option = 0);

    //���������ݵ�SERVICES_ID������(������Proxy),ָ���ض�UIN,������Щ����ʹ��
    template< class T> int request_send_to_peer(unsigned int cmd,
                                                unsigned int qquin,
                                                const SERVICES_ID &rcv_svc,
                                                const T &info,
                                                unsigned int app_id = 0,
                                                unsigned int option = 0 );

    //���������ݵ�SERVICES_ID������,����ָ��UIN�ͣ����������ID
    template <class T> int request_send_to_peer(unsigned int cmd,
                                                unsigned int qquin,
                                                unsigned int backfill_trans_id,
                                                const SERVICES_ID &rcv_svc,
                                                const T &info,
                                                unsigned int app_id = 0,
                                                unsigned int option = 0
                                               );

    // ���������ݵ�SERVICES_ID������(������Proxy),ָ���ض�UIN,������Щ����ʹ��
    int request_send_buf_to_peer(unsigned int cmd,
                                 unsigned int qquin,
                                 const SERVICES_ID &rcv_svc,
                                 const unsigned char *buf,
                                 unsigned int buf_len,
                                 unsigned int app_id = 0,
                                 unsigned int option = 0
                                );

    // ���������ݵ�proxy������, ָ���ض�UIN,������
    int request_send_buf_to_proxy(unsigned int cmd,
                                  unsigned int qquin,
                                  const SERVICES_ID &proxy_svc,
                                  const SERVICES_ID &recv_svc,
                                  const unsigned char *buf,
                                  unsigned int buf_len,
                                  unsigned int app_id = 0,
                                  unsigned int option = 0
                                 );

    //--------------------------------------------------------------------------------------
    //���������ݵ����������,�������QQUIN����
    template< class T> int request_send_to_proxy(unsigned int cmd,
                                                 const SERVICES_ID &proxy_svc,
                                                 unsigned short rcvtype,
                                                 const T &info,
                                                 unsigned int app_id = 0,
                                                 unsigned int option = 0 );

    //���������ݵ����������,��ָ��UIN����
    template< class T> int request_send_to_proxy(unsigned int cmd,
                                                 unsigned int qquin,
                                                 const SERVICES_ID &proxy_svc,
                                                 unsigned short rcvtype,
                                                 const T &info,
                                                 unsigned int app_id = 0,
                                                 unsigned int option = 0);

    //���������ݵ����������,,ʹ�������QQUIN,RCV SERVICESID(�����ô�����ת��ָ��������),
    template< class T> int request_send_to_proxy(unsigned int cmd,
                                                 unsigned int qquin,
                                                 const SERVICES_ID &proxy_svc,
                                                 const SERVICES_ID &recvsvc,
                                                 const T &info,
                                                 unsigned int app_id = 0,
                                                 unsigned int option = 0);

    //���������ݵ����������,,ʹ�������QQUIN,,RCV SERVICESID(�����ô�����ת��ָ��������),���������ID
    template< class T> int request_send_to_proxy(unsigned int cmd,
                                                 unsigned int qquin,
                                                 unsigned int backfill_trans_id,
                                                 const SERVICES_ID &proxy_svc,
                                                 const SERVICES_ID &recvsvc,
                                                 const T &info,
                                                 unsigned int app_id = 0,
                                                 unsigned int option = 0);

    //--------------------------------------------------------------------------------------
    //Ӧ����ط�������,ֻ���ǳ��˽��������ʱ��ſ��Ե����������,����....
    template< class T> int response_sendback(unsigned int cmd,
                                             const T &info,
                                             unsigned int option = 0);

    //������Ϣ,Ӧ��һ������,���ǿ���ָ����Ӧ��QQUIN,ֻ���ǳ��˽��������ʱ��ſ��Ե����������,����....
    //���緢���ʼ���FRAME�е��������ռ���,���Ƿ�������Ӧ���Ƿ�����
    //ΪɶҪ��2��Ϊ�������ƶ��������أ�����Կ���һ��,�Ҳ�����
    template< class T> int response_sendback2(unsigned int cmd,
                                              unsigned int qquin,
                                              const T &info,
                                              unsigned int option = 0);

    //Ӧ����ط�������,������һ��buf,���buf����һ�������Ŀ��Э���
    // ���������response_sendback2��������info���������buf,
    // һ��ֻ����Ҫ�ص����buf,���Լ��ⲻ����ʱ��ʹ�ã�����
    int response_buf_sendback(unsigned int cmd,
                              unsigned int uin,
                              const unsigned char *buf,
                              unsigned int buf_len,
                              unsigned int option = 0);

    //--------------------------------------------------------------------------------------
    //��Ȱ��Ҳ�Ҫ���������󼯺�,������dir�İ汾��������
    template< class T> int sendmsg_to_service(unsigned int cmd,
                                              unsigned int qquin,
                                              unsigned int transaction_id,
                                              unsigned int backfill_trans_id,
                                              const SERVICES_ID &rcv_svc,
                                              const SERVICES_ID &proxy_svc,
                                              const SERVICES_ID &snd_svc,
                                              const T &tmpvalue,
                                              unsigned int app_id,
                                              unsigned int option );

    // �������ݵ�SERVICES_ID������,��󼯺�, һ���÷�װ���ĺ���
    int sendbuf_to_service(unsigned int cmd,
                           unsigned int qquin,
                           unsigned int transaction_id,
                           unsigned int backfill_trans_id,
                           const SERVICES_ID &rcv_svc,
                           const SERVICES_ID &proxy_svc,
                           const SERVICES_ID &snd_svc,
                           const unsigned char *buf,
                           size_t buf_len,
                           unsigned int app_id,
                           unsigned int option);

protected:
    //��ʼ��
    int initialize_trans(Zerg_App_Frame *proc_frame, unsigned int transid);

    //ȡ��������ʱ��
    void cancel_touch_timer();
    //ȡ����ʱ��ʱ��
    void cancel_timeout_timer();

    /*!
    @brief                   ���ó�ʱ��ʱ��
    @param  sec              ��ʱ����
    @param  usec             ��ʱ��΢����
    @param  active_auto_stop ����һ��active ���Ƿ��Զ�ȡ����ʱ��ʱ����Ĭ��Ϊfalse
    */
    int set_timeout_timer(int sec, int usec = 0, bool active_stop = true);

    //���ô�����ʱ��
    int set_timetouch_timer(int sec, int usec = 0);

    //��ʱ������
    virtual int timer_timeout(const ZCE_Time_Value &time, const void *arg);
    //�ر�
    virtual int handle_close ();

    //����������¼�
    int process_trans_event();

    //lock��ʵ��������������Ŀ���Ǳ�֤��ͬһʱ�̣�ֻ����һ���û���һ������
    //�Ե�ǰ�û��ģ���ǰ���������ֽ��м���
    int lock_qquin_cmd();
    //�Ե�ǰ�û��ģ���ǰ���������ֽ��н���
    void unlock_qquin_cmd();

    //�Ե�ǰ�û���һ����ID���м���
    int lock_qquin_key(unsigned int trnas_lock_id);
    //�Ե�ǰ�û���һ����ID���н���
    void unlock_qquin_key(unsigned int trnas_lock_id);

    //��������׶�
    inline void set_phase(int transphase);
    //���������״̬
    inline void set_state(TRANSACTION_RUN_STATE txstate);

    //����������Ҫ��������(Ҳ������ȷ��Ҫ�ȴ���������ʲô)
    inline void set_wait_cmd(unsigned int need_cmd = CMD_INVALID_CMD);
    //�����ܵ���FRAME�����ݺ�����
    virtual int check_receive_frame(const Zerg_App_Frame *recv_frame);
    //�յ�һ�����ݺ�
    virtual int receive_trans_msg(Zerg_App_Frame *proc_frame);

protected:

    //�ر�����ĵ�Service
    int close_request_service() const;

    //����û�������֡ͷ����UIN���������UIN�Ƿ�һ��,�����û�����
    inline int check_req_qquin(unsigned int qq_uin) const;

    //���ڼ�������IP��ַ�Ƿ����ڲ�IP��ַ
    inline int check_request_internal() const;

    //DUMP���е��������Ϣ
    void dump_transa_info(std::ostringstream &strstream) const;
    //DUMP��������������Ϣ
    void output_trans_info(const char *outstr = "Output Transaction Info") const;

public:
    //��Ч�Ľ׶�ID
    static const int INVALID_PHASE_ID = 0;

    //TIME ID
    static const int TRANSACTION_TIME_ID[];

    //����Է�װҪ����ϸ񣬵����Ҹо����Ⱪ¶�Ľӿ��㹻�ˡ�
protected:

    //�¼�������
    Transaction_Manager     *trans_manager_;
    //����ID
    unsigned int            transaction_id_;
    //�Ƿ��������
    bool                    trans_locker_;

    //���������UIN
    unsigned int            req_qq_uin_;
    //��������������
    unsigned int            trans_command_;

    //��������״̬
    TRANSACTION_RUN_STATE   trans_run_state_;

    //����׶�
    int                     trans_phase_;

    //��������ķ�����
    SERVICES_ID             req_snd_service_;
    //��������Ĵ���
    SERVICES_ID             req_rcv_service_;
    //��������Ĵ���
    SERVICES_ID             req_proxy_service_;

    //���������ID
    unsigned int            req_trans_id_;
    //����ĵĻỰID:�����ǻ�������id����˼����,��ʵû��ôʹ��
    unsigned int            req_session_id_;
    //�����app_id
    unsigned int            req_game_app_id_;

    //�����IP��ַ��ʲô.
    unsigned int            req_ip_address_;
    //�����FRAME�ĵ�ѡ��
    unsigned int            req_frame_option_;

    //��ǰ���յ�Frame,Ϊ��ʱ����,һ���������,
    Zerg_App_Frame         *tmp_recv_frame_;

    //����ʱ�Ķ�ʱ��ID
    int                     trans_timeout_id_;
    //����active���Ƿ��Զ�ֹͣtime out��ʱ��
    bool                    active_auto_stop_;
    //����Ķ�ʱ��ID
    int                     trans_touchtimer_id_;

    //��Ҫ��������
    unsigned int            wait_cmd_;
    //����Ĵ���ʱ��
    time_t                  trans_create_time_;

    //��־���ٵ����ȼ�
    ZCE_LOG_PRIORITY        trace_log_pri_;

    // ������Ĵ����룬����������EXIT_PROCESS_FAILʱʹ��
    int                     process_errno_;
};

//�õ����յ�������
inline Zerg_App_Frame *Transaction_Base::get_recv_appframe()
{
    return tmp_recv_frame_;
}
//�õ���ǰ�Ľ׶�
inline int Transaction_Base::get_phase()
{
    return trans_phase_;
}
//�õ�ID
inline int Transaction_Base::get_transaction_id() const
{
    return transaction_id_;
}
//�õ�CMD
inline unsigned int Transaction_Base::get_req_cmd() const
{
    return trans_command_;
}
//�õ���Ҫ��CMD
inline unsigned int Transaction_Base::get_wait_cmd() const
{
    return wait_cmd_;
}

//���ý��׽׶�
inline void Transaction_Base::set_phase(int transphase)
{
    trans_phase_ = transphase;
}

//���������״̬
inline void Transaction_Base::set_state(Transaction_Base::TRANSACTION_RUN_STATE txstate)
{
    trans_run_state_ = txstate;
}

//����������Ҫ��������
//need_cmd == 0��ʾ����������
inline void Transaction_Base::set_wait_cmd(unsigned int need_cmd)
{
    wait_cmd_ = need_cmd;
}

//�õ�������
inline  Transaction_Manager *Transaction_Base::get_trans_mgr()  const
{
    return trans_manager_;
}

//����ͷ�Ͱ����qq_uin�Լ����͵�service_id�Ƿ�һ��,
inline int Transaction_Base::check_req_qquin(unsigned int qq_uin) const
{
    if (qq_uin != req_qq_uin_)
    {
        ZLOG_MSG(RS_ERROR,"[framework] QQuin in header , body and snd_service is diffrent.body: %u| request appframe header: %u.",
                   qq_uin, req_qq_uin_);
        return SOAR_RET::ERROR_QQUIN_INCERTITUDE;
    }

    return 0;
}

//ȡ�����������IP��ַ��Ϣ
inline unsigned int Transaction_Base::get_req_ipaddress() const
{
    return req_ip_address_;
}

//���ڼ�������IP��ַ�Ƿ����ڲ�IP��ַ,�Ƿ���0�����Ƿ��ط�0
inline int Transaction_Base::check_request_internal() const
{
    return (ZCE_LIB::is_internal(req_ip_address_)) ? 0 : -1;
}

//��������Ϣ��Service,
template <class T>
int Transaction_Base::request_send_to_peer(unsigned int cmd,
                                           const SERVICES_ID &rcv_svc,
                                           const T &info,
                                           unsigned int app_id,
                                           unsigned int option)
{
    SERVICES_ID proxy_svc(0, 0);
    return sendmsg_to_service(cmd,
                              req_qq_uin_,
                              this->transaction_id_,
                              0,
                              rcv_svc,
                              proxy_svc,
                              *(trans_manager_->self_svc_info()),
                              info,
                              app_id,
                              option);
}

//��������Ϣ��Service,
template <class T>
int Transaction_Base::request_send_to_peer(unsigned int cmd,
                                           unsigned int qquin,
                                           unsigned int backfill_trans_id,
                                           const SERVICES_ID &rcv_svc,
                                           const T &info,
                                           unsigned int app_id,
                                           unsigned int option)
{
    SERVICES_ID proxy_svc(0, 0);
    return sendmsg_to_service(cmd,
                              qquin,
                              this->transaction_id_,
                              backfill_trans_id,
                              rcv_svc,
                              proxy_svc,
                              *(trans_manager_->self_svc_info()),
                              info,
                              app_id,
                              option);
}

//��ĳ��Service��������,ͬʱʹ��UIN
template< class T>
int Transaction_Base::request_send_to_peer(unsigned int cmd,
                                           unsigned int qquin,
                                           const SERVICES_ID &rcv_svc,
                                           const T &info,
                                           unsigned int app_id,
                                           unsigned int option )
{
    SERVICES_ID proxy_svc(0, 0);
    return sendmsg_to_service(cmd,
                              qquin,
                              this->transaction_id_,
                              0,
                              rcv_svc,
                              proxy_svc,
                              *(trans_manager_->self_svc_info()),
                              info,
                              app_id,
                              option);

}

//--------------------------------------------------------------------------------------------------------------------------
//���������ݵ����������,�������QQUIN����
template< class T>
int Transaction_Base::request_send_to_proxy(unsigned int cmd,
                                            const SERVICES_ID &proxy_svc,
                                            unsigned short rcvtype,
                                            const T &info,
                                            unsigned int app_id,
                                            unsigned int option)
{
    SERVICES_ID rcv_svc(rcvtype, 0);
    return sendmsg_to_service(cmd,
                              this->req_qq_uin_,
                              this->transaction_id_,
                              0,
                              rcv_svc,
                              proxy_svc,
                              *(trans_manager_->self_svc_info()),
                              info,
                              app_id,
                              option);
}

//���������ݵ����������,�ò���ָ��UIN����
template< class T>
int Transaction_Base::request_send_to_proxy(unsigned int cmd,
                                            unsigned int qquin,
                                            const SERVICES_ID &proxy_svc,
                                            unsigned short rcvtype,
                                            const T &info,
                                            unsigned int app_id,
                                            unsigned int option)
{
    SERVICES_ID rcv_svc(rcvtype, 0);
    return sendmsg_to_service(cmd,
                              qquin,
                              this->transaction_id_,
                              0,
                              rcv_svc,
                              proxy_svc,
                              *(trans_manager_->self_svc_info()),
                              info,
                              app_id,
                              option);
}

//���������ݵ����������,,ʹ�������QQUIN,RCV SERVICESID(�����ô�����ת��ָ��������),
//ע��RECV SVC ��PROXY SVC������˳��,��Ҫ�����
template< class T>
int Transaction_Base::request_send_to_proxy(unsigned int cmd,
                                            unsigned int qquin,
                                            const SERVICES_ID &proxy_svc,
                                            const SERVICES_ID &recvsvc,
                                            const T &info,
                                            unsigned int app_id,
                                            unsigned int option)
{
    return sendmsg_to_service(cmd,
                              qquin,
                              this->transaction_id_,
                              0,
                              recvsvc,
                              proxy_svc,
                              *(trans_manager_->self_svc_info()),
                              info,
                              app_id,
                              option);
}

//���������ݵ����������,ʹ�������QQUIN,,RCV SERVICESID(�����ô�����ת��ָ��������),���������ID
//ע��RECV SVC ��PROXY SVC������˳��
template< class T>
int Transaction_Base::request_send_to_proxy(unsigned int cmd,
                                            unsigned int qquin,
                                            unsigned int backfill_trans_id,
                                            const SERVICES_ID &recvsvc,
                                            const SERVICES_ID &proxy_svc,
                                            const T &info,
                                            unsigned int app_id,
                                            unsigned int option)
{
    return sendmsg_to_service(cmd,
                              qquin,
                              this->transaction_id_,
                              backfill_trans_id,
                              recvsvc,
                              proxy_svc,
                              *(trans_manager_->self_svc_info()),
                              info,
                              app_id,
                              option);
}

//--------------------------------------------------------------------------------------------------------------------------
//������Ϣ,Ӧ��һ������,ֻ���ǳ��˽��������ʱ��ſ��Ե����������,����....
template< class T>
int Transaction_Base::response_sendback(unsigned int cmd,
                                        const T &info,
                                        unsigned int option)
{

    //����UDP���صĴ��벿��
    if (req_frame_option_ & Zerg_App_Frame::DESC_UDP_FRAME)
    {
        option |= Zerg_App_Frame::DESC_UDP_FRAME;
    }

    //
    return sendmsg_to_service(cmd,
                              this->req_qq_uin_,
                              this->transaction_id_,
                              this->req_trans_id_,
                              this->req_snd_service_,
                              this->req_proxy_service_,
                              this->req_rcv_service_,
                              info,
                              this->req_game_app_id_,
                              option);
}

//������Ϣ,Ӧ��һ������,���ǿ���ָ����Ӧ��QQUIN,ֻ���ǳ��˽��������ʱ��ſ��Ե����������,����....
//���緢���ʼ���FRAME�е��������ռ���,���Ƿ�������Ӧ���Ƿ�����
//ΪɶҪ��2��Ϊ�������ƶ��������أ�����Կ���һ��,�Ҳ�����
template< class T>
int Transaction_Base::response_sendback2(unsigned int cmd,
                                         unsigned int qquin,
                                         const T &info,
                                         unsigned int option)
{
    //����UDP���صĴ��벿��
    if (req_frame_option_ & Zerg_App_Frame::DESC_UDP_FRAME)
    {
        option |= Zerg_App_Frame::DESC_UDP_FRAME;
    }

    //
    return sendmsg_to_service(cmd,
                              qquin,
                              this->transaction_id_,
                              this->req_trans_id_,
                              this->req_snd_service_,
                              this->req_proxy_service_,
                              this->req_rcv_service_,
                              info,
                              this->req_game_app_id_,
                              option);
}

//--------------------------------------------------------------------------------------------------------------------------
//����������д������Ϣ������
template< class T>
int Transaction_Base::sendmsg_to_service(unsigned int cmd,
                                         unsigned int qquin,
                                         unsigned int transaction_id,
                                         unsigned int backfill_trans_id,
                                         const SERVICES_ID &rcv_svc,
                                         const SERVICES_ID &proxy_svc,
                                         const SERVICES_ID &snd_svc,
                                         const T &info,
                                         unsigned int app_id,
                                         unsigned int option)
{
    //������������Ҫ��Ҫ��أ�����Ĵ�����м��
    if (req_frame_option_ & Zerg_App_Frame::DESC_MONITOR_TRACK)
    {
        option |= Zerg_App_Frame::DESC_MONITOR_TRACK;
    }

    //������͵�
    if (app_id == 0)
    {
        app_id = req_game_app_id_;
    }

    //���ù������ķ��ͺ���
    return trans_manager_->mgr_sendmsg_to_service(cmd,
                                                  qquin,
                                                  transaction_id,
                                                  backfill_trans_id,
                                                  rcv_svc,
                                                  proxy_svc,
                                                  snd_svc,
                                                  info,
                                                  app_id,
                                                  option);
}

#endif //SOARING_LIB_TRANSACTION_MANAGER_H_

