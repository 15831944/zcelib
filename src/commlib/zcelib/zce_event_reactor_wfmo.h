/*!
* @copyright  2004-2014  Apache License, Version 2.0 FULLSAIL
* @filename   zce_event_reactor_wfmo.h
* @author     Sailzeng <sailerzeng@gmail.com>
* @version    
* @date       Sunday, August 17, 2014
* @brief      WFMO ��WaitForMultipleObject����д�������Windowsר�еĶ�����
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

    
};

#endif //ZCE_LIB_EVENT_REACTOR_WFMO_H_

