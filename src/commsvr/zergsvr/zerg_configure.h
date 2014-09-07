#ifndef ZERG_SERVER_CONFIG_H_
#define ZERG_SERVER_CONFIG_H_


/*!
* @brief     С����������� 
*             
* @note       
*/
struct ZERG_CONFIG_DATA
{

public:

    ///����
    static const size_t MAX_SELF_SERVICES_ID = 4;

    ///���ļ�ص�FRAME������,��ϣ��̫��,��������Ӱ��Ч��
    static const size_t MAX_MONITOR_FRAME_NUMBER = 16;

    ///�����������ӷ���������
    static const size_t MAX_AUTO_CONNECT_SVRS = 512;
    
    ///���ĸ��ټ�¼�������������ͣ������Զ�ѡ��·�ɣ��㲥��
    static const size_t MAX_RECORD_SERVICES_TYPE = 16;

    ///Ĭ�ϵ�ACCEPT�˿ڵ�backlog��LightHTTP���ֵ��������1024�������Ҫ��Ȼ���½�ܶ��ˣ����Ե���������
    static const int DEFUALT_ZERG_BACKLOG = 512;

    ///
    static const uint32_t DEFUALT_CONNECT_SEND_DEQUE_SIZE = 128;
    ///
    static const uint32_t DEFUALT_ACCEPT_SEND_DEQUE_SIZE = 32;

public:
    
    //������֧�ֵ�SVC ID������������>=1
    size_t valid_svc_num_=0;
    //������֧�ֵ�SVC ID��ע�⣬self_svc_id_��Զ�����ڵ�һ��
    SERVICES_ID  bind_svcid_ary_[MAX_SELF_SERVICES_ID];

    //�������ӵķ���������
    size_t auto_connect_num_ = 0;
    //�������ӵķ���������
    SERVICES_ID  auto_connect_svrs_[MAX_AUTO_CONNECT_SVRS];

    //ZERG�ı����Ƿ�ʹ��
    bool zerg_insurance_ = true;

    int accept_backlog_ = DEFUALT_ZERG_BACKLOG;

    // �������ӵķ���BUFFER���������FRAME�ĸ��� �������ٶ������ϴ�ĺ�˷���������Ĵ�һЩ, ĿǰDB��256������ȫ����32
    uint32_t acpt_send_deque_size_ = DEFUALT_ACCEPT_SEND_DEQUE_SIZE;
    // ÿ��connect ��ȥ�ģ�tcp���ӵķ��Ͷ��г���
    uint32_t cnnt_send_deque_size_ = DEFUALT_CONNECT_SEND_DEQUE_SIZE;

    // #��CONNECT���յ�����,��Сʱ��,0-50��������������>0,����15-60������
    uint32_t connect_timeout_ = 60; 
    // RECEIVEһ�����ݵĳ�ʱʱ��,Ϊ0��ʾ������,�������ҵ�����߼��ж�һ��
    uint32_t recv_timeout_ = 0; 

    // �Ƿ���Ϊ���������
    bool is_proxy_ = false; 

    // #������ӵķ��������� ##ǰ��128000�����1024
    uint32_t max_accept_svr_ = 1024;

    // #��һ�����������ظ����Է��͵Ĵ���,Ŀǰ���ֵû���ô��ˣ�
    uint32_t retry_error_ = 3;

    // ���չܵ�����, Ĭ��50M
    uint32_t recv_pipe_len_ = 50 * 1024 * 1024;
    // ���͹ܵ�����, Ĭ��50M
    uint32_t send_pipe_len_ = 50 * 1024 * 1024;

    ///�ܾ����������ӵ�IP��ַ�б��ÿո�ֿ�
    std::string reject_ip_;
    ///�������ӵ�IP��ַ�б��ÿո�ֿ�
    std::string allow_ip_;

    ///��ص����������
    size_t monitor_cmd_count_ = 0;
    ///��ص�����
    uint32_t monitor_cmd_list_[MAX_MONITOR_FRAME_NUMBER];
};




/*!
* @brief      С������ô�����
*             
* @note       
*/
class Zerg_Server_Config :public Server_Config_Base
{
public:
    //
    Zerg_Server_Config();
    virtual ~Zerg_Server_Config();

    //
public:

    /*!
    * @brief      ȡ������Ϣ,ȡ��������Ϣ��, ��Ҫ����������������OK
    * @return     virtual int
    * @param      argc
    * @param      argv
    */
    virtual int initialize(int argc, const char *argv[]);


    ///��ȡ�����ļ����õ��ļ����ò���
    virtual int load_cfgfile();


    /*!
    * @brief      ��ȡ������־�������һЩ������Ϣ���Ա���ٻ���
    * @param      out_lvl �������
    */
    virtual void dump_cfg_info(ZCE_LOG_PRIORITY out_lvl);


    //����SVCID�õ�SVC INFO��ַ��Ϣ
    int get_svcinfo_by_svcid(const SERVICES_ID &svc_id, SERVICES_INFO  &svc_info) const;
    

public:

    ///ZERG����������
    ZERG_CONFIG_DATA  zerg_cfg_data_;

    ///��������ü��ϱ�
    SERVICES_INFO_TABLE  services_info_cfg_;
};

#endif //_ZERG_SERVER_CONFIG_H_

