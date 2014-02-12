

#ifndef ZCE_LIB_ASYNC_FRAMEWORK_COROUTINE_
#define ZCE_LIB_ASYNC_FRAMEWORK_COROUTINE_


#include "zce_boost_lord_rings.h"
#include "zce_os_adapt_coroutine.h"

//------------------------------------------------------------------------------------


/*!
* @brief      Э�̶���
*             
*/
class ZCE_CRTNAsync_Coroutine
{
    friend class ZCE_CRTNAsync_Main;
public:

    ///Э�̵�״̬ö��
    enum STATE_COROUTINE
    {
        STATE_RUNNIG  = 1,
        STATE_END,
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

    ///
    inline void set_command(unsigned int cmd);
    ///

public:

    ///static ����������Э�����к���������Э�̶�������к���
    static void static_do(ZCE_CRTNAsync_Coroutine *);

protected:

    ///
    coroutine_t      handle_;

    ///��Ӧ���������
    unsigned int     command_;

    ///
    unsigned int     coroutine_id_;

    ///
    STATE_COROUTINE  state_;
    

};

//------------------------------------------------------------------------------------


/*!
* @brief      ��Э�̣����ع�����
*             
*/
class ZCE_CRTNAsync_Main
{

protected:
    
    //
    typedef ZCE_LIB::lordrings<ZCE_CRTNAsync_Coroutine *>                REG_COROUTINE_POOL;
    //
    typedef std::unordered_map<unsigned int, REG_COROUTINE_POOL>         ID_TO_REGCOR_POOL_MAP;

    //
    typedef std::unordered_map<unsigned int, ZCE_CRTNAsync_Coroutine * > CMD_TO_COROUTINE_MAP;

public:

    //
    ZCE_CRTNAsync_Main();
    virtual ~ZCE_CRTNAsync_Main();


    /*!
    * @brief      ��ʼ�������Ƹ��ֳ��ӣ������Ĵ�С
    * @return     int
    * @param      crtn_type_num
    * @param      running_number
    * @note       
    */
    int initialize(size_t crtn_type_num = DEFUALT_CRTN_TYPE_NUM,
        size_t running_number = DEFUALT_RUNNIG_CRTN_SIZE);

    ///ע��һ��Э�̣�����reg_cmd��Ӧ��
    int register_coroutine(unsigned int reg_cmd,
        ZCE_CRTNAsync_Coroutine* coroutine_base,
        size_t init_clone_num,
        size_t stack_size);

    ///����һ��Э��
    int active_coroutine(unsigned int cmd,unsigned int *id);

    ///�л���ID��Ӧ���Ǹ��߳�
    int switch_to_coroutine(unsigned int id);

protected:

    ///����һ��Э��
    int allocate_coroutine(unsigned int cmd);

    ///�ͷ�һ��Э��
    int free_coroutine(unsigned int cmd);
protected:


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

