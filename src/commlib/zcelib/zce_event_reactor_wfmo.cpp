#include "zce_predefine.h"
#include "zce_time_value.h"
#include "zce_event_handle_base.h"
#include "zce_os_adapt_socket.h"
#include "zce_trace_log_debug.h"
#include "zce_event_reactor_wfmo.h"




//
ZCE_WFMO_Reactor::ZCE_WFMO_Reactor()
{
    initialize();
}



ZCE_WFMO_Reactor::~ZCE_WFMO_Reactor()
{
}


//��ʼ��
int ZCE_WFMO_Reactor::initialize()
{
    //ȫ������Ϊ��Ч
    for (size_t i = 0; i < MAXIMUM_WAIT_OBJECTS; ++i)
    {
        watch_handle_ary_[i] = ZCE_INVALID_HANDLE;
    }
    for (size_t i = 0; i < MAXIMUM_WAIT_OBJECTS; ++i)
    {
        watch_socket_ary_[i] = ZCE_INVALID_HANDLE;
    }

    return ZCE_Reactor::initialize(MAXIMUM_WAIT_OBJECTS);
}


int ZCE_WFMO_Reactor::register_handler(ZCE_Event_Handler *event_handler, 
    int event_mask)
{
    int ret = 0;
    //ע��ڶ���������0����Ϊ��һҪ��ADD���ڶ��������ε������,��������ǰ������Ϊ�ع��鷳
    ret = ZCE_Reactor::register_handler(event_handler, 0);
    if (0 != ret)
    {
        return -1;
    }
    size_t watch_size = handler_map_.size();
    event_handler->set_mask(event_mask);
    //�����SOCKET���粿��
    if (ZCE_BIT_IS_SET(event_mask, ZCE_Event_Handler::READ_MASK)
        || ZCE_BIT_IS_SET(event_mask, ZCE_Event_Handler::ACCEPT_MASK)
        || ZCE_BIT_IS_SET(event_mask, ZCE_Event_Handler::CONNECT_MASK)
        || ZCE_BIT_IS_SET(event_mask, ZCE_Event_Handler::EXCEPT_MASK) 
        || ZCE_BIT_IS_SET(event_mask, ZCE_Event_Handler::WRITE_MASK)  )
    {
        WSAEVENT socket_event;
        ret = wfmo_socket_event(event_handler, socket_event);
        if (0 != ret)
        {
            //�����ع�
            ret = ZCE_Reactor::remove_handler(event_handler, false);
            return ret;
        }

        //WaitForMultipleObjects
        watch_handle_ary_[watch_size - 1] = socket_event;
        watch_socket_ary_[watch_size - 1] = event_handler->get_handle();
    }
    else if (ZCE_BIT_IS_SET(event_mask, ZCE_Event_Handler::INOTIFY_MASK))
    {
        watch_handle_ary_[watch_size - 1] = event_handler->get_handle();
    }
    else
    {
        ZCE_ASSERT(false);
    }
    return 0;
}

//�ӷ�Ӧ��ע��һ��ZCE_Event_Handler��ͬʱȡ�������е�mask
int ZCE_WFMO_Reactor::remove_handler(ZCE_Event_Handler *event_handler, 
    bool call_handle_close)
{
    
    size_t watch_size = handler_map_.size();
    ZCE_HANDLE socket_handle = event_handler->get_handle();
    //
    for (size_t i = 0; i < watch_size; ++i)
    {
        if (watch_socket_ary_[i] == socket_handle)
        {
            watch_socket_ary_[i] = ZCE_INVALID_HANDLE;
            watch_handle_ary_[i] = ZCE_INVALID_HANDLE;

            watch_socket_ary_[i] = watch_socket_ary_[watch_size - 1];
            watch_handle_ary_[i] = watch_handle_ary_[watch_size - 1];

            ::WSACloseEvent(watch_handle_ary_[i]);
        }
    }

    return ZCE_Reactor::remove_handler(event_handler, call_handle_close);
}

//����IO��������
int ZCE_WFMO_Reactor::handle_events(ZCE_Time_Value *time_out, size_t *size_event)
{
    int ret = 0;
    *size_event = 0;

    DWORD wait_msec = static_cast<DWORD>(time_out->total_msec());
    wait_msec = 0;
    DWORD watch_ary_size = static_cast<DWORD>(handler_map_.size());
    DWORD wait_status = WaitForMultipleObjects(watch_ary_size,
        watch_handle_ary_,
        FALSE,
        wait_msec);

    if (wait_status == WAIT_FAILED)
    {
        ZLOG_ERROR("[%s] ::WaitForMultipleObjects fail,error [%u].",
            __ZCE_FUNCTION__,
            ZCE_OS::last_error());
        return -1;
    }

    //����ǳ�ʱ
    if (wait_status == WAIT_TIMEOUT)
    {
        errno = ETIMEDOUT;
        return -1;
    }

    *size_event = 1;
    size_t activate_id = wait_status - WAIT_OBJECT_0;

    ZCE_Event_Handler *event_hdl = NULL;
    ret = find_event_handler(watch_handle_ary_[activate_id], event_hdl);
    if (0 != ret)
    {
        return -1;
    }

    int event_mask = event_hdl->get_mask();
    if (ZCE_BIT_IS_SET(event_mask, ZCE_Event_Handler::INOTIFY_MASK))
    {
        event_hdl->handle_input();
    }


    return 0;
}

int ZCE_WFMO_Reactor::wfmo_socket_event(ZCE_Event_Handler *event_handler,
    WSAEVENT &socket_event)
{
    int ret = 0;
    int event_mask = event_handler->get_mask();

    socket_event = ::WSACreateEvent();
    if (socket_event == WSA_INVALID_EVENT)
    {
        return -1;
    }
    long wmfo_net_event = 0;
    if (ZCE_BIT_IS_SET(event_mask, ZCE_Event_Handler::READ_MASK))
    {
        wmfo_net_event |= FD_READ | FD_CLOSE;
    }
    if (ZCE_BIT_IS_SET(event_mask, ZCE_Event_Handler::WRITE_MASK))
    {
        wmfo_net_event |= FD_WRITE;
    }
    if (ZCE_BIT_IS_SET(event_mask, ZCE_Event_Handler::CONNECT_MASK))
    {
        wmfo_net_event |= FD_CONNECT;
    }
    if (ZCE_BIT_IS_SET(event_mask, ZCE_Event_Handler::ACCEPT_MASK))
    {
        wmfo_net_event |= FD_ACCEPT;
    }
    if (ZCE_BIT_IS_SET(event_mask, ZCE_Event_Handler::EXCEPT_MASK))
    {
        wmfo_net_event |= FD_OOB;
    }
    ret = ::WSAEventSelect((SOCKET)event_handler->get_handle(), 
        socket_event, 
        wmfo_net_event);
    if (ret != 0)
    {
        return -1;
    }

    return 0;
}

