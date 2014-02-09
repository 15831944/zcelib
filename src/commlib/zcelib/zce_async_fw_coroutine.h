

#ifndef ZCE_LIB_ASYNC_FRAMEWORK_COROUTINE_
#define ZCE_LIB_ASYNC_FRAMEWORK_COROUTINE_


#include "zce_boost_lord_rings.h"
#include "zce_os_adapt_coroutine.h"

//------------------------------------------------------------------------------------

class ZCE_CRTNAsync_Coroutine
{

public:

    ///Э�̵�״̬ö��
    enum STATE_COROUTINE
    {
        STATE_RUNNIG  = 1,
        STATE_ENDRUN,
        STATE_TIMEOUT,
        STATE_EXIT,
    };

public:
    ZCE_CRTNAsync_Coroutine();
protected:
    ~ZCE_CRTNAsync_Coroutine();

public:

    ///ȡ��Э�̵ľ��
    coroutine_t *get_handle();

    ///Э�̶�������к���
    void coroutine_do();
    
    ///�л���Main
    void switch_to_main();

    ///�л���Э�̣�Ҳ�����л������Լ�����
    void switch_to_coroutine();

    ///����Э�̵�״̬
    void set_state(ZCE_CRTNAsync_Coroutine::STATE_COROUTINE state);

    ///Э������������ʼ������
    virtual int coroutine_start();

    ///Э������
    virtual int coroutine_run() = 0;

    ///Э�̽��������������ͷ���Դ������
    virtual int coroutine_end();
    
    ///
    ZCE_CRTNAsync_Coroutine *clone();
public:

    ///static ����������Э�����к���������Э�̶�������к���
    static void coroutine_do(ZCE_CRTNAsync_Coroutine *);

protected:

    ///
    coroutine_t      handle_;

    ///
    STATE_COROUTINE  state_;

};

//------------------------------------------------------------------------------------

class ZCE_CRTNAsync_Main
{

protected:
    
    //
    typedef ZCE_LIB::lordrings<ZCE_CRTNAsync_Coroutine *>                REG_COROUTINE_POOL;
    //
    typedef std::unordered_map<unsigned int, REG_COROUTINE_POOL* >       ID_TO_REGCOR_POOL_MAP;

    //
    typedef std::unordered_map<unsigned int, ZCE_CRTNAsync_Coroutine * > CMD_TO_COROUTINE_MAP;

public:

    ZCE_CRTNAsync_Main();
    virtual ~ZCE_CRTNAsync_Main();


    ///��ʼ����
    int initialize(size_t crtn_type_num = DEFUALT_CRTN_TYPE_NUM,
        size_t running_number = DEFUALT_RUNNIG_CRTN_SIZE);

    ///ע��һ��Э�̣�����reg_cmd��Ӧ��
    int register_cmd(unsigned int reg_cmd,
        ZCE_CRTNAsync_Coroutine* coroutine_base,
        size_t init_clone_num);

    ///
    int active_coroutine(unsigned int cmd);

    ///
    int switch_to_coroutine(unsigned int id);

protected:

    ///
    static const size_t DEFUALT_CRTN_TYPE_NUM = 512;
    ///
    static const size_t DEFUALT_INIT_POOL_SIZE = 8;
    ///
    static const size_t DEFUALT_RUNNIG_CRTN_SIZE = 1024;
    ///
    static const size_t DEFUALT_POOL_ADD_CRTN_SIZE = 256;

protected:
    
    //����ID������
    unsigned int           corout_id_builder_;
    

    //Э�̵ĳ��ӣ�����ע�������
    ID_TO_REGCOR_POOL_MAP  coroutine_pool_;

    ///�������е�Э��
    CMD_TO_COROUTINE_MAP   running_coroutine_;


};


#endif //#ifndef ZCE_LIB_ASYNC_FRAMEWORK_COROUTINE_

