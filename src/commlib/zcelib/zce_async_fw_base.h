

#ifndef ZCE_LIB_ASYNC_FRAMEWORK_BASE_
#define ZCE_LIB_ASYNC_FRAMEWORK_BASE_


#include "zce_boost_lord_rings.h"
#include "zce_os_adapt_coroutine.h"
#include "zce_timer_handler_base.h"

/*!
* @brief      �첽����Ļ���
*
*/
class ZCE_Async_Object :public ZCE_Timer_Handler
{
    friend class ZCE_Async_ObjectMgr;
public:

    ///Э�̵�״̬ö��
    enum RUNNING_STATE
    {
        STATE_RUNNIG = 1,
        STATE_TIMEOUT = 2,
        STATE_EXIT = 3,
        STATE_FORCE_EXIT = 4,
    };

public:
    ZCE_Async_Object(ZCE_Async_ObjectMgr *async_mgr);
protected:
    ~ZCE_Async_Object();

public:
    

    ///Э������������ʼ������
    virtual int init();

    ///Э������
    virtual int run() = 0;

    ///Э�̽��������������ͷ���Դ������
    virtual int end_cleanup();

    ///
    virtual ZCE_Async_Object *clone(ZCE_Async_ObjectMgr *async_mgr) =0;

public:

    ///��Ч��ID
    static const unsigned int  INVALID_IDENTITY = 0;

protected:

    ///�첽����ID
    unsigned int identity_;

    ///������
    ZCE_Async_ObjectMgr *async_mgr_;

    ///��Ӧ����Ĵ��������
    unsigned int  active_cmd_;

    ///����״̬
    RUNNING_STATE  running_state_;


};


class ZCE_Timer_Queue;

class ZCE_Async_ObjectMgr
{

protected:

    ///
    typedef ZCE_LIB::lordrings<ZCE_Async_Object *>  ASYNC_OBJECT_POOL;

    ///
    struct ASYNC_OBJECT_RECORD
    {

        ASYNC_OBJECT_POOL coroutine_pool_;


        //������ͳ����Ϣ

        //���������������
        uint64_t start_num_;

        //��������������
        uint64_t end_num_;

        ///ǿ�н���������
        uint64_t force_end_num_;

        //���й��̷�����ʱ������
        uint64_t timeout_num_;

        //����ʱ״̬�쳣����������
        uint64_t exception_num_;

        //����������ʱ��
        uint64_t run_consume_ms_;
    };

    //
    typedef std::unordered_map<unsigned int, ASYNC_OBJECT_RECORD> ID_TO_REGCOR_POOL_MAP;
    //
    typedef std::unordered_map<unsigned int, ZCE_Async_Object * > ID_TO_COROUTINE_MAP;

public:

    //
    ZCE_Async_ObjectMgr(ZCE_Timer_Queue *timer_queue);
    virtual ~ZCE_Async_ObjectMgr();


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
        ZCE_Async_Object* coroutine_base,
        size_t init_clone_num,
        size_t stack_size);

    ///����һ��Э��
    int active_coroutine(unsigned int cmd, unsigned int *id);

    ///�л���ID��Ӧ���Ǹ��߳�
    int yeild_coroutine(unsigned int id);

protected:

    ///�ӳ����������һ��
    int allocate_from_pool(unsigned int cmd, ZCE_Async_Object *&crt_crtn);

    ///
    int free_to_pool(ZCE_Async_Object *);

protected:

    ///
    static const size_t DEFUALT_CRTN_TYPE_NUM = 1024;
    ///
    static const size_t DEFUALT_INIT_POOL_SIZE = 2;
    ///
    static const size_t DEFUALT_RUNNIG_CRTN_SIZE = 1024;
    ///
    static const size_t POOL_EXTEND_COROUTINE_NUM = 128;

protected:

    //����ID������
    unsigned int id_builder_;

    //Э�̵ĳ��ӣ�����ע�������
    ID_TO_REGCOR_POOL_MAP reg_coroutine_;

    ///�������е�Э��
    ID_TO_COROUTINE_MAP running_coroutine_;

    ///��ʱ���Ĺ�����
    ZCE_Timer_Queue *timer_queue_;

};

#endif //#ifndef ZCE_LIB_ASYNC_FRAMEWORK_BASE_

