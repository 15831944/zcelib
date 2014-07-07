

#ifndef ZCE_LIB_ASYNC_FRAMEWORK_COROUTINE_
#define ZCE_LIB_ASYNC_FRAMEWORK_COROUTINE_


#include "zce_async_fw_base.h"

//====================================================================================


/*!
* @brief      Э�̶���
*             ע�⣬Ϊ�˱���һЩ������ı�©�������ѡ��ļ̳з�ʽ��private
*/
class ZCE_Async_Coroutine : private ZCE_Async_Object
{
    friend class ZCE_Async_CoroutineMgr;

    //
    enum COROUTINE_STATE
    {
        //Я�̴��ݸ���������״ֵ̬��
        COROUTINE_CONTINUE = 0x10001,
        COROUTINE_EXIT = 0x10002,

        //��ʱ�󣬹�����֪ͨЯ�̵�״ֵ̬
        COROUTINE_TIMEOUT = 0x20002,
    };


public:
    /*!
    * @brief      ���캯����
    * @param      async_mgr ,Э���첽��������ָ��
    */
    ZCE_Async_Coroutine(ZCE_Async_CoroutineMgr *async_mgr);
protected:
    /*!
    * @brief      ��������
    */
    ~ZCE_Async_Coroutine();

public:

    /*!
    * @brief      ��ʼ���������ڹ��캯������ã��ڷ������ǰִ��һ�Σ�
    * @return     int 0��ʶ�ɹ�
    */
    virtual int initialize(unsigned int reg_cmd);

    /*!
    * @brief      �������ٺ�����������ǰ�ĵ���
    * @return     int
    */
    virtual int finish();

protected:

    ///Э�̶�������к���
    void coroutine_do();

    ///Э������,��Ҫ���صĺ���
    virtual int coroutine_run() = 0;

    ///�л���Main��Э�̻����������
    void yeild_main_continue();

    ///�л���Main,Э���˳�
    void yeild_main_exit();

    ///�л���Э�̣�Ҳ�����л������Լ�����
    void yeild_coroutine();


    /*!
    * @brief      �ȴ�time_out ʱ���ʱ�����ö�ʱ�����л�Э�̵�main
    * @return     int
    * @param      time_out
    */
    int waitfor_timeout(const ZCE_Time_Value &time_out);


    /*!
    * @brief      �̳�ZCE_Async_Object�ĺ�����
    * @param[out] continue_run ���ز��������ص�ǰ��Э���Ƿ�Ҫ����������ȥ
    */
    virtual void on_run(bool &continue_run);

    /*!
    * @brief      �첽����ʱ����
    * @param[in]  now_time  ������ʱ��ʱ�䣬
    * @param[out] continue_run �첽�����Ƿ��������,
    */
    virtual void on_timeout(const ZCE_Time_Value &now_time,
                            bool &continue_run);



protected:

    ///��С�Ķ�ջ
    static const size_t MIN_STACK_SIZE = 16 * 1024;
    ///Ĭ�϶�ջ
    static const size_t DEF_STACK_SIZE = 64 * 1024;
    ///���Ķ�ջ
    static const size_t MAX_STACK_SIZE = 256 * 1024;

public:

    ///static ����������Э�����к���������Э�̶�������к���
    static void static_do(ZCE_Async_Coroutine *coroutine);

protected:

    ///��С�Ķ�ջ
    size_t min_stack_size = 16 * 1024;

    ///Э�̶���
    coroutine_t      handle_;

    ///Э�̵Ķ�ջ��С��
    size_t           stack_size_;

    ///Э�̵�״̬
    COROUTINE_STATE  coroutine_state_;
};

//====================================================================================

/*!
* @brief      Э�̶������ع�����
*
*/
class ZCE_Async_CoroutineMgr : public ZCE_Async_ObjectMgr
{
public:

    //
    ZCE_Async_CoroutineMgr(ZCE_Timer_Queue *timer_queue);
    virtual ~ZCE_Async_CoroutineMgr();

protected:

    ///Ĭ���첽������ӵĳ�ʼ��������
    static const size_t COROUTINE_POOL_INIT_SIZE = 1;
    ///Ĭ�ϳ�����չ��ʱ����չ���첽���������
    static const size_t COROUTINE_POOL_EXTEND_SIZE = 16;

};


#endif //#ifndef ZCE_LIB_ASYNC_FRAMEWORK_COROUTINE_

