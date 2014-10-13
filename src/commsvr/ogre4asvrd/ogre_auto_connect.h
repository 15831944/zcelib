
//Jovi(HuangHao),Sail(ZENGXING)

#ifndef OGRE_SERVER_AUTO_CONNECT_H_
#define OGRE_SERVER_AUTO_CONNECT_H_


#include "ogre_configure.h"

class Ogre_TCP_Svc_Handler;



typedef std::vector <TCP_PEER_MODULE_INFO> ARRAY_OF_PEER_FPRECV_MODULE;




class Ogre_Connect_Server
{
public:

    //���캯��
    Ogre_Connect_Server();
    ~Ogre_Connect_Server();

    ///��ȡ����
    int get_config(const Ogre_Server_Config *config);

    //�������еķ�����
    int connect_all_server(size_t &szserver, size_t &szsucc) ;


    //����SVRINFO,����Ƿ����������ӵķ���.����������
    int connect_server_by_peerid(const SOCKET_PERR_ID &socket_peer) ;


protected:

    ///
    ZCE_Socket_Connector ogre_connector_;

    ///
    size_t auto_connect_num_ = 0;
    ///
    TCP_PEER_MODULE_INFO     autocnt_module_ary_[OGRE_CONFIG_DATA::MAX_AUTO_CONNECT_PEER_NUM];

};

#endif //OGRE_SERVER_AUTO_CONNECT_H_

