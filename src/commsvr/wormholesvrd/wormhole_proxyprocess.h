#ifndef WORMHOLE_PROXY_PROCESS_H_
#define WORMHOLE_PROXY_PROCESS_H_


/*!
* @brief      
*             
* @note       
*/
class Interface_WH_Proxy
{
public:

    // ���������
    enum PROXY_TYPE
    {
        PROXY_TYPE_INVALID           = 0, // ��Ч��TYPE
        PROXY_TYPE_ECHO              = 1, // �����е��������ݻ���
        PROXY_TYPE_TRANSMIT          = 2, // ֱ�ӽ���ת��������������֡�����κδ���
        PROXY_TYPE_DB_MODAL          = 3, // ����UINȡģ����Proxyת��������DBServer�ͽ��ڷ�����
        PROXY_TYPE_COPY_TRANS_ALL    = 4, // �����ݸ���ת�����������õķ�����
        PROXY_TYPE_DB_MODAL_MG       = 5, // ����APPID��UIN����Proxyת��
    };

public:

    ///���캯��
    Interface_WH_Proxy();
    virtual ~Interface_WH_Proxy();

    //��ʼ�������ʵ��
    virtual int init_proxy_instance();

    /// ͨ�������ļ�ȡ�ô����������Ϣ,�������ֻȡ��������ֵĴ���,Ҫʹ��ָ���������
    virtual int get_proxy_config(const ZCE_Conf_PropertyTree *conf_tree);

    ///����Ĵ���,����������֡�ĸ���
    virtual int process_proxy(Zerg_App_Frame *proc_frame) = 0;


protected:

    /*!
    * @brief      ����Ĺ�����,��������ʲô��չ��Ҫ,���Է������,
    * @return     int
    * @param      cmd  Ҫ����CMD
    * @note
    */
    inline int filter_command(unsigned long cmd);

public:
    
    /*!
    * @brief      ����ӿ�����Ĺ���
    * @return     Interface_WH_Proxy*
    * @param      proxytype
    */
    static Interface_WH_Proxy *create_proxy_factory(PROXY_TYPE proxytype);


protected:

    // Ĭ�ϳ�ʼ���ĳ���
    static const size_t INIT_PROCESS_FRAME = 64;


    //
    Zerg_MMAP_BusPipe *zerg_mmap_pipe_;
};






/*!
* @brief      ���ʹ�������
*             
* @note       
*/
class Echo_Proxy_Process : public Interface_WH_Proxy
{

protected:

public:
    // Echo����
    Echo_Proxy_Process();
    virtual ~Echo_Proxy_Process();

    virtual int get_proxy_config(const ZCE_Conf_PropertyTree *conf_tree);
    // ���д���Ĵ���
    virtual int process_proxy(Zerg_App_Frame *proc_frame);
};




/****************************************************************************************************
Transmit_Proxy ֱ�ӽ���ת�����������κδ����Proxy��ʽ
****************************************************************************************************/
class Transmit_Proxy : public Interface_WH_Proxy
{
public:
    Transmit_Proxy();
    virtual ~Transmit_Proxy();
    //
    virtual int get_proxy_config(const ZCE_Conf_PropertyTree *conf_tree);
    //
    virtual int process_proxy(Zerg_App_Frame *proc_frame);
};



/****************************************************************************************************
class  DBModalProxyProcess DBȡģ��������ת���Ĵ���ʽ
****************************************************************************************************/

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

class DBModalProxyProcess : public Interface_WH_Proxy
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
    virtual int get_proxy_config(const ZCE_Conf_PropertyTree *conf_tree);
    //
    virtual int process_proxy(Zerg_App_Frame *proc_frame);
};



/****************************************************************************************************
class  CopyTransmitAllProxyProcess �����ݸ���ת�����������õķ�����
****************************************************************************************************/
class CopyTransmitAllProxyProcess : public Interface_WH_Proxy
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
    virtual int get_proxy_config(const ZCE_Conf_PropertyTree *conf_tree);
    //
    virtual int process_proxy(Zerg_App_Frame *proc_frame);
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
class DBModalMGProxyProcess : public Interface_WH_Proxy
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
    virtual int get_proxy_config(const ZCE_Conf_PropertyTree *conf_tree);
    //
    virtual int process_proxy(Zerg_App_Frame *proc_frame);

private:
    //
    DBModalMGProxyInfo *add_proxy();
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


#endif  //WORMHOLE_PROXY_PROCESS_H_

