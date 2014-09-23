#ifndef ARBITER_PROXY_PROCESS_H_
#define ARBITER_PROXY_PROCESS_H_

/****************************************************************************************************
class Interface_Proxy_Process
****************************************************************************************************/
class InterfaceProxyProcess
{
public:

    // ���������
    enum PROXY_TYPE
    {
        PROXY_TYPE_INVALID           = 0, // ��Ч��TYPE
        PROXY_TYPE_ECHO              = 1, // �����е��������ݻ���
        PROXY_TYPE_DB_MODAL          = 2, // ����UINȡģ����Proxyת��������DBServer�ͽ��ڷ�����
        PROXY_TYPE_TRANSMIT          = 3, // ֱ�ӽ���ת��������������֡�����κδ���
        PROXY_TYPE_COPY_TRANS_ALL    = 4, // �����ݸ���ת�����������õķ�����
        PROXY_TYPE_DB_MODAL_MG       = 5, // ����APPID��UIN����Proxyת��������������ҵ��
        PROXY_TYPE_COPY_TRANS_ALL_MG = 6, // �����ݸ���ת�����������õķ�����,������ҵ��
    };

protected:

    // Ĭ�ϳ�ʼ���ĳ���
    static const size_t  INIT_PROCESS_FRAME = 64;

    // �Ƿ���֡
    uint32_t                           if_check_frame_;

    // �����������,
    hash_set<unsigned int>        proxy_processcmd_;

    //
    Zerg_MMAP_BusPipe             *zerg_mmap_pipe_;

protected:

    // ����Ĺ�����,��������ʲô��չ��Ҫ,���Է������,
    inline int FilterCommand(unsigned long cmd);

    // ��ʼ���ڴ�ܵ�
    int InitArbiterMMAPPipe();

public:

    // ���캯��
    InterfaceProxyProcess();
    virtual ~InterfaceProxyProcess();

    //
    virtual int InitProxyInstance();

    // ͨ��xml�ļ�ȡ�ô����������Ϣ,�������ֻȡ��������ֵĴ���,Ҫʹ��ָ���������
    virtual int get_proxy_config(conf_proxysvr::LPCONFIG cfg);

    // ����Ĵ���,����������֡�ĸ���
    virtual int process_proxy(Comm_App_Frame *proc_frame) = 0;

public:
    // ����ӿ�����Ĺ���
    static InterfaceProxyProcess *CreatePorxyFactory(PROXY_TYPE proxytype);
};

/******************************************************************************************
Author          : Sail ZENGXING  Date Of Creation: 2005��12��1��
Function        : Interface_Proxy_Process::FilterCommand
Return          : inline int == 0 ��ʾ�ɹ�
Parameter List  :
Param1: unsigned long cmd Ҫ����CMD
Description     : ��������,���Ƿ���Ҫ�Լ������,
Calls           :
Called By       :
Other           : Inline,
Modify Record   :
******************************************************************************************/
inline int InterfaceProxyProcess::FilterCommand(unsigned long cmd)
{
    // ��鴦�������
    if (proxy_processcmd_.size() > 0)
    {
        if (proxy_processcmd_.find(cmd) == proxy_processcmd_.end())
        {
            return TSS_RET::ERR_PROXY_APPFRAME_CMD_ERROR;
        }
    }

    // ���سɹ�
    return TSS_RET::TSS_RET_SUCC;
}

/****************************************************************************************************
class  Echo_Proxy_Process ���ʹ�������,
****************************************************************************************************/
class Echo_Proxy_Process : public InterfaceProxyProcess
{

protected:

public:
    // Echo����
    Echo_Proxy_Process();
    virtual ~Echo_Proxy_Process();

    virtual int get_proxy_config(conf_proxysvr::LPCONFIG cfg);
    // ���д���Ĵ���
    virtual int process_proxy(Comm_App_Frame *proc_frame);
};

class DBModalProxyInfo
{
public:
    // �ֲ���λ��
    unsigned int distribute_offset_;

    // �ֲ���ȡģ
    unsigned int distribute_module_;

    // ·�ɵķ���������
    unsigned short router_svr_type_;

    // ��·�����ã�
    std::vector<unsigned int> normal_router_cfg_;

    // ��¡·�ɵ����ã�
    std::vector<unsigned int> clone_router_cfg_;

};

/****************************************************************************************************
class  DBModalProxyProcess DBȡģ��������ת���Ĵ���ʽ
****************************************************************************************************/
class DBModalProxyProcess : public InterfaceProxyProcess
{

protected:
    // �Ӳ���������ƫ��ȡ8��16���,�϶����Ա�֤���������ƫ����10%����,�����㾡�ܷ���
    // �Ҳ��Ե�����ȡģ�ķ�ʽ��ģ10���Ƶķ�ʽ.

    // ����ֲ��ķ���������QQGame,����ΪOFFSETȡ8Ӧ���и��õı���,����,����
    // ȡ16���Ա�֤TC��Leader�Ƕ��ڻ���1��,��ȫ��һ,��ȫ��һ,
    // Ϊʲô�Ѿ��̶���256��ģ��,��Ϊ���ᳬ��,�պ�Ҳ���׸�,�����ҷ�������

