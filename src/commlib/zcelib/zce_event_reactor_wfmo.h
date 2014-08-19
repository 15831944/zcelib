/*!
* @copyright  2004-2014  Apache License, Version 2.0 FULLSAIL
* @filename   zce_event_reactor_wfmo.h
* @author     Sailzeng <sailerzeng@gmail.com>
* @version    
* @date       Sunday, August 17, 2014
* @brief      WFMO ��WaitForMultipleObjects����д�������Windowsר�еĶ�����
*             
*             
* @details    
*             
*             
*             
* @note       
*             
*/

#ifndef ZCE_LIB_EVENT_REACTOR_WFMO_H_
#define ZCE_LIB_EVENT_REACTOR_WFMO_H_

#include "zce_event_reactor_base.h"

/*!
* @brief      WaitforMu ��IO��Ӧ����IO��·����ģ��
*
*/
class ZCE_WFMO_Reactor : public ZCE_Reactor
{

public:

    /*!
    * @brief    ���캯��
    */
    ZCE_WFMO_Reactor();


    /*!
    * @brief      ���������� virtual��ม�
    */
    virtual ~ZCE_WFMO_Reactor();

public:

    /*!
    * @brief      ��ʼ��
    * @return     int              ����0��ʾ�ɹ���������ʾʧ��
    */
    int initialize();

    /*!
    * @brief      ע��һ��ZCE_Event_Handler����Ӧ��,EPOLL����ȷ��ע������ģ�������Ҫ�����������
    * @return     int             ����0��ʾ�ɹ���������ʾʧ��
    * @param[in]  event_handler   ע��ľ��
    * @param[in]  event_mask      ע���ͬʱ���õ�MASK��־����ο�@ref EVENT_MASK ,���Զ��ֵ|ʹ�á�
    */
    virtual int register_handler(ZCE_Event_Handler *event_handler, int event_mask);

    /*!
    * @brief      �ӷ�Ӧ��ע��һ��ZCE_Event_Handler��ͬʱȡ�������е�mask
    * @return     int               0��ʾ�ɹ�������ʧ��
    * @param[in]  event_handler     ע���ľ��
    * @param[in]  call_handle_close ע�����Ƿ��Զ����þ����handle_close����
    * */
    virtual int remove_handler(ZCE_Event_Handler *event_handler, bool call_handle_close);

    /*!
    * @brief      ȡ��ĳЩmask��־��
    * @return     int           ����0��ʾ�ɹ���������ʾʧ��
    * @param[in]  event_handler �����ľ��
    * @param[in]  event_mask    Ҫȡ����MASK��־����ο�@ref EVENT_MASK ,���Զ��ֵ|ʹ�á�
    * */
    virtual int cancel_wakeup(ZCE_Event_Handler *event_handler, int event_mask);

    /*!
    * @brief      ��ĳЩmask��־��
    * @return     int             ����0��ʾ�ɹ���������ʾʧ��
    * @param[in]  event_handler   �����ľ��
    * @param[in]  event_mask      Ҫ�򿪵ı�־����ο�@ref EVENT_MASK ,���Զ��ֵ|ʹ�á�
    * */
    virtual int schedule_wakeup(ZCE_Event_Handler *event_handler, int event_mask);


    /*!
    * @brief      ����IO��������
    * @return        int           ����0��ʾ�ɹ���������ʾʧ��
    * @param[in,out] time_out      ��ʱʱ�䣬��Ϻ󷵻�ʣ��ʱ��
    * @param[out]    size_event    �����ľ������
    */
    virtual int handle_events(ZCE_Time_Value *time_out, size_t *size_event);



    static int wfmo_socket_event(ZCE_Event_Handler *event_handler,
        WSAEVENT &socket_event);

protected:

    ///WaitForMultipleObjects�ȴ���Ŀ¼���
    ///ΪʲôҪ������ظ��Ľṹ����Ҫ��Ϊ�˷���ʹ��WaitForMultipleObjects���ٶ�
    ZCE_HANDLE     watch_handle_ary_[MAXIMUM_WAIT_OBJECTS];

    ///
    ZCE_HANDLE     watch_socket_ary_[MAXIMUM_WAIT_OBJECTS];
};

#endif //ZCE_LIB_EVENT_REACTOR_WFMO_H_

