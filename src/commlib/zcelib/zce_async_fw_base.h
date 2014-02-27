

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
    virtual int initialize();

    ///Э������
    virtual int run() = 0;

    ///Э�̽��������������ͷ���Դ������
    virtual int finish();

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

/*!
* @brief      �첽����Ĺ���������
*
*/
class ZCE_Async_ObjectMgr
{

protected:

    ///�첽������ӣ�
    typedef ZCE_LIB::lordrings<ZCE_Async_Object *>  ASYNC_OBJECT_POOL;

    ///
    struct ASYNC_OBJECT_RECORD
    {
        //�첽������ӣ�
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

    ///�첽����������Ĺ��캯��
    ZCE_Async_ObjectMgr(ZCE_Timer_Queue *timer_queue);
    virtual ~ZCE_Async_ObjectMgr();


    /*!
    * @brief      ��ʼ�������Ƹ��ֳ��ӣ������Ĵ�С
    * @return     int
    * @param      crtn_type_num
    * @param      running_number
    */
    int initialize(size_t crtn_type_num = DEFUALT_ASYNC_TYPE_NUM,
        size_t running_number = DEFUALT_RUNNIG_ASYNC_SIZE);

    /*!
    * @brief      ע��һ��Э�̣�����reg_cmd��Ӧ��
    * @return     int 
    * @param      reg_cmd
    * @param      async_base
    * @param      init_clone_num
    */
    int register_asyncobj(unsigned int reg_cmd,
        ZCE_Async_Object* async_base,
        size_t init_clone_num);

    /*!
    * @brief      ����һ��Э��
    * @return     int
    * @param      cmd
    * @param      id
    */
    int active_asyncobj(unsigned int cmd, unsigned int *id);




protected:

    ///�ӳ����������һ��
    int allocate_from_pool(unsigned int cmd, ZCE_Async_Object *&alloc_aysnc);

    ///�ͷ�һ���첽���󵽳�������
    int free_to_pool(ZCE_Async_Object *free_async);

protected:

    ///Ĭ�ϵ��첽������������
    static const size_t DEFUALT_ASYNC_TYPE_NUM = 1024;
    ///ÿ���첽������ӵĳ�ʼ��������
    static const size_t DEFUALT_INIT_POOL_SIZE = 2;
    ///Ĭ��ͬʱ���е�һ���ֶ��������
    static const size_t DEFUALT_RUNNIG_ASYNC_SIZE = 1024;
    ///Ĭ�ϳ�����չ��ʱ����չ���첽���������
    static const size_t POOL_EXTEND_ASYNC_NUM = 128;

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