    // ·�ɱ�, keyΪservice_type, valueΪ��Ӧ·����Ϣ
    std::map<unsigned short, DBModalProxyInfo*> dbmodal_proxy_map_;

public:
    DBModalProxyProcess();
    virtual ~DBModalProxyProcess();
    //
    virtual int get_proxy_config(conf_proxysvr::LPCONFIG cfg);
    //
    virtual int process_proxy(Comm_App_Frame *proc_frame);
};

/****************************************************************************************************
class  TransmitProxyProcess ֱ�ӽ���ת�����������κδ����Proxy��ʽ
****************************************************************************************************/
class TransmitProxyProcess : public InterfaceProxyProcess
{
public:
    TransmitProxyProcess();
    virtual ~TransmitProxyProcess();
    //
    virtual int get_proxy_config(conf_proxysvr::LPCONFIG cfg);
    //
    virtual int process_proxy(Comm_App_Frame *proc_frame);
};

/****************************************************************************************************
class  CopyTransmitAllProxyProcess �����ݸ���ת�����������õķ�����
****************************************************************************************************/
class CopyTransmitAllProxyProcess : public InterfaceProxyProcess
{
protected:
    //
    static const size_t MAX_NUM_COPY_SVC = 512;
protected:

    // Ҫ���Ƶķ���������
    unsigned short                copytrans_svctype_;
    // Ҫ���Ƶ�����
    size_t                        copytrans_svcnum_;
    // Ҫ���Ƶ�FRAME�ĳߴ�
    unsigned int copytrans_svcid_[MAX_NUM_COPY_SVC];

public:
    CopyTransmitAllProxyProcess();
    virtual ~CopyTransmitAllProxyProcess();
    //
    virtual int get_proxy_config(conf_proxysvr::LPCONFIG cfg);
    //
    virtual int process_proxy(Comm_App_Frame *proc_frame);
};

/****************************************************************************
struct DBModalMGKey DBModalMGProxyProcessʹ�õ��������key
**************************************************************************/
struct DBModalMGKey
{
public:
    uint32_t app_id_;
    uint32_t service_type_;
    bool operator<(const DBModalMGKey r)const
    {
        return ((this->app_id_!=r.app_id_)?this->app_id_<r.app_id_:this->service_type_<r.service_type_);
    }
};

struct DBModalMGRouteItem
{
public:
    // ·����ڵ�hashֵ
    uint16_t hash_;
    // ��·��id
    unsigned int normal_router_;
    // ��¡·��id
    unsigned int clone_router_;
    // ��··��id
    unsigned int passby_router_;

    bool operator<(DBModalMGRouteItem r)const
    {
        return this->hash_ < r.hash_;
    }
};

class DBModalMGProxyInfo
{
public:
    // ·�ɵķ���������
    unsigned short router_svr_type_;
    // ·�ɱ�
    std::vector<DBModalMGRouteItem> route_cfg_;

    const DBModalMGRouteItem *find_route(unsigned int uin);
};
/****************************************************************************************************
class  DBModalProxyMGProcess �����ఴ��APPID��UIN��������ת���Ĵ���ʽ
****************************************************************************************************/
class DBModalMGProxyProcess : public InterfaceProxyProcess
{
protected:
    //
    std::vector<DBModalMGProxyInfo*> dbmodal_mg_proxys_;
    //
    std::map<DBModalMGKey, DBModalMGProxyInfo*> dbmodal_mg_proxy_map_;

public:
    DBModalMGProxyProcess();
    virtual ~DBModalMGProxyProcess();
    //
    virtual int get_proxy_config(conf_proxysvr::LPCONFIG cfg);
    //
    virtual int process_proxy(Comm_App_Frame *proc_frame);

private:
    //
    DBModalMGProxyInfo *add_proxy(conf_proxysvr::RouteInfo *route_info);
    //
    int add_entry(uint32_t app_id, uint32_t service_type, DBModalMGProxyInfo *proxy_info);
    //
    const DBModalMGRouteItem *find_proxy(uint32_t app_id, uint32_t service_type, uint32_t uin, uint32_t &recv_service);
    //
    void clear_all_entrys();
};

class CopyTransmitAllMGProxyInfo
{
public:
    uint32_t service_type_;
    std::vector<uint32_t> svcid_;
};

/****************************************************************************************************
class  CopyTransmitAllMGProxyProcess �����ݸ���ת�����������õķ�����,������ҵ��
****************************************************************************************************/
class CopyTransmitAllMGProxyProcess : public InterfaceProxyProcess
{
protected:
    std::vector<CopyTransmitAllMGProxyInfo*> proxys_;
    std::map<uint32_t, CopyTransmitAllMGProxyInfo*> proxy_map_;

public:
    CopyTransmitAllMGProxyProcess();
    virtual ~CopyTransmitAllMGProxyProcess();
    //
    virtual int get_proxy_config(conf_proxysvr::LPCONFIG cfg);
    //
    virtual int process_proxy(Comm_App_Frame *proc_frame);

private:
    //
    CopyTransmitAllMGProxyInfo *add_proxy(conf_proxysvr::CopySvrIdMG *copy_svr_id);
    //
    int add_entry(uint32_t app_id, CopyTransmitAllMGProxyInfo *proxy_info);
    //
    CopyTransmitAllMGProxyInfo *find_proxy(uint32_t app_id);
    //
    void clear_all_entrys();
};

#endif  //ARBITER_PROXY_PROCESS_H_

