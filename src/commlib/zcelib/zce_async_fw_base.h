/*!
* @copyright  2004-2014  Apache License, Version 2.0 FULLSAIL
* @filename   zce_async_fw_base.h
* @author     Sailzeng <sailerzeng@gmail.com>
* @version    
* @date       Saturday, March 01, 2014
* @brief      
*             
*             
* @details    
*             
*             
*             
* @note       
*             
*/


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
    ZCE_Async_Object(ZCE_Async_ObjectMgr *async_mgr);
protected:
    ~ZCE_Async_Object();

public:
    

    /*!
    * @brief      Э�̳�ʼ���Ĺ������ڷ������ǰִ��һ�Σ�
    * @return     int 0��ʶ�ɹ�
    */
    virtual int initialize();
    
    /*!
    * @brief      ��¡�Լ�
    * @return     ZCE_Async_Object*
    * @param      async_mgr 
    */
    virtual ZCE_Async_Object *clone(ZCE_Async_ObjectMgr *async_mgr) = 0;

    /*!
    * @brief      �첽����ʼ
    */
    virtual void on_start();

    /*!
    * @brief      �첽��������
    * @param      continue_run
    */
    virtual void on_run(bool &continue_run) = 0;

    
    /*!
    * @brief      �첽�������н��������������ͷ���Դ������
    */
    virtual void on_finish();


protected:


    /*!
    @brief                   ���ó�ʱ��ʱ��,����������ص�����
    @param  sec              ��ʱ����
    @param  usec             ��ʱ��΢����
    */
    int set_timeout_timer(int sec, int usec = 0);

    /*!
    @brief                   ���ô�����ʱ��,����������ص�����
    @param  sec              ��ʱ����
    @param  usec             ��ʱ��΢����
    */
    int set_timetouch_timer(int sec, int usec = 0);


    /*!
    * @brief      ȡ����ʱ��ʱ��
    */
    void cancel_timeout_timer();

    /*!
    * @brief      ȡ��������ʱ��
    */
    void cancel_touch_timer();
    
protected:

    ///�첽����ID
    unsigned int asyncobj_id_;

    ///������
    ZCE_Async_ObjectMgr *async_mgr_;

    ///��Ӧ����Ĵ��������
    unsigned int  create_cmd_;

    //��ʱ�Ķ�ʱ��ID
    int timeout_id_;

    //�����Ķ�ʱ��ID
    int touchtimer_id_;
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

    ///�첽�����¼
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
    typedef std::unordered_map<unsigned int, ASYNC_OBJECT_RECORD> ID_TO_REGASYNC_POOL_MAP;
    //
    typedef std::unordered_map<unsigned int, ZCE_Async_Object * > RUNNING_ASYNOBJ_MAP;

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
    * @brief      ����һ���첽����
    * @return     int
    * @param      cmd
    * @param      id
    */
    int create_asyncobj(unsigned int cmd, unsigned int *id);


    /*!
    * @brief      ����ĳ���Ѿ����е��첽����,�麯����FSM��Э���Լ�ʵ��
    * @return     int
    * @param      id 
    */
    virtual int active_asyncobj(unsigned int id) = 0;


    /*!
    * @brief      ȡ�ö�ʱ����������
    * @return     ZCE_Timer_Queue*
    */
    inline ZCE_Timer_Queue * get_timer_queue();
    
protected:

    /*!
    * @brief      �ӳ����������һ���첽����
    * @return     int
    * @param      cmd
    * @param      alloc_aysnc
    */
    int allocate_from_pool(unsigned int cmd, ZCE_Async_Object *&alloc_aysnc);

    /*!
    * @brief      �ͷ�һ���첽���󵽳�������
    * @return     int
    * @param      free_async
    */
    int free_to_pool(ZCE_Async_Object *free_async);

    
    /*!
    * @brief      ͨ��ID��Ѱ��һ���������е��첽����
    * @return     int  ����0��ʾ�ɹ�
    * @param[in]  id   ���е��첽����ı�ʶID
    * @param[out] running_aysnc ��ѯ�����첽����
    */
    int find_running_asyncobj(unsigned int id, ZCE_Async_Object *&running_aysnc);
    

protected:

    ///Ĭ�ϵ��첽������������
    static const size_t DEFUALT_ASYNC_TYPE_NUM = 1024;
    ///ÿ���첽������ӵĳ�ʼ��������
    static const size_t DEFUALT_INIT_POOL_SIZE = 2;
    ///Ĭ��ͬʱ���е�һ���ֶ��������
    static const size_t DEFUALT_RUNNIG_ASYNC_SIZE = 1024;
    ///Ĭ�ϳ�����չ��ʱ����չ���첽���������
    static const size_t POOL_EXTEND_ASYNC_NUM = 128;

public:
    ///��Ч������ID
    static const unsigned int INVALID_IDENTITY = 0;
    ///��Ч�ĵ�����
    static const unsigned int INVALID_COMMAND = 0;

protected:

    //����ID������
    unsigned int id_builder_;

    //Э�̵ĳ��ӣ�����ע�������
    ID_TO_REGASYNC_POOL_MAP aysncobj_pool_;

    ///�������е�Э��
    RUNNING_ASYNOBJ_MAP running_aysncobj_;

    ///��ʱ���Ĺ�����
    ZCE_Timer_Queue *timer_queue_;

};

#endif //#ifndef ZCE_LIB_ASYNC_FRAMEWORK_BASE_

