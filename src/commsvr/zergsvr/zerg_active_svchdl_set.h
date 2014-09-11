
#ifndef ZERG_TCP_SERVICES_INFO_SET_H_
#define ZERG_TCP_SERVICES_INFO_SET_H_



class TCP_Svc_Handler;




/*!
* @brief      SVC ID SET
*
* @note       ��˼һ��,�Ƿ�Ӧ���������װ����Ҳ��ì�ܣ������TCP_Svc_Handlerֱ��ʹ��STL�ķ�װ���ݣ�
*             ��ô���ǵķ�װ���پۺ͵ķ�ʽ������ô�ĺá���TCP_Svc_Handler��Ҫ�ٴη�װ��������
*/
class Active_SvcHandle_Set
{
protected:

    //
    typedef unordered_map<SERVICES_ID, TCP_Svc_Handler *, HASH_OF_SVCID> MapOfSvrPeerInfo;
    //
    MapOfSvrPeerInfo  svr_info_set_;

public:
    //���캯��,
    Active_SvcHandle_Set();
    ~Active_SvcHandle_Set();

    //
    void init_services_peerinfo(size_t szpeer);

    //��ѯ������Ϣ
    int find_services_peerinfo(const SERVICES_ID &svrinfo,
                               TCP_Svc_Handler * &);


    /*!
    * @brief      ��������������Ϣ
    * @return     int
    * @param      svc_id      �����ӵķ�������SVC ID
    * @param      new_svchdl  �����ӵķ������ľ��
    */
    int add_services_peerinfo(const SERVICES_ID &svc_id,
                              TCP_Svc_Handler *new_svchdl);


    /*!
    * @brief
    * @return     int        ��������������Ϣ
    * @param      svc_id     �����ӵķ�������SVC ID
    * @param      new_svchdl �����ӵķ��������
    * @param      old_svchdl ���ԭ����һ��svrinfo��Ӧ��Hdler,����֪ͨ��,
    * @note       ���ص�old_svchdl,������������
    */
    int replace_services_peerInfo(const SERVICES_ID &svc_id,
                                  TCP_Svc_Handler *new_svchdl,
                                  TCP_Svc_Handler *&old_svchdl);


    /*!
    * @brief      ����SERVICES_ID,ɾ��PEER��Ϣ,
    * @return     size_t   ɾ������
    * @param      svc_id   Ҫɾ����������SVC ID
    * @note
    */
    size_t del_services_peerInfo(const SERVICES_ID &svc_id);

    ///��ǰ������
    size_t get_services_peersize();

    //
    void dump_svr_peerinfo(ZCE_LOG_PRIORITY out_lvl);

    //�ر����е�PEER
    void clear_and_closeall();

};



#endif //_ZERG_TCP_SERVICES_INFO_SET_H_


