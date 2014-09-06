
#ifndef ZERG_TCP_SERVICES_INFO_SET_H_
#define ZERG_TCP_SERVICES_INFO_SET_H_



class TCP_Svc_Handler;


/*!
* @brief      
*             
* @note       
*/
class Service_Info_Set
{
protected:

    //
    typedef unordered_map<SERVICES_ID, TCP_Svc_Handler *, HashofSvrInfo> MapOfSvrPeerInfo;
    //
    MapOfSvrPeerInfo  svr_info_set_;

public:
    //���캯��,
    Service_Info_Set();
    ~Service_Info_Set();

    //
    void init_services_peerinfo(size_t szpeer);

    //��ѯ������Ϣ
    int find_services_peerinfo(const SERVICES_ID &svrinfo,
        TCP_Svc_Handler * &);

    //��������������Ϣ
    int add_services_peerinfo(const SERVICES_ID &svrinfo,
        TCP_Svc_Handler *new_svchdl);

    //��������������Ϣ
    int replace_services_peerInfo(const SERVICES_ID &svrinfo,
        TCP_Svc_Handler *new_svchdl,
        TCP_Svc_Handler *&old_svchdl);

    //ɾ��������Ϣ
    size_t del_services_peerInfo(const SERVICES_ID &svrinfo);

    //��ǰ������
    size_t get_services_peersize();

    //
    void dump_svr_peerinfo(std::ostringstream &ostr_stream, size_t startno, size_t numquery);

    //�ر����е�PEER
    void clear_and_closeall();

};



#endif //_ZERG_TCP_SERVICES_INFO_SET_H_


