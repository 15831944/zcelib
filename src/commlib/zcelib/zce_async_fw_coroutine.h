

#ifndef ZCE_LIB_ASYNC_FRAMEWORK_COROUTINE_
#define ZCE_LIB_ASYNC_FRAMEWORK_COROUTINE_


#include "zce_async_fw_base.h"

//====================================================================================


/*!
* @brief      Э�̶���
*             
*/
class ZCE_Async_Coroutine :public ZCE_Async_Object
{
    friend class ZCE_Async_CoroutineMgr;

    //
    enum COROUTINE_STATE
    {
        COROUTINE_CONTINUE = 0x10001,
        COROUTINE_EXIT = 0x20001,
    };

public:
    ZCE_Async_Coroutine(ZCE_Async_CoroutineMgr *async_mgr);
protected:
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




    ///Э�̶�������к���
    void coroutine_do();
    
    ///�л���Main��Э�̻����������
    void yeild_main_continue();

    ///�л���Main,Э���˳�
    void yeild_main_exit();

    ///�л���Э�̣�Ҳ�����л������Լ�����
    void yeild_coroutine();

    ///Э������
    virtual int coroutine_run() = 0;


    /*!
    * @brief
    * @return     void
    * @param      continue_run
    */
    virtual void on_run(bool &continue_run);
    
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
    
    ///Э�̶���
    coroutine_t      handle_;

    ///Э�̵Ķ�ջ��С��
    size_t           stack_size_;

    ///Э�̵�״̬
    COROUTINE_STATE  coroutine_state_;
};

//====================================================================================

/*!
* @brief      ��Э�̣����ع�����
*             
*/
class ZCE_Async_CoroutineMgr :public ZCE_Async_ObjectMgr
{
public:

    //
    ZCE_Async_CoroutineMgr(ZCE_Timer_Queue *timer_queue);
    virtual ~ZCE_Async_CoroutineMgr();
    

};


#endif //#ifndef ZCE_LIB_ASYNC_FRAMEWORK_COROUTINE_

